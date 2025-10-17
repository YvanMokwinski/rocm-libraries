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

#include "internal/level2/rocsparse_bsrsv.h"
#include "rocsparse_bsrsv_strided_batched.hpp"

#include "rocsparse_control.hpp"
#include "rocsparse_utility.hpp"

#include "../level2/rocsparse_csrsv.hpp"

#include "rocsparse_common.hpp"

namespace rocsparse
{

    template <uint32_t BLOCKSIZE, uint32_t WFSIZE, bool SLEEP, typename T, typename I, typename J>
    ROCSPARSE_DEVICE_ILF void bsrsv_lower_general_device(J mb,
                                                         T alpha,
                                                         const I* __restrict__ bsr_row_ptr,
                                                         const J* __restrict__ bsr_col_ind,
                                                         const T* __restrict__ bsr_val,
                                                         J block_dim,
                                                         const T* __restrict__ x,
                                                         T* __restrict__ y,
                                                         int* __restrict__ done_array,
                                                         J* __restrict__ map,
                                                         J* __restrict__ zero_pivot,
                                                         rocsparse_index_base idx_base,
                                                         rocsparse_diag_type  diag_type,
                                                         rocsparse_direction  dir)
    {
        const int lid = hipThreadIdx_x & (WFSIZE - 1);
        const int wid = hipThreadIdx_x / WFSIZE;

        // Index into the row map
        const J idx = hipBlockIdx_x * BLOCKSIZE / WFSIZE + wid;

        // Do not run out of bounds
        if(idx >= mb)
        {
            return;
        }

        // Get the BSR row this wavefront will operate on
        const J row = map[idx];

        // Current row entry and exit point
        const I row_begin = bsr_row_ptr[row] - idx_base;
        const I row_end   = bsr_row_ptr[row + 1] - idx_base;

        // Initialize local_col with mb
        J local_col = mb;

        // Initialize y with alpha and x
        for(J bi = lid; bi < block_dim; bi += WFSIZE)
        {
            y[row * block_dim + bi] = alpha * x[row * block_dim + bi];
        }

        // Loop over the current row
        I j;
        for(j = row_begin; j < row_end; ++j)
        {
            // Current column index
            local_col = bsr_col_ind[j] - idx_base;

            // Processing lower triangular

            // Ignore all diagonal entries and above
            if(local_col >= row)
            {
                break;
            }

            // Spin loop until dependency has been resolved
            rocsparse::spin_loop<SLEEP>(&done_array[local_col], __HIP_MEMORY_SCOPE_AGENT);

            // Wait for y to be visible globally
            __builtin_amdgcn_fence(__ATOMIC_ACQUIRE, "agent");

            // Local sum computation
            for(J bi = lid; bi < block_dim; bi += WFSIZE)
            {
                // Local sum accumulator
                T local_sum = static_cast<T>(0);

                for(J bj = 0; bj < block_dim; ++bj)
                {
                    local_sum = rocsparse::fma(
                        bsr_val[BSR_IND(j, bi, bj, dir)], y[local_col * block_dim + bj], local_sum);
                }

                // Write local sum to y
                y[row * block_dim + bi] -= local_sum;
            }
        }

        bool pivot = false;

        // Process diagonal
        if(local_col == row)
        {
            for(J bi = 0; bi < block_dim; ++bi)
            {
                // Load diagonal matrix entry
                const T diag = (diag_type == rocsparse_diag_type_non_unit)
                                   ? bsr_val[block_dim * block_dim * j + bi + bi * block_dim]
                                   : static_cast<T>(1);

                // Load result of bi-th BSR row
                T val = y[row * block_dim + bi];
                // Check for numerical pivot
                if(diag == static_cast<T>(0))
                {
                    pivot = true;
                }
                else
                {
                    // Divide result of bi-th BSR row by diagonal entry
                    y[row * block_dim + bi] = val /= diag;
                }

                // Update remaining non-diagonal entries
                for(J bj = bi + lid + 1; bj < block_dim; bj += WFSIZE)
                {
                    y[row * block_dim + bj] -= val * bsr_val[BSR_IND(j, bj, bi, dir)];
                }
            }
        }

        // Write "row is done" flag
        if(lid == 0)
        {
            __hip_atomic_store(&done_array[row], 1, __ATOMIC_RELEASE, __HIP_MEMORY_SCOPE_AGENT);

            if(pivot == true)
            {
                rocsparse::atomic_min(zero_pivot, row + idx_base);
            }
        }
    }

