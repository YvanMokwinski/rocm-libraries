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

#include "hipdss_spequilibrate_ruiz.hpp"
#include "../hipdss_spequilibrate_descr.hpp"
#include "hipdssContext.hpp"
#include "hipdss_control.hpp"
namespace hipdss
{
    template <typename T>
    __device__ __forceinline__ T atomic_min(T* ptr, T val)
    {
        return atomicMin(ptr, val);
    }

    template <typename T>
    __device__ __forceinline__ T atomic_max(T* ptr, T val)
    {
        return atomicMax(ptr, val);
    }

    __device__ __forceinline__ float sqrt(float val)
    {
        return std::sqrt(val);
    }
    __device__ __forceinline__ double sqrt(double val)
    {
        return std::sqrt(val);
    }

    __device__ __forceinline__ float abs(float x)
    {
        return x < 0.0f ? -x : x;
    }
    __device__ __forceinline__ double abs(double x)
    {
        return x < 0.0 ? -x : x;
    }
    __device__ __forceinline__ float abs(rocsparse_float_complex x)
    {
        return std::abs(x);
    }
    __device__ __forceinline__ double abs(rocsparse_double_complex x)
    {
        return std::abs(x);
    }

    __device__ __forceinline__ rocsparse_float_complex sqrt(rocsparse_float_complex val)
    {
        float x = std::real(val);
        float y = std::imag(val);

        float sgnp = (y < 0.0f) ? -1.0f : 1.0f;
        float absz = hipdss::abs(val);

        return rocsparse_float_complex(std::sqrt((absz + x) * 0.5f),
                                       sgnp * std::sqrt((absz - x) * 0.5f));
    }
    __device__ __forceinline__ rocsparse_double_complex sqrt(rocsparse_double_complex val)
    {
        double x = std::real(val);
        double y = std::imag(val);

        double sgnp = (y < 0.0) ? -1.0 : 1.0;
        double absz = hipdss::abs(val);

        return rocsparse_double_complex(std::sqrt((absz + x) * 0.5),
                                        sgnp * std::sqrt((absz - x) * 0.5));
    }

}

template <uint32_t BLOCKSIZE, typename J, typename T, typename U>
HIPDSS_DEVICE_ILF void ruiz_part0_device(int64_t m, int64_t n, U* __restrict__ D_left, U* __restrict__ D_right)
{
    const auto gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
    if(gid < m)
    {
        D_left += gid;
        *D_left = static_cast<U>(1);
    }

    if(gid < n)
    {
        D_right += gid;
        *D_right = static_cast<U>(1);
    }
}

template <uint32_t BLOCKSIZE, typename J, typename T, typename U>
HIPDSS_KERNEL(BLOCKSIZE)
void ruiz_part0_kernel(int64_t m, int64_t n, U* __restrict__ D_left, int64_t D_left_batch_dist, U* __restrict__ D_right, int64_t D_right_batch_dist)
{
    const auto batch_index = hipBlockIdx_y;
    D_left += D_left_batch_dist * batch_index;
    D_right += D_right_batch_dist * batch_index;
    ruiz_part0_device<BLOCKSIZE, J, T, U>(m, n, D_left, D_right);
}

//
// Compute max column with atomics.
//
template <uint32_t BLOCKSIZE, typename J, typename T, typename U>
HIPDSS_DEVICE_ILF void ruiz_part1_device(int64_t nnz, rocsparse_index_base base, const J* __restrict__ ind, const T* __restrict__ val, U* __restrict__ col_max)
{
    const auto gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
    if(gid < nnz)
    {
        ind += gid;
        val += gid;
        hipdss::atomic_max(col_max + ind[0] - base, std::abs(val[0]));
    }
}

template <uint32_t BLOCKSIZE, typename J, typename T, typename U>
HIPDSS_KERNEL(BLOCKSIZE)
void ruiz_part1_kernel(int64_t nnz, rocsparse_index_base base, const J* __restrict__ ind, const T* __restrict__ val, int64_t val_batch_dist, U* __restrict__ col_max, int64_t col_max_batch_dist, int32_t* __restrict__ converged, int64_t converged_stride)
{
    const auto batch_index = hipBlockIdx_y;
    if(!converged[batch_index * converged_stride])
    {
        val += val_batch_dist * batch_index;
        col_max += col_max_batch_dist * batch_index;

        ruiz_part1_device<BLOCKSIZE>(nnz, base, ind, val, col_max);
    }
}

