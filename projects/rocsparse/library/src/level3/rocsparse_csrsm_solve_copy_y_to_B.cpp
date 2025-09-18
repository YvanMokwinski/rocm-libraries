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
#include "rocsparse_csrsm_solve_copy_y_to_B.hpp"
#include "rocsparse_control.hpp"
#include "rocsparse_handle.hpp"

namespace rocsparse
{
    template <uint32_t BLOCKSIZE, typename T>
    __device__ static void
        csrsm_solve_copy_y_to_B_device(const int64_t m, T* B, const int64_t ldb, const T* y)
    {
        const size_t tid = hipBlockIdx_x * BLOCKSIZE + hipThreadIdx_x;
        if(tid < m)
        {
            B[tid * ldb] = y[tid];
        }
    }

    template <uint32_t BLOCKSIZE, typename T>
    __launch_bounds__(BLOCKSIZE) __global__ static void csrsm_solve_copy_y_to_B_kernel(
        const int64_t m, T* B, const int64_t ldb, int64_t B_stride, const T* y, int64_t y_stride)
    {
        const auto i = hipBlockIdx_y;
        csrsm_solve_copy_y_to_B_device<BLOCKSIZE, T>(m, B + i * B_stride, ldb, y + i * y_stride);
    }

    template <typename T>
    static rocsparse_status csrsm_solve_copy_y_to_B_launch(rocsparse_handle handle,
                                                           const int64_t    batch_count,
                                                           const int64_t    m,
                                                           void*            B,
                                                           const int64_t    ldb,
                                                           const int64_t    B_stride,
                                                           const void*      y,
                                                           const int64_t    y_stride)
    {
        static constexpr uint32_t BLOCKSIZE = 1024;
        RETURN_IF_HIPLAUNCHKERNELGGL_ERROR((csrsm_solve_copy_y_to_B_kernel<BLOCKSIZE>),
                                           dim3((m - 1) / BLOCKSIZE + 1, batch_count),
                                           dim3(BLOCKSIZE),
                                           0,
                                           handle->stream,
                                           m,
                                           reinterpret_cast<T*>(B),
                                           ldb,
                                           B_stride,
                                           reinterpret_cast<const T*>(y),
                                           y_stride);
        return rocsparse_status_success;
    }

    typedef rocsparse_status (*launch_t)(rocsparse_handle handle,
                                         int64_t          batch_count,
                                         int64_t          m,
                                         void*            B,
                                         int64_t          ldb,
                                         int64_t          B_stride,
                                         const void*      y,
                                         int64_t          y_stride);

    static launch_t find(rocsparse_datatype datatype)
    {
        switch(datatype)
        {
        case rocsparse_datatype_f32_r:
        {
            return csrsm_solve_copy_y_to_B_launch<float>;
        }
        case rocsparse_datatype_f32_c:
        {
            return csrsm_solve_copy_y_to_B_launch<rocsparse_float_complex>;
        }
        case rocsparse_datatype_f64_r:
        {
            return csrsm_solve_copy_y_to_B_launch<double>;
        }
        case rocsparse_datatype_f64_c:
        {
            return csrsm_solve_copy_y_to_B_launch<rocsparse_double_complex>;
        }
        default:
        {
            return nullptr;
        }
        }
    }
}

rocsparse_status rocsparse::csrsm_solve_copy_y_to_B(rocsparse_handle   handle,
                                                    const int64_t      m,
                                                    rocsparse_datatype datatype,
                                                    void*              B,
                                                    const int64_t      ldb,
                                                    const void*        y)
{
    rocsparse::launch_t launch = rocsparse::find(datatype);
    if(launch != nullptr)
    {
        RETURN_IF_ROCSPARSE_ERROR(launch(handle,
                                         m,
                                         static_cast<int64_t>(1),
                                         B,
                                         ldb,
                                         static_cast<int64_t>(0),
                                         y,
                                         static_cast<int64_t>(0)));
        return rocsparse_status_success;
    }
    return rocsparse_status_internal_error;
}

rocsparse_status rocsparse::csrsm_strided_batched_solve_copy_y_to_B(rocsparse_handle   handle,
                                                                    const int64_t      batch_count,
                                                                    const int64_t      m,
                                                                    rocsparse_datatype datatype,
                                                                    void*              B,
                                                                    const int64_t      ldb,
                                                                    const int64_t      B_stride,
                                                                    const void*        y,
                                                                    const int64_t      y_stride)
{
    rocsparse::launch_t launch = rocsparse::find(datatype);
    if(launch != nullptr)
    {
        RETURN_IF_ROCSPARSE_ERROR(launch(handle, m, batch_count, B, ldb, B_stride, y, y_stride));
        return rocsparse_status_success;
    }
    return rocsparse_status_internal_error;
}
