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

#include "internal/level2/rocsparse_csrsv.h"
#include "rocsparse_control.hpp"
#include "rocsparse_csrsv.hpp"
#include "rocsparse_csrsv_strided_batched.hpp"

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */
namespace rocsparse
{

    static rocsparse_status csrsv_solve_checkarg(rocsparse_handle          handle, // 0
                                                 rocsparse_operation       trans, // 1
                                                 int64_t                   m, // 2
                                                 int64_t                   nnz, // 3
                                                 rocsparse_datatype        alpha_datatype, // 4
                                                 const void*               alpha, // 5
                                                 const rocsparse_mat_descr descr, // 6
                                                 rocsparse_datatype        csr_val_datatype, // 7
                                                 const void*               csr_val, // 8
                                                 rocsparse_indextype    csr_row_ptr_indextype, // 9
                                                 const void*            csr_row_ptr, // 10
                                                 rocsparse_indextype    csr_col_ind_indextype, // 11
                                                 const void*            csr_col_ind, // 12
                                                 rocsparse_mat_info     info, // 13
                                                 rocsparse_datatype     x_datatype, // 14
                                                 const void*            x, // 15
                                                 int64_t                x_inc,
                                                 rocsparse_datatype     y_datatype, // 17
                                                 void*                  y, // 18
                                                 rocsparse_solve_policy policy, // 19
                                                 void*                  temp_buffer // 20
    )
    {
        ROCSPARSE_ROUTINE_TRACE;
        // Check for valid handle and matrix descriptor
        ROCSPARSE_CHECKARG_HANDLE(0, handle);
        ROCSPARSE_CHECKARG_ENUM(1, trans);
        ROCSPARSE_CHECKARG_SIZE(2, m);
        ROCSPARSE_CHECKARG_SIZE(3, nnz);
        ROCSPARSE_CHECKARG_ENUM(4, alpha_datatype);
        ROCSPARSE_CHECKARG_POINTER(5, alpha);
        ROCSPARSE_CHECKARG_POINTER(6, descr);
        ROCSPARSE_CHECKARG_ENUM(7, csr_val_datatype);
        ROCSPARSE_CHECKARG_ARRAY(8, nnz, csr_val);
        ROCSPARSE_CHECKARG_ENUM(9, csr_row_ptr_indextype);
        ROCSPARSE_CHECKARG_ARRAY(10, m, csr_row_ptr);
        ROCSPARSE_CHECKARG_ENUM(11, csr_col_ind_indextype);
        ROCSPARSE_CHECKARG_ARRAY(12, nnz, csr_col_ind);
        ROCSPARSE_CHECKARG_POINTER(13, info);
        ROCSPARSE_CHECKARG_ENUM(14, x_datatype);
        ROCSPARSE_CHECKARG_ARRAY(15, m, x);
        ROCSPARSE_CHECKARG_ENUM(17, y_datatype);
        ROCSPARSE_CHECKARG_ARRAY(18, m, y);
        ROCSPARSE_CHECKARG_ENUM(19, policy);
        ROCSPARSE_CHECKARG_POINTER(20, temp_buffer);
        return rocsparse_status_success;
    }

}

rocsparse_status rocsparse::csrsv_solve(rocsparse_handle          handle,
                                        rocsparse_operation       trans,
                                        int64_t                   m,
                                        int64_t                   nnz,
                                        rocsparse_datatype        alpha_datatype,
                                        const void*               alpha,
                                        const rocsparse_mat_descr descr,
                                        rocsparse_datatype        csr_val_datatype,
                                        const void*               csr_val,
                                        rocsparse_indextype       csr_row_ptr_indextype,
                                        const void*               csr_row_ptr,
                                        rocsparse_indextype       csr_col_ind_indextype,
                                        const void*               csr_col_ind,
                                        rocsparse_mat_info        info,
                                        rocsparse_datatype        x_datatype,
                                        const void*               x,
                                        int64_t                   x_inc,
                                        rocsparse_datatype        y_datatype,
                                        void*                     y,
                                        rocsparse_solve_policy    policy,
                                        rocsparse_csrsv_info      csrsv_info,
                                        void*                     temp_buffer)
{
    ROCSPARSE_ROUTINE_TRACE;

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_solve_checkarg(handle,
                                                              trans,
                                                              m,
                                                              nnz,
                                                              alpha_datatype,
                                                              alpha,
                                                              descr,
                                                              csr_val_datatype,
                                                              csr_val,
                                                              csr_row_ptr_indextype,
                                                              csr_row_ptr,
                                                              csr_col_ind_indextype,
                                                              csr_col_ind,
                                                              info,
                                                              x_datatype,
                                                              x,
                                                              x_inc,
                                                              y_datatype,
                                                              y,
                                                              policy,
                                                              temp_buffer));

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_strided_batched_solve(handle,
                                                                     trans,
                                                                     static_cast<int64_t>(1),
                                                                     m,
                                                                     nnz,
                                                                     alpha_datatype,
                                                                     alpha,
                                                                     static_cast<int64_t>(0),
                                                                     descr,
                                                                     csr_val_datatype,
                                                                     csr_val,
                                                                     static_cast<int64_t>(0),
                                                                     csr_row_ptr_indextype,
                                                                     csr_row_ptr,
                                                                     csr_col_ind_indextype,
                                                                     csr_col_ind,
                                                                     info,
                                                                     x_datatype,
                                                                     x,
                                                                     x_inc,
                                                                     static_cast<int64_t>(0),
                                                                     y_datatype,
                                                                     y,
                                                                     static_cast<int64_t>(0),
                                                                     policy,
                                                                     csrsv_info,
                                                                     temp_buffer));

    return rocsparse_status_success;
}
