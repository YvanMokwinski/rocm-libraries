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

#include "rocsparse_csrsm.hpp"
#include "rocsparse_csrsm_strided_batched.hpp"
#include "rocsparse_utility.hpp"

rocsparse_status rocsparse::csrsm_solve(rocsparse_handle             handle,
                                        const rocsparse_operation    trans_A,
                                        const rocsparse_operation    trans_B,
                                        const int64_t                m,
                                        const int64_t                nrhs,
                                        const int64_t                nnz,
                                        const rocsparse_datatype     alpha_datatype,
                                        const void*                  alpha,
                                        const rocsparse_mat_descr    descr,
                                        const rocsparse_datatype     csr_val_datatype,
                                        const void*                  csr_val,
                                        const rocsparse_indextype    csr_row_ptr_indextype,
                                        const void*                  csr_row_ptr,
                                        const rocsparse_indextype    csr_col_ind_indextype,
                                        const void*                  csr_col_ind,
                                        const rocsparse_datatype     B_datatype,
                                        void*                        B,
                                        const int64_t                ldb,
                                        const rocsparse_order        order_B,
                                        const rocsparse_mat_info     info,
                                        const rocsparse_solve_policy policy,
                                        rocsparse_csrsm_info         csrsm_info,
                                        void*                        temp_buffer)
{
    ROCSPARSE_ROUTINE_TRACE;
    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_strided_batched_solve(handle,
                                                                     trans_A,
                                                                     trans_B,
                                                                     static_cast<int64_t>(1),
                                                                     m,
                                                                     nrhs,
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
                                                                     B_datatype,
                                                                     B,
                                                                     ldb,
                                                                     static_cast<int64_t>(0),
                                                                     order_B,
                                                                     info,
                                                                     policy,
                                                                     csrsm_info,
                                                                     temp_buffer));
    return rocsparse_status_success;
}
