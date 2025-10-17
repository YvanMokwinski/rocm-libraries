/*! \file */
/* ************************************************************************
 * Copyright (C) 2020-2025 Advanced Micro Devices, Inc. All rights Reserved.
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

#include "rocsparse_control.hpp"
#include "rocsparse_csrsm_info.hpp"
#include "rocsparse_datatype_utils.hpp"
#include "rocsparse_indextype_utils.hpp"
namespace rocsparse
{
    rocsparse_status csrsm_buffer_size(rocsparse_handle            handle,
                                       rocsparse_operation         trans_A,
                                       rocsparse_operation         trans_B,
                                       rocsparse_datatype          alpha_datatype,
                                       int64_t                     alpha_stride,
                                       rocsparse_const_spmat_descr A,
                                       rocsparse_const_dnmat_descr B,
                                       rocsparse_solve_policy      policy,
                                       size_t*                     buffer_size);

    rocsparse_status csrsm_analysis(rocsparse_handle            handle,
                                    rocsparse_operation         trans_A,
                                    rocsparse_operation         trans_B,
                                    rocsparse_datatype          alpha_datatype,
                                    int64_t                     alpha_stride,
                                    rocsparse_const_spmat_descr A,
                                    rocsparse_const_dnmat_descr B,
                                    rocsparse_analysis_policy   analysis,
                                    rocsparse_solve_policy      solve,
                                    rocsparse_csrsm_info*       p_csrsm_info,
                                    void*                       temp_buffer);

    rocsparse_status csrsm_solve(rocsparse_handle            handle,
                                 rocsparse_operation         trans_A,
                                 rocsparse_operation         trans_B,
                                 rocsparse_datatype          alpha_datatype,
                                 const void*                 alpha,
                                 int64_t                     alpha_stride,
                                 rocsparse_const_spmat_descr A,
                                 rocsparse_dnmat_descr       B,
                                 rocsparse_solve_policy      policy,
                                 rocsparse_csrsm_info        csrsm_info,
                                 void*                       temp_buffer);

}
