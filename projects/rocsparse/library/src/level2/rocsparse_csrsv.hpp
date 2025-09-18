/*! \file */
/* ************************************************************************
 * Copyright (C) 2018-2025 Advanced Micro Devices, Inc. All rights Reserved.
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
#include "rocsparse_csrsv_strided_batched.hpp"
#include "rocsparse_datatype_utils.hpp"
#include "rocsparse_handle.hpp"
#include "rocsparse_indextype_utils.hpp"
#include "rocsparse_logging.hpp"
#include "rocsparse_utility.hpp"

namespace rocsparse
{

    rocsparse_status gtrm_analysis(rocsparse_handle          handle,
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
                                   rocsparse::trm_info_t*    info,
                                   void**                    zero_pivot,
                                   void*                     temp_buffer);

    rocsparse_status csrsv_buffer_size(rocsparse_handle          handle,
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
                                       size_t*                   buffer_size);

    rocsparse_status csrsv_analysis(rocsparse_handle          handle,
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
                                    void*                     temp_buffer);

    rocsparse_status csrsv_solve(rocsparse_handle          handle,
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
                                 void*                     temp_buffer);

    template <typename I, typename J, typename T>
    rocsparse_status trm_analysis(rocsparse_handle          handle,
                                  rocsparse_operation       trans,
                                  J                         m,
                                  I                         nnz,
                                  const rocsparse_mat_descr descr,
                                  const T*                  csr_val,
                                  const I*                  csr_row_ptr,
                                  const J*                  csr_col_ind,
                                  rocsparse::trm_info_t*    info,
                                  J**                       zero_pivot,
                                  void*                     temp_buffer)
    {
        RETURN_IF_ROCSPARSE_ERROR(gtrm_analysis(handle,
                                                trans,
                                                m,
                                                nnz,
                                                descr,
                                                rocsparse::get_datatype<T>(),
                                                csr_val,
                                                rocsparse::get_indextype<I>(),
                                                csr_row_ptr,
                                                rocsparse::get_indextype<J>(),
                                                csr_col_ind,
                                                info,
                                                (void**)zero_pivot,
                                                temp_buffer));
        return rocsparse_status_success;
    }

    template <typename I, typename J, typename T>
    rocsparse_status csrsv_analysis_template(rocsparse_handle          handle,
                                             rocsparse_operation       trans,
                                             J                         m,
                                             I                         nnz,
                                             const rocsparse_mat_descr descr,
                                             const T*                  csr_val,
                                             const I*                  csr_row_ptr,
                                             const J*                  csr_col_ind,
                                             rocsparse_mat_info        info,
                                             rocsparse_analysis_policy analysis,
                                             rocsparse_solve_policy    solve,
                                             void*                     temp_buffer)
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_analysis(handle,
                                                            trans,
                                                            m,
                                                            nnz,
                                                            descr,
                                                            rocsparse::get_datatype<T>(),
                                                            csr_val,
                                                            rocsparse::get_indextype<I>(),
                                                            csr_row_ptr,
                                                            rocsparse::get_indextype<J>(),
                                                            csr_col_ind,
                                                            info,
                                                            analysis,
                                                            solve,
                                                            temp_buffer));
        return rocsparse_status_success;
    }

    template <typename I, typename J, typename T>
    rocsparse_status csrsv_buffer_size_template(rocsparse_handle          handle,
                                                rocsparse_operation       trans,
                                                J                         m,
                                                I                         nnz,
                                                const rocsparse_mat_descr descr,
                                                const T*                  csr_val,
                                                const I*                  csr_row_ptr,
                                                const J*                  csr_col_ind,
                                                rocsparse_mat_info        info,
                                                size_t*                   buffer_size)
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_buffer_size(handle,
                                                               trans,
                                                               m,
                                                               nnz,
                                                               descr,
                                                               rocsparse::get_datatype<T>(),
                                                               csr_val,
                                                               rocsparse::get_indextype<I>(),
                                                               csr_row_ptr,
                                                               rocsparse::get_indextype<J>(),
                                                               csr_col_ind,
                                                               info,
                                                               buffer_size));
        return rocsparse_status_success;
    }

    template <typename I, typename J, typename T>
    rocsparse_status csrsv_solve_template(rocsparse_handle          handle,
                                          rocsparse_operation       trans,
                                          J                         m,
                                          I                         nnz,
                                          const T*                  alpha,
                                          const rocsparse_mat_descr descr,
                                          const T*                  csr_val,
                                          const I*                  csr_row_ptr,
                                          const J*                  csr_col_ind,
                                          rocsparse_mat_info        info,
                                          const T*                  x,
                                          int64_t                   x_inc,
                                          T*                        y,
                                          rocsparse_solve_policy    policy,
                                          void*                     temp_buffer)
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_solve(handle,
                                                         trans,
                                                         m,
                                                         nnz,
                                                         rocsparse::get_datatype<T>(),
                                                         alpha,
                                                         descr,
                                                         rocsparse::get_datatype<T>(),
                                                         csr_val,
                                                         rocsparse::get_indextype<I>(),
                                                         csr_row_ptr,
                                                         rocsparse::get_indextype<J>(),
                                                         csr_col_ind,
                                                         info,
                                                         rocsparse::get_datatype<T>(),
                                                         x,
                                                         (int64_t)1,
                                                         rocsparse::get_datatype<T>(),
                                                         y,
                                                         policy,
                                                         temp_buffer));
        return rocsparse_status_success;
    }
}
