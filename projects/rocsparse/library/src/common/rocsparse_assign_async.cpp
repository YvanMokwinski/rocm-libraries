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

#include "rocsparse_assign_async.hpp"
#include "rocsparse_control.hpp"

namespace rocsparse
{
    template <typename T>
    ROCSPARSE_KERNEL(32)
    void assign_kernel(T* dest, T value)
    {
        const uint32_t batch_index = blockIdx.y;
        if(hipThreadIdx_x == 0)
        {
            dest[batch_index] = value;
        }
    }
  
  template <typename T>
  ROCSPARSE_DEVICE_ILF void gassign_device(int64_t n,
					   T* __restrict__ dest,
					   int64_t inc)
  {
    const auto tid = hipBlockIdx_x * 512 + hipThreadIdx_x;
    if(tid < n)
      {
	*(dest + tid * inc) = 1;
      }
  }
  
  template <typename T>
  ROCSPARSE_KERNEL(512)
  void gassign_kernel(int64_t n,
		      void* __restrict__ dest,
		      int64_t inc)
  {
    const uint32_t batch_index = blockIdx.y;
    gassign_device<T>(n, reinterpret_cast<T*__restrict__>(dest)+batch_index, inc);
  }
  
}

template <typename T>
rocsparse_status rocsparse::assign_async(int64_t n, T* dest, T value, hipStream_t stream)
{
    RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
        rocsparse::assign_kernel, dim3(1, n), dim3(32), 0, stream, dest, value);
    return rocsparse_status_success;
}

rocsparse_status rocsparse::gassign_async_zero(int64_t batch_count,
					       int64_t n,
					       rocsparse_datatype datatype,
					       void* dest,
					       int64_t inc,
					       hipStream_t stream)
{
  auto kernel = rocsparse::gassign_kernel<float>;
  switch(datatype)
    {
    case rocsparse_datatype_f16_r:
      {
	//	kernel = rocsparse::gassign_kernel<rocsparse_bfloat16>;
	break;
      }
    case rocsparse_datatype_f32_r:
      {
	kernel = rocsparse::gassign_kernel<float>;
	break;
      }
    case rocsparse_datatype_bf16_r:
      {
	//	kernel = rocsparse::gassign_kernel<rocsparse_bfloat16>;
	break;
      }
    case rocsparse_datatype_f32_c:
      {
	kernel = rocsparse::gassign_kernel<rocsparse_float_complex>;
	break;
      }
    case rocsparse_datatype_f64_r:
      {
	kernel = rocsparse::gassign_kernel<double>;
	break;
      }
    case rocsparse_datatype_f64_c:
      {
	kernel = rocsparse::gassign_kernel<rocsparse_double_complex>;
	break;
      }
    case rocsparse_datatype_i8_r:
      {
	kernel = rocsparse::gassign_kernel<int8_t>;
	break;
      }
    case rocsparse_datatype_u8_r:
      {
	kernel = rocsparse::gassign_kernel<uint8_t>;
	break;
      }
    case rocsparse_datatype_i32_r:
      {
	kernel = rocsparse::gassign_kernel<int32_t>;
	break;
      }
    case rocsparse_datatype_u32_r:
      {
	kernel = rocsparse::gassign_kernel<uint32_t>;
	break;
      }
    }
  RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(kernel, dim3( (n - 1) / 512 + 1 , batch_count), dim3(512), 0, stream, n, dest, inc);
  return rocsparse_status_success;
}

rocsparse_status rocsparse::gassign_async_one(int64_t batch_count,
					       int64_t n,
					       rocsparse_datatype datatype,
					       void* dest,
					       int64_t inc,
					       hipStream_t stream)
{
  auto kernel = rocsparse::gassign_kernel<float>;
  switch(datatype)
    {
    case rocsparse_datatype_f16_r:
      {
	//	kernel = rocsparse::gassign_kernel<rocsparse_bfloat16>;
	break;
      }
    case rocsparse_datatype_f32_r:
      {
	kernel = rocsparse::gassign_kernel<float>;
	break;
      }
    case rocsparse_datatype_bf16_r:
      {
	//	kernel = rocsparse::gassign_kernel<rocsparse_bfloat16>;
	break;
      }
    case rocsparse_datatype_f32_c:
      {
	kernel = rocsparse::gassign_kernel<rocsparse_float_complex>;
	break;
      }
    case rocsparse_datatype_f64_r:
      {
	kernel = rocsparse::gassign_kernel<double>;
	break;
      }
    case rocsparse_datatype_f64_c:
      {
	kernel = rocsparse::gassign_kernel<rocsparse_double_complex>;
	break;
      }
    case rocsparse_datatype_i8_r:
      {
	kernel = rocsparse::gassign_kernel<int8_t>;
	break;
      }
    case rocsparse_datatype_u8_r:
      {
	kernel = rocsparse::gassign_kernel<uint8_t>;
	break;
      }
    case rocsparse_datatype_i32_r:
      {
	kernel = rocsparse::gassign_kernel<int32_t>;
	break;
      }
    case rocsparse_datatype_u32_r:
      {
	kernel = rocsparse::gassign_kernel<uint32_t>;
	break;
      }
    }
  RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(kernel, dim3( (n - 1) / 512 + 1 , batch_count), dim3(512), 0, stream, n, dest, inc);
  return rocsparse_status_success;
}



template rocsparse_status
    rocsparse::assign_async(int64_t n, int32_t* dest, int32_t value, hipStream_t stream);

template rocsparse_status
    rocsparse::assign_async(int64_t n, int64_t* dest, int64_t value, hipStream_t stream);

rocsparse_status rocsparse::assign_max_async(int64_t             n,
                                             rocsparse_indextype indextype,
                                             void*               dest,
                                             hipStream_t         stream)
{
    switch(indextype)
    {
    case rocsparse_indextype_i32:
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::assign_async(
            n, reinterpret_cast<int32_t*>(dest), std::numeric_limits<int32_t>::max(), stream));
        return rocsparse_status_success;
    }
    case rocsparse_indextype_i64:
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::assign_async(
            n, reinterpret_cast<int64_t*>(dest), std::numeric_limits<int64_t>::max(), stream));
        return rocsparse_status_success;
    }
    // LCOV_EXCL_START
    case rocsparse_indextype_u16:
    {
        RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(rocsparse_status_not_implemented,
                                               "unsupported indextype: rocsparse_indextype_u16");
    }
    }
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
    // LCOV_EXCL_STOP
}
