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
#include "internal/level2/rocsparse_csrsv_strided_batched.h"

#include "rocsparse_csrsv.hpp"

#include "../conversion/rocsparse_coo2csr.hpp"
#include "../conversion/rocsparse_csr2coo.hpp"
#include "../conversion/rocsparse_identity.hpp"
#include "../level1/rocsparse_gthr.hpp"
#include "csrsv_device.h"
#include "rocsparse_assign_async.hpp"
#include "rocsparse_common.h"
#include "rocsparse_control.hpp"
#include "rocsparse_csrsv_strided_batched.hpp"
#include "rocsparse_utility.hpp"

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */
namespace rocsparse_csrsv_strided_batched
{
    static rocsparse_status solve_checkarg(rocsparse_handle          handle,
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
                                           void*                     temp_buffer) //14
    {
        ROCSPARSE_ROUTINE_TRACE;
        ROCSPARSE_CHECKARG_HANDLE(0, handle);
        ROCSPARSE_CHECKARG_ENUM(1, trans);
        ROCSPARSE_CHECKARG_SIZE(2, batch_count);
        ROCSPARSE_CHECKARG_SIZE(3, m);
        ROCSPARSE_CHECKARG_SIZE(4, nnz);
        ROCSPARSE_CHECKARG_POINTER(7, descr);
        ROCSPARSE_CHECKARG_ARRAY(8, nnz * batch_count, csr_val);
        ROCSPARSE_CHECKARG_ARRAY(10, m, csr_row_ptr);
        ROCSPARSE_CHECKARG_ARRAY(11, nnz, csr_col_ind);
        ROCSPARSE_CHECKARG_POINTER(12, info);
        ROCSPARSE_CHECKARG_ARRAY(13, m * batch_count, x);
        ROCSPARSE_CHECKARG_ARRAY(15, m * batch_count, y);
        ROCSPARSE_CHECKARG_ENUM(17, policy);
        ROCSPARSE_CHECKARG_ARRAY(
            18,
            m * batch_count,
            temp_buffer); // trick since quick return is m == 0, these are ignored
        ROCSPARSE_CHECKARG_ARRAY(5,
                                 m * batch_count,
                                 alpha); // trick since quick return is m == 0, these are ignored

        return rocsparse_status_continue;
    }

    template <typename... P>
    static rocsparse_status solve(P... p)
    {
        ROCSPARSE_ROUTINE_TRACE;

        const rocsparse_status status = rocsparse_csrsv_strided_batched::solve_checkarg(p...);
        if(status != rocsparse_status_continue)
        {
            RETURN_IF_ROCSPARSE_ERROR(status);
            return rocsparse_status_success;
        }
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_strided_batched_solve(p...));
        return rocsparse_status_success;
    }

    extern "C" rocsparse_status
        rocsparse_csrsv_strided_batched_solve(rocsparse_handle          handle,
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
                                              void*                     temp_buffer)
    try
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_csrsv_strided_batched::solve(handle,
                                                                         trans,
                                                                         batch_count,
                                                                         m,
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
                                                                         info,
                                                                         x_datatype,
                                                                         x,
                                                                         x_inc,
                                                                         x_stride,
                                                                         y_datatype,
                                                                         y,
                                                                         y_stride,
                                                                         policy,
                                                                         temp_buffer));
        return rocsparse_status_success;
    }
    catch(...)
    {
        RETURN_ROCSPARSE_EXCEPTION();
    }

    static rocsparse_status xsolve_checkarg(rocsparse_handle          handle,
                                            rocsparse_operation       trans,
                                            rocsparse_int             batch_count,
                                            rocsparse_int             m,
                                            rocsparse_int             nnz,
                                            const void*               alpha_device_host,
                                            int64_t                   alpha_stride,
                                            const rocsparse_mat_descr descr,
                                            const void*               csr_val,
                                            int64_t                   csr_val_stride,
                                            const rocsparse_int*      csr_row_ptr,
                                            const rocsparse_int*      csr_col_ind,
                                            rocsparse_mat_info        info,
                                            const void*               x,
                                            int64_t                   x_stride,
                                            void*                     y,
                                            int64_t                   y_stride,
                                            rocsparse_solve_policy    policy,
                                            void*                     temp_buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;
        // Check for valid handle and matrix descriptor
        ROCSPARSE_CHECKARG_HANDLE(0, handle);
        ROCSPARSE_CHECKARG_ENUM(1, trans);
        ROCSPARSE_CHECKARG_SIZE(2, m);
        ROCSPARSE_CHECKARG_SIZE(3, nnz);
        ROCSPARSE_CHECKARG_POINTER(4, alpha_device_host);
        ROCSPARSE_CHECKARG_POINTER(5, descr);
        ROCSPARSE_CHECKARG_ARRAY(6, nnz, csr_val);
        ROCSPARSE_CHECKARG_ARRAY(7, m, csr_row_ptr);
        ROCSPARSE_CHECKARG_ARRAY(8, nnz, csr_col_ind);
        ROCSPARSE_CHECKARG_POINTER(9, info);
        ROCSPARSE_CHECKARG_ARRAY(10, m, x);
        ROCSPARSE_CHECKARG_ARRAY(11, m, y);
        ROCSPARSE_CHECKARG_ENUM(12, policy);
        ROCSPARSE_CHECKARG_POINTER(13, temp_buffer);
        return rocsparse_status_continue;
    }

    template <typename T>
    static rocsparse_status xsolve(rocsparse_handle          handle,
                                   rocsparse_operation       trans,
                                   rocsparse_int             batch_count,
                                   rocsparse_int             m,
                                   rocsparse_int             nnz,
                                   const T*                  alpha,
                                   int64_t                   alpha_stride,
                                   const rocsparse_mat_descr descr,
                                   const T*                  csr_val,
                                   int64_t                   csr_val_stride,
                                   const rocsparse_int*      csr_row_ptr,
                                   const rocsparse_int*      csr_col_ind,
                                   rocsparse_mat_info        info,
                                   const T*                  x,
                                   int64_t                   x_stride,
                                   T*                        y,
                                   int64_t                   y_stride,
                                   rocsparse_solve_policy    policy,
                                   void*                     temp_buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;

        const rocsparse_status status
            = rocsparse_csrsv_strided_batched::xsolve_checkarg(handle,
                                                               trans,
                                                               batch_count,
                                                               m,
                                                               nnz,
                                                               alpha,
                                                               alpha_stride,
                                                               descr,
                                                               csr_val,
                                                               csr_val_stride,
                                                               csr_row_ptr,
                                                               csr_col_ind,
                                                               info,
                                                               x,
                                                               x_stride,
                                                               y,
                                                               y_stride,
                                                               policy,
                                                               temp_buffer);

        if(status != rocsparse_status_continue)
        {
            RETURN_IF_ROCSPARSE_ERROR(status);
            return rocsparse_status_success;
        }

        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse::csrsv_strided_batched_solve(handle,
                                                   trans,
                                                   batch_count,
                                                   m,
                                                   nnz,
                                                   rocsparse::get_datatype<T>(),
                                                   alpha,
                                                   alpha_stride,
                                                   descr,
                                                   rocsparse::get_datatype<T>(),
                                                   csr_val,
                                                   csr_val_stride,
                                                   rocsparse::get_indextype<rocsparse_int>(),
                                                   csr_row_ptr,
                                                   rocsparse::get_indextype<rocsparse_int>(),
                                                   csr_col_ind,
                                                   info,
                                                   rocsparse::get_datatype<T>(),
                                                   x,
                                                   (int64_t)1,
                                                   x_stride,
                                                   rocsparse::get_datatype<T>(),
                                                   y,
                                                   y_stride,
                                                   policy,
                                                   temp_buffer));

        return rocsparse_status_success;
    }
}

