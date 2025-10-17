/* ************************************************************************
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights Reserved.
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
#pragma once
#include "rocsparse-types.h"

extern "C" {
typedef struct _rocsparse_spic0_descr* rocsparse_spic0_descr;

typedef enum _rocsparse_spic0_alg
{
    rocsparse_spic0_alg_default
} rocsparse_spic0_alg;

typedef enum _rocsparse_spic0_stage
{
    rocsparse_spic0_stage_analysis,
    rocsparse_spic0_stage_compute
} rocsparse_spic0_stage;

typedef enum _rocsparse_spic0_input
{
    rocsparse_spic0_input_alg,
    rocsparse_spic0_input_analysis_policy,
    rocsparse_spic0_input_compute_datatype,
    rocsparse_spic0_input_boost_enable,
    rocsparse_spic0_input_singular_tol,
} rocsparse_spic0_input;

typedef enum _rocsparse_spic0_input_data
{
    rocsparse_spic0_input_data_boost_tol,
    rocsparse_spic0_input_data_boost_val,
} rocsparse_spic0_input_data;

typedef enum _rocsparse_spic0_output
{
    rocsparse_spic0_output_singular_pivot
} rocsparse_spic0_output;

ROCSPARSE_EXPORT
rocsparse_status rocsparse_create_spic0_descr(rocsparse_handle       handle,
                                              rocsparse_spic0_descr* p_spic0_descr,
                                              rocsparse_error*       p_error);
ROCSPARSE_EXPORT
rocsparse_status rocsparse_destroy_spic0_descr(rocsparse_handle      handle,
                                               rocsparse_spic0_descr spic0_descr,
                                               rocsparse_error*      p_error);

ROCSPARSE_EXPORT
rocsparse_status rocsparse_spic0_set_input_data(rocsparse_handle           handle,
                                                rocsparse_spic0_descr      spic0_descr,
                                                rocsparse_spic0_input_data spic0_input_data,
                                                const void*                input,
                                                size_t                     input_size_in_bytes,
                                                rocsparse_error*           p_error);

ROCSPARSE_EXPORT
rocsparse_status rocsparse_spic0_set_input(rocsparse_handle      handle,
                                           rocsparse_spic0_descr spic0_descr,
                                           rocsparse_spic0_input spic0_input,
                                           const void*           input,
                                           size_t                input_size_in_bytes,
                                           rocsparse_error*      p_error);

ROCSPARSE_EXPORT
rocsparse_status rocsparse_spic0_get_output(rocsparse_handle       handle,
                                            rocsparse_spic0_descr  spic0_descr,
                                            rocsparse_spic0_output spic0_output,
                                            void*                  output,
                                            size_t                 output_size_in_bytes,
                                            rocsparse_error*       p_error);

ROCSPARSE_EXPORT
rocsparse_status rocsparse_spic0_buffer_size(rocsparse_handle            handle,
                                             rocsparse_spic0_descr       spic0_descr,
                                             rocsparse_const_spmat_descr A,
                                             rocsparse_spic0_stage       spic0_stage,
                                             size_t*                     p_buffer_size_in_bytes,
                                             rocsparse_error*            p_error);

ROCSPARSE_EXPORT
rocsparse_status rocsparse_spic0(rocsparse_handle      handle,
                                 rocsparse_spic0_descr spic0_descr,
                                 rocsparse_spmat_descr A,
                                 rocsparse_spic0_stage spic0_stage,
                                 size_t                buffer_size_in_bytes,
                                 void*                 buffer,
                                 rocsparse_error*      p_error);
}