//
// Update D_right.
//
template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
HIPDSS_DEVICE_ILF void
    ruiz_part2_device(int64_t n, rocsparse_index_base base, const J* __restrict__ ind, const T* __restrict__ val, U* __restrict__ col_max, U* __restrict__ D, double* __restrict__ nrm, const double* __restrict__ tol)
{
    static constexpr U s_one = static_cast<U>(1);
    const int64_t      gid   = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
    if(gid < n)
    {
        const auto mx = col_max[gid];
        const U    s  = (mx > 0) ? static_cast<U>(1) / hipdss::sqrt(U(mx)) : static_cast<U>(1);
        const U    c  = (mx > 0) ? (s_one - mx) : static_cast<U>(0);
        if(c > *tol)
        {
            D[gid] *= s;
            col_max[gid] = s;
            hipdss::atomic_max(nrm, static_cast<double>(c));
        }
        else
        {
            col_max[gid] = static_cast<U>(1);
        }
    }
}

template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
HIPDSS_KERNEL(BLOCKSIZE)
void ruiz_part2_kernel(int64_t n, rocsparse_index_base base, const J* __restrict__ ind, const T* __restrict__ val, int64_t val_dist, U* __restrict__ col_max, int64_t col_max_stride, U* __restrict__ D, int64_t D_dist, double* __restrict__ nrm, int64_t nrm_stride, int32_t* __restrict__ converged, int64_t converged_stride, const double* tol, int64_t tol_dist)
{
    const auto batch_index = hipBlockIdx_y;
    if(!converged[batch_index * converged_stride])
    {
        ruiz_part2_device<BLOCKSIZE, I, J, T, U>(
            n, base, ind, val + val_dist * batch_index, col_max + col_max_stride * batch_index, D + D_dist * batch_index, nrm + nrm_stride * batch_index, tol + tol_dist * batch_index);
#if 0
      if (tid == 0)
	{
	  rocsparse::atomic_max(converged+batch_index * converged_stride, static_cast<int32_t>((*(nrm + nrm_stride * batch_index) <= *(tol + tol_dist * batch_index)) ? 1 : 0));
	}
#endif
    }
}

//
// Update D_left and A..
//
template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
HIPDSS_DEVICE_ILF void
    ruiz_part3_device(int64_t m, rocsparse_index_base base, const I* __restrict__ ptr, const J* __restrict__ ind, T* __restrict__ val, U* __restrict__ D_left, const U* __restrict__ D_right_factor, double* __restrict__ nrm, const double* __restrict__ tol)
{
    static constexpr U s_one = static_cast<U>(1);
    const int64_t      gid   = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
    if(gid < m)
    {
        const I b    = ptr[gid] - base;
        const I size = ptr[gid + 1] - base - b;
        val += b;
        ind += b;

        U mx = static_cast<U>(0);
        for(I k = 0; k < size; ++k)
        {
            const U e = static_cast<U>(std::abs(val[k]));
            mx        = (e > mx) ? e : mx;
        }

        const U s = (mx > 0) ? static_cast<U>(1) / hipdss::sqrt(U(mx)) : static_cast<U>(1);
        const U c = (mx > 0) ? (s_one - mx) : static_cast<U>(0);

        //
        // Update.
        //
        if(c > *tol)
        {
            D_left[gid] *= s;
            for(I k = 0; k < size; ++k)
            {
                val[k] *= s * D_right_factor[ind[k] - base];
            }
            hipdss::atomic_max(nrm, static_cast<double>(c));
        }
        else
        {
            for(I k = 0; k < size; ++k)
            {
                // hipdss::atomic_min(converged, static_cast<int32_t>((c <= tol) ? 1 :
                // 0));
                val[k] *= D_right_factor[ind[k] - base];
            }
        }
    }
}

