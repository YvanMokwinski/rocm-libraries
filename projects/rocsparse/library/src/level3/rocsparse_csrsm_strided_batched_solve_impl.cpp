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
#include "internal/level3/rocsparse_csrsm.h"
// #include "internal/level3/rocsparse_csrsm_strided_batched.h"
#include "rocsparse_csrsm.hpp"

#include "rocsparse_assign_async.hpp"
#include "rocsparse_common.h"
#include "rocsparse_common.hpp"
#include "rocsparse_utility.hpp"

#include "../level1/rocsparse_gthr.hpp"
#include "../level2/rocsparse_csrsv.hpp"
//#include "rocsparse_csrsm_strided_batched_solve_kernel.hpp"
#include "../level3/rocsparse_csrsm_strided_batched.hpp"
#include "rocsparse_csrsm_solve_copy_y_to_B.hpp"

namespace rocsparse
{
    typedef rocsparse_status (*csrsm_strided_batched_kernel_t)(
        rocsparse_handle    handle,
        rocsparse_operation opB,
        int64_t             batch_count,
        int64_t             m,
        int64_t             nrhs,
        const void*         alpha_,
        int64_t             alpha_stride,
        const void* __restrict__ csr_row_ptr_,
        const void* __restrict__ csr_col_ind_,
        const void* __restrict__ csr_val_,
        int64_t csr_val_stride,
        void* __restrict__ B_,
        int64_t ldb,
        int64_t B_stride,
        int32_t* __restrict__ done_array,
        int64_t done_array_stride,
        const void* __restrict__ map_,
        void* __restrict__ zero_pivot_,
        int64_t              zero_pivot_stride,
        rocsparse_index_base idx_base,
        rocsparse_fill_mode  fill_mode,
        rocsparse_diag_type  diag_type);
    rocsparse_status
        csrsm_strided_batched_kernel_launch_find(rocsparse::csrsm_strided_batched_kernel_t* f_,
                                                 uint32_t                                   A,
                                                 uint32_t                                   B,
                                                 bool                                       C,
                                                 rocsparse_indextype                        i_type_,
                                                 rocsparse_indextype                        j_type_,
                                                 rocsparse_datatype a_type_);

}
rocsparse_status rocsparse::csrsm_solve(rocsparse_handle             handle,
                                        const rocsparse_operation    op_A,
                                        const rocsparse_operation    op_B,
                                        const rocsparse_datatype     alpha_datatype,
                                        const void*                  alpha,
                                        const int64_t                alpha_stride,
                                        rocsparse_const_spmat_descr  A,
                                        rocsparse_dnmat_descr        B,
                                        const rocsparse_solve_policy policy,
                                        rocsparse_csrsm_info         csrsm_info,
                                        void*                        temp_buffer)
{
    ROCSPARSE_ROUTINE_TRACE;
    const int64_t nrhs = (op_B == rocsparse_operation_none) ? B->cols : B->rows;

    if(A->rows == 0 || nrhs == 0 || A->batch_count == 0)
    {
        return rocsparse_status_success;
    }

    rocsparse_mat_descr descr = A->descr;
    ROCSPARSE_CHECKARG(
        6, descr, (descr->type != rocsparse_matrix_type_general), rocsparse_status_not_implemented);
    ROCSPARSE_CHECKARG(6,
                       descr,
                       (descr->storage_mode != rocsparse_storage_mode_sorted),
                       rocsparse_status_requires_sorted_storage);

    if(nrhs == 1)
    {
        //
        // Call csrsv.
        //

        _rocsparse_dnvec_descr b(
            B->batch_count,
            A->rows,
            B->data_type,
            B->const_values,
            B->values,
            (op_B == rocsparse_operation_none && B->order == rocsparse_order_column) ? 1 : B->ld,
            B->batch_stride);
        _rocsparse_dnvec_descr y(
            B->batch_count, A->rows, B->data_type, temp_buffer, temp_buffer, 1, A->rows);

        temp_buffer = reinterpret_cast<void*>(
            reinterpret_cast<char*>(temp_buffer)
            + ((rocsparse::datatype_sizeof(B->data_type) * A->rows * B->batch_count - 1) / 256 + 1)
                  * 256);

        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_solve(handle,
                                                         op_A,
                                                         alpha_datatype,
                                                         alpha,
                                                         alpha_stride,
                                                         A,
                                                         &b,
                                                         &y,
                                                         policy,
                                                         csrsm_info,
                                                         temp_buffer));

        if((op_B == rocsparse_operation_none && B->order == rocsparse_order_column))
        {
            if(B->batch_stride == B->rows * B->cols)
            {
                RETURN_IF_HIP_ERROR(hipMemcpyAsync(B->values,
                                                   y.const_values,
                                                   A->batch_count * A->rows
                                                       * rocsparse::datatype_sizeof(B->data_type),
                                                   hipMemcpyDeviceToDevice,
                                                   handle->stream));
            }
            else
            {
                for(int64_t i = 0; i < A->batch_count; ++i)
                {
                    RETURN_IF_HIP_ERROR(hipMemcpyAsync(
                        reinterpret_cast<char*>(B->values)
                            + rocsparse::datatype_sizeof(B->data_type) * i * B->batch_stride,
                        reinterpret_cast<const char*>(y.const_values)
                            + rocsparse::datatype_sizeof(y.data_type) * i * y.batch_stride,
                        A->rows * rocsparse::datatype_sizeof(B->data_type),
                        hipMemcpyDeviceToDevice,
                        handle->stream));
                }
            }
        }
        else
        {
            RETURN_IF_ROCSPARSE_ERROR(
                rocsparse::csrsm_strided_batched_solve_copy_y_to_B(handle,
                                                                   A->batch_count,
                                                                   A->rows,
                                                                   B->data_type,
                                                                   B->values,
                                                                   B->batch_stride,
                                                                   B->ld,
                                                                   y.const_values,
                                                                   y.batch_stride));
        }

        return rocsparse_status_success;
    }

    // Stream
    hipStream_t stream = handle->stream;

    // Buffer
    char* ptr = reinterpret_cast<char*>(temp_buffer);

    ptr += 256;

    // Each thread block performs at most blockdim columns of the
    // rhs matrix. Therefore, the number of blocks depend on nrhs
    // and the blocksize.
    // Because of this, we might need a larger done_array compared
    // to csrsv.
    uint32_t blockdim = 512;
    while(nrhs <= blockdim && blockdim > 32)
        blockdim >>= 1;
    blockdim <<= 1;

    const int32_t narrays = (nrhs - 1) / blockdim + 1;

    // done array
    int32_t* done_array = reinterpret_cast<int32_t*>(ptr);
    ptr += ((sizeof(int32_t) * A->rows * narrays * A->batch_count - 1) / 256 + 1) * 256;

    // Temporary array to store transpose of B
    void*                    Bt          = B->values;
    const rocsparse_datatype Bt_datatype = B->data_type;
    int64_t                  Bt_stride   = 0;
    if((op_B == rocsparse_operation_none && B->order == rocsparse_order_column))
    {
        Bt                              = ptr;
        const int64_t local_batch_count = (B->batch_stride == 0) ? B->batch_count : 1;
        ptr += ((rocsparse::datatype_sizeof(Bt_datatype) * A->rows * nrhs * local_batch_count - 1)
                    / 256
                + 1)
               * 256;
        Bt_stride = A->rows * nrhs;
    }

    // Temporary array to store transpose of A
    void* At = nullptr;
    if(op_A == rocsparse_operation_transpose || op_A == rocsparse_operation_conjugate_transpose)
    {
        At = ptr;
    }

    // Initialize buffers
    RETURN_IF_HIP_ERROR(hipMemsetAsync(
        done_array, 0, sizeof(int32_t) * A->rows * narrays * A->batch_count, stream));

    const int64_t done_array_stride = A->rows * narrays;

    const rocsparse::trm_info_t* trm_info = csrsm_info->get(op_A, descr->fill_mode);

    csrsm_info->set_pivot_batch_count(A->batch_count, stream);

    // If diag type is unit, re-initialize zero pivot to remove structural zeros
    if(descr->diag_type == rocsparse_diag_type_unit)
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::assign_max_async(
            A->batch_count, A->col_type, csrsm_info->get_zero_pivot(), stream));
    }

    // Leading dimension
    int64_t Bt_ld = B->ld;

    // Transpose B if B is not transposed yet to improve performance
    if((op_B == rocsparse_operation_none && B->order == rocsparse_order_column))
    {
        // Leading dimension for transposed B
        Bt_ld        = nrhs;
        double s_one = 1;
        if(B->data_type == rocsparse_datatype_f32_r || B->data_type == rocsparse_datatype_f32_c)
        {
            *reinterpret_cast<float*>(&s_one) = 1;
        }
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse::dense_transpose_strided_batched(handle,
                                                       rocsparse_pointer_mode_host,
                                                       A->batch_count,
                                                       A->rows,
                                                       nrhs,
                                                       B->data_type,
                                                       &s_one,
                                                       0,
                                                       B->data_type,
                                                       B,
                                                       B->ld,
                                                       B->batch_stride,
                                                       B->data_type,
                                                       Bt,
                                                       Bt_ld,
                                                       Bt_stride));
    }

    // Pointers to differentiate between transpose mode
    const void*         local_csr_row_ptr    = A->const_row_data;
    const void*         local_csr_col_ind    = A->const_col_data;
    const void*         local_csr_val        = A->const_val_data;
    int64_t             local_csr_val_stride = A->batch_stride;
    rocsparse_fill_mode fill_mode            = descr->fill_mode;

    // When computing transposed triangular solve, we first need to update the
    // transposed matrix values
    if(op_A == rocsparse_operation_transpose || op_A == rocsparse_operation_conjugate_transpose)
    {
        void*                    csrt_val          = At;
        const int64_t            csrt_val_stride   = A->nnz;
        const rocsparse_datatype csrt_val_datatype = A->data_type;
        // Gather values
        RETURN_IF_ROCSPARSE_ERROR((rocsparse::gthr_strided_batched(handle,
                                                                   A->batch_count,
                                                                   A->nnz,
                                                                   A->data_type,
                                                                   local_csr_val,
                                                                   local_csr_val_stride,
                                                                   csrt_val_datatype,
                                                                   csrt_val,
                                                                   csrt_val_stride,
                                                                   A->row_type,
                                                                   trm_info->get_transposed_perm(),
                                                                   rocsparse_index_base_zero)));

        if(op_A == rocsparse_operation_conjugate_transpose)
        {
            RETURN_IF_ROCSPARSE_ERROR(rocsparse::conjugate_strided_batched(
                handle, A->batch_count, A->nnz, csrt_val_datatype, csrt_val, csrt_val_stride));
        }

        local_csr_row_ptr    = trm_info->get_transposed_row_ptr();
        local_csr_col_ind    = trm_info->get_transposed_col_ind();
        local_csr_val        = csrt_val;
        local_csr_val_stride = csrt_val_stride;
        fill_mode            = (fill_mode == rocsparse_fill_mode_lower) ? rocsparse_fill_mode_upper
                                                                        : rocsparse_fill_mode_lower;
    }

    rocsparse::csrsm_strided_batched_kernel_t launch_kernel;

    {
        // Determine gcnArch and ASIC revision
        const std::string gcn_arch_name = rocsparse::handle_get_arch_name(handle);
        const int         asicRev       = handle->asic_rev;
        const bool        S = (gcn_arch_name == rocpsarse_arch_names::gfx908 && asicRev < 2);
        RETURN_IF_ROCSPARSE_ERROR(csrsm_strided_batched_kernel_launch_find(
            &launch_kernel, blockdim, 64, S, A->row_type, A->col_type, A->data_type));
    }

    RETURN_IF_ROCSPARSE_ERROR(launch_kernel(handle,
                                            op_B,
                                            A->batch_count,
                                            A->rows,
                                            nrhs,
                                            alpha,
                                            static_cast<int64_t>(0),
                                            local_csr_row_ptr,
                                            local_csr_col_ind,
                                            local_csr_val,
                                            local_csr_val_stride,
                                            Bt,
                                            Bt_ld,
                                            Bt_stride,
                                            done_array,
                                            done_array_stride,
                                            trm_info->get_row_map(),
                                            csrsm_info->get_zero_pivot(),
                                            static_cast<int64_t>(1),
                                            descr->base,
                                            fill_mode,
                                            descr->diag_type));

    // Transpose B back if B was not initially transposed
    if((op_B == rocsparse_operation_none && B->order == rocsparse_order_column))
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::dense_transpose_back_strided_batched(handle,
                                                                                  A->batch_count,
                                                                                  A->rows,
                                                                                  nrhs,
                                                                                  B->data_type,
                                                                                  Bt,
                                                                                  Bt_ld,
                                                                                  Bt_stride,
                                                                                  B->data_type,
                                                                                  B->values,
                                                                                  B->ld,
                                                                                  B->batch_stride));
    }

    return rocsparse_status_success;
}