    template <uint32_t BLOCKSIZE, uint32_t WFSIZE, bool SLEEP, typename T, typename I, typename J>
    ROCSPARSE_DEVICE_ILF void bsrsv_upper_general_device(J mb,
                                                         T alpha,
                                                         const I* __restrict__ bsr_row_ptr,
                                                         const J* __restrict__ bsr_col_ind,
                                                         const T* __restrict__ bsr_val,
                                                         J block_dim,
                                                         const T* __restrict__ x,
                                                         T* __restrict__ y,
                                                         int* __restrict__ done_array,
                                                         J* __restrict__ map,
                                                         J* __restrict__ zero_pivot,
                                                         rocsparse_index_base idx_base,
                                                         rocsparse_diag_type  diag_type,
                                                         rocsparse_direction  dir)
    {
        const int lid = hipThreadIdx_x & (WFSIZE - 1);
        const int wid = hipThreadIdx_x / WFSIZE;

        // Index into the row map
        const J idx = hipBlockIdx_x * BLOCKSIZE / WFSIZE + wid;

        // Do not run out of bounds
        if(idx >= mb)
        {
            return;
        }

        // Get the BSR row this wavefront will operate on
        const J row = map[idx];

        // Current row entry and exit point
        const I row_begin = bsr_row_ptr[row] - idx_base;
        const I row_end   = bsr_row_ptr[row + 1] - idx_base;

        // Initialize local_col with mb
        J local_col = mb;

        // Initialize y with alpha and x
        for(J bi = lid; bi < block_dim; bi += WFSIZE)
        {
            y[row * block_dim + bi] = alpha * x[row * block_dim + bi];
        }

        // Loop over the current row
        I j;
        for(j = row_end - 1; j >= row_begin; --j)
        {
            // Current column index
            local_col = bsr_col_ind[j] - idx_base;

            // Processing upper triangular

            // Ignore all diagonal entries and below
            if(local_col <= row)
            {
                break;
            }

            // Spin loop until dependency has been resolved
            rocsparse::spin_loop<SLEEP>(&done_array[local_col], __HIP_MEMORY_SCOPE_AGENT);

            // Wait for y to be visible globally
            __builtin_amdgcn_fence(__ATOMIC_ACQUIRE, "agent");

            // Local sum computation
            for(J bi = lid; bi < block_dim; bi += WFSIZE)
            {
                // Local sum accumulator
                T local_sum = static_cast<T>(0);

                for(J bj = 0; bj < block_dim; ++bj)
                {
                    local_sum = rocsparse::fma(
                        bsr_val[BSR_IND(j, bi, bj, dir)], y[local_col * block_dim + bj], local_sum);
                }

                // Write local sum to y
                y[row * block_dim + bi] -= local_sum;
            }
        }

        bool pivot = false;

        // Process diagonal
        if(local_col == row)
        {
            for(J bi = block_dim - 1; bi >= 0; --bi)
            {
                // Load diagonal matrix entry
                const T diag = (diag_type == rocsparse_diag_type_non_unit)
                                   ? bsr_val[block_dim * block_dim * j + bi + bi * block_dim]
                                   : static_cast<T>(1);

                // Load result of bi-th BSR row
                T val = y[row * block_dim + bi];

                // Check for numerical pivot
                if(diag == static_cast<T>(0))
                {
                    pivot = true;
                }
                else
                {
                    // Divide result of bi-th BSR row by diagonal entry
                    y[row * block_dim + bi] = val /= diag;
                }

                // Update remaining non-diagonal entries
                for(J bj = lid; bj < bi; bj += WFSIZE)
                {
                    y[row * block_dim + bj] -= val * bsr_val[BSR_IND(j, bj, bi, dir)];
                }
            }
        }

        // Write "row is done" flag
        if(lid == 0)
        {
            __hip_atomic_store(&done_array[row], 1, __ATOMIC_RELEASE, __HIP_MEMORY_SCOPE_AGENT);

            if(pivot == true)
            {
                rocsparse::atomic_min(zero_pivot, row + idx_base);
            }
        }
    }