template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
HIPDSS_KERNEL(BLOCKSIZE)
void ruiz_part3_kernel(int64_t m, rocsparse_index_base base, const I* __restrict__ ptr, const J* __restrict__ ind, T* __restrict__ val, int64_t val_batch_dist, U* __restrict__ D_left, int64_t D_left_batch_dist, const U* __restrict__ D_right_factor, int64_t D_right_factor_batch_dist, double* __restrict__ nrm, int64_t nrm_stride, int32_t* __restrict__ converged, int64_t converged_stride, const double* tol, int64_t tol_dist, int32_t* stop)
{
    const int64_t batch_index = hipBlockIdx_y;
    if(!converged[batch_index * converged_stride])
    {
        ruiz_part3_device<BLOCKSIZE>(
            m, base, ptr, ind, val + val_batch_dist * batch_index, D_left + D_left_batch_dist * batch_index, D_right_factor + D_right_factor_batch_dist * batch_index, nrm + nrm_stride * batch_index, tol + tol_dist * batch_index);
#if 0
      __syncthreads();
      const int64_t tid = hipThreadIdx_x;
      if (tid == 0)
	{
	  rocsparse::atomic_max(converged+batch_index * converged_stride, static_cast<int32_t>((*(nrm + nrm_stride * batch_index) <= *(tol + tol_dist * batch_index)) ? 1 : 0));
	  //	  rocsparse::atomic_max(stop,*(converged+batch_index * converged_stride));
	}
#endif
        //      rocsparse::atomic_min(converged + converged_stride * batch_index, (
        //      *(nrm + nrm_stride * batch_index) > tol ) ? : );
    }
}

template <uint32_t BLOCKSIZE>
HIPDSS_KERNEL(BLOCKSIZE)
void ruiz_stop_kernel(int64_t batch_count, const double* __restrict__ nrm, int64_t nrm_stride, int32_t* __restrict__ converged, int64_t converged_stride, const double* tol, int64_t tol_dist, int32_t* stop)
{
    const int64_t tid = hipThreadIdx_x;
    if(tid == 0 && hipBlockIdx_x == 0)
    {

        __threadfence_system();
        stop[0] = 1;
        __threadfence_system();

        for(int32_t i = 0; i < batch_count; ++i)
        {
            converged[i] = 1;
            auto c       = nrm[i * nrm_stride];
            if(c > tol[tol_dist * i])
            {
                converged[i] = 0;
            }
        }
        for(int32_t i = 0; i < batch_count; ++i)
        {
            if(!converged[i])
            {
                __threadfence_system();
                stop[0] = 0;
                __threadfence_system();
                break;
            }
        }
    }
}

template <typename I, typename J, typename T, typename U>
hipdssStatus_t
    ruiz_run_iter(hipdssHandle_t handle, rocsparse_spmat_descr A, rocsparse_dnvec_descr D_left, rocsparse_dnvec_descr D_right, rocsparse_const_dnvec_descr Tol, size_t buffer_size_in_bytes, void* buffer, int32_t* converged, int32_t* stop)
{
    rocsparse_format A_format;

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_get_format(A, &A_format));

    int64_t              A_rows{};
    int64_t              A_cols{};
    int64_t              A_nnz{};
    void*                A_row_data{};
    void*                A_col_data{};
    void*                A_val_data{};
    rocsparse_indextype  A_row_indextype{};
    rocsparse_indextype  A_col_indextype{};
    rocsparse_index_base A_base{};
    rocsparse_datatype   A_val_datatype{};
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_csr_get(
        A, &A_rows, &A_cols, &A_nnz, &A_row_data, &A_col_data, &A_val_data, &A_row_indextype, &A_col_indextype, &A_base, &A_val_datatype));
    int32_t A_batch_count;
    RETURN_IF_ROCSPARSE_ERROR(
        rocsparse_spmat_get_strided_batch(A, &A_batch_count));

    // WTF
    int64_t A_batch_stride = A_nnz;

#if 0
  WTF/;/
  RETURN_IF_ROCSPARSE_ERROR(rocsparse__get_csr_strided_batch(A,
							      &A_batch_count,
							      &batch_stride));
