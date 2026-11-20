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

#include "hipdss_sptrsm.hpp"
#include "hipdssContext.hpp"
#include "hipdss_control.hpp"
#include "hipdss_enums.hpp"
#include "rocsparse/internal/generic/rocsparse_sptrsm.h"
#include "rocsparse/rocsparse-auxiliary.h"

hipdssStatus_t hipdss::sptrsm_create_and_configure(hipdssHandle_t                 handle,
                                                   rocsparse_sptrsm_descr*        p_sptrsm_descr,
                                                   const hipdss::sptrsm_config_t* config)
{

    //
    const auto alg              = config->get_alg();
    const auto op_A             = config->get_op_A();
    const auto op_X             = config->get_op_X();
    const auto analysis_policy  = config->get_analysis_policy();
    const auto compute_datatype = config->get_compute_datatype();
    const auto scalar_datatype  = config->get_scalar_datatype();
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_create_sptrsm_descr(p_sptrsm_descr));
    auto sptrsm_descr = p_sptrsm_descr[0];

    //
    // Operation A
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_set_input(*handle,
                                                         sptrsm_descr,
                                                         rocsparse_sptrsm_input_operation_A,
                                                         &op_A,
                                                         sizeof(op_A),
                                                         nullptr));
    //
    // Operation X
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_set_input(*handle,
                                                         sptrsm_descr,
                                                         rocsparse_sptrsm_input_operation_X,
                                                         &op_X,
                                                         sizeof(op_X),
                                                         nullptr));

    //
    // Algorithm
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_set_input(*handle,
                                                         sptrsm_descr,
                                                         rocsparse_sptrsm_input_alg,
                                                         &alg,
                                                         sizeof(alg),
                                                         nullptr));

    //
    // Scalar datatype
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_set_input(*handle,
                                                         sptrsm_descr,
                                                         rocsparse_sptrsm_input_scalar_datatype,
                                                         &scalar_datatype,
                                                         sizeof(scalar_datatype),
                                                         nullptr));

    //
    // Compute datatype
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_set_input(*handle,
                                                         sptrsm_descr,
                                                         rocsparse_sptrsm_input_compute_datatype,
                                                         &compute_datatype,
                                                         sizeof(compute_datatype),
                                                         nullptr));

    //
    // Analysis policy.
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_set_input(*handle,
                                                         sptrsm_descr,
                                                         rocsparse_sptrsm_input_analysis_policy,
                                                         &analysis_policy,
                                                         sizeof(analysis_policy),
                                                         nullptr));

    //
    // Scalar alpha
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_set_pointer_mode(*handle,
                                                         rocsparse_pointer_mode_host));

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_set_input(*handle,
                                                         sptrsm_descr,
                                                         rocsparse_sptrsm_input_scalar_alpha,
                                                         handle->get_host_one(rocsparse_datatype2hipDataType(scalar_datatype)),
                                                         sizeof(const void*),
                                                         nullptr));

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdss::sptrsm_analysis(hipdssHandle_t              handle,
                                       rocsparse_sptrsm_descr      sptrsm_descr,
                                       rocsparse_const_spmat_descr F,
                                       rocsparse_const_dnmat_descr rhs,
                                       rocsparse_dnmat_descr       x)
{

    size_t buffer_size_in_bytes = std::numeric_limits<size_t>::max();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_buffer_size(*handle,
                                                           sptrsm_descr,
                                                           F,
                                                           rhs,
                                                           x,
                                                           rocsparse_sptrsm_stage_analysis,
                                                           &buffer_size_in_bytes,
                                                           nullptr));

    void* buffer{};
    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_malloc(&buffer,
                                                  buffer_size_in_bytes));
    }

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm(*handle,
                                               sptrsm_descr,
                                               F,
                                               rhs,
                                               x,
                                               rocsparse_sptrsm_stage_analysis,
                                               buffer_size_in_bytes,
                                               buffer,
                                               nullptr));

    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_free(buffer));
    }

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdss::sptrsm_compute(hipdssHandle_t              handle,
                                      rocsparse_sptrsm_descr      sptrsm_descr,
                                      rocsparse_spmat_descr       F,
                                      rocsparse_const_dnmat_descr rhs,
                                      rocsparse_dnmat_descr       x)
{
    size_t buffer_size_in_bytes = std::numeric_limits<size_t>::max();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_buffer_size(*handle,
                                                           sptrsm_descr,
                                                           F,
                                                           rhs,
                                                           x,
                                                           rocsparse_sptrsm_stage_compute,
                                                           &buffer_size_in_bytes,
                                                           nullptr));

    void* buffer{};
    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_malloc(&buffer,
                                                  buffer_size_in_bytes));
    }

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm(*handle,
                                               sptrsm_descr,
                                               F,
                                               rhs,
                                               x,
                                               rocsparse_sptrsm_stage_compute,
                                               buffer_size_in_bytes,
                                               buffer,
                                               nullptr));

    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_free(buffer));
    }

    return HIPDSS_STATUS_SUCCESS;
}
