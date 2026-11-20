/*! \file */
/* ************************************************************************
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights Reserved.
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

#include "hipdssContext.hpp"
#include "hipdssMatrix.hpp"
#include "hipdss_control.hpp"
#include <algorithm>
#include <hip/hip_complex.h>

namespace hipdss
{
    template <uint32_t BLOCKSIZE, typename T, typename J>
    HIPDSS_DEVICE_ILF void device_permute_row_col_major(int64_t M,
                                                        int64_t N,
                                                        const T* __restrict__ s_val,
                                                        int64_t s_ld,
                                                        T* __restrict__ t_val,
                                                        int64_t t_ld,
                                                        const J* __restrict__ row_perm,
                                                        J perm_base)
    {
        const auto gid = hipThreadIdx_x + int64_t(BLOCKSIZE) * hipBlockIdx_x;
        if(gid < M * N)
        {
            const auto row          = gid % M;
            const auto col          = gid / M;
            t_val[col * t_ld + row] = s_val[col * s_ld + (row_perm[row] - perm_base)];
        }
    }

    template <uint32_t BLOCKSIZE, typename T, typename J>
    HIPDSS_DEVICE_ILF void device_permute_row_row_major(int64_t M,
                                                        int64_t N,
                                                        const T* __restrict__ s_val,
                                                        int64_t s_ld,
                                                        T* __restrict__ t_val,
                                                        int64_t t_ld,
                                                        const J* __restrict__ row_perm,
                                                        J perm_base)
    {
        const auto gid = hipThreadIdx_x + int64_t(BLOCKSIZE) * hipBlockIdx_x;
        if(gid < M * N)
        {
            const auto row          = gid / N;
            const auto col          = gid % N;
            t_val[row * t_ld + col] = s_val[(row_perm[row] - perm_base) * s_ld + col];
        }
    }

    template <uint32_t BLOCKSIZE, typename T, typename J>
    HIPDSS_KERNEL(BLOCKSIZE)
    void kernel_permute_row_col_major(int64_t     M,
                                      int64_t     N,
                                      int64_t     batch_count,
                                      const void* s_val_,
                                      int64_t     s_ld,
                                      int64_t     s_batch_stride,
                                      void*       t_val_,
                                      int64_t     t_ld,
                                      int64_t     t_batch_stride,
                                      const void* row_perm_,
                                      int64_t     perm_base)
    {
        const T* s_val = reinterpret_cast<const T*>(s_val_);
        T*       t_val = reinterpret_cast<T*>(t_val_);
        // gridDim.y is capped, so stride over the batch.
        for(int64_t batch_index = hipBlockIdx_y; batch_index < batch_count;
            batch_index += hipGridDim_y)
        {
            device_permute_row_col_major<BLOCKSIZE, T, J>(M,
                                                          N,
                                                          s_val + s_batch_stride * batch_index,
                                                          s_ld,
                                                          t_val + t_batch_stride * batch_index,
                                                          t_ld,
                                                          reinterpret_cast<const J*>(row_perm_),
                                                          static_cast<J>(perm_base));
        }
    }

    template <uint32_t BLOCKSIZE, typename T, typename J>
    HIPDSS_KERNEL(BLOCKSIZE)
    void kernel_permute_row_row_major(int64_t     M,
                                      int64_t     N,
                                      int64_t     batch_count,
                                      const void* s_val_,
                                      int64_t     s_ld,
                                      int64_t     s_batch_stride,
                                      void*       t_val_,
                                      int64_t     t_ld,
                                      int64_t     t_batch_stride,
                                      const void* row_perm_,
                                      int64_t     perm_base)
    {
        const T* s_val = reinterpret_cast<const T*>(s_val_);
        T*       t_val = reinterpret_cast<T*>(t_val_);
        // gridDim.y is capped, so stride over the batch.
        for(int64_t batch_index = hipBlockIdx_y; batch_index < batch_count;
            batch_index += hipGridDim_y)
        {
            device_permute_row_row_major<BLOCKSIZE, T, J>(M,
                                                          N,
                                                          s_val + s_batch_stride * batch_index,
                                                          s_ld,
                                                          t_val + t_batch_stride * batch_index,
                                                          t_ld,
                                                          reinterpret_cast<const J*>(row_perm_),
                                                          static_cast<J>(perm_base));
        }
    }

    template <uint32_t BLOCKSIZE>
    using kernel_permute_row_t
        = decltype(&kernel_permute_row_col_major<BLOCKSIZE, double, int32_t>);

    template <uint32_t BLOCKSIZE, typename T>
    static kernel_permute_row_t<BLOCKSIZE> select_kernel(hipdssLayout_t layout,
                                                         bool           use_int64)
    {
        switch(layout)
        {
        case HIPDSS_LAYOUT_COL_MAJOR:
            return use_int64 ? kernel_permute_row_col_major<BLOCKSIZE, T, int64_t>
                             : kernel_permute_row_col_major<BLOCKSIZE, T, int32_t>;
        case HIPDSS_LAYOUT_ROW_MAJOR:
            return use_int64 ? kernel_permute_row_row_major<BLOCKSIZE, T, int64_t>
                             : kernel_permute_row_row_major<BLOCKSIZE, T, int32_t>;
        }
        return nullptr;
    }

    template <uint32_t BLOCKSIZE>
    static kernel_permute_row_t<BLOCKSIZE>
        select_kernel(hipdssLayout_t layout,
                      hipDataType    val_type,
                      hipDataType    indextype)
    {
        if(indextype != HIP_R_32I && indextype != HIP_R_64I)
        {
            return nullptr;
        }

        const bool use_int64 = (indextype == HIP_R_64I);
        switch(val_type)
        {
        case HIP_R_32F:
            return select_kernel<BLOCKSIZE, float>(layout,
                                                   use_int64);
        case HIP_R_64F:
            return select_kernel<BLOCKSIZE, double>(layout,
                                                    use_int64);
        case HIP_C_32F:
            return select_kernel<BLOCKSIZE, hipFloatComplex>(layout,
                                                             use_int64);
        case HIP_C_64F:
            return select_kernel<BLOCKSIZE, hipDoubleComplex>(layout,
                                                              use_int64);
        default:
            return nullptr;
        }
    }
} // namespace hipdss

hipdssStatus_t hipdssMatrixRowPermutation(hipdssHandle_t        handle,
                                          hipdssMatrix_t        source,
                                          hipdssMatrix_t        target,
                                          hipdss::idvec_descr_t permutation)
{

    if(handle == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    if(source == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    if(target == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    if(permutation == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    if(!source->is_dense())
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    if(!target->is_dense())
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    const int64_t nrows = source->get_nrows();
    const int64_t ncols = source->get_ncols();

    if(target->get_nrows() != nrows)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    if(target->get_ncols() != ncols)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    const hipdssLayout_t layout = source->get_layout();
    if(target->get_layout() != layout)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    const hipDataType val_type = source->get_val_type();
    if(target->get_val_type() != val_type)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    const int64_t min_ld = (layout == HIPDSS_LAYOUT_COL_MAJOR) ? nrows : ncols;
    if(source->get_ld() < min_ld)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    if(target->get_ld() < min_ld)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    if(permutation->get_size() != nrows)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    const hipdssIndexBase_t perm_base = permutation->get_base();
    if(perm_base != HIPDSS_BASE_ZERO && perm_base != HIPDSS_BASE_ONE)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    const int64_t batch_count = source->get_batch_count();
    if(batch_count < 0)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    if(target->get_batch_count() != batch_count)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    const int64_t min_batch_stride
        = (batch_count > 1) ? ((layout == HIPDSS_LAYOUT_COL_MAJOR) ? source->get_ld() * ncols
                                                                   : source->get_ld() * nrows)
                            : 0;
    if(source->get_batch_stride() < min_batch_stride)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    const int64_t min_target_batch_stride
        = (batch_count > 1) ? ((layout == HIPDSS_LAYOUT_COL_MAJOR) ? target->get_ld() * ncols
                                                                   : target->get_ld() * nrows)
                            : 0;
    if(target->get_batch_stride() < min_target_batch_stride)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    if(nrows == 0 || ncols == 0 || batch_count == 0)
    {
        return HIPDSS_STATUS_SUCCESS;
    }

    const void* s_values = source->get_const_val_data();
    if(s_values == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    void* t_values = target->get_val_data();
    if(t_values == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    const void* p_values = permutation->const_device_data();
    if(p_values == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    static constexpr uint32_t BLOCKSIZE = 1024;
    const auto                kernel    = hipdss::select_kernel<BLOCKSIZE>(layout,
                                                         val_type,
                                                         permutation->get_indextype());
    if(kernel == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_NOT_SUPPORTED);
    }

    static constexpr int64_t max_grid_y = 65535;
    RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(kernel,
                                       dim3((nrows * ncols - 1) / BLOCKSIZE + 1,
                                            std::min(batch_count, max_grid_y)),
                                       dim3(BLOCKSIZE),
                                       0,
                                       handle->get_stream(),
                                       nrows,
                                       ncols,
                                       batch_count,
                                       s_values,
                                       source->get_ld(),
                                       source->get_batch_stride(),
                                       t_values,
                                       target->get_ld(),
                                       target->get_batch_stride(),
                                       p_values,
                                       static_cast<int64_t>(perm_base));
    return HIPDSS_STATUS_SUCCESS;
}
