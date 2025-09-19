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
#include "rocsparse_datatype_utils.hpp"
#include "rocsparse_indextype_utils.hpp"

namespace rocsparse
{
    rocsparse_status csrsm_strided_batched_buffer_size(rocsparse_handle          handle,
                                                       rocsparse_operation       trans_A,
                                                       rocsparse_operation       trans_B,
                                                       int64_t                   batch_count,
                                                       int64_t                   m,
                                                       int64_t                   nrhs,
                                                       int64_t                   nnz,
                                                       rocsparse_datatype        alpha_datatype,
                                                       const void*               alpha,
                                                       int64_t                   alpha_stride,
                                                       const rocsparse_mat_descr descr,
                                                       rocsparse_datatype        csr_val_datatype,
                                                       const void*               csr_val,
                                                       int64_t                   csr_val_stride,
                                                       rocsparse_indextype    csr_row_ptr_indextype,
                                                       const void*            csr_row_ptr,
                                                       rocsparse_indextype    csr_col_ind_indextype,
                                                       const void*            csr_col_ind,
                                                       rocsparse_datatype     B_datatype,
                                                       const void*            B,
                                                       int64_t                ldb,
                                                       int64_t                B_stride,
                                                       rocsparse_order        order_B,
                                                       rocsparse_mat_info     info,
                                                       rocsparse_solve_policy policy,
                                                       size_t*                buffer_size);

    rocsparse_status csrsm_strided_batched_analysis(rocsparse_handle          handle,
                                                    rocsparse_operation       trans_A,
                                                    rocsparse_operation       trans_B,
                                                    int64_t                   batch_count,
                                                    int64_t                   m,
                                                    int64_t                   nrhs,
                                                    int64_t                   nnz,
                                                    rocsparse_datatype        alpha_datatype,
                                                    const void*               alpha,
                                                    int64_t                   alpha_stride,
                                                    const rocsparse_mat_descr descr,
                                                    rocsparse_datatype        csr_val_datatype,
                                                    const void*               csr_val,
                                                    int64_t                   csr_val_stride,
                                                    rocsparse_indextype       csr_row_ptr_indextype,
                                                    const void*               csr_row_ptr,
                                                    rocsparse_indextype       csr_col_ind_indextype,
                                                    const void*               csr_col_ind,
                                                    rocsparse_datatype        B_datatype,
                                                    const void*               B,
                                                    int64_t                   ldb,
                                                    int64_t                   B_stride,
                                                    rocsparse_order           order_B,
                                                    rocsparse_mat_info        info,
                                                    rocsparse_analysis_policy analysis,
                                                    rocsparse_solve_policy    solve,
                                                    rocsparse_csrsm_info*     p_csrsm_info,
                                                    void*                     temp_buffer);

    rocsparse_status csrsm_strided_batched_solve(rocsparse_handle          handle,
                                                 rocsparse_operation       trans_A,
                                                 rocsparse_operation       trans_B,
                                                 int64_t                   batch_count,
                                                 int64_t                   m,
                                                 int64_t                   nrhs,
                                                 int64_t                   nnz,
                                                 rocsparse_datatype        alpha_datatype,
                                                 const void*               alpha,
                                                 int64_t                   alpha_stride,
                                                 const rocsparse_mat_descr descr,
                                                 rocsparse_datatype        csr_val_datatype,
                                                 const void*               csr_val,
                                                 int64_t                   csr_val_stride,
                                                 rocsparse_indextype       csr_row_ptr_indextype,
                                                 const void*               csr_row_ptr,
                                                 rocsparse_indextype       csr_col_ind_indextype,
                                                 const void*               csr_col_ind,
                                                 rocsparse_datatype        B_datatype,
                                                 void*                     B,
                                                 int64_t                   ldb,
                                                 int64_t                   B_stride,
                                                 rocsparse_order           order_B,
                                                 const rocsparse_mat_info  info,
                                                 rocsparse_csrsm_info      csrsm_info,
                                                 rocsparse_solve_policy    policy,
                                                 void*                     temp_buffer);
}
