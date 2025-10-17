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
#include "../level2/rocsparse_csrsv.hpp"
#include "rocsparse_csrsm_strided_batched.hpp"
#include "rocsparse_primitives.hpp"
#include "rocsparse_utility.hpp"

rocsparse_status rocsparse::csrsm_buffer_size(rocsparse_handle            handle,
                                              rocsparse_operation         op_A,
                                              rocsparse_operation         op_B,
                                              rocsparse_datatype          alpha_datatype,
                                              int64_t                     alpha_stride,
                                              rocsparse_const_spmat_descr A,
                                              rocsparse_const_dnmat_descr B,
                                              rocsparse_solve_policy      policy,
                                              size_t*                     buffer_size)
{
    ROCSPARSE_ROUTINE_TRACE;
    const int64_t nrhs = (op_B == rocsparse_operation_none) ? B->cols : B->rows;

    if(A->rows == 0 || nrhs == 0 || A->batch_count == 0)
    {
        *buffer_size = 0;
        return rocsparse_status_success;
    }

    const int64_t B_batch_count      = ((B->batch_stride == 0) ? 1 : B->batch_count);
    const int64_t A_data_batch_count = ((A->batch_stride == 0) ? 1 : A->batch_count);

    RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
        ((A->batch_count != B->batch_count) && (A->batch_count != 1 && B->batch_count != 1))
            ? rocsparse_status_invalid_value
            : rocsparse_status_success,
        "Incompatible batch counts, they must be equal.");
    if(nrhs == 1)
    {
        //
        // Call csrsv.
        //
        size_t buffer_size_analysis;
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse::csrsv_analysis_buffer_size(handle, op_A, A, &buffer_size_analysis));
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse::csrsv_analysis_buffer_size(handle, op_A, A, buffer_size));
        *buffer_size += buffer_size_analysis;
        *buffer_size
            += ((rocsparse::datatype_sizeof(B->data_type) * A->rows * B_batch_count - 1) / 256 + 1)
               * 256;
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
    const size_t  J_sizeof = rocsparse::indextype_sizeof(A->col_type);

    // int32_t done_array
    *buffer_size
        += ((sizeof(int32_t) * A->rows * narrays * A_data_batch_count - 1) / 256 + 1) * 256;

    // workspace
    *buffer_size += ((J_sizeof * A->rows - 1) / 256 + 1) * 256;

    // int32_t workspace2
    *buffer_size += ((sizeof(int32_t) * A->rows - 1) / 256 + 1) * 256;

    uint32_t startbit = 0;
    uint32_t endbit   = rocsparse::clz(A->rows);

    // rocprim buffer
    {
        size_t rocprim_size;
        auto   sort_pairs_buffer_size
            = find_radix_sort_pairs_buffer_size(rocsparse_indextype_i32, A->col_type);
        static constexpr bool using_double_buffers = true;
        RETURN_IF_ROCSPARSE_ERROR(sort_pairs_buffer_size(
            handle, A->rows, startbit, endbit, &rocprim_size, using_double_buffers));
        *buffer_size += rocprim_size;
    }

    // Additional buffer to store transpose of B, if op_B == rocsparse_operation_none
    if(op_B == rocsparse_operation_none && B->order == rocsparse_order_column)
    {
        *buffer_size
            += ((rocsparse::datatype_sizeof(B->data_type) * B->rows * B->cols * B_batch_count - 1)
                    / 256
                + 1)
               * 256;
    }

    // Additional buffer to store transpose A, if transA != rocsparse_operation_none
    if(op_A == rocsparse_operation_transpose || op_A == rocsparse_operation_conjugate_transpose)
    {
        size_t rocprim_size{};
        auto   sort_pairs_buffer_size = find_radix_sort_pairs_buffer_size(A->col_type, A->row_type);
        static constexpr bool using_double_buffers = true;
        RETURN_IF_ROCSPARSE_ERROR(sort_pairs_buffer_size(
            handle, A->nnz, startbit, endbit, &rocprim_size, using_double_buffers));
        size_t transpose_size = rocprim_size;

        // rocPRIM does not support in-place sorting, so we need an additional buffer
        transpose_size += ((J_sizeof * A->nnz - 1) / 256 + 1) * 256;

        const size_t a = ((rocsparse::indextype_sizeof(A->row_type) * A->nnz - 1) / 256 + 1) * 256;
        const size_t b
            = ((rocsparse::datatype_sizeof(A->data_type) * A->nnz * A_data_batch_count - 1) / 256
               + 1)
              * 256;

        transpose_size += rocsparse::max(a, b);
        *buffer_size += transpose_size;
    }

    return rocsparse_status_success;
}
