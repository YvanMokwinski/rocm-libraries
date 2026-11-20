/*! \file */
/* ************************************************************************
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */

#include "hipdssMatrix.hpp"
#include "hipdss_control.hpp"
#include "hipdss_enums.hpp"
#include "rocsparse_datatype_utils.hpp"

namespace hipdss
{
    static hipdssStatus_t rocsparse_dnvec_set_zero(hipdssHandle_t        handle,
                                                   rocsparse_dnvec_descr that)
    {
        int64_t            size{};
        void*              v{};
        rocsparse_datatype type{};

        RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_get(that,
                                                      &size,
                                                      &v,
                                                      &type));

        RETURN_IF_HIP_ERROR(hipMemsetAsync(v,
                                           0,
                                           rocsparse::datatype_sizeof(type) * size,
                                           handle->get_stream()));
        return HIPDSS_STATUS_SUCCESS;
    }

    static hipdssStatus_t rf_update_factorization(hipdssHandle_t  handle,
                                                  hipdss_rf_descr rf_descr,
                                                  hipdssMatrix_t  inputMatrix)
    {
        //
        // 1. Operation F = 0, F += A. This copies A and set the fill-in to zere.
        //    batch support.
        //
        //
        // Set internal factorization to zero.
        //
        auto spvec_descr = rf_descr->get_A2F_spvec();
        if(spvec_descr == nullptr)
        {
            auto A2F = rf_descr->get_A2F();
            RETURN_IF_HIPDSS_ERROR(A2F->to_device(handle));

            RETURN_IF_ROCSPARSE_ERROR(rocsparse_create_const_spvec_descr(&spvec_descr,
                                                                         A2F->get_size(),
                                                                         A2F->get_size(),
                                                                         A2F->device_data(),
                                                                         inputMatrix->get_const_val_data(),
                                                                         hipDataType2rocsparse_indextype(A2F->get_indextype()),
                                                                         hipdssIndexBase_t2rocsparse_index_base(A2F->get_base()),
                                                                         hipDataType2rocsparse_datatype(inputMatrix->get_val_type())));
            rf_descr->set_A2F_spvec(spvec_descr);
        }

        auto a2f   = rf_descr->get_A2F_spvec(*inputMatrix);
        auto F_val = rf_descr->get_F_val();

        RETURN_IF_HIPDSS_ERROR(rocsparse_dnvec_set_zero(handle,
                                                        F_val));

        RETURN_IF_ROCSPARSE_ERROR(rocsparse_scatter(*handle,
                                                    a2f,
                                                    F_val));
        return HIPDSS_STATUS_SUCCESS;
    }

}