#endif

    const int64_t m                   = A_rows;
    const int64_t n                   = A_cols;
    const int64_t nnz                 = A_nnz;
    const auto    base                = A_base;
    auto          row_data            = reinterpret_cast<const I* __restrict__>(A_row_data);
    auto          col_data            = reinterpret_cast<const J* __restrict__>(A_col_data);
    auto          val_data            = reinterpret_cast<T* __restrict__>(A_val_data);
    const int64_t batch_count         = A_batch_count;
    const int64_t val_data_batch_dist = A_batch_stride;
    const int64_t col_max_batch_dist  = A_cols;

    if(buffer_size_in_bytes < ((batch_count * sizeof(int32_t) - 1) / 256 + 1) * 256 + ((batch_count * sizeof(double) - 1) / 256 + 1) * 256 + ((batch_count * sizeof(U) - 1) / 256 + 1) * 256)
    {
        RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE,
                                            "buffer_size_in_bytes is too small");
    }

    //
    char* c   = reinterpret_cast<char*>(buffer); //
    auto  nrm = reinterpret_cast<double*>(c);
    c += ((batch_count * sizeof(double) - 1) / 256 + 1) * 256;
    auto col_max = reinterpret_cast<U*>(c);
    c += ((n * sizeof(U) - 1) / 256 + 1) * 256; //
    //  rocsparse_pointer_mode tol_pointer_mode  = rocsparse_pointer_mode_host;

    int32_t     tol_batch_count{};
    int64_t     tol_batch_dist{};
    const void* Tol_const_values{};
    if(Tol)
    {
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse_const_dnvec_get_values(Tol, &Tol_const_values));
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_get_strided_batch(
            Tol, &tol_batch_count, &tol_batch_dist));
    }
    const double* tol = reinterpret_cast<const double*>(Tol_const_values);

    double h;
    THROW_IF_HIP_ERROR(hipMemcpy(&h, tol, sizeof(double), hipMemcpyDefault));
    //
    const int64_t nrm_stride       = 1;
    const int64_t converged_stride = 1;

    int32_t D_right_batch_count;
    int64_t D_right_batch_dist;
    void*   D_right_values{};
    RETURN_IF_ROCSPARSE_ERROR(
        rocsparse_dnvec_get_values(D_right, &D_right_values));
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_get_strided_batch(
        D_right, &D_right_batch_count, &D_right_batch_dist));
    int32_t D_left_batch_count;
    int64_t D_left_batch_dist;
    void*   D_left_values{};
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_get_values(D_left, &D_left_values));
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_get_strided_batch(
        D_left, &D_left_batch_count, &D_left_batch_dist));

    //
    // Part 1: Compute maximum columns.
    //

    dim3 part1_grid_blocks(((A_nnz - 1) / 512 + 1) * 512, batch_count);
    dim3 part1_grid_threads(512);
    dim3 part2_grid_blocks(((A_cols - 1) / 512 + 1) * 512, batch_count);
    dim3 part2_grid_threads(512);
    dim3 part3_grid_blocks(((A_rows - 1) / 512 + 1) * 512, batch_count);
    dim3 part3_grid_threads(512);

    //
    // CHANGE, maybe we can't.
    //
    RETURN_IF_HIP_ERROR(
        hipMemsetAsync(col_max, 0, n * sizeof(U), handle->get_stream()));
    RETURN_IF_HIP_ERROR(
        hipMemsetAsync(nrm, 0, batch_count * sizeof(double), handle->get_stream()));
    stop[0] = 0;

    RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
        (ruiz_part1_kernel<512, J, T, U>), part1_grid_blocks, part1_grid_threads, 0, handle->get_stream(),
        //
        nnz,
        base,
        col_data,
        val_data,
        val_data_batch_dist,
        col_max,
        col_max_batch_dist,
        nullptr,
        converged_stride);

    //
    // Part 2: Update D_right and compute part of the stopping criteria.
    //

    RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
        (ruiz_part2_kernel<512, I, J, T, U>), part2_grid_blocks, part2_grid_threads, 0, handle->get_stream(),
        //
        n,
        base,
        col_data,
        val_data,
        val_data_batch_dist,
        col_max,
        col_max_batch_dist,
        reinterpret_cast<U*>(D_right_values),
        D_right_batch_dist,
        nrm,
        nrm_stride,
        nullptr,
        converged_stride,
        tol,
        tol_batch_dist);

    //
    // Part 3: Update D_left and update A.
    //
    RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
        (ruiz_part3_kernel<512, I, J, T, U>), part3_grid_blocks, part3_grid_threads, 0, handle->get_stream(),
        //
        m,
        base,
        row_data,
        col_data,
        val_data,
        val_data_batch_dist,
        reinterpret_cast<U*>(D_left_values),
        D_left_batch_dist,
        col_max,
        col_max_batch_dist,
        // reinterpret_cast<const U*>(D_right->const_values),
        // D_right_batch_dist,
        nrm,
        nrm_stride,
        converged,
        converged_stride,
        tol,
        tol_batch_dist,
        nullptr);

    RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
        (ruiz_stop_kernel<64>), dim3(1), dim3(64), 0, handle->get_stream(), batch_count, nrm, nrm_stride, converged, converged_stride, tol, tol_batch_dist, stop);
    //  hipStreamSynchronize(handle->get_stream());
    //  volatile int32_t * v = (volatile int32_t*)stop;
    for(int i = 0; i < 1; ++i)
        std::cout << "stop " << stop[0] << std::endl;
    //  RETURN_IF_HIP_ERROR(hipMemcpy(stop,converged,sizeof(int32_t),hipMemcpyDefault));
    return HIPDSS_STATUS_SUCCESS;
}