    template <uint32_t BLOCKSIZE,
              uint32_t WFSIZE,
              uint32_t BSRDIM,
              bool     SLEEP,
              typename T,
              typename I,
              typename J>
    ROCSPARSE_DEVICE_ILF void bsrsv_lower_shared_device(J mb,
                                                        T alpha,
                                                        const I* __restrict__ bsr_row_ptr,
                                                        const J* __restrict__ bsr_col_ind,
                                                        const T* __restrict__ bsr_val,
                                                        J block_dim,
                                                        const T* __restrict__ x,
                                                        T* __restrict__ y,
                                                        int* __restrict__ done_array,
                                                        J* __restrict__ map,
                                                        J* __restrict__ zero_pivot,
                                                        rocsparse_index_base idx_base,
                                                        rocsparse_diag_type  diag_type,
                                                        rocsparse_direction  dir)
    {
        const int lid = hipThreadIdx_x & (WFSIZE - 1);
        const int wid = hipThreadIdx_x / WFSIZE;

        // Index into the row map
        const J idx = hipBlockIdx_x * BLOCKSIZE / WFSIZE + wid;

        // Do not run out of bounds
        if(idx >= mb)
        {
            return;
        }

        // Get the BSR row this wavefront will operate on
        const J row = map[idx];

        // Current row entry and exit point
        const I row_begin = bsr_row_ptr[row] - idx_base;
        const I row_end   = bsr_row_ptr[row + 1] - idx_base;

        // Initialize local_col with mb
        J local_col = mb;

        // Initialize local summation variable with alpha and x
        T local_sum = alpha * ((lid < block_dim) ? x[row * block_dim + lid] : static_cast<T>(0));

        // Shared memory to hold BSR blocks and updated sums
        __shared__ T sdata1[BLOCKSIZE / WFSIZE * BSRDIM * BSRDIM];
        __shared__ T sdata2[BLOCKSIZE / WFSIZE * BSRDIM];

        T* bsr_values  = &sdata1[wid * BSRDIM * BSRDIM];
        T* bsr_updates = &sdata2[wid * BSRDIM];

        // Loop over the current row
        I j;
        for(j = row_begin; j < row_end; ++j)
        {
            // Current column index
            local_col = bsr_col_ind[j] - idx_base;

            // Load BSR block values
            // Each wavefront loads a full BSR block into shared memory
            // Pad remaining entries with zero
            const int bi = lid & (BSRDIM - 1);
            const int bj = lid / BSRDIM;

            for(J k = bj; k < BSRDIM; k += WFSIZE / BSRDIM)
            {
                bsr_values[bi + k * BSRDIM] = (bi < block_dim && k < block_dim)
                                                  ? bsr_val[BSR_IND(j, bi, k, dir)]
                                                  : static_cast<T>(0);
            }

            // Processing lower triangular

            // Ignore all diagonal entries and above
            if(local_col >= row)
            {
                break;
            }

            // Spin loop until dependency has been resolved
            rocsparse::spin_loop<SLEEP>(&done_array[local_col], __HIP_MEMORY_SCOPE_AGENT);

            // Wait for y to be visible globally
            __builtin_amdgcn_fence(__ATOMIC_ACQUIRE, "agent");

            // Load all updated dependencies into shared memory
            if(lid < BSRDIM)
            {
                bsr_updates[lid]
                    = (lid < block_dim) ? y[local_col * block_dim + lid] : static_cast<T>(0);
            }

            __threadfence_block();

            // Local sum computation
            if(lid < block_dim)
            {
                for(J l = 0; l < BSRDIM; ++l)
                {
                    local_sum
                        = rocsparse::fma(-bsr_values[lid + l * BSRDIM], bsr_updates[l], local_sum);
                }
            }
        }

        // Initialize zero pivot
        bool pivot = false;

        // Process diagonal
        if(local_col == row)
        {
            for(J bi = 0; bi < block_dim; ++bi)
            {
                // Load diagonal matrix entry
                const T diag = (diag_type == rocsparse_diag_type_non_unit)
                                   ? bsr_values[bi + bi * BSRDIM]
                                   : static_cast<T>(1);

                // Load result of bi-th BSR row
                T val = rocsparse::shfl(local_sum, bi);

                // Check for numerical pivot
                if(diag == static_cast<T>(0))
                {
                    pivot = true;
                }
                else
                {
                    // Divide result of bi-th row by diagonal entry
                    val /= diag;
                }

                // Update remaining non-diagonal entries
                if(lid < block_dim)
                {
                    if(bi < lid)
                    {
                        local_sum = rocsparse::fma(-val, bsr_values[lid + bi * BSRDIM], local_sum);
                    }
                    else if(lid == bi)
                    {
                        local_sum = val;
                    }
                }
            }
        }

        if(lid < block_dim)
        {
            // Store the rows results in y
            y[row * block_dim + lid] = local_sum;
        }

        if(lid == 0)
        {
            // Write "row is done" flag
            __hip_atomic_store(&done_array[row], 1, __ATOMIC_RELEASE, __HIP_MEMORY_SCOPE_AGENT);

            // Find the minimum pivot, if applicable
            if(pivot == true)
            {
                rocsparse::atomic_min(zero_pivot, row + idx_base);
            }
        }
    }

