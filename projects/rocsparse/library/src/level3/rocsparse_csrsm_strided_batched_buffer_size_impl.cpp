/*! \file */
/* ************************************************************************
 * Copyright (C) 2020-2025 Advanced Micro Devices, Inc. All rights Reserved.
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
#include "../level2/rocsparse_csrsv_strided_batched.hpp"
#include "rocsparse_csrsm_strided_batched.hpp"
#include "rocsparse_primitives.hpp"
#include "rocsparse_utility.hpp"

rocsparse_status
    rocsparse::csrsm_strided_batched_buffer_size(rocsparse_handle          handle,
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
                                                 rocsparse_order           order_B,
                                                 rocsparse_mat_info        info,
                                                 rocsparse_solve_policy    policy,
                                                 size_t*                   buffer_size)
{
    ROCSPARSE_ROUTINE_TRACE;

    if(m == 0 || nrhs == 0 || batch_count == 0)
    {
        *buffer_size = 0;
        return rocsparse_status_success;
    }

    const int64_t B_batch_count       = ((B_stride == 0) ? 1 : batch_count);
    const int64_t csr_val_batch_count = ((csr_val_stride == 0) ? 1 : batch_count);

    if(nrhs == 1)
    {
        //
        // Call csrsv.
        //
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse::csrsv_strided_batched_buffer_size(handle,
                                                         trans_A,
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
                                                         buffer_size));
        *buffer_size
            += ((rocsparse::datatype_sizeof(B_datatype) * m * B_batch_count - 1) / 256 + 1) * 256;
        return rocsparse_status_success;
    }

    // max_nnz
    *buffer_size = 256;

    // Each thread block performs at most blockdim columns of the
    // rhs matrix. Therefore, the number of blocks depend on nrhs
    // and the blocksize.
    // Because of this, we might need a larger done_array compared
    // to csrsv.
    int32_t blockdim = 512;
    while(nrhs <= blockdim && blockdim > 32)
    {
        blockdim >>= 1;
    }

    blockdim <<= 1;
    const int32_t narrays  = (nrhs - 1) / blockdim + 1;
    const size_t  J_sizeof = rocsparse::indextype_sizeof(csr_col_ind_indextype);

    // int32_t done_array
    *buffer_size += ((sizeof(int32_t) * m * narrays * batch_count - 1) / 256 + 1) * 256;

    // workspace
    *buffer_size += ((J_sizeof * m - 1) / 256 + 1) * 256;

    // int32_t workspace2
    *buffer_size += ((sizeof(int32_t) * m - 1) / 256 + 1) * 256;

    uint32_t startbit = 0;
    uint32_t endbit   = rocsparse::clz(m);

    // rocprim buffer
    {
        size_t rocprim_size;
        auto   sort_pairs_buffer_size
            = find_radix_sort_pairs_buffer_size(rocsparse_indextype_i32, csr_col_ind_indextype);
        static constexpr bool using_double_buffers = true;
        RETURN_IF_ROCSPARSE_ERROR(sort_pairs_buffer_size(
            handle, m, startbit, endbit, &rocprim_size, using_double_buffers));
        *buffer_size += rocprim_size;
    }

    // Additional buffer to store transpose of B, if trans_B == rocsparse_operation_none
    if(trans_B == rocsparse_operation_none && order_B == rocsparse_order_column)
    {
        const int64_t B_batch_count = ((B_stride == 0) ? 1 : batch_count);
        *buffer_size
            += ((rocsparse::datatype_sizeof(csr_val_datatype) * m * nrhs * B_batch_count - 1) / 256
                + 1)
               * 256;
    }

    // Additional buffer to store transpose A, if transA != rocsparse_operation_none
    if(trans_A == rocsparse_operation_transpose
       || trans_A == rocsparse_operation_conjugate_transpose)
    {
        size_t transpose_size;
        size_t rocprim_size{};
        auto   sort_pairs_buffer_size
            = find_radix_sort_pairs_buffer_size(csr_col_ind_indextype, csr_row_ptr_indextype);
        static constexpr bool using_double_buffers = true;
        RETURN_IF_ROCSPARSE_ERROR(sort_pairs_buffer_size(
            handle, nnz, startbit, endbit, &rocprim_size, using_double_buffers));

        // rocPRIM does not support in-place sorting, so we need an additional buffer
        transpose_size += ((J_sizeof * nnz - 1) / 256 + 1) * 256;

        const size_t a
            = ((rocsparse::indextype_sizeof(csr_row_ptr_indextype) * nnz - 1) / 256 + 1) * 256;
        const size_t b
            = ((rocsparse::datatype_sizeof(csr_val_datatype) * nnz * csr_val_batch_count - 1) / 256
               + 1)
              * 256;

        transpose_size += rocsparse::max(a, b);

        *buffer_size += transpose_size;
    }

    return rocsparse_status_success;
}
