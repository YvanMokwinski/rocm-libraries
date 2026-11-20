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

#include "hipdss_spic0.hpp"
#include "hipdssContext.hpp"
#include "hipdssMatrix.hpp"
#include "hipdss_control.hpp"
#include "rocsparse/internal/generic/rocsparse_spic0.h"
#include "rocsparse/rocsparse-auxiliary.h"

hipdssStatus_t hipdss::spic0_analysis(hipdssHandle_t        handle,
                                      rocsparse_spic0_descr spic0_descr,
                                      hipdssMatrix_t        factorization_matrix,
                                      rocsparse_error*      p_error)
{

    //
    // Let's suppose the we seek the lower part, which is not good.
    //
    const rocsparse_fill_mode uplo = rocsparse_fill_mode_lower;
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(*factorization_matrix,
                                                            rocsparse_spmat_fill_mode,
                                                            &uplo,
                                                            sizeof(uplo)));

    const rocsparse_diag_type diag = rocsparse_diag_type_unit;
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(*factorization_matrix,
                                                            rocsparse_spmat_diag_type,
                                                            &diag,
                                                            sizeof(diag)));

    const rocsparse_matrix_type matrix_type = rocsparse_matrix_type_general;
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(*factorization_matrix,
                                                            rocsparse_spmat_matrix_type,
                                                            &matrix_type,
                                                            sizeof(matrix_type)));

    size_t buffer_size_in_bytes = std::numeric_limits<size_t>::max();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0_buffer_size(*handle,
                                                          spic0_descr,
                                                          *factorization_matrix,
                                                          *factorization_matrix,
                                                          rocsparse_spic0_stage_analysis,
                                                          &buffer_size_in_bytes,
                                                          nullptr));

    void* buffer{};
    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_malloc(&buffer,
                                                  buffer_size_in_bytes));
    }

    //
    // Call in place Incomplete Cholesky factorization.
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0(*handle,
                                              spic0_descr,
                                              *factorization_matrix,
                                              *factorization_matrix,
                                              rocsparse_spic0_stage_analysis,
                                              buffer_size_in_bytes,
                                              buffer,
                                              nullptr));

    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_free(buffer));
    }
    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdss::spic0_compute(hipdssHandle_t        handle,
                                     rocsparse_spic0_descr spic0_descr,
                                     hipdssMatrix_t        factorization_matrix,
                                     rocsparse_error*      p_error)
{

    const rocsparse_fill_mode uplo = rocsparse_fill_mode_lower;
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(*factorization_matrix,
                                                            rocsparse_spmat_fill_mode,
                                                            &uplo,
                                                            sizeof(uplo)));

    const rocsparse_diag_type diag = rocsparse_diag_type_unit;
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(*factorization_matrix,
                                                            rocsparse_spmat_diag_type,
                                                            &diag,
                                                            sizeof(diag)));

    const rocsparse_matrix_type matrix_type = rocsparse_matrix_type_general;
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(*factorization_matrix,
                                                            rocsparse_spmat_matrix_type,
                                                            &matrix_type,
                                                            sizeof(matrix_type)));

    size_t buffer_size_in_bytes = std::numeric_limits<size_t>::max();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0_buffer_size(*handle,
                                                          spic0_descr,
                                                          *factorization_matrix,
                                                          *factorization_matrix,
                                                          rocsparse_spic0_stage_compute,
                                                          &buffer_size_in_bytes,
                                                          nullptr));
    void* buffer{};
    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_malloc(&buffer,
                                                  buffer_size_in_bytes));
    }

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0(*handle,
                                              spic0_descr,
                                              *factorization_matrix,
                                              *factorization_matrix,
                                              rocsparse_spic0_stage_compute,
                                              buffer_size_in_bytes,
                                              buffer,
                                              nullptr));

    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_free(buffer));
    }

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdss::spic0_create_and_configure(hipdssHandle_t                handle,
                                                  rocsparse_spic0_descr*        p_spic0_descr,
                                                  const hipdss::spic0_config_t* config,
                                                  rocsparse_error*              p_error)
{

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0_descr_create(*handle,
                                                           p_spic0_descr,
                                                           p_error));

    auto spic0_descr = p_spic0_descr[0];

    const auto alg = config->get_alg();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0_set_input(*handle,
                                                        spic0_descr,
                                                        rocsparse_spic0_input_alg,
                                                        &alg,
                                                        sizeof(alg),
                                                        p_error));

    const auto analysis_policy = config->get_analysis_policy();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0_set_input(*handle,
                                                        spic0_descr,
                                                        rocsparse_spic0_input_analysis_policy,
                                                        &analysis_policy,
                                                        sizeof(analysis_policy),
                                                        p_error));

    const auto compute_datatype = config->get_compute_datatype();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0_set_input(*handle,
                                                        spic0_descr,
                                                        rocsparse_spic0_input_compute_datatype,
                                                        &compute_datatype,
                                                        sizeof(compute_datatype),
                                                        p_error));

    const auto singularity_tolerance_pointer_mode = config->get_singularity_tolerance_pointer_mode();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_set_pointer_mode(*handle,
                                                         singularity_tolerance_pointer_mode));

    const auto singularity_tolerance = config->get_singularity_tolerance();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0_set_input(*handle,
                                                        spic0_descr,
                                                        rocsparse_spic0_input_singularity_tolerance,
                                                        singularity_tolerance,
                                                        sizeof(void*),
                                                        p_error));

    const auto boost_enable = config->get_boost_enable();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0_set_input(*handle,
                                                        spic0_descr,
                                                        rocsparse_spic0_input_boost_enable,
                                                        &boost_enable,
                                                        sizeof(int32_t),
                                                        p_error));

    if(boost_enable)
    {

        const auto boost_tolerance_pointer_mode = config->get_boost_tolerance_pointer_mode();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_set_pointer_mode(*handle,
                                                             boost_tolerance_pointer_mode));

        const auto boost_tolerance = config->get_boost_tolerance();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0_set_input(*handle,
                                                            spic0_descr,
                                                            rocsparse_spic0_input_boost_tolerance,
                                                            boost_tolerance,
                                                            sizeof(void*),
                                                            p_error));

        const auto boost_value_pointer_mode = config->get_boost_value_pointer_mode();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_set_pointer_mode(*handle,
                                                             boost_value_pointer_mode));

        const auto boost_value = config->get_boost_value();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spic0_set_input(*handle,
                                                            spic0_descr,
                                                            rocsparse_spic0_input_boost_value,
                                                            boost_value,
                                                            sizeof(void*),
                                                            p_error));
    }
    return HIPDSS_STATUS_SUCCESS;
}