    template <uint32_t BLOCKSIZE,
              uint32_t WFSIZE,
              uint32_t BSRDIM,
              bool     SLEEP,
              typename T,
              typename I,
              typename J>
    ROCSPARSE_DEVICE_ILF void bsrsv_upper_shared_device(J mb,
                                                        T alpha,
                                                        const I* __restrict__ bsr_row_ptr,
                                                        const J* __restrict__ bsr_col_ind,
                                                        const T* __restrict__ bsr_val,
                                                        J block_dim,
                                                        const T* __restrict__ x,
                                                        T* __restrict__ y,
                                                        int* __restrict__ done_array,
                                                        J* __restrict__ map,
                                                        J* __restrict__ zero_pivot,
                                                        rocsparse_index_base idx_base,
                                                        rocsparse_diag_type  diag_type,
                                                        rocsparse_direction  dir)
    {
        const int lid = hipThreadIdx_x & (WFSIZE - 1);
        const int wid = hipThreadIdx_x / WFSIZE;

        // Index into the row map
        const J idx = hipBlockIdx_x * BLOCKSIZE / WFSIZE + wid;

        // Do not run out of bounds
        if(idx >= mb)
        {
            return;
        }

        // Get the BSR row this wavefront will operate on
        const J row = map[idx];

        // Current row entry and exit point
        const I row_begin = bsr_row_ptr[row] - idx_base;
        const I row_end   = bsr_row_ptr[row + 1] - idx_base;

        // Initialize local_col with mb
        J local_col = mb;

        // Initialize local summation variable with alpha and x
        T local_sum = alpha * ((lid < block_dim) ? x[row * block_dim + lid] : static_cast<T>(0));

        // Shared memory to hold BSR blocks and updated sums
        __shared__ T sdata1[BLOCKSIZE / WFSIZE * BSRDIM * BSRDIM];
        __shared__ T sdata2[BLOCKSIZE / WFSIZE * BSRDIM];

        T* bsr_values  = &sdata1[wid * BSRDIM * BSRDIM];
        T* bsr_updates = &sdata2[wid * BSRDIM];

        // Loop over the current row
        I j;
        for(j = row_end - 1; j >= row_begin; --j)
        {
            // Current column index
            local_col = bsr_col_ind[j] - idx_base;

            // Load BSR block values
            // Each wavefront loads a full BSR block into shared memory
            // Pad remaining entries with zero
            const int bi = lid & (BSRDIM - 1);
            const int bj = lid / BSRDIM;

            for(J k = bj; k < BSRDIM; k += WFSIZE / BSRDIM)
            {
                bsr_values[bi + k * BSRDIM] = (bi < block_dim && k < block_dim)
                                                  ? bsr_val[BSR_IND(j, bi, k, dir)]
                                                  : static_cast<T>(0);
            }

            // Processing upper triangular

            // Ignore all diagonal entries and below
            if(local_col <= row)
            {
                break;
            }

            // Spin loop until dependency has been resolved
            rocsparse::spin_loop<SLEEP>(&done_array[local_col], __HIP_MEMORY_SCOPE_AGENT);

            // Wait for y to be visible globally
            __builtin_amdgcn_fence(__ATOMIC_ACQUIRE, "agent");

            // Load all updated dependencies into shared memory
            if(lid < BSRDIM)
            {
                bsr_updates[lid]
                    = (lid < block_dim) ? y[local_col * block_dim + lid] : static_cast<T>(0);
            }

            __threadfence_block();

            // Local sum computation
            if(lid < block_dim)
            {
                for(J l = 0; l < BSRDIM; ++l)
                {
                    local_sum
                        = rocsparse::fma(-bsr_values[lid + l * BSRDIM], bsr_updates[l], local_sum);
                }
            }
        }

        // Initialize zero pivot
        bool pivot = false;

        // Process diagonal
        if(local_col == row)
        {
            for(J bi = block_dim - 1; bi >= 0; --bi)
            {
                // Load diagonal matrix entry
                const T diag = (diag_type == rocsparse_diag_type_non_unit)
                                   ? bsr_values[bi + bi * BSRDIM]
                                   : static_cast<T>(1);

                // Load result of bi-th BSR row
                T val = rocsparse::shfl(local_sum, bi);

                // Check for numerical pivot
                if(diag == static_cast<T>(0))
                {
                    pivot = true;
                }
                else
                {
                    // Divide result of bi-th row by diagonal entry
                    val /= diag;
                }

                // Update remaining non-diagonal entries
                if(lid < block_dim)
                {
                    if(bi > lid)
                    {
                        local_sum = rocsparse::fma(-val, bsr_values[lid + bi * BSRDIM], local_sum);
                    }
                    else if(lid == bi)
                    {
                        local_sum = val;
                    }
                }
            }
        }

        if(lid < block_dim)
        {
            // Store the rows results in y
            y[row * block_dim + lid] = local_sum;
        }

        if(lid == 0)
        {
            // Write "row is done" flag
            __hip_atomic_store(&done_array[row], 1, __ATOMIC_RELEASE, __HIP_MEMORY_SCOPE_AGENT);

            // Find the minimum pivot, if applicable
            if(pivot == true)
            {
                rocsparse::atomic_min(zero_pivot, row + idx_base);
            }
        }
    }
}

