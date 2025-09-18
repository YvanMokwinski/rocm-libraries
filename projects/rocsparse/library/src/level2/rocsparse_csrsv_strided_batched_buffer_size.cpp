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
    rocsparse_status buffer_size_checkarg(rocsparse_handle          handle, //0
                                          rocsparse_operation       trans, //1
                                          int64_t                   batch_count, //2
                                          int64_t                   m, //3
                                          int64_t                   nnz, //4
                                          const rocsparse_mat_descr descr, //5
                                          rocsparse_datatype        csr_val_datatype, //6
                                          const void*               csr_val, //7
                                          int64_t                   csr_val_stride, //8
                                          rocsparse_indextype       csr_row_ptr_indextype, //9
                                          const void*               csr_row_ptr, //10
                                          rocsparse_indextype       csr_col_ind_indextype, //11
                                          const void*               csr_col_ind, //12
                                          rocsparse_mat_info        info, //13
                                          size_t*                   buffer_size) //14
    {
        ROCSPARSE_ROUTINE_TRACE;
        ROCSPARSE_CHECKARG_HANDLE(0, handle);
        ROCSPARSE_CHECKARG_ENUM(1, trans);
        ROCSPARSE_CHECKARG_SIZE(2, batch_count);
        ROCSPARSE_CHECKARG_SIZE(3, m);
        ROCSPARSE_CHECKARG_SIZE(4, nnz);
        ROCSPARSE_CHECKARG_POINTER(5, descr);
        ROCSPARSE_CHECKARG_ENUM(6, csr_val_datatype);
        ROCSPARSE_CHECKARG_ARRAY(7, nnz, csr_val);
        ROCSPARSE_CHECKARG_ENUM(9, csr_row_ptr_indextype);
        ROCSPARSE_CHECKARG_ARRAY(10, m, csr_row_ptr);
        ROCSPARSE_CHECKARG_ARRAY(12, nnz, csr_col_ind);
        ROCSPARSE_CHECKARG_POINTER(13, info);
        ROCSPARSE_CHECKARG_POINTER(14, buffer_size);
        return rocsparse_status_continue;
    }

    rocsparse_status xbuffer_size_checkarg(rocsparse_handle          handle,
                                           rocsparse_operation       trans,
                                           int64_t                   batch_count,
                                           int64_t                   m,
                                           int64_t                   nnz,
                                           const rocsparse_mat_descr descr,
                                           const void*               csr_val,
                                           int64_t                   csr_val_stride,
                                           const void*               csr_row_ptr,
                                           const void*               csr_col_ind,
                                           rocsparse_mat_info        info,
                                           size_t*                   buffer_size)
    {
        ROCSPARSE_ROUTINE_TRACE;
        ROCSPARSE_CHECKARG_HANDLE(0, handle);
        ROCSPARSE_CHECKARG_ENUM(1, trans);
        ROCSPARSE_CHECKARG_SIZE(2, batch_count);
        ROCSPARSE_CHECKARG_SIZE(3, m);
        ROCSPARSE_CHECKARG_SIZE(4, nnz);
        ROCSPARSE_CHECKARG_POINTER(5, descr);
        ROCSPARSE_CHECKARG_ARRAY(6, nnz, csr_val);
        ROCSPARSE_CHECKARG_ARRAY(8, m, csr_row_ptr);
        ROCSPARSE_CHECKARG_ARRAY(9, nnz, csr_col_ind);
        ROCSPARSE_CHECKARG_POINTER(10, info);
        ROCSPARSE_CHECKARG_POINTER(11, buffer_size);
        return rocsparse_status_continue;
    }

    template <typename... P>
    rocsparse_status buffer_size(P... p)
    {
        ROCSPARSE_ROUTINE_TRACE;

        const rocsparse_status status = rocsparse_csrsv_strided_batched::buffer_size_checkarg(p...);
        if(status != rocsparse_status_continue)
        {
            RETURN_IF_ROCSPARSE_ERROR(status);
            return rocsparse_status_success;
        }
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_strided_batched_buffer_size(p...));
        return rocsparse_status_success;
    }

    template <typename T>
    rocsparse_status xbuffer_size(rocsparse_handle          handle,
                                  rocsparse_operation       trans,
                                  rocsparse_int             batch_count,
                                  rocsparse_int             m,
                                  rocsparse_int             nnz,
                                  const rocsparse_mat_descr descr,
                                  const void*               csr_val,
                                  rocsparse_int             csr_val_stride,
                                  const void*               csr_row_ptr,
                                  const void*               csr_col_ind,
                                  rocsparse_mat_info        info,
                                  size_t*                   buffer_size)
    {
        ROCSPARSE_ROUTINE_TRACE;

        const rocsparse_status status
            = rocsparse_csrsv_strided_batched::xbuffer_size_checkarg(handle,
                                                                     trans,
                                                                     batch_count,
                                                                     m,
                                                                     nnz,
                                                                     descr,
                                                                     csr_val,
                                                                     csr_val_stride,
                                                                     csr_row_ptr,
                                                                     csr_col_ind,
                                                                     info,
                                                                     buffer_size);
        if(status != rocsparse_status_continue)
        {
            RETURN_IF_ROCSPARSE_ERROR(status);
            return rocsparse_status_success;
        }
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse::csrsv_strided_batched_buffer_size(handle,
                                                         trans,
                                                         batch_count,
                                                         m,
                                                         nnz,
                                                         descr,
                                                         rocsparse::get_datatype<T>(),
                                                         csr_val,
                                                         csr_val_stride,
                                                         rocsparse::get_indextype<rocsparse_int>(),
                                                         csr_row_ptr,
                                                         rocsparse::get_indextype<rocsparse_int>(),
                                                         csr_col_ind,
                                                         info,
                                                         buffer_size));

        return rocsparse_status_success;
    }

}

