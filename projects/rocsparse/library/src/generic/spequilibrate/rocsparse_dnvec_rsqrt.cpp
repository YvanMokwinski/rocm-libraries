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

#include "rocsparse_common.hpp"
#include "rocsparse_control.hpp"
#include "rocsparse_scalar.hpp"

// ============================================================================
// Element-wise inverse square root: x[i] = 1 / sqrt(x[i])
// ============================================================================

template <uint32_t BLOCKSIZE, typename T, typename U>
ROCSPARSE_DEVICE_ILF void dnvec_rsqrt_device(int64_t              size,
					     T* __restrict__  x,
					     int64_t          x_inc,
					     const U* __restrict__ tol,
					     int64_t          tol_inc)
{
  const int64_t gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(gid < size)
    {
      x[gid * x_inc] = (rocsparse::abs(x[gid * x_inc]) <= tol[gid * tol_inc])
	? static_cast<T>(1)
	: rocsparse::sqrt(x[gid * x_inc]);
    }
}

template <uint32_t BLOCKSIZE, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void dnvec_rsqrt_kernel(int64_t                    size,
			void* __restrict__          x,
			int64_t                    x_inc,
			int64_t                    x_stride,
			const void* __restrict__   tol,
			int64_t                    tol_inc,
			int64_t                    tol_stride)
{
  const int64_t batch_index = hipBlockIdx_y;
  dnvec_rsqrt_device<BLOCKSIZE>(
    size,
    reinterpret_cast<T* __restrict__>(x) + batch_index * x_stride,
    x_inc,
    reinterpret_cast<const U* __restrict__>(tol) + batch_index * tol_stride,
    tol_inc);
}

// ============================================================================
// Function pointer type
// ============================================================================

typedef void (*dnvec_rsqrt_kernel_t)(int64_t,
				     void* __restrict__, int64_t, int64_t,
				     const void* __restrict__, int64_t, int64_t);

// ============================================================================
// Dispatch helper
// ============================================================================

namespace
{
  template <typename T>
  static dnvec_rsqrt_kernel_t find_dnvec_rsqrt_kernel_U(rocsparse_datatype U_type)
  {
    switch(U_type)
      {
      case rocsparse_datatype_f32_r: return dnvec_rsqrt_kernel<512, T, float>;
      case rocsparse_datatype_f64_r: return dnvec_rsqrt_kernel<512, T, double>;
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r: return nullptr;
      }
    return nullptr;
  }
}

static dnvec_rsqrt_kernel_t find_dnvec_rsqrt_kernel(rocsparse_datatype T_type, rocsparse_datatype U_type)
{
  switch(T_type)
    {
    case rocsparse_datatype_f32_r: return find_dnvec_rsqrt_kernel_U<float>(U_type);
    case rocsparse_datatype_f64_r: return find_dnvec_rsqrt_kernel_U<double>(U_type);
    case rocsparse_datatype_f32_c: return find_dnvec_rsqrt_kernel_U<rocsparse_float_complex>(U_type);
    case rocsparse_datatype_f64_c: return find_dnvec_rsqrt_kernel_U<rocsparse_double_complex>(U_type);
    case rocsparse_datatype_f16_r:
    case rocsparse_datatype_i8_r:
    case rocsparse_datatype_u8_r:
    case rocsparse_datatype_i32_r:
    case rocsparse_datatype_u32_r:
    case rocsparse_datatype_bf16_r: return nullptr;
    }
  return nullptr;
}

// ============================================================================
// Public function
// ============================================================================

static constexpr uint32_t RSQRT_BLOCKSIZE = 512;

extern "C" rocsparse_status rocsparse_dnvec_rsqrt(rocsparse_handle            handle,
						  rocsparse_dnvec_descr       x,
						  rocsparse_const_dnvec_descr tol)
{
  ROCSPARSE_CHECKARG_HANDLE(0, handle);
  ROCSPARSE_CHECKARG_POINTER(1, x);
  ROCSPARSE_CHECKARG_POINTER(2, tol);

  hipStream_t stream = handle->stream;
  auto kernel = find_dnvec_rsqrt_kernel(x->data_type, tol->data_type);
  if(kernel == nullptr)
    {
      RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(rocsparse_status_not_implemented,
						"Unsupported data type for dnvec_rsqrt");
    }

  const int64_t size        = x->size;
  const int64_t batch_count = x->batch_count;

  ROCSPARSE_CHECKARG(2, tol, tol->size != 1 && tol->batch_count != batch_count, rocsparse_status_invalid_size);

  RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
    kernel,
    dim3((size - 1) / RSQRT_BLOCKSIZE + 1, batch_count),
    dim3(RSQRT_BLOCKSIZE),
    0,
    stream,
    size,
    x->values,
    x->inc,
    x->batch_stride,
    tol->values,
    tol->inc,
    (tol->size == 1) ? 0 : tol->batch_stride);

  return rocsparse_status_success;
}
