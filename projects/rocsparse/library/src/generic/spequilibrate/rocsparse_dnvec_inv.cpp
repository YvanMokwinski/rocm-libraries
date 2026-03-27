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
// Element-wise inverse: x[i] = 1 / x[i]
// ============================================================================

template <uint32_t BLOCKSIZE, typename T>
ROCSPARSE_DEVICE_ILF void dnvec_inv_device(int64_t        size,
					    T* __restrict__ x)
{
  const int64_t gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(gid < size)
    {
      x[gid] = static_cast<T>(1) / x[gid];
    }
}

template <uint32_t BLOCKSIZE, typename T>
ROCSPARSE_KERNEL(BLOCKSIZE)
void dnvec_inv_kernel(int64_t              size,
		      void* __restrict__   x,
		      int64_t              x_stride)
{
  const int64_t batch_index = hipBlockIdx_y;
  dnvec_inv_device<BLOCKSIZE>(
    size,
    reinterpret_cast<T* __restrict__>(x) + batch_index * x_stride);
}

// ============================================================================
// Function pointer type
// ============================================================================

typedef void (*dnvec_inv_kernel_t)(int64_t,
				   void* __restrict__, int64_t);

// ============================================================================
// Dispatch helper
// ============================================================================

static dnvec_inv_kernel_t find_dnvec_inv_kernel(rocsparse_datatype T_type)
{
  switch(T_type)
    {
    case rocsparse_datatype_f32_r: return dnvec_inv_kernel<512, float>;
    case rocsparse_datatype_f64_r: return dnvec_inv_kernel<512, double>;
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

static constexpr uint32_t INV_BLOCKSIZE = 512;

rocsparse_status rocsparse_dnvec_inv(rocsparse_handle      handle,
				     rocsparse_dnvec_descr x)
{
  hipStream_t stream;
  rocsparse_get_stream(handle, &stream);

  auto kernel = find_dnvec_inv_kernel(x->data_type);
  if(kernel == nullptr)
    return rocsparse_status_not_implemented;

  const int64_t size        = x->size;
  const int64_t batch_count = x->batch_count;

  RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
    kernel,
    dim3((size - 1) / INV_BLOCKSIZE + 1, batch_count),
    dim3(INV_BLOCKSIZE),
    0,
    stream,
    size,
    x->values,
    x->batch_stride);

  return rocsparse_status_success;
}
