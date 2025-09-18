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

#include "rocsparse_csrsm.hpp"
#include "rocsparse_datatype_utils.hpp"
#include "rocsparse_indextype_utils.hpp"
#include "rocsparse_utility.hpp"

namespace rocsparse
{
    template <typename I, typename J, typename T>
    inline rocsparse_status csrsm_solve_template(rocsparse_handle          handle,
                                                 rocsparse_operation       trans_A,
                                                 rocsparse_operation       trans_B,
                                                 int64_t                   m,
                                                 int64_t                   nrhs,
                                                 int64_t                   nnz,
                                                 const T*                  alpha,
                                                 const rocsparse_mat_descr descr,
                                                 const T*                  csr_val,
                                                 const I*                  csr_row_ptr,
                                                 const J*                  csr_col_ind,
                                                 T*                        B,
                                                 int64_t                   ldb,
                                                 rocsparse_order           order_B,
                                                 const rocsparse_mat_info  info,
                                                 rocsparse_solve_policy    policy,
                                                 void*                     temp_buffer)
    {
        static const rocsparse_datatype  alpha_datatype        = rocsparse::get_datatype<T>();
        static const rocsparse_datatype  csr_val_datatype      = rocsparse::get_datatype<T>();
        static const rocsparse_datatype  B_datatype            = rocsparse::get_datatype<T>();
        static const rocsparse_indextype csr_row_ptr_indextype = rocsparse::get_indextype<I>();
        static const rocsparse_indextype csr_col_ind_indextype = rocsparse::get_indextype<I>();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_solve(handle,
                                                         trans_A,
                                                         trans_B,
                                                         m,
                                                         nrhs,
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
                                                         B_datatype,
                                                         B,
                                                         ldb,
                                                         order_B,
                                                         info,
                                                         policy,
                                                         temp_buffer));
        return rocsparse_status_success;
    }

    template <typename I, typename J, typename T>
    inline rocsparse_status csrsm_buffer_size_template(rocsparse_handle          handle,
                                                       rocsparse_operation       trans_A,
                                                       rocsparse_operation       trans_B,
                                                       int64_t                   m,
                                                       int64_t                   nrhs,
                                                       int64_t                   nnz,
                                                       const T*                  alpha,
                                                       const rocsparse_mat_descr descr,
                                                       const T*                  csr_val,
                                                       const I*                  csr_row_ptr,
                                                       const J*                  csr_col_ind,
                                                       const T*                  B,
                                                       int64_t                   ldb,
                                                       rocsparse_order           order_B,
                                                       rocsparse_mat_info        info,
                                                       rocsparse_solve_policy    policy,
                                                       size_t*                   buffer_size)
    {
        static const rocsparse_datatype  alpha_datatype        = rocsparse::get_datatype<T>();
        static const rocsparse_datatype  csr_val_datatype      = rocsparse::get_datatype<T>();
        static const rocsparse_datatype  B_datatype            = rocsparse::get_datatype<T>();
        static const rocsparse_indextype csr_row_ptr_indextype = rocsparse::get_indextype<I>();
        static const rocsparse_indextype csr_col_ind_indextype = rocsparse::get_indextype<I>();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_buffer_size(handle,
                                                               trans_A,
                                                               trans_B,
                                                               m,
                                                               nrhs,
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
                                                               B_datatype,
                                                               B,
                                                               ldb,
                                                               order_B,
                                                               info,
                                                               policy,
                                                               buffer_size));

        return rocsparse_status_success;
    }

    template <typename I, typename J, typename T>
    inline rocsparse_status csrsm_analysis_template(rocsparse_handle          handle,
                                                    rocsparse_operation       trans_A,
                                                    rocsparse_operation       trans_B,
                                                    int64_t                   m,
                                                    int64_t                   nrhs,
                                                    int64_t                   nnz,
                                                    const T*                  alpha,
                                                    const rocsparse_mat_descr descr,
                                                    const T*                  csr_val,
                                                    const I*                  csr_row_ptr,
                                                    const J*                  csr_col_ind,
                                                    const T*                  B,
                                                    int64_t                   ldb,
                                                    rocsparse_mat_info        info,
                                                    rocsparse_analysis_policy analysis,
                                                    rocsparse_solve_policy    solve,
                                                    void*                     temp_buffer)
    {
        static const rocsparse_datatype  alpha_datatype        = rocsparse::get_datatype<T>();
        static const rocsparse_datatype  csr_val_datatype      = rocsparse::get_datatype<T>();
        static const rocsparse_datatype  B_datatype            = rocsparse::get_datatype<T>();
        static const rocsparse_indextype csr_row_ptr_indextype = rocsparse::get_indextype<I>();
        static const rocsparse_indextype csr_col_ind_indextype = rocsparse::get_indextype<I>();
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_analysis(handle,
                                                            trans_A,
                                                            trans_B,
                                                            m,
                                                            nrhs,
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
                                                            B_datatype,
                                                            B,
                                                            ldb,
                                                            rocsparse_order_column,
                                                            info,
                                                            analysis,
                                                            solve,
                                                            temp_buffer));
        return rocsparse_status_success;
    }
}
