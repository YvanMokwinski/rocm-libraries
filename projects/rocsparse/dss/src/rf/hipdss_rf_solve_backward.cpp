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
#include "hipdss_datatype_utils.hpp"
#include "hipdss_enums.hpp"
#include "hipdss_rf_descr.hpp"

hipdssStatus_t hipdss::rf_solve_backward(hipdssHandle_t  handle,
                                         hipdss_rf_descr rf_descr,
                                         hipdssMatrix_t  RHS,
                                         hipdssMatrix_t  X)
{
    const auto factorization_type = rf_descr->get_factorization_type();

    auto       F          = rf_descr->get_factorizationMatrix();
    const auto F_datatype = F->get_val_type();

    auto                        operation   = static_cast<rocsparse_operation>(-1);
    auto                        uplo        = static_cast<rocsparse_fill_mode>(-1);
    auto                        diag        = static_cast<rocsparse_diag_type>(-1);
    const rocsparse_matrix_type matrix_type = rocsparse_matrix_type_general; // TO REVISIT

    switch(factorization_type)
    {

    case rf_factorization_type_lu:
    {
        uplo = rocsparse_fill_mode_upper;
        diag = rocsparse_diag_type_non_unit;

        operation = rocsparse_operation_none;

        break;
    }

    case rf_factorization_type_chol:
    {
        uplo = rocsparse_fill_mode_lower;
        diag = rocsparse_diag_type_non_unit;

        operation = (hipdss::datatype_is_complex(F->get_val_type()))
                        ? rocsparse_operation_conjugate_transpose
                        : rocsparse_operation_transpose;

        break;
    }

    case rf_factorization_type_ldl:
    {
        uplo = rocsparse_fill_mode_lower;
        diag = rocsparse_diag_type_unit;

        operation = (hipdss::datatype_is_complex(F->get_val_type()))
                        ? rocsparse_operation_conjugate_transpose
                        : rocsparse_operation_transpose;

        break;
    }
    }

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(*F,
                                                            rocsparse_spmat_fill_mode,
                                                            &uplo,
                                                            sizeof(uplo)));

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(*F,
                                                            rocsparse_spmat_diag_type,
                                                            &diag,
                                                            sizeof(diag)));

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(*F,
                                                            rocsparse_spmat_matrix_type,
                                                            &matrix_type,
                                                            sizeof(matrix_type)));

    auto backward_sptrsm_descr = rf_descr->get_backward_sptrsm_descr();
    if(backward_sptrsm_descr == nullptr)
    {
        auto* config = new hipdss::sptrsm_config_t();
        config->set_alg(rocsparse_sptrsm_alg_default);
        config->set_op_A(operation);
        config->set_compute_datatype(hipDataType2rocsparse_datatype(F_datatype));
        config->set_scalar_datatype(hipDataType2rocsparse_datatype(F_datatype));
        config->set_op_X(rocsparse_operation_none);
        config->set_analysis_policy(rocsparse_analysis_policy_reuse);
        rf_descr->set_backward_sptrsm_config(config);

        RETURN_IF_HIPDSS_ERROR(hipdss::sptrsm_create_and_configure(handle,
                                                                   &backward_sptrsm_descr,
                                                                   rf_descr->get_backward_sptrsm_config()));

        RETURN_IF_HIPDSS_ERROR(hipdss::sptrsm_analysis(handle,
                                                       backward_sptrsm_descr,
                                                       *F,
                                                       *RHS,
                                                       *X));

        rf_descr->set_backward_sptrsm_descr(backward_sptrsm_descr);
    }

    RETURN_IF_HIPDSS_ERROR(hipdss::sptrsm_compute(handle,
                                                  backward_sptrsm_descr,
                                                  *F,
                                                  *RHS,
                                                  *X));

    return HIPDSS_STATUS_SUCCESS;
}