hipdssStatus_t hipdss::rf_factorization(hipdssHandle_t  handle,
                                        hipdss_rf_descr rf_descr,
                                        hipdssConfig_t  solverConfig,
                                        hipdssData_t    solverData,
                                        hipdssMatrix_t  inputMatrix)
{

    auto F = rf_descr->get_factorizationMatrix();

    {
        rocsparse_dnvec_descr F_val{};
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_create_dnvec_descr(&F_val,
                                                               F->get_nnz(),
                                                               F->get_val_data(),
                                                               hipDataType2rocsparse_datatype(F->get_val_type())));

        rf_descr->set_F_val(F_val);
    }

    RETURN_IF_HIPDSS_ERROR(F->create_rocsparse_spmat_descr());

    const auto factorization_type = rf_descr->get_factorization_type();
    switch(factorization_type)
    {
    case rf_factorization_type_lu:
    {
        auto spilu0_descr = rf_descr->get_spilu0_descr();
        if(spilu0_descr == nullptr)
        {

            auto* config = new hipdss::spilu0_config_t();
            config->set_alg(rocsparse_spilu0_alg_default);
            config->set_analysis_policy(rocsparse_analysis_policy_force);
            config->set_compute_datatype(hipDataType2rocsparse_datatype(F->get_val_type()));
            config->set_boost_enable(0);
            config->set_boost_tolerance_pointer_mode(rocsparse_pointer_mode_host);
            config->set_boost_tolerance(nullptr);
            config->set_boost_value_pointer_mode(rocsparse_pointer_mode_host);
            config->set_boost_value(nullptr);
            config->set_singularity_tolerance_pointer_mode(rocsparse_pointer_mode_host);
            config->set_singularity_tolerance_host_value(1.0e-8);
            config->set_singularity_tolerance(config->get_singularity_tolerance_host_value());

            rf_descr->set_spilu0_config(config);

            RETURN_IF_HIPDSS_ERROR(hipdss::spilu0_create_and_configure(handle,
                                                                       &spilu0_descr,
                                                                       rf_descr->get_spilu0_config(),
                                                                       nullptr));
            rf_descr->set_spilu0_descr(spilu0_descr);

            //
            // ILU0 Analysis
            //
            RETURN_IF_HIPDSS_ERROR(hipdss::spilu0_analysis(handle,
                                                           spilu0_descr,
                                                           F,
                                                           nullptr));
        }
        break;
    }

    case rf_factorization_type_chol:
    {
        auto spic0_descr = rf_descr->get_spic0_descr();
        if(spic0_descr == nullptr)
        {

            auto* config = new hipdss::spic0_config_t();
            config->set_alg(rocsparse_spic0_alg_default);
            config->set_analysis_policy(rocsparse_analysis_policy_force);
            config->set_compute_datatype(hipDataType2rocsparse_datatype(F->get_val_type()));
            config->set_boost_enable(0);
            config->set_boost_tolerance_pointer_mode(rocsparse_pointer_mode_host);
            config->set_boost_tolerance(nullptr);
            config->set_boost_value_pointer_mode(rocsparse_pointer_mode_host);
            config->set_boost_value(nullptr);
            config->set_singularity_tolerance_pointer_mode(rocsparse_pointer_mode_host);
            config->set_singularity_tolerance_host_value(1.0e-8);
            config->set_singularity_tolerance(config->get_singularity_tolerance_host_value());
            rf_descr->set_spic0_config(config);

            RETURN_IF_HIPDSS_ERROR(hipdss::spic0_create_and_configure(handle,
                                                                      &spic0_descr,
                                                                      rf_descr->get_spic0_config(),
                                                                      nullptr));
            rf_descr->set_spic0_descr(spic0_descr);

            RETURN_IF_HIPDSS_ERROR(hipdss::spic0_analysis(handle,
                                                          spic0_descr,
                                                          F,
                                                          nullptr));
        }
        break;
    }

    case rf_factorization_type_ldl:
    {
        auto spildlt0_descr = rf_descr->get_spildlt0_descr();
        if(spildlt0_descr == nullptr)
        {

            auto* config = new hipdss::spildlt0_config_t();
            config->set_alg(rocsparse_spildlt0_alg_default);
            config->set_analysis_policy(rocsparse_analysis_policy_force);
            config->set_compute_datatype(hipDataType2rocsparse_datatype(F->get_val_type()));
            config->set_boost_enable(0);
            config->set_boost_tolerance_pointer_mode(rocsparse_pointer_mode_host);
            config->set_boost_tolerance(nullptr);
            config->set_boost_value_pointer_mode(rocsparse_pointer_mode_host);
            config->set_boost_value(nullptr);
            config->set_singularity_tolerance_pointer_mode(rocsparse_pointer_mode_host);
            config->set_singularity_tolerance_host_value(1.0e-8);
            config->set_singularity_tolerance(config->get_singularity_tolerance_host_value());
            rf_descr->set_spildlt0_config(config);

            RETURN_IF_HIPDSS_ERROR(hipdss::spildlt0_create_and_configure(handle,
                                                                         &spildlt0_descr,
                                                                         config,
                                                                         nullptr));
            rf_descr->set_spildlt0_descr(spildlt0_descr);

            RETURN_IF_HIPDSS_ERROR(hipdss::spildlt0_analysis(handle,
                                                             spildlt0_descr,
                                                             F,
                                                             nullptr));
        }
        break;
    }
    }

    RETURN_IF_HIPDSS_ERROR(hipdss::rf_update_factorization(handle,
                                                           rf_descr,
                                                           inputMatrix));

    switch(factorization_type)
    {
    case rf_factorization_type_lu:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::spilu0_compute(handle,
                                                      rf_descr->get_spilu0_descr(),
                                                      F,
                                                      nullptr));

        break;
    }

    case rf_factorization_type_chol:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::spic0_compute(handle,
                                                     rf_descr->get_spic0_descr(),
                                                     F,
                                                     nullptr));
        break;
    }

    case rf_factorization_type_ldl:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::spildlt0_compute(handle,
                                                        rf_descr->get_spildlt0_descr(),
                                                        F,
                                                        nullptr));
        break;
    }
    }

    return HIPDSS_STATUS_SUCCESS;
}
