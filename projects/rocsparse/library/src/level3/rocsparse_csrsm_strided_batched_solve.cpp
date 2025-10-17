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
#include "internal/level3/rocsparse_csrsm_strided_batched.h"
#include "rocsparse_csrsm_strided_batched.hpp"
#include "rocsparse_utility.hpp"

namespace rocsparse
{
    rocsparse_status
        xcsrsm_strided_batched_solve_checkarg(rocsparse_handle          handle, // 0
                                              const rocsparse_operation trans_A, // 1
                                              const rocsparse_operation trans_B, // 2
                                              const int64_t             batch_count, // 3
                                              const int64_t             m, // 4
                                              const int64_t             nrhs, // 5
                                              const int64_t             nnz, // 6
                                              const rocsparse_datatype  alpha_datatype, // 7
                                              const void*               alpha, // 8
                                              const int64_t             alpha_stride, // 9
                                              const rocsparse_mat_descr descr, // 10
                                              const rocsparse_datatype  csr_val_datatype, // 11
                                              const void*               csr_val, // 12
                                              const int64_t             csr_val_stride, // 13
                                              const rocsparse_indextype csr_row_ptr_indextype, // 14
                                              const void*               csr_row_ptr, // 15
                                              const rocsparse_indextype csr_col_ind_indextype, // 16
                                              const void*               csr_col_ind, // 17
                                              const rocsparse_datatype  B_datatype, // 18
                                              void*                     B, // 19
                                              const int64_t             B_ld, // 20
                                              const int64_t             B_stride, // 21
                                              rocsparse_order           B_order, //22
                                              const rocsparse_mat_info  info, // 23
                                              const rocsparse_solve_policy policy, // 24
                                              void*                        temp_buffer // 25
        )
    {
        ROCSPARSE_ROUTINE_TRACE;
        ROCSPARSE_CHECKARG_HANDLE(0, handle);
        ROCSPARSE_CHECKARG_ENUM(1, trans_A);
        ROCSPARSE_CHECKARG_ENUM(2, trans_B);
        ROCSPARSE_CHECKARG_SIZE(3, batch_count);
        ROCSPARSE_CHECKARG_SIZE(4, m);
        ROCSPARSE_CHECKARG_SIZE(5, nrhs);
        ROCSPARSE_CHECKARG_SIZE(6, nnz);
        ROCSPARSE_CHECKARG_ENUM(7, alpha_datatype);
        ROCSPARSE_CHECKARG_POINTER(8, alpha);
        // (9,alpha_stride);
        ROCSPARSE_CHECKARG_POINTER(10, descr);
        ROCSPARSE_CHECKARG_ENUM(11, csr_val_datatype);
        ROCSPARSE_CHECKARG_ARRAY(12, nnz, csr_val);
        // (13,csr_val_stride);
        ROCSPARSE_CHECKARG_ENUM(14, csr_row_ptr_indextype);
        ROCSPARSE_CHECKARG_ARRAY(15, m, csr_row_ptr);
        ROCSPARSE_CHECKARG_ENUM(16, csr_col_ind_indextype);
        ROCSPARSE_CHECKARG_ARRAY(17, nnz, csr_col_ind);
        ROCSPARSE_CHECKARG_ENUM(18, B_datatype);
        ROCSPARSE_CHECKARG_POINTER(19, B);
        // (20,B_ld);
        // (21,B_stride);
        ROCSPARSE_CHECKARG_ENUM(22, B_order);
        ROCSPARSE_CHECKARG_POINTER(23, info);
        ROCSPARSE_CHECKARG_ENUM(24, policy);
        ROCSPARSE_CHECKARG_POINTER(25, temp_buffer);
        return rocsparse_status_success;
    }
}

extern "C" rocsparse_status
    rocsparse_csrsm_strided_batched_solve(rocsparse_handle          handle,
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
                                          int64_t                   B_ld,
                                          int64_t                   B_stride,
                                          rocsparse_order           B_order,
                                          rocsparse_mat_info        info,
                                          rocsparse_solve_policy    policy,
                                          size_t                    buffer_size_in_bytes,
                                          void*                     buffer,
                                          rocsparse_error*          p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;
    RETURN_IF_ROCSPARSE_ERROR(
        rocsparse::xcsrsm_strided_batched_solve_checkarg(handle,
                                                         trans_A,
                                                         trans_B,
                                                         batch_count,
                                                         m,
                                                         nrhs,
                                                         nnz,
                                                         alpha_datatype,
                                                         alpha,
                                                         alpha_stride,
                                                         descr,
                                                         csr_val_datatype,
                                                         csr_val,
                                                         csr_val_stride,
                                                         csr_row_ptr_indextype,
                                                         csr_row_ptr,
                                                         csr_col_ind_indextype,
                                                         csr_col_ind,
                                                         B_datatype,
                                                         B,
                                                         B_ld,
                                                         B_stride,
                                                         B_order,
                                                         info,
                                                         policy,
                                                         buffer));

    rocsparse_csrsm_info csrsm_info = info->get_csrsm_info();
    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_strided_batched_solve(handle,
                                                                     trans_A,
                                                                     trans_B,
                                                                     batch_count,
                                                                     m,
                                                                     nrhs,
                                                                     nnz,
                                                                     alpha_datatype,
                                                                     alpha,
                                                                     alpha_stride,
                                                                     descr,
                                                                     csr_val_datatype,
                                                                     csr_val,
                                                                     csr_val_stride,
                                                                     csr_row_ptr_indextype,
                                                                     csr_row_ptr,
                                                                     csr_col_ind_indextype,
                                                                     csr_col_ind,
                                                                     B_datatype,
                                                                     B,
                                                                     B_ld,
                                                                     B_stride,
                                                                     B_order,
                                                                     info,
                                                                     policy,
                                                                     csrsm_info,
                                                                     buffer));
    return rocsparse_status_success;
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
