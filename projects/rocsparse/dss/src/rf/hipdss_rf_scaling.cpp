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

#include "../spequilibrate/hipdss_spequilibrate.h"
#include "hipdssContext.hpp"
#include "hipdss_control.hpp"

hipdssStatus_t hipdss::rf_scaling(hipdssHandle_t        handle,
                                  hipdss_rf_descr       rf_descr,
                                  rocsparse_spmat_descr A)
{
    rocsparse_error*           p_error = nullptr;
    hipdss_spequilibrate_descr spequilibrate_descr;
    RETURN_IF_HIPDSS_ERROR(
        hipdss_spequilibrate_descr_create(handle,
                                          &spequilibrate_descr,
                                          p_error));

    const hipdss_spequilibrate_alg alg = hipdss_spequilibrate_alg_ruiz;
    RETURN_IF_HIPDSS_ERROR(hipdss_spequilibrate_set_input(
        handle,
        spequilibrate_descr,
        hipdss_spequilibrate_input_alg,
        &alg,
        sizeof(alg),
        p_error));

    const double tolerance = 1.0e-5;

    const int64_t nmaxiter = 40;
    RETURN_IF_HIPDSS_ERROR(hipdss_spequilibrate_set_input(
        handle,
        spequilibrate_descr,
        hipdss_spequilibrate_input_ruiz_nmaxiter,
        &nmaxiter,
        sizeof(nmaxiter),
        p_error));

    RETURN_IF_HIPDSS_ERROR(hipdss_spequilibrate_set_input(
        handle,
        spequilibrate_descr,
        hipdss_spequilibrate_input_ruiz_tol,
        &tolerance,
        sizeof(tolerance),
        p_error));

    void*  buffer{};
    size_t buffer_size_in_bytes = std::numeric_limits<size_t>::max();
    RETURN_IF_HIPDSS_ERROR(hipdss_spequilibrate_buffer_size(
        handle,
        spequilibrate_descr,
        A,
        rf_descr->get_DR(),
        rf_descr->get_DC(),
        hipdss_spequilibrate_stage_analysis,
        &buffer_size_in_bytes,
        p_error));

    if(buffer_size_in_bytes > 0)
        RETURN_IF_HIP_ERROR(handle->device_malloc(&buffer,
                                                  buffer_size_in_bytes));

    RETURN_IF_HIPDSS_ERROR(hipdss_spequilibrate(
        handle,
        spequilibrate_descr,
        A,
        rf_descr->get_DR(),
        rf_descr->get_DC(),
        hipdss_spequilibrate_stage_analysis,
        buffer_size_in_bytes,
        buffer,
        p_error));

    if(buffer_size_in_bytes > 0)
        RETURN_IF_HIP_ERROR(handle->device_free(buffer));

    buffer_size_in_bytes = std::numeric_limits<size_t>::max();
    RETURN_IF_HIPDSS_ERROR(hipdss_spequilibrate_buffer_size(
        handle,
        spequilibrate_descr,
        A,
        rf_descr->get_DR(),
        rf_descr->get_DC(),
        hipdss_spequilibrate_stage_compute,
        &buffer_size_in_bytes,
        p_error));
    if(buffer_size_in_bytes > 0)
        RETURN_IF_HIP_ERROR(handle->device_malloc(&buffer,
                                                  buffer_size_in_bytes));

    RETURN_IF_HIPDSS_ERROR(hipdss_spequilibrate(
        handle,
        spequilibrate_descr,
        A,
        rf_descr->get_DR(),
        rf_descr->get_DC(),
        hipdss_spequilibrate_stage_compute,
        buffer_size_in_bytes,
        buffer,
        p_error));

    if(buffer_size_in_bytes > 0)
        RETURN_IF_HIP_ERROR(handle->device_free(buffer));

    int64_t gpu_niter;
    RETURN_IF_HIPDSS_ERROR(hipdss_spequilibrate_get_output(handle,
                                                           spequilibrate_descr,
                                                           hipdss_spequilibrate_output_ruiz_iter,
                                                           &gpu_niter,
                                                           sizeof(gpu_niter),
                                                           p_error));

    double gpu_residual;
    RETURN_IF_HIPDSS_ERROR(hipdss_spequilibrate_get_output(handle,
                                                           spequilibrate_descr,
                                                           hipdss_spequilibrate_output_ruiz_nrm,
                                                           &gpu_residual,
                                                           sizeof(gpu_residual),
                                                           p_error));

    RETURN_IF_HIP_ERROR(handle->synchronize_stream());

    std::cout << "gpu niter " << gpu_niter << std::endl;
    std::cout << "gpu residual " << gpu_residual << std::endl;

    RETURN_IF_HIPDSS_ERROR(hipdss_spequilibrate_descr_destroy(handle,
                                                              spequilibrate_descr,
                                                              p_error));

    return HIPDSS_STATUS_SUCCESS;
}