rocsparse_status rocsparse::bsrsv_solve_dispatch(rocsparse_handle            handle,
                                                 rocsparse_operation         trans,
                                                 rocsparse_const_spmat_descr mat,
                                                 rocsparse_datatype alpha_device_host_datatype,
                                                 const void*        alpha_device_host,
                                                 int64_t            alpha_device_host_stride,
                                                 rocsparse_const_dnvec_descr x,
                                                 rocsparse_dnvec_descr       y,
                                                 rocsparse_solve_policy      policy,
                                                 void*                       temp_buffer)
{
    ROCSPARSE_ROUTINE_TRACE;

    // Stream
    hipStream_t stream = handle->stream;

    // Buffer
    char* ptr = reinterpret_cast<char*>(temp_buffer);

    ptr += 256;

    // done array
    int* done_array = reinterpret_cast<int*>(ptr);
    ptr += ((sizeof(int) * mb - 1) / 256 + 1) * 256;

    // Initialize buffers
    RETURN_IF_HIP_ERROR(hipMemsetAsync(done_array, 0, sizeof(int) * mb, stream));

    auto                   info       = mat->info;
    auto                   bsrsv_info = info->get_bsrsv_info();
    rocsparse::trm_info_t* trm_info   = info->get_bsrsv_info(trans, descr->fill_mode);
    if(trm_info == nullptr)
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_pointer);
    }

    // If diag type is unit, re-initialize zero pivot to remove structural zeros
    if(descr->diag_type == rocsparse_diag_type_unit)
    {
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse::assign_async(reinterpret_cast<J*>(bsrsv_info->get_zero_pivot()),
                                    std::numeric_limits<J>::max(),
                                    stream));
    }

    // Pointers to differentiate between transpose mode
    const rocsparse_int* local_bsr_row_ptr = bsr_row_ptr;
    const rocsparse_int* local_bsr_col_ind = bsr_col_ind;
    const T*             local_bsr_val     = bsr_val;
    rocsparse_fill_mode  fill_mode         = descr->fill_mode;

    // When computing transposed triangular solve, we first need to update the
    // transposed matrix values
    if(trans == rocsparse_operation_transpose)
    {
        T* bsrt_val = reinterpret_cast<T*>(ptr);

        // Gather transposed values
        LAUNCH_BSRSV_GTHR(256, 64, block_dim);

        local_bsr_row_ptr = (rocsparse_int*)trm_info->get_transposed_row_ptr();
        local_bsr_col_ind = (rocsparse_int*)trm_info->get_transposed_col_ind();
        local_bsr_val     = (T*)bsrt_val;

        fill_mode = (fill_mode == rocsparse_fill_mode_lower) ? rocsparse_fill_mode_upper
                                                             : rocsparse_fill_mode_lower;

        RETURN_IF_ROCSPARSE_ERROR(launch_bsrsv_solve_kernel(handle, alpha, mat, x, y));
    }
    else
    {
        RETURN_IF_ROCSPARSE_ERROR(launch_bsrsv_solve_kernel(handle, alpha, mat, x, y));
    }

    // Determine gcn_arch and ASIC revision
    const std::string gcn_arch_name = rocsparse::handle_get_arch_name(handle);
    const int         asicRev       = handle->asic_rev;

    // Launch shared memory based kernel for small BSR block dimensions
    launch_bsrsv_kernel(fill_mode, 128, handle->wavefront_size, block_dim, gcn_arch_name, asicRev);
    return rocsparse_status_success;
}

