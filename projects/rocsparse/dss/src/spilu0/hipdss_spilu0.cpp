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

#include "hipdss_spilu0.hpp"
#include "hipdssContext.hpp"
#include "hipdssMatrix.hpp"
#include "hipdss_control.hpp"
#include "rocsparse/internal/generic/rocsparse_spilu0.h"
#include "rocsparse/rocsparse-auxiliary.h"

hipdssStatus_t hipdss::spilu0_analysis(hipdssHandle_t         handle,
                                       rocsparse_spilu0_descr spilu0_descr,
                                       hipdssMatrix_t         factorization_matrix,
                                       rocsparse_error*       p_error)
{

    //
    // 1-2: Determine buffer size of the analysis phase.
    //
    size_t buffer_size_in_bytes = std::numeric_limits<size_t>::max();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0_buffer_size(*handle,
                                                           spilu0_descr,
                                                           *factorization_matrix,
                                                           *factorization_matrix,
                                                           rocsparse_spilu0_stage_analysis,
                                                           &buffer_size_in_bytes,
                                                           p_error));

    void* buffer{};
    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_malloc(&buffer,
                                                  buffer_size_in_bytes));
    }

    // 2-2: Execute the analysis phase.
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0(*handle,
                                               spilu0_descr,
                                               *factorization_matrix,
                                               *factorization_matrix,
                                               rocsparse_spilu0_stage_analysis,
                                               buffer_size_in_bytes,
                                               buffer,
                                               p_error));

    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_free(buffer));
    }
    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdss::spilu0_compute(hipdssHandle_t         handle,
                                      rocsparse_spilu0_descr spilu0_descr,
                                      hipdssMatrix_t         factorization_matrix,
                                      rocsparse_error*       p_error)
{

    //
    // 1-2: Determine buffer size of the compute phase.
    //
    size_t buffer_size_in_bytes = std::numeric_limits<size_t>::max();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0_buffer_size(*handle,
                                                           spilu0_descr,
                                                           *factorization_matrix,
                                                           *factorization_matrix,
                                                           rocsparse_spilu0_stage_compute,
                                                           &buffer_size_in_bytes,
                                                           p_error));
    void* buffer{};
    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_malloc(&buffer,
                                                  buffer_size_in_bytes));
    }

    //
    // 2-2: Execute the compute phase.
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0(*handle,
                                               spilu0_descr,
                                               *factorization_matrix,
                                               *factorization_matrix,
                                               rocsparse_spilu0_stage_compute,
                                               buffer_size_in_bytes,
                                               buffer,
                                               p_error));

    if(buffer_size_in_bytes > 0)
    {
        RETURN_IF_HIP_ERROR(handle->device_free(buffer));
    }

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdss::spilu0_create_and_configure(hipdssHandle_t                 handle,
                                                   rocsparse_spilu0_descr*        p_spilu0_descr,
                                                   const hipdss::spilu0_config_t* config,
                                                   rocsparse_error*               p_error)
{
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0_descr_create(*handle,
                                                            p_spilu0_descr,
                                                            p_error));

    auto spilu0_descr = p_spilu0_descr[0];

    //
    // 1-5: Set algorithm.
    //
    const auto alg = config->get_alg();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0_set_input(*handle,
                                                         spilu0_descr,
                                                         rocsparse_spilu0_input_alg,
                                                         &alg,
                                                         sizeof(alg),
                                                         p_error));

    //
    // 2-5: Set analysis policy.
    //
    const auto analysis_policy = config->get_analysis_policy();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0_set_input(*handle,
                                                         spilu0_descr,
                                                         rocsparse_spilu0_input_analysis_policy,
                                                         &analysis_policy,
                                                         sizeof(analysis_policy),
                                                         p_error));

    //
    // 3-5: Set compute datatype.
    //
    const auto compute_datatype = config->get_compute_datatype();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0_set_input(*handle,
                                                         spilu0_descr,
                                                         rocsparse_spilu0_input_compute_datatype,
                                                         &compute_datatype,
                                                         sizeof(compute_datatype),
                                                         p_error));

    //
    // 4-5: Set singularity tolerance.
    //
    const auto singularity_tolerance_pointer_mode = config->get_singularity_tolerance_pointer_mode();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_set_pointer_mode(*handle,
                                                         singularity_tolerance_pointer_mode));

    const auto singularity_tolerance = config->get_singularity_tolerance();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0_set_input(*handle,
                                                         spilu0_descr,
                                                         rocsparse_spilu0_input_singularity_tolerance,
                                                         singularity_tolerance,
                                                         sizeof(void*),
                                                         p_error));

    //
    // 5-5 Set boost.
    //
    const auto boost_enable = config->get_boost_enable();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0_set_input(*handle,
                                                         spilu0_descr,
                                                         rocsparse_spilu0_input_boost_enable,
                                                         &boost_enable,
                                                         sizeof(int32_t),
                                                         p_error));
    if(boost_enable)
    {
        //
        // 5a-5: Set boost tolerance.
        //
        const auto boost_tolerance_pointer_mode = config->get_boost_tolerance_pointer_mode();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_set_pointer_mode(*handle,
                                                             boost_tolerance_pointer_mode));

        const auto boost_tolerance = config->get_boost_tolerance();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0_set_input(*handle,
                                                             spilu0_descr,
                                                             rocsparse_spilu0_input_boost_tolerance,
                                                             boost_tolerance,
                                                             sizeof(void*),
                                                             p_error));

        //
        // 5b-5 Set boost value.
        //
        const auto boost_value_pointer_mode = config->get_boost_value_pointer_mode();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_set_pointer_mode(*handle,
                                                             boost_value_pointer_mode));

        const auto boost_value = config->get_boost_value();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spilu0_set_input(*handle,
                                                             spilu0_descr,
                                                             rocsparse_spilu0_input_boost_value,
                                                             boost_value,
                                                             sizeof(void*),
                                                             p_error));
    }
    return HIPDSS_STATUS_SUCCESS;
}