namespace hipdss
{

    hipdssStatus_t
        spequilibrate_ruiz(hipdssHandle_t handle, hipdss_spequilibrate_descr descr_, hipdss_spequilibrate_stage stage, rocsparse_spmat_descr A, rocsparse_dnvec_descr D_left, rocsparse_dnvec_descr D_right, size_t buffer_size_in_bytes, void* buffer)
    {
        auto descr = reinterpret_cast<_hipdss_spequilibrate_ruiz_descr*>(descr_->get_impl());

        auto Tol = descr->get_batch_tolerances();

#if 0
    RETURN_IF_ROCSPARSE_ERROR((( buffer_size_in_bytes < descr->get_buffer_size_in_bytes())
			       ? rocsparse_status_invalid_size
			       : rocsparse_status_success));

    RETURN_IF_HIPDSS_ERROR((D_left->data_type != D_right->data_type)
			    ? HIPDSS_STATUS_INVALID_VALUE
			    : HIPDSS_STATUS_SUCCESS);
#endif

        int64_t A_rows{}, A_cols{}, A_nnz{};
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse_spmat_get_size(A, &A_rows, &A_cols, &A_nnz));
        //
        // Set converged to zero.
        //
        //  c+= ((batch_count * sizeof(int32_t) - 1) / 256 + 1)*256;

        int32_t A_batch_count;
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse_spmat_get_strided_batch(A, &A_batch_count));

        int32_t* stop;
        RETURN_IF_HIP_ERROR(hipHostMalloc(&stop, sizeof(int32_t)));

        int32_t* converged;
        RETURN_IF_HIP_ERROR(hipMallocAsync(
            &converged, sizeof(int32_t) * A_batch_count, handle->get_stream()));

        RETURN_IF_HIP_ERROR(hipMemsetAsync(
            converged, 0, A_batch_count * sizeof(int32_t), handle->get_stream()));

        void* D_right_values{};
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse_dnvec_get_values(D_right, &D_right_values));
        void* D_left_values{};
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_get_values(D_left, &D_left_values));

        rocsparse_int D_left_batch_count{};
        int64_t       D_left_batch_dist{};
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_get_strided_batch(
            D_left, &D_left_batch_count, &D_left_batch_dist));
        rocsparse_int D_right_batch_count{};
        int64_t       D_right_batch_dist{};
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_get_strided_batch(
            D_right, &D_right_batch_count, &D_right_batch_dist));
        //
        // Check if the symbolic transpose is available.
        //
        dim3 part0_grid_blocks(((std::max(A_rows, A_cols) - 1) / 512 + 1) * 512,
                               A_batch_count);
        dim3 part0_grid_threads(512);

        //
        // Set D_left and D_right to 1.
        //
        RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
            (ruiz_part0_kernel<512, int32_t, float, float>), part0_grid_blocks, part0_grid_threads, 0, handle->get_stream(),
            //
            A_rows,
            A_cols,
            reinterpret_cast<float*>(D_left_values),
            D_left_batch_dist,
            reinterpret_cast<float*>(D_right_values),
            D_right_batch_dist);

        auto run_iter = ruiz_run_iter<int32_t, int32_t, float, float>;

        //
        // Run iterations.
        //
        int64_t niter = 0;
        for(int64_t iter = 0; iter < 200; ++iter)
        {
            run_iter(handle, A, D_left, D_right, Tol, buffer_size_in_bytes, buffer, converged, stop);
            volatile int32_t* s = (volatile int32_t*)stop;
            if(s[0])
            {
                niter = iter;
                break;
            }
        }

        std::cout << "niter =  " << niter << std::endl;
        return HIPDSS_STATUS_SUCCESS;
    }

} // namespace hipdss
