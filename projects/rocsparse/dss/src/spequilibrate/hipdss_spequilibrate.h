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

#ifndef HIPDSS_SPEQUILIBRATE_H
#define HIPDSS_SPEQUILIBRATE_H

#include "hipdss.h"
#include <rocsparse/rocsparse.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _hipdss_spequilibrate_descr* hipdss_spequilibrate_descr;

typedef enum _hipdss_spequilibrate_alg
{
    hipdss_spequilibrate_alg_default,
    hipdss_spequilibrate_alg_ruiz
} hipdss_spequilibrate_alg;

typedef enum _hipdss_spequilibrate_input
{
    hipdss_spequilibrate_input_alg,
    hipdss_spequilibrate_input_ruiz_nmaxiter,
    hipdss_spequilibrate_input_ruiz_tol
} hipdss_spequilibrate_input;

typedef enum _hipdss_spequilibrate_input_data
{
    hipdss_spequilibrate_input_data_ruiz_tol,
} hipdss_spequilibrate_input_data;

typedef enum _hipdss_spequilibrate_output
{
    hipdss_spequilibrate_output_ruiz_iter,
    hipdss_spequilibrate_output_ruiz_nrm
} hipdss_spequilibrate_output;

typedef enum _hipdss_spequilibrate_stage
{
    hipdss_spequilibrate_stage_analysis,
    hipdss_spequilibrate_stage_compute
} hipdss_spequilibrate_stage;

HIPDSS_EXPORT
hipdssStatus_t hipdss_spequilibrate_set_input(hipdssHandle_t             handle,
                                              hipdss_spequilibrate_descr descr,
                                              hipdss_spequilibrate_input value,
                                              const void*                data,
                                              size_t                     data_size_in_bytes,
                                              rocsparse_error*           p_error);

HIPDSS_EXPORT
hipdssStatus_t hipdss_spequilibrate_get_output(hipdssHandle_t              handle,
                                               hipdss_spequilibrate_descr  descr,
                                               hipdss_spequilibrate_output value,
                                               void*                       data,
                                               size_t                      data_size_in_bytes,
                                               rocsparse_error*            p_error);

HIPDSS_EXPORT
hipdssStatus_t
    hipdss_spequilibrate_descr_create(hipdssHandle_t              handle,
                                      hipdss_spequilibrate_descr* p_descr,
                                      rocsparse_error*            p_error);

HIPDSS_EXPORT
hipdssStatus_t
    hipdss_spequilibrate_descr_destroy(hipdssHandle_t             handle,
                                       hipdss_spequilibrate_descr descr,
                                       rocsparse_error*           p_error);

HIPDSS_EXPORT
hipdssStatus_t hipdss_spequilibrate_buffer_size(
    hipdssHandle_t handle, hipdss_spequilibrate_descr descr, rocsparse_const_spmat_descr A, rocsparse_const_dnvec_descr D_left, rocsparse_const_dnvec_descr D_right, hipdss_spequilibrate_stage stage, size_t* p_buffer_size_in_bytes, rocsparse_error* p_error);

HIPDSS_EXPORT
hipdssStatus_t hipdss_spequilibrate(
    hipdssHandle_t handle, hipdss_spequilibrate_descr descr, rocsparse_spmat_descr A, rocsparse_dnvec_descr D_left, rocsparse_dnvec_descr D_right, hipdss_spequilibrate_stage stage, size_t buffer_size_in_bytes, void* buffer, rocsparse_error* p_error);

#ifdef __cplusplus
}
#endif

#endif /* HIPDSS_SPEQUILIBRATE_H */
