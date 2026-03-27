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

#include "rocsparse_dnvec_descr.hpp"
#include "rocsparse_common.hpp"
#include "rocsparse_control.hpp"
#include "rocsparse-auxiliary.h"

// ============================================================================
// Element-wise (Hadamard) multiplication: target[i] *= src[i]
// ============================================================================
#if 0

template <uint32_t BLOCKSIZE, typename T, typename U,
  	  typename std::enable_if<std::is_same<T, float>::value ||
				  std::is_same<T, double>::value,
				  int>::type
	  = 0>

ROCSPARSE_DEVICE_ILF void dnvec_hadamard_device(int64_t              size,
						 const U* __restrict__ src,
						 T* __restrict__       target)
{
  const int64_t gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(gid < size)
    {
      // target[gid] *= src[gid];
	    target[gid] = rocsparse::fma(static_cast<T>(src[gid]), target[gid], static_cast<T>(0));
    }
}

template <uint32_t BLOCKSIZE, typename T, typename U,
 	  typename std::enable_if<std::is_same<T, rocsparse_float_complex>::value ||
				  std::is_same<T, rocsparse_double_complex>::value,
				  int>::type
	  = 0>
ROCSPARSE_DEVICE_ILF void dnvec_hadamard_device(int64_t              size,
						 const U* __restrict__ src,
						 T* __restrict__       target)
{
  const int64_t gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(gid < size)
    {
      target[gid] *= src[gid];
    }
}
#endif


template <uint32_t BLOCKSIZE, typename T, typename U>
ROCSPARSE_DEVICE_ILF void dnvec_hadamard_device(int64_t              size,
						 const U* __restrict__ src,
						 T* __restrict__       target)
{
  const int64_t gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(gid < size)
    {
      // target[gid] *= src[gid];
       target[gid] = rocsparse::fma(static_cast<T>(src[gid]), target[gid], static_cast<T>(0));
    }
}


template <uint32_t BLOCKSIZE, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void dnvec_hadamard_kernel(int64_t              size,
			   const void* __restrict__ src,
			   int64_t     src_stride,
			   void* __restrict__       target,
			   int64_t     target_stride)
{
  const int64_t batch_index = hipBlockIdx_y;
  dnvec_hadamard_device<BLOCKSIZE>(
				   size,
    reinterpret_cast<const U* __restrict__>(src) + batch_index * src_stride,
    reinterpret_cast<T* __restrict__>(target) + batch_index * target_stride);
}

// ============================================================================
// Function pointer type
// ============================================================================

typedef void (*dnvec_hadamard_kernel_t)(int64_t,
					const void* __restrict__, int64_t,
					void* __restrict__, int64_t);

// ============================================================================
// Dispatch helpers
// ============================================================================
namespace
{
  template <typename T>
  static dnvec_hadamard_kernel_t find_dnvec_hadamard_kernel_U(rocsparse_datatype U_type)
  {
    switch(U_type)
      {
      case rocsparse_datatype_f32_r: return dnvec_hadamard_kernel<512, T, float>;
      case rocsparse_datatype_f64_r: return dnvec_hadamard_kernel<512, T, double>;
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

static dnvec_hadamard_kernel_t find_dnvec_hadamard_kernel(rocsparse_datatype T_type,
							  rocsparse_datatype U_type)
{
  switch(T_type)
    {
    case rocsparse_datatype_f32_r: return find_dnvec_hadamard_kernel_U<float>(U_type);
    case rocsparse_datatype_f64_r: return find_dnvec_hadamard_kernel_U<double>(U_type);
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

// ============================================================================
// Public function
// ============================================================================

static constexpr uint32_t HADAMARD_BLOCKSIZE = 512;

extern "C" rocsparse_status rocsparse_dnvec_hadamard(rocsparse_handle            handle,
					  rocsparse_const_dnvec_descr src,
					  rocsparse_dnvec_descr       target)
{
  hipStream_t stream;
  rocsparse_get_stream(handle, &stream);

  auto kernel = find_dnvec_hadamard_kernel(target->data_type, src->data_type);
  if(kernel == nullptr)
    return rocsparse_status_not_implemented;

  const int64_t size        = target->size;
  const int64_t batch_count = target->batch_count;

  RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
    kernel,
    dim3((size - 1) / HADAMARD_BLOCKSIZE + 1, batch_count),
    dim3(HADAMARD_BLOCKSIZE),
    0,
    stream,
    size,
    src->const_values,
    src->batch_stride,
    target->values,
    target->batch_stride);

  return rocsparse_status_success;
}