static rocsparse_status rocsparse::bsrsv_buffer_size(rocsparse_handle            handle,
                                                     rocsparse_operation         trans,
                                                     rocsparse_const_spmat_descr mat,
                                                     size_t*                     buffer_size)
{
    ROCSPARSE_ROUTINE_TRACE;
    RETURN_IF_ROCSPARSE_ERROR(
        rocsparse::csrsv_analysis_buffer_size(handle, trans, mat, buffer_size));
    return rocsparse_status_success;
}

static rocsparse_status rocsparse::bsrsv_solve_buffer_size(rocsparse_handle            handle,
                                                           rocsparse_operation         trans,
                                                           rocsparse_const_spmat_descr mat,
                                                           size_t*                     buffer_size)
{
    ROCSPARSE_ROUTINE_TRACE;
    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_solve_buffer_size(handle, trans, mat, &buffer_size));
    if(trans == rocsparse_operation_transpose)
    {
        /* Remove additional CSR buffer */
        *buffer_size -= ((sizeof(T) * mat->nnz - 1) / 256 + 1) * 256;
        /* Add BSR buffer instead */
        *buffer_size
            += ((sizeof(T) * mat->nnz * mat->block_dim * mat->block_dim - 1) / 256 + 1) * 256;
    }
    return rocsparse_status_success;
}

rocsparse_status rocsparse::bsrsv_strided_batched_analysis(rocsparse_handle          handle,
                                                           rocsparse_operation       trans,
                                                           rocsparse_spmat_descr     mat,
                                                           rocsparse_analysis_policy analysis,
                                                           rocsparse_solve_policy    solve,
                                                           void*                     temp_buffer)
{
    ROCSPARSE_ROUTINE_TRACE;
    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_ENUM(1, trans);
    ROCSPARSE_CHECKARG_POINTER(2, mat);
    ROCSPARSE_CHECKARG_ENUM(3, analysis);
    ROCSPARSE_CHECKARG_ENUM(4, solve);

    ROCSPARSE_CHECKARG(
        2,
        trans,
        (trans != rocsparse_operation_none && trans != rocsparse_operation_transpose),
        rocsparse_status_not_implemented);

    rocsparse_mat_descr descr = mat->descr;
    // Check matrix type
    ROCSPARSE_CHECKARG(
        2, descr, (descr->type != rocsparse_matrix_type_general), rocsparse_status_not_implemented);

    // Check matrix sorting mode

    ROCSPARSE_CHECKARG(2,
                       descr,
                       (descr->storage_mode != rocsparse_storage_mode_sorted),
                       rocsparse_status_requires_sorted_storage);

    // Quick return if possible
    if(mat->rows == 0)
    {
        return rocsparse_status_success;
    }

    rocsparse_mat_info info = mat->info;

    if(analysis == rocsparse_analysis_policy_reuse)
    {
        auto trm = info->get_bsrsv_info(trans, descr->fill_mode);
        if((descr->fill_mode == rocsparse_fill_mode_lower) && (trans == rocsparse_operation_none))
        {
            trm = (trm != nullptr) ? trm : info->get_bsric0_info(trans, descr->fill_mode);
            trm = (trm != nullptr) ? trm : info->get_bsrilu0_info(trans, descr->fill_mode);
            trm = (trm != nullptr) ? trm : info->get_bsrsm_info(trans, descr->fill_mode);
        }

        if(trm != nullptr)
        {
            info->set_bsrsv_info(trans, descr->fill_mode, trm);
            return rocsparse_status_success;
        }
    }

    auto bsrsv_info = info->get_bsrsv_info();
    RETURN_IF_ROCSPARSE_ERROR(bsrsv_info->recreate(handle,
                                                   trans,
                                                   mat->rows,
                                                   mat->nnz,
                                                   mat->descr,
                                                   mat->val_data,
                                                   mat->row_data,
                                                   mat->col_data,
                                                   temp_buffer));
    return rocsparse_status_success;
}
