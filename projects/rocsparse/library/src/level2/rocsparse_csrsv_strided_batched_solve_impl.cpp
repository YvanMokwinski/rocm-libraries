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

#include "../level1/rocsparse_gthr.hpp"
#include "csrsv_device.h"
#include "rocsparse_assign_async.hpp"
#include "rocsparse_common.h"
#include "rocsparse_control.hpp"
#include "rocsparse_csrsv_solve_kernel.hpp"
#include "rocsparse_csrsv_strided_batched.hpp"
#include "rocsparse_utility.hpp"

rocsparse_status rocsparse::csrsv_strided_batched_solve(rocsparse_handle          handle,
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
                                                        rocsparse_indextype csr_row_ptr_indextype,
                                                        const void*         csr_row_ptr,
                                                        rocsparse_indextype csr_col_ind_indextype,
                                                        const void*         csr_col_ind,
                                                        rocsparse_mat_info  info,
                                                        rocsparse_datatype  x_datatype,
                                                        const void*         x,
                                                        int64_t             x_inc,
                                                        int64_t             x_stride,
                                                        rocsparse_datatype  y_datatype,
                                                        void*               y,
                                                        int64_t             y_stride,
                                                        rocsparse_solve_policy policy,
                                                        void*                  temp_buffer)
{
    ROCSPARSE_ROUTINE_TRACE;

    // Quick return if possible
    if(m == 0 || batch_count == 0)
    {
        return rocsparse_status_success;
    }
    // Check matrix type
    ROCSPARSE_CHECKARG(7,
                       descr,
                       (descr->type != rocsparse_matrix_type_general
                        && descr->type != rocsparse_matrix_type_triangular),
                       rocsparse_status_not_implemented);

    // Check matrix sorting mode
    ROCSPARSE_CHECKARG(7,
                       descr,
                       (descr->storage_mode != rocsparse_storage_mode_sorted),
                       rocsparse_status_requires_sorted_storage);

    // Stream
    hipStream_t stream = handle->stream;

    // Buffer
    char* ptr = reinterpret_cast<char*>(temp_buffer);

    ptr += 256;

    // done array
    int32_t*     done_array = reinterpret_cast<int32_t*>(ptr);
    const size_t done_array_size_in_bytes
        = ((sizeof(int32_t) * m * batch_count - 1) / 256 + 1) * 256;
    ptr += done_array_size_in_bytes;

    // Initialize buffers
    RETURN_IF_HIP_ERROR(hipMemsetAsync(done_array, 0, done_array_size_in_bytes, stream));

    const rocsparse::trm_info_t* csrsv
        = (descr->fill_mode == rocsparse_fill_mode_upper)
              ? ((trans == rocsparse_operation_none) ? info->csrsv_upper_info
                                                     : info->csrsvt_upper_info)
              : ((trans == rocsparse_operation_none) ? info->csrsv_lower_info
                                                     : info->csrsvt_lower_info);

    if(csrsv == nullptr)
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_pointer);
    }

    // If diag type is unit, re-initialize zero pivot to remove structural zeros
    if(descr->diag_type == rocsparse_diag_type_unit)
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::assign_max_async(
            batch_count, csr_col_ind_indextype, info->zero_pivot, stream));
    }

    // Pointers to differentiate between transpose mode
    const void*         local_csr_row_ptr    = csr_row_ptr;
    const void*         local_csr_col_ind    = csr_col_ind;
    const void*         local_csr_val        = csr_val;
    int64_t             local_csr_val_stride = csr_val_stride;
    rocsparse_fill_mode fill_mode            = descr->fill_mode;

    // When computing transposed triangular solve, we first need to update the
    // transposed matrix values
    if(trans == rocsparse_operation_transpose || trans == rocsparse_operation_conjugate_transpose)
    {
        void* csrt_val = ptr;
        RETURN_IF_ROCSPARSE_ERROR((rocsparse::gthr_strided_batched(handle,
                                                                   batch_count,
                                                                   nnz,
                                                                   csr_val_datatype,
                                                                   csr_val,
                                                                   csr_val_stride,
                                                                   csr_val_datatype,
                                                                   csrt_val,
                                                                   nnz,
                                                                   csr_row_ptr_indextype,
                                                                   csrsv->get_transposed_perm(),
                                                                   rocsparse_index_base_zero)));

        if(trans == rocsparse_operation_conjugate_transpose)
        {
            RETURN_IF_ROCSPARSE_ERROR((rocsparse::conjugate_strided_batched(
                handle, batch_count, nnz, csr_val_datatype, csrt_val, nnz)));
        }

        local_csr_row_ptr    = csrsv->get_transposed_row_ptr();
        local_csr_col_ind    = csrsv->get_transposed_col_ind();
        local_csr_val        = csrt_val;
        local_csr_val_stride = nnz;
        fill_mode            = (fill_mode == rocsparse_fill_mode_lower) ? rocsparse_fill_mode_upper
                                                                        : rocsparse_fill_mode_lower;
    }

    // Determine gcn_arch
    const std::string gcn_arch_name = rocsparse::handle_get_arch_name(handle);
    const int         asicRev       = handle->asic_rev;

    // gfx908
    const bool     sleep_  = (gcn_arch_name == rocpsarse_arch_names::gfx908 && asicRev < 2);
    const uint32_t wfsize_ = sleep_ ? 64 : handle->wavefront_size;

    rocsparse::csrsv_launch_kernel_t csrsv_launch_kernel{};
    RETURN_IF_ROCSPARSE_ERROR(csrsv_launch_kernel_find(&csrsv_launch_kernel,
                                                       512,
                                                       wfsize_,
                                                       sleep_,
                                                       csr_row_ptr_indextype,
                                                       csr_col_ind_indextype,
                                                       csr_val_datatype));
#undef CSRSV_DIM

    csrsv_launch_kernel(handle,
                        batch_count,
                        m,
                        alpha,
                        alpha_stride,
                        local_csr_row_ptr,
                        local_csr_col_ind,
                        local_csr_val,
                        local_csr_val_stride,
                        x,
                        x_inc,
                        x_stride,
                        y,
                        y_stride,
                        done_array,
                        csrsv->get_row_map(),
                        0,
                        info->zero_pivot,
                        descr->base,
                        fill_mode,
                        descr->diag_type,
                        handle->pointer_mode == rocsparse_pointer_mode_host);
    return rocsparse_status_success;
}
