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
#include "rocsparse_utility.hpp"

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */
namespace rocsparse
{
    static rocsparse_status csrsv_analysis_checkarg(rocsparse_handle          handle, // 0
                                                    rocsparse_operation       trans, // 1
                                                    rocsparse_int             m, // 2
                                                    rocsparse_int             nnz, // 3
                                                    const rocsparse_mat_descr descr, // 4
                                                    rocsparse_datatype        csr_val_datatype, // 5
                                                    const void*               csr_val, // 6
                                                    rocsparse_indextype csr_row_ptr_indextype, // 7
                                                    const void*         csr_row_ptr, // 8
                                                    rocsparse_indextype csr_col_ind_indextype, // 9
                                                    const void*         csr_col_ind, // 10
                                                    rocsparse_mat_info  info, // 11
                                                    rocsparse_analysis_policy analysis, // 12
                                                    rocsparse_solve_policy    solve, // 13
                                                    void*                     temp_buffer // 14
    )
    {
        ROCSPARSE_ROUTINE_TRACE;
        // Check for valid handle and matrix descriptor
        ROCSPARSE_CHECKARG_HANDLE(0, handle);
        ROCSPARSE_CHECKARG_ENUM(1, trans);
        ROCSPARSE_CHECKARG_SIZE(2, m);
        ROCSPARSE_CHECKARG_SIZE(3, nnz);
        ROCSPARSE_CHECKARG_POINTER(4, descr);

        ROCSPARSE_CHECKARG_ENUM(5, csr_val_datatype);
        ROCSPARSE_CHECKARG_ARRAY(6, nnz, csr_val);
        ROCSPARSE_CHECKARG_ENUM(7, csr_row_ptr_indextype);
        ROCSPARSE_CHECKARG_ARRAY(8, m, csr_row_ptr);
        ROCSPARSE_CHECKARG_ENUM(9, csr_col_ind_indextype);
        ROCSPARSE_CHECKARG_ARRAY(10, nnz, csr_col_ind);
        ROCSPARSE_CHECKARG_POINTER(11, info);
        ROCSPARSE_CHECKARG_ENUM(12, analysis);
        ROCSPARSE_CHECKARG_ENUM(13, solve);
        const rocsparse_int accept_temp_buffer = m;
        ROCSPARSE_CHECKARG_ARRAY(14, accept_temp_buffer, temp_buffer);
        return rocsparse_status_success;
    }
}

rocsparse_status rocsparse::csrsv_analysis(rocsparse_handle          handle,
                                           rocsparse_operation       trans,
                                           int64_t                   m,
                                           int64_t                   nnz,
                                           const rocsparse_mat_descr descr,
                                           rocsparse_datatype        csr_val_datatype,
                                           const void*               csr_val,
                                           rocsparse_indextype       csr_row_ptr_indextype,
                                           const void*               csr_row_ptr,
                                           rocsparse_indextype       csr_col_ind_indextype,
                                           const void*               csr_col_ind,
                                           rocsparse_mat_info        info,
                                           rocsparse_analysis_policy analysis,
                                           rocsparse_solve_policy    solve,
                                           rocsparse_csrsv_info*     p_csrsv_info,
                                           void*                     temp_buffer)
{
    ROCSPARSE_ROUTINE_TRACE;

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_analysis_checkarg(handle,
                                                                 trans,
                                                                 m,
                                                                 nnz,
                                                                 descr,
                                                                 csr_val_datatype,
                                                                 csr_val,
                                                                 csr_row_ptr_indextype,
                                                                 csr_row_ptr,
                                                                 csr_col_ind_indextype,
                                                                 csr_col_ind,
                                                                 info,
                                                                 analysis,
                                                                 solve,
                                                                 temp_buffer));
    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_strided_batched_analysis(handle,
                                                                        trans,
                                                                        static_cast<int64_t>(1),
                                                                        m,
                                                                        nnz,
                                                                        descr,
                                                                        csr_val_datatype,
                                                                        csr_val,
                                                                        static_cast<int64_t>(0),
                                                                        csr_row_ptr_indextype,
                                                                        csr_row_ptr,
                                                                        csr_col_ind_indextype,
                                                                        csr_col_ind,
                                                                        info,
                                                                        analysis,
                                                                        solve,
                                                                        p_csrsv_info,
                                                                        temp_buffer));

    return rocsparse_status_success;
}
