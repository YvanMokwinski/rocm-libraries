/*! \file */
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

#include "rocsparse_control.hpp"
#include "rocsparse_datatype_utils.hpp"
#include "rocsparse_indextype_utils.hpp"

namespace rocsparse
{

    rocsparse_status csrsv_strided_batched_buffer_size(rocsparse_handle          handle,
                                                       rocsparse_operation       trans,
                                                       int64_t                   batch_count,
                                                       int64_t                   m,
                                                       int64_t                   nnz,
                                                       const rocsparse_mat_descr descr,
                                                       rocsparse_datatype        csr_val_datatype,
                                                       const void*               csr_val,
                                                       int64_t                   csr_val_stride,
                                                       rocsparse_indextype csr_row_ptr_indextype,
                                                       const void*         csr_row_ptr,
                                                       rocsparse_indextype csr_col_ind_indextype,
                                                       const void*         csr_col_ind,
                                                       rocsparse_mat_info  info,
                                                       size_t*             buffer_size);

    rocsparse_status csrsv_strided_batched_analysis(rocsparse_handle          handle,
                                                    rocsparse_operation       trans,
                                                    int64_t                   batch_count,
                                                    int64_t                   m,
                                                    int64_t                   nnz,
                                                    const rocsparse_mat_descr descr,
                                                    rocsparse_datatype        csr_val_datatype,
                                                    const void*               csr_val,
                                                    int64_t                   csr_val_stride,
                                                    rocsparse_indextype       csr_row_ptr_indextype,
                                                    const void*               csr_row_ptr,
                                                    rocsparse_indextype       csr_col_ind_indextype,
                                                    const void*               csr_col_ind,
                                                    rocsparse_mat_info        info,
                                                    rocsparse_analysis_policy analysis_policy,
                                                    rocsparse_solve_policy    solve_policy,
                                                    void*                     temp_buffer);

    rocsparse_status csrsv_strided_batched_solve(rocsparse_handle          handle,
                                                 rocsparse_operation       trans,
                                                 int64_t                   batch_count,
                                                 int64_t                   m,
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
                                                 rocsparse_mat_info        info,
                                                 rocsparse_datatype        x_datatype,
                                                 const void*               x,
                                                 int64_t                   x_inc,
                                                 int64_t                   x_stride,
                                                 rocsparse_datatype        y_datatype,
                                                 void*                     y,
                                                 int64_t                   y_stride,
                                                 rocsparse_solve_policy    policy,
                                                 void*                     temp_buffer);

    rocsparse_status launch_csrsv_analysis_kernel(rocsparse_handle    handle,
                                                  rocsparse_operation trans,
                                                  int64_t             m,
                                                  rocsparse_indextype csr_row_ptr_indextype,
                                                  const void* __restrict__ csr_row_ptr,
                                                  rocsparse_indextype csr_col_ind_indextype,
                                                  const void* __restrict__ csr_col_ind,
                                                  rocsparse_indextype csr_diag_ind_indextype,
                                                  void* __restrict__ csr_diag_ind,
                                                  int32_t* __restrict__ done_array,
                                                  void* __restrict__ max_nnz,
                                                  void* __restrict__ zero_pivot,
                                                  rocsparse_index_base idx_base,
                                                  rocsparse_diag_type  diag_type,
                                                  rocsparse_fill_mode  mode);

}