extern "C" rocsparse_status
    rocsparse_csrsv_strided_batched_buffer_size(rocsparse_handle          handle,
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
                                                size_t*                   buffer_size)
try
{
    RETURN_IF_ROCSPARSE_ERROR((rocsparse_csrsv_strided_batched::buffer_size(handle,
                                                                            trans,
                                                                            batch_count,
                                                                            m,
                                                                            nnz,
                                                                            descr,
                                                                            csr_val_datatype,
                                                                            csr_val,
                                                                            csr_val_stride,
                                                                            csr_row_ptr_indextype,
                                                                            csr_row_ptr,
                                                                            csr_col_ind_indextype,
                                                                            csr_col_ind,
                                                                            info,
                                                                            buffer_size)));
    return rocsparse_status_success;
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */
#define CIMPL(NAME, T)                                                         \
    extern "C" rocsparse_status NAME(rocsparse_handle          handle,         \
                                     rocsparse_operation       trans,          \
                                     rocsparse_int             batch_count,    \
                                     rocsparse_int             m,              \
                                     rocsparse_int             nnz,            \
                                     const rocsparse_mat_descr descr,          \
                                     const T*                  csr_val,        \
                                     int64_t                   csr_val_stride, \
                                     const rocsparse_int*      csr_row_ptr,    \
                                     const rocsparse_int*      csr_col_ind,    \
                                     rocsparse_mat_info        info,           \
                                     size_t*                   buffer_size)    \
    try                                                                        \
    {                                                                          \
        RETURN_IF_ROCSPARSE_ERROR(                                             \
            (rocsparse_csrsv_strided_batched::xbuffer_size<T>(handle,          \
                                                              trans,           \
                                                              batch_count,     \
                                                              m,               \
                                                              nnz,             \
                                                              descr,           \
                                                              csr_val,         \
                                                              csr_val_stride,  \
                                                              csr_row_ptr,     \
                                                              csr_col_ind,     \
                                                              info,            \
                                                              buffer_size)));  \
        return rocsparse_status_success;                                       \
    }                                                                          \
    catch(...)                                                                 \
    {                                                                          \
        RETURN_ROCSPARSE_EXCEPTION();                                          \
    }

CIMPL(rocsparse_scsrsv_strided_batched_buffer_size, float);
CIMPL(rocsparse_dcsrsv_strided_batched_buffer_size, double);
CIMPL(rocsparse_ccsrsv_strided_batched_buffer_size, rocsparse_float_complex);
CIMPL(rocsparse_zcsrsv_strided_batched_buffer_size, rocsparse_double_complex);

#undef CIMPL
