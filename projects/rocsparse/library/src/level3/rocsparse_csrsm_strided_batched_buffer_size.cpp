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
    static rocsparse_status
        xcsrsm_strided_batched_buffer_size_checkarg(rocsparse_handle          handle,
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
                                                    rocsparse_order           B_order,
                                                    rocsparse_mat_info        info,
                                                    rocsparse_solve_policy    policy,
                                                    size_t*                   buffer_size)
    {
        ROCSPARSE_ROUTINE_TRACE;
        return rocsparse_status_success;
    }

}

extern "C"

    rocsparse_status
    rocsparse_csrsm_strided_batched_buffer_size(rocsparse_handle          handle,
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
                                                rocsparse_order           B_order,
                                                rocsparse_mat_info        info,
                                                rocsparse_solve_policy    policy,
                                                size_t*                   buffer_size,
                                                rocsparse_error*          p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;
    RETURN_IF_ROCSPARSE_ERROR(
        rocsparse::xcsrsm_strided_batched_buffer_size_checkarg(handle,
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
                                                               ldb,
                                                               B_stride,
                                                               B_order,
                                                               info,
                                                               policy,
                                                               buffer_size));

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_strided_batched_buffer_size(handle,
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
                                                                           ldb,
                                                                           B_stride,
                                                                           B_order,
                                                                           info,
                                                                           policy,
                                                                           buffer_size));
    return rocsparse_status_success;
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}

#if 0

  template<typename T>
  rocsparse_status xcsrsm_strided_batched_buffer_size(rocsparse_handle          handle,
						     rocsparse_operation       trans_A,
						     rocsparse_operation       trans_B,
						     int64_t                         batch_count,
						     int64_t                         m,
						     int64_t                         nrhs,
						     int64_t                         nnz,
						     rocsparse_datatype  alpha_datatype,
						     const void*                  alpha,
						     int64_t alpha_stride,
						     const rocsparse_mat_descr descr,
						     rocsparse_datatype csr_val_datatype,
						     const void*                  csr_val,
						     int64_t csr_val_stride,
						     rocsparse_indextype csr_row_ptr_indextype,
						     const void*                  csr_row_ptr,
						     rocsparse_indextype csr_col_ind_indextype,
						     const void*                  csr_col_ind,
						     rocsparse_datatype B_datatype,
						     const void*                  B,
						     int64_t                   ldb,
						     int64_t B_stride,
						     rocsparse_order           order_B,
						     rocsparse_mat_info        info,
						     rocsparse_solve_policy    policy,
						     size_t*                   buffer_size)
  {
      ROCSPARSE_ROUTINE_TRACE;
      const rocsparse_datatype datatype = rocsparse::get_datatype<T>();
      RETURN_IF_ROCSPARSE_ERROR(rocsparse::xcsrsm_strided_batched_buffer_size_checkarg(handle,
										       trans_A,
										       trans_B,
										       batch_count,
										       m,
										       nrhs,
										       nnz,
										       alpha,
										       alpha_stride,
										       descr,
										       csr_val,
										       csr_val_stride,
										       csr_row_ptr,
										       csr_col_ind,
										       B,
										       ldb,
										       B_stride,
										       order_B,
										       info,
										       policy,
										       buffer_size));

      RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_strided_batched_buffer_size(handle,
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
									     ldb,
									     B_stride,
									     order_B,
									     info,
									     policy,
									     buffer_size));
      return rocsparse_status_success;
    }

}

#define CIMPL(NAME, T)                                                                  \
    rocsparse_status NAME(rocsparse_handle             handle,                          \
                          rocsparse_operation          trans_A,                         \
                          rocsparse_operation          trans_B,                         \
                          rocsparse_int                batch_count,                     \
                          rocsparse_int                m,                               \
                          rocsparse_int                nrhs,                            \
                          rocsparse_int                nnz,                             \
                          const T*                     alpha,                           \
                          int64_t                      alpha_stride,                    \
                          const rocsparse_mat_descr    descr,                           \
                          const T*                     csr_val,                         \
                          int64_t                      csr_val_stride,                  \
                          const rocsparse_int*         csr_row_ptr,                     \
                          const rocsparse_int*         csr_col_ind,                     \
                          const rocsparse_mat_info     info,                            \
                          const rocsparse_solve_policy policy,                          \
                          void*                        buffer_size)                     \
    try                                                                                 \
    {                                                                                   \
        RETURN_IF_ROCSPARSE_ERROR(xcsrsm_strided_batched_buffer_size<T>(handle,         \
                                                                        trans_A,        \
                                                                        trans_B,        \
                                                                        batch_count,    \
                                                                        m,              \
                                                                        nrhs,           \
                                                                        nnz,            \
                                                                        alpha,          \
                                                                        alpha_stride,   \
                                                                        descr,          \
                                                                        csr_val,        \
                                                                        csr_val_stride, \
                                                                        csr_row_ptr,    \
                                                                        csr_col_ind,    \
                                                                        info,           \
                                                                        policy,         \
                                                                        temp_buffer));  \
        return rocsparse_status_success;                                                \
    }                                                                                   \
    catch(...)                                                                          \
    {                                                                                   \
        RETURN_ROCSPARSE_EXCEPTION();                                                   \
    }

CIMPL(rocsparse_scsrsm_strided_batched_buffer_size,float);
CIMPL(rocsparse_dcsrsm_strided_batched_buffer_size,double);
CIMPL(rocsparse_ccsrsm_strided_batched_buffer_size,rocsparse_float_complex);
CIMPL(rocsparse_zcsrsm_strided_batched_buffer_size,rocsparse_double_complex);
#undef CIMPL

#endif