#define C_IMPL(NAME, T)                                                                   \
    extern "C" rocsparse_status NAME(rocsparse_handle          handle,                    \
                                     rocsparse_operation       trans,                     \
                                     rocsparse_int             batch_count,               \
                                     rocsparse_int             m,                         \
                                     rocsparse_int             nnz,                       \
                                     const T*                  alpha,                     \
                                     int64_t                   alpha_stride,              \
                                     const rocsparse_mat_descr descr,                     \
                                     const T*                  csr_val,                   \
                                     int64_t                   csr_val_stride,            \
                                     const rocsparse_int*      csr_row_ptr,               \
                                     const rocsparse_int*      csr_col_ind,               \
                                     rocsparse_mat_info        info,                      \
                                     const T*                  x,                         \
                                     int64_t                   x_stride,                  \
                                     T*                        y,                         \
                                     int64_t                   y_stride,                  \
                                     rocsparse_solve_policy    policy,                    \
                                     void*                     temp_buffer)               \
    try                                                                                   \
    {                                                                                     \
        ROCSPARSE_ROUTINE_TRACE;                                                          \
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_csrsv_strided_batched::xsolve(handle,         \
                                                                          trans,          \
                                                                          batch_count,    \
                                                                          m,              \
                                                                          nnz,            \
                                                                          alpha,          \
                                                                          alpha_stride,   \
                                                                          descr,          \
                                                                          csr_val,        \
                                                                          csr_val_stride, \
                                                                          csr_row_ptr,    \
                                                                          csr_col_ind,    \
                                                                          info,           \
                                                                          x,              \
                                                                          x_stride,       \
                                                                          y,              \
                                                                          y_stride,       \
                                                                          policy,         \
                                                                          temp_buffer));  \
        return rocsparse_status_success;                                                  \
    }                                                                                     \
    catch(...)                                                                            \
    {                                                                                     \
        RETURN_ROCSPARSE_EXCEPTION();                                                     \
    }

C_IMPL(rocsparse_scsrsv_strided_batched_solve, float);
C_IMPL(rocsparse_dcsrsv_strided_batched_solve, double);
C_IMPL(rocsparse_ccsrsv_strided_batched_solve, rocsparse_float_complex);
C_IMPL(rocsparse_zcsrsv_strided_batched_solve, rocsparse_double_complex);

#undef C_IMPL
