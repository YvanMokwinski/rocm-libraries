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

#include "rocsparse_utility.hpp"
#include "rocsparse_control.hpp"

//
// Compute the maximum absolute value per row and per column of a CSR matrix
// in a single pass. Each thread processes one row:
//   - iterates through the row entries to find the row max,
//   - atomically updates the column max array for each entry.
//
// Template parameters:
//   BLOCKSIZE - number of threads per block
//   I         - row pointer type (e.g. int32_t, int64_t)
//   J         - column index type (e.g. int32_t, int64_t)
//   T         - value type (e.g. float, double, rocsparse_float_complex, ...)
//   U         - floating data type used for the output (real type of T)
//
// Parameters:
//   m         - number of rows
//   base      - index base (0 or 1)
//   ptr       - CSR row pointer array, size m+1
//   ind       - CSR column index array, size nnz
//   val       - CSR value array, size nnz
//   row_max   - output: max |val| per row, size m  (caller must zero-initialize)
//   col_max   - output: max |val| per column, size n (caller must zero-initialize)
//
template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_DEVICE_ILF void csr_max_row_col_device(int64_t              m,
						  rocsparse_index_base base,
						  const I* __restrict__ ptr,
						  const J* __restrict__ ind,
						  const T* __restrict__ val,
						  U* __restrict__       row_max,
						  U* __restrict__       col_max)
{
  const int64_t row = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(row < m)
    {
      const I row_begin = ptr[row] - base;
      const I row_end   = ptr[row + 1] - base;
      U       rmax      = static_cast<U>(0);
      for(I k = row_begin; k < row_end; ++k)
	{
	  const U aval = static_cast<U>(std::abs(val[k]));
	  rmax         = (aval > rmax) ? aval : rmax;
	  rocsparse::atomic_max(col_max + ind[k] - base, aval);
	}
      row_max[row] = rmax;
    }
}

//
// Global kernel wrapper.
//   - hipBlockIdx_x * BLOCKSIZE + hipThreadIdx_x  maps to the row index
//   - hipBlockIdx_y                               maps to the batch index
//
// Grid:  dim3((m - 1) / BLOCKSIZE + 1, batch_count)
// Block: dim3(BLOCKSIZE)
//
template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void csr_max_row_col_kernel(int64_t              m,
			    rocsparse_index_base base,
			    const void* __restrict__ ptr,
			    const void* __restrict__ ind,
			    const void* __restrict__ val,
			    int64_t     val_stride,
			    void* __restrict__       row_max,
			    int64_t     row_max_stride,
			    void* __restrict__       col_max,
			    int64_t     col_max_stride,
			    int32_t*    converged)
{
  const int64_t batch_index = hipBlockIdx_y;
  if(converged[batch_index] == 0)
    {
      csr_max_row_col_device<BLOCKSIZE>(
	m,
	base,
	reinterpret_cast<const I* __restrict__>(ptr),
	reinterpret_cast<const J* __restrict__>(ind),
	reinterpret_cast<const T* __restrict__>(val) + batch_index * val_stride,
	reinterpret_cast<U* __restrict__>(row_max) + batch_index * row_max_stride,
	reinterpret_cast<U* __restrict__>(col_max) + batch_index * col_max_stride);
    }
}

//
// Compute the maximum absolute value per row and per column of a CSC matrix
// in a single pass. Each thread processes one column:
//   - iterates through the column entries to find the column max,
//   - atomically updates the row max array for each entry.
//
// Parameters:
//   n         - number of columns
//   base      - index base (0 or 1)
//   ptr       - CSC column pointer array, size n+1
//   ind       - CSC row index array, size nnz
//   val       - CSC value array, size nnz
//   row_max   - output: max |val| per row, size m  (caller must zero-initialize)
//   col_max   - output: max |val| per column, size n (caller must zero-initialize)
//
template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_DEVICE_ILF void csc_max_row_col_device(int64_t              n,
						  rocsparse_index_base base,
						  const I* __restrict__ ptr,
						  const J* __restrict__ ind,
						  const T* __restrict__ val,
						  U* __restrict__       row_max,
						  U* __restrict__       col_max)
{
  const int64_t col = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(col < n)
    {
      const I col_begin = ptr[col] - base;
      const I col_end   = ptr[col + 1] - base;
      U       cmax      = static_cast<U>(0);
      for(I k = col_begin; k < col_end; ++k)
	{
	  const U aval = static_cast<U>(std::abs(val[k]));
	  cmax         = (aval > cmax) ? aval : cmax;
	  rocsparse::atomic_max(row_max + ind[k] - base, aval);
	}
      col_max[col] = cmax;
    }
}

//
// Global kernel wrapper for CSC.
//   - hipBlockIdx_x * BLOCKSIZE + hipThreadIdx_x  maps to the column index
//   - hipBlockIdx_y                               maps to the batch index
//
// Grid:  dim3((n - 1) / BLOCKSIZE + 1, batch_count)
// Block: dim3(BLOCKSIZE)
//
template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void csc_max_row_col_kernel(int64_t              n,
			    rocsparse_index_base base,
			    const void* __restrict__ ptr,
			    const void* __restrict__ ind,
			    const void* __restrict__ val,
			    int64_t     val_stride,
			    void* __restrict__       row_max,
			    int64_t     row_max_stride,
			    void* __restrict__       col_max,
			    int64_t     col_max_stride,
			    int32_t*    converged)
{
  const int64_t batch_index = hipBlockIdx_y;
  if(converged[batch_index] == 0)
    {
      csc_max_row_col_device<BLOCKSIZE>(
	n,
	base,
	reinterpret_cast<const I* __restrict__>(ptr),
	reinterpret_cast<const J* __restrict__>(ind),
	reinterpret_cast<const T* __restrict__>(val) + batch_index * val_stride,
	reinterpret_cast<U* __restrict__>(row_max) + batch_index * row_max_stride,
	reinterpret_cast<U* __restrict__>(col_max) + batch_index * col_max_stride);
    }
}

//
// Function pointer type matching csc_max_row_col_kernel's signature.
//
typedef void (*csc_max_row_col_kernel_t)(int64_t              n,
					 rocsparse_index_base base,
					 const void* __restrict__ ptr,
					 const void* __restrict__ ind,
					 const void* __restrict__ val,
					 int64_t     val_stride,
					 void* __restrict__       row_max,
					 int64_t     row_max_stride,
					 void* __restrict__       col_max,
					 int64_t     col_max_stride,
					 int32_t*    converged);

//
// Function pointer type matching csr_max_row_col_kernel's signature.
//
typedef void (*csr_max_row_col_kernel_t)(int64_t              m,
					 rocsparse_index_base base,
					 const void* __restrict__ ptr,
					 const void* __restrict__ ind,
					 const void* __restrict__ val,
					 int64_t     val_stride,
					 void* __restrict__       row_max,
					 int64_t     row_max_stride,
					 void* __restrict__       col_max,
					 int64_t     col_max_stride,
					 int32_t*    converged);

//
// Resolve (I, J, T, U) template parameters from runtime enums.
// Returns the appropriately instantiated csr_max_row_col_kernel<512,...>.
//
namespace
{
  template <typename I, typename J, typename T>
  static csr_max_row_col_kernel_t find_kernel_U(rocsparse_datatype U_type)
  {
    switch(U_type)
      {
      case rocsparse_datatype_f32_r:
	return csr_max_row_col_kernel<512, I, J, T, float>;
      case rocsparse_datatype_f64_r:
	return csr_max_row_col_kernel<512, I, J, T, double>;
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I, typename J>
  static csr_max_row_col_kernel_t find_kernel_T(rocsparse_datatype T_type,
						rocsparse_datatype U_type)
  {
    switch(T_type)
      {
      case rocsparse_datatype_f32_r:
	return find_kernel_U<I, J, float>(U_type);
      case rocsparse_datatype_f64_r:
	return find_kernel_U<I, J, double>(U_type);
      case rocsparse_datatype_f32_c:
	return find_kernel_U<I, J, rocsparse_float_complex>(U_type);
      case rocsparse_datatype_f64_c:
	return find_kernel_U<I, J, rocsparse_double_complex>(U_type);
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I>
  static csr_max_row_col_kernel_t find_kernel_J(rocsparse_indextype J_type,
						rocsparse_datatype  T_type,
						rocsparse_datatype  U_type)
  {
    switch(J_type)
      {
      case rocsparse_indextype_u16:
	return nullptr;
      case rocsparse_indextype_i32:
	return find_kernel_T<I, int32_t>(T_type, U_type);
      case rocsparse_indextype_i64:
	return find_kernel_T<I, int64_t>(T_type, U_type);
      }
    return nullptr;
  }
}

csr_max_row_col_kernel_t find_csr_max_row_col_kernel(rocsparse_indextype I_type,
						     rocsparse_indextype J_type,
						     rocsparse_datatype  T_type,
						     rocsparse_datatype  U_type)
{
  switch(I_type)
    {
    case rocsparse_indextype_u16:
      return nullptr;
    case rocsparse_indextype_i32:
      return find_kernel_J<int32_t>(J_type, T_type, U_type);
    case rocsparse_indextype_i64:
      return find_kernel_J<int64_t>(J_type, T_type, U_type);
    }
  return nullptr;
}

//
// Resolve (I, J, T, U) template parameters from runtime enums.
// Returns the appropriately instantiated csc_max_row_col_kernel<512,...>.
//
namespace
{
  template <typename I, typename J, typename T>
  static csc_max_row_col_kernel_t find_csc_kernel_U(rocsparse_datatype U_type)
  {
    switch(U_type)
      {
      case rocsparse_datatype_f32_r:
	return csc_max_row_col_kernel<512, I, J, T, float>;
      case rocsparse_datatype_f64_r:
	return csc_max_row_col_kernel<512, I, J, T, double>;
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I, typename J>
  static csc_max_row_col_kernel_t find_csc_kernel_T(rocsparse_datatype T_type,
						    rocsparse_datatype U_type)
  {
    switch(T_type)
      {
      case rocsparse_datatype_f32_r:
	return find_csc_kernel_U<I, J, float>(U_type);
      case rocsparse_datatype_f64_r:
	return find_csc_kernel_U<I, J, double>(U_type);
      case rocsparse_datatype_f32_c:
	return find_csc_kernel_U<I, J, rocsparse_float_complex>(U_type);
      case rocsparse_datatype_f64_c:
	return find_csc_kernel_U<I, J, rocsparse_double_complex>(U_type);
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I>
  static csc_max_row_col_kernel_t find_csc_kernel_J(rocsparse_indextype J_type,
						    rocsparse_datatype  T_type,
						    rocsparse_datatype  U_type)
  {
    switch(J_type)
      {
      case rocsparse_indextype_u16:
	return nullptr;
      case rocsparse_indextype_i32:
	return find_csc_kernel_T<I, int32_t>(T_type, U_type);
      case rocsparse_indextype_i64:
	return find_csc_kernel_T<I, int64_t>(T_type, U_type);
      }
    return nullptr;
  }
}

csc_max_row_col_kernel_t find_csc_max_row_col_kernel(rocsparse_indextype I_type,
						     rocsparse_indextype J_type,
						     rocsparse_datatype  T_type,
						     rocsparse_datatype  U_type)
{
  switch(I_type)
    {
    case rocsparse_indextype_u16:
      return nullptr;
    case rocsparse_indextype_i32:
      return find_csc_kernel_J<int32_t>(J_type, T_type, U_type);
    case rocsparse_indextype_i64:
      return find_csc_kernel_J<int64_t>(J_type, T_type, U_type);
    }
  return nullptr;
}

//
// Compute the maximum absolute value per row and per column of an ELL matrix
// in a single pass. Each thread processes one row:
//   - iterates through the ell_width entries of that row,
//   - tracks the row max in a register,
//   - atomically updates the column max array for each valid entry.
//
// ELL layout is column-major: element (row, p) is at index p * m + row.
// Invalid (padding) entries have col < 0 or col >= n.
//
// Parameters:
//   m         - number of rows
//   n         - number of columns
//   ell_width - max number of non-zeros per row
//   base      - index base (0 or 1)
//   ind       - ELL column index array, size m * ell_width
//   val       - ELL value array, size m * ell_width
//   row_max   - output: max |val| per row, size m  (caller must zero-initialize)
//   col_max   - output: max |val| per column, size n (caller must zero-initialize)
//
template <uint32_t BLOCKSIZE, typename J, typename T, typename U>
ROCSPARSE_DEVICE_ILF void ell_max_row_col_device(int64_t              m,
						  int64_t              n,
						  int64_t              ell_width,
						  rocsparse_index_base base,
						  const J* __restrict__ ind,
						  const T* __restrict__ val,
						  U* __restrict__       row_max,
						  U* __restrict__       col_max)
{
  const int64_t row = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(row < m)
    {
      U rmax = static_cast<U>(0);
      for(int64_t p = 0; p < ell_width; ++p)
	{
	  const int64_t idx = p * m + row;
	  const J       col = ind[idx] - base;
	  if(col >= 0 && col < n)
	    {
	      const U aval = static_cast<U>(std::abs(val[idx]));
	      rmax         = (aval > rmax) ? aval : rmax;
	      rocsparse::atomic_max(col_max + col, aval);
	    }
	  else
	    {
	      break;
	    }
	}
      row_max[row] = rmax;
    }
}

//
// Global kernel wrapper for ELL.
//   - hipBlockIdx_x * BLOCKSIZE + hipThreadIdx_x  maps to the row index
//   - hipBlockIdx_y                               maps to the batch index
//
// Grid:  dim3((m - 1) / BLOCKSIZE + 1, batch_count)
// Block: dim3(BLOCKSIZE)
//
template <uint32_t BLOCKSIZE, typename J, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void ell_max_row_col_kernel(int64_t              m,
			    int64_t              n,
			    int64_t              ell_width,
			    rocsparse_index_base base,
			    const void* __restrict__ ind,
			    const void* __restrict__ val,
			    int64_t     val_stride,
			    void* __restrict__       row_max,
			    int64_t     row_max_stride,
			    void* __restrict__       col_max,
			    int64_t     col_max_stride,
			    int32_t*    converged)
{
  const int64_t batch_index = hipBlockIdx_y;
  if(converged[batch_index] == 0)
    {
      ell_max_row_col_device<BLOCKSIZE>(
	m,
	n,
	ell_width,
	base,
	reinterpret_cast<const J* __restrict__>(ind),
	reinterpret_cast<const T* __restrict__>(val) + batch_index * val_stride,
	reinterpret_cast<U* __restrict__>(row_max) + batch_index * row_max_stride,
	reinterpret_cast<U* __restrict__>(col_max) + batch_index * col_max_stride);
    }
}

//
// Function pointer type matching ell_max_row_col_kernel's signature.
//
typedef void (*ell_max_row_col_kernel_t)(int64_t              m,
					 int64_t              n,
					 int64_t              ell_width,
					 rocsparse_index_base base,
					 const void* __restrict__ ind,
					 const void* __restrict__ val,
					 int64_t     val_stride,
					 void* __restrict__       row_max,
					 int64_t     row_max_stride,
					 void* __restrict__       col_max,
					 int64_t     col_max_stride,
					 int32_t*    converged);

//
// Resolve (J, T, U) template parameters from runtime enums.
// Returns the appropriately instantiated ell_max_row_col_kernel<512,...>.
//
namespace
{
  template <typename J, typename T>
  static ell_max_row_col_kernel_t find_ell_kernel_U(rocsparse_datatype U_type)
  {
    switch(U_type)
      {
      case rocsparse_datatype_f32_r:
	return ell_max_row_col_kernel<512, J, T, float>;
      case rocsparse_datatype_f64_r:
	return ell_max_row_col_kernel<512, J, T, double>;
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename J>
  static ell_max_row_col_kernel_t find_ell_kernel_T(rocsparse_datatype T_type,
						    rocsparse_datatype U_type)
  {
    switch(T_type)
      {
      case rocsparse_datatype_f32_r:
	return find_ell_kernel_U<J, float>(U_type);
      case rocsparse_datatype_f64_r:
	return find_ell_kernel_U<J, double>(U_type);
      case rocsparse_datatype_f32_c:
	return find_ell_kernel_U<J, rocsparse_float_complex>(U_type);
      case rocsparse_datatype_f64_c:
	return find_ell_kernel_U<J, rocsparse_double_complex>(U_type);
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }
}

ell_max_row_col_kernel_t find_ell_max_row_col_kernel(rocsparse_indextype J_type,
						     rocsparse_datatype  T_type,
						     rocsparse_datatype  U_type)
{
  switch(J_type)
    {
    case rocsparse_indextype_u16:
      return nullptr;
    case rocsparse_indextype_i32:
      return find_ell_kernel_T<int32_t>(T_type, U_type);
    case rocsparse_indextype_i64:
      return find_ell_kernel_T<int64_t>(T_type, U_type);
    }
  return nullptr;
}

// ============================================================================
// COO format
// ============================================================================
//
// Each thread processes one nonzero entry:
//   - writes its |val| into row_max via atomic max,
//   - writes its |val| into col_max via atomic max.
//
// Both row_max and col_max must be zero-initialized by the caller.
//
template <uint32_t BLOCKSIZE, typename I, typename T, typename U>
ROCSPARSE_DEVICE_ILF void coo_max_row_col_device(int64_t              nnz,
						  rocsparse_index_base base,
						  const I* __restrict__ row_ind,
						  const I* __restrict__ col_ind,
						  const T* __restrict__ val,
						  U* __restrict__       row_max,
						  U* __restrict__       col_max)
{
  const int64_t gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(gid < nnz)
    {
      const U aval = static_cast<U>(std::abs(val[gid]));
      rocsparse::atomic_max(row_max + row_ind[gid] - base, aval);
      rocsparse::atomic_max(col_max + col_ind[gid] - base, aval);
    }
}

template <uint32_t BLOCKSIZE, typename I, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void coo_max_row_col_kernel(int64_t              nnz,
			    rocsparse_index_base base,
			    const void* __restrict__ row_ind,
			    const void* __restrict__ col_ind,
			    const void* __restrict__ val,
			    int64_t     val_stride,
			    void* __restrict__       row_max,
			    int64_t     row_max_stride,
			    void* __restrict__       col_max,
			    int64_t     col_max_stride,
			    int32_t*    converged)
{
  const int64_t batch_index = hipBlockIdx_y;
  if(converged[batch_index] == 0)
    {
      coo_max_row_col_device<BLOCKSIZE>(
	nnz,
	base,
	reinterpret_cast<const I* __restrict__>(row_ind),
	reinterpret_cast<const I* __restrict__>(col_ind),
	reinterpret_cast<const T* __restrict__>(val) + batch_index * val_stride,
	reinterpret_cast<U* __restrict__>(row_max) + batch_index * row_max_stride,
	reinterpret_cast<U* __restrict__>(col_max) + batch_index * col_max_stride);
    }
}

typedef void (*coo_max_row_col_kernel_t)(int64_t              nnz,
					 rocsparse_index_base base,
					 const void* __restrict__ row_ind,
					 const void* __restrict__ col_ind,
					 const void* __restrict__ val,
					 int64_t     val_stride,
					 void* __restrict__       row_max,
					 int64_t     row_max_stride,
					 void* __restrict__       col_max,
					 int64_t     col_max_stride,
					 int32_t*    converged);

namespace
{
  template <typename I, typename T>
  static coo_max_row_col_kernel_t find_coo_kernel_U(rocsparse_datatype U_type)
  {
    switch(U_type)
      {
      case rocsparse_datatype_f32_r:
	return coo_max_row_col_kernel<512, I, T, float>;
      case rocsparse_datatype_f64_r:
	return coo_max_row_col_kernel<512, I, T, double>;
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I>
  static coo_max_row_col_kernel_t find_coo_kernel_T(rocsparse_datatype T_type,
						    rocsparse_datatype U_type)
  {
    switch(T_type)
      {
      case rocsparse_datatype_f32_r:
	return find_coo_kernel_U<I, float>(U_type);
      case rocsparse_datatype_f64_r:
	return find_coo_kernel_U<I, double>(U_type);
      case rocsparse_datatype_f32_c:
	return find_coo_kernel_U<I, rocsparse_float_complex>(U_type);
      case rocsparse_datatype_f64_c:
	return find_coo_kernel_U<I, rocsparse_double_complex>(U_type);
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }
}

coo_max_row_col_kernel_t find_coo_max_row_col_kernel(rocsparse_indextype I_type,
						     rocsparse_datatype  T_type,
						     rocsparse_datatype  U_type)
{
  switch(I_type)
    {
    case rocsparse_indextype_u16:
      return nullptr;
    case rocsparse_indextype_i32:
      return find_coo_kernel_T<int32_t>(T_type, U_type);
    case rocsparse_indextype_i64:
      return find_coo_kernel_T<int64_t>(T_type, U_type);
    }
  return nullptr;
}

// ============================================================================
// COO_AOS format (Array-of-Structures: interleaved row,col pairs)
// ============================================================================
//
// Each thread processes one nonzero entry:
//   - row = ind[2*gid], col = ind[2*gid + 1]
//
template <uint32_t BLOCKSIZE, typename I, typename T, typename U>
ROCSPARSE_DEVICE_ILF void coo_aos_max_row_col_device(int64_t              nnz,
						      rocsparse_index_base base,
						      const I* __restrict__ ind,
						      const T* __restrict__ val,
						      U* __restrict__       row_max,
						      U* __restrict__       col_max)
{
  const int64_t gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(gid < nnz)
    {
      const U aval = static_cast<U>(std::abs(val[gid]));
      const I row  = ind[2 * gid] - base;
      const I col  = ind[2 * gid + 1] - base;
      rocsparse::atomic_max(row_max + row, aval);
      rocsparse::atomic_max(col_max + col, aval);
    }
}

template <uint32_t BLOCKSIZE, typename I, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void coo_aos_max_row_col_kernel(int64_t              nnz,
				rocsparse_index_base base,
				const void* __restrict__ ind,
				const void* __restrict__ val,
				int64_t     val_stride,
				void* __restrict__       row_max,
				int64_t     row_max_stride,
				void* __restrict__       col_max,
				int64_t     col_max_stride,
				int32_t*    converged)
{
  const int64_t batch_index = hipBlockIdx_y;
  if(converged[batch_index] == 0)
    {
      coo_aos_max_row_col_device<BLOCKSIZE>(
	nnz,
	base,
	reinterpret_cast<const I* __restrict__>(ind),
	reinterpret_cast<const T* __restrict__>(val) + batch_index * val_stride,
	reinterpret_cast<U* __restrict__>(row_max) + batch_index * row_max_stride,
	reinterpret_cast<U* __restrict__>(col_max) + batch_index * col_max_stride);
    }
}

typedef void (*coo_aos_max_row_col_kernel_t)(int64_t              nnz,
					     rocsparse_index_base base,
					     const void* __restrict__ ind,
					     const void* __restrict__ val,
					     int64_t     val_stride,
					     void* __restrict__       row_max,
					     int64_t     row_max_stride,
					     void* __restrict__       col_max,
					     int64_t     col_max_stride,
					     int32_t*    converged);

namespace
{
  template <typename I, typename T>
  static coo_aos_max_row_col_kernel_t find_coo_aos_kernel_U(rocsparse_datatype U_type)
  {
    switch(U_type)
      {
      case rocsparse_datatype_f32_r:
	return coo_aos_max_row_col_kernel<512, I, T, float>;
      case rocsparse_datatype_f64_r:
	return coo_aos_max_row_col_kernel<512, I, T, double>;
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I>
  static coo_aos_max_row_col_kernel_t find_coo_aos_kernel_T(rocsparse_datatype T_type,
							    rocsparse_datatype U_type)
  {
    switch(T_type)
      {
      case rocsparse_datatype_f32_r:
	return find_coo_aos_kernel_U<I, float>(U_type);
      case rocsparse_datatype_f64_r:
	return find_coo_aos_kernel_U<I, double>(U_type);
      case rocsparse_datatype_f32_c:
	return find_coo_aos_kernel_U<I, rocsparse_float_complex>(U_type);
      case rocsparse_datatype_f64_c:
	return find_coo_aos_kernel_U<I, rocsparse_double_complex>(U_type);
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }
}

coo_aos_max_row_col_kernel_t find_coo_aos_max_row_col_kernel(rocsparse_indextype I_type,
							     rocsparse_datatype  T_type,
							     rocsparse_datatype  U_type)
{
  switch(I_type)
    {
    case rocsparse_indextype_u16:
      return nullptr;
    case rocsparse_indextype_i32:
      return find_coo_aos_kernel_T<int32_t>(T_type, U_type);
    case rocsparse_indextype_i64:
      return find_coo_aos_kernel_T<int64_t>(T_type, U_type);
    }
  return nullptr;
}

// ============================================================================
// BSR format (Block Compressed Sparse Row)
// ============================================================================
//
// Each thread processes one block-row. For each block in the row, it iterates
// over the block_dim x block_dim dense sub-block to find the maximum absolute
// value per point-row and per point-column.
//
// Row max is accumulated in shared memory per block-row, column max via atomics.
//
// Parameters:
//   mb        - number of block-rows
//   nb        - number of block-columns
//   block_dim - block dimension (square blocks)
//   dir       - storage direction within blocks (row or column major)
//   base      - index base
//   bsr_ptr   - BSR row pointer, size mb+1
//   bsr_ind   - BSR column indices, size nnzb
//   bsr_val   - BSR values, size nnzb * block_dim * block_dim
//   row_max   - output: max |val| per point-row, size mb*block_dim
//   col_max   - output: max |val| per point-column, size nb*block_dim
//
template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_DEVICE_ILF void bsr_max_row_col_device(int64_t              mb,
						  int64_t              nb,
						  int64_t              block_dim,
						  rocsparse_direction  dir,
						  rocsparse_index_base base,
						  const I* __restrict__ bsr_ptr,
						  const J* __restrict__ bsr_ind,
						  const T* __restrict__ bsr_val,
						  U* __restrict__       row_max,
						  U* __restrict__       col_max)
{
  const int64_t block_row = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(block_row < mb)
    {
      const I block_begin = bsr_ptr[block_row] - base;
      const I block_end   = bsr_ptr[block_row + 1] - base;

      for(int64_t bi = 0; bi < block_dim; ++bi)
	{
	  U rmax = static_cast<U>(0);
	  for(I blk = block_begin; blk < block_end; ++blk)
	    {
	      const J block_col = bsr_ind[blk] - base;
	      for(int64_t bj = 0; bj < block_dim; ++bj)
		{
		  const int64_t val_idx
		    = (dir == rocsparse_direction_row)
		    ? (block_dim * block_dim * blk + bi * block_dim + bj)
		    : (block_dim * block_dim * blk + bi + bj * block_dim);
		  const U aval = static_cast<U>(std::abs(bsr_val[val_idx]));
		  rmax         = (aval > rmax) ? aval : rmax;
		  rocsparse::atomic_max(col_max + block_col * block_dim + bj, aval);
		}
	    }
	  row_max[block_row * block_dim + bi] = rmax;
	}
    }
}

template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void bsr_max_row_col_kernel(int64_t              mb,
			    int64_t              nb,
			    int64_t              block_dim,
			    rocsparse_direction  dir,
			    rocsparse_index_base base,
			    const void* __restrict__ bsr_ptr,
			    const void* __restrict__ bsr_ind,
			    const void* __restrict__ bsr_val,
			    int64_t     val_stride,
			    void* __restrict__       row_max,
			    int64_t     row_max_stride,
			    void* __restrict__       col_max,
			    int64_t     col_max_stride,
			    int32_t*    converged)
{
  const int64_t batch_index = hipBlockIdx_y;
  if(converged[batch_index] == 0)
    {
      bsr_max_row_col_device<BLOCKSIZE>(
	mb,
	nb,
	block_dim,
	dir,
	base,
	reinterpret_cast<const I* __restrict__>(bsr_ptr),
	reinterpret_cast<const J* __restrict__>(bsr_ind),
	reinterpret_cast<const T* __restrict__>(bsr_val) + batch_index * val_stride,
	reinterpret_cast<U* __restrict__>(row_max) + batch_index * row_max_stride,
	reinterpret_cast<U* __restrict__>(col_max) + batch_index * col_max_stride);
    }
}

typedef void (*bsr_max_row_col_kernel_t)(int64_t              mb,
					 int64_t              nb,
					 int64_t              block_dim,
					 rocsparse_direction  dir,
					 rocsparse_index_base base,
					 const void* __restrict__ bsr_ptr,
					 const void* __restrict__ bsr_ind,
					 const void* __restrict__ bsr_val,
					 int64_t     val_stride,
					 void* __restrict__       row_max,
					 int64_t     row_max_stride,
					 void* __restrict__       col_max,
					 int64_t     col_max_stride,
					 int32_t*    converged);

namespace
{
  template <typename I, typename J, typename T>
  static bsr_max_row_col_kernel_t find_bsr_kernel_U(rocsparse_datatype U_type)
  {
    switch(U_type)
      {
      case rocsparse_datatype_f32_r:
	return bsr_max_row_col_kernel<512, I, J, T, float>;
      case rocsparse_datatype_f64_r:
	return bsr_max_row_col_kernel<512, I, J, T, double>;
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I, typename J>
  static bsr_max_row_col_kernel_t find_bsr_kernel_T(rocsparse_datatype T_type,
						    rocsparse_datatype U_type)
  {
    switch(T_type)
      {
      case rocsparse_datatype_f32_r:
	return find_bsr_kernel_U<I, J, float>(U_type);
      case rocsparse_datatype_f64_r:
	return find_bsr_kernel_U<I, J, double>(U_type);
      case rocsparse_datatype_f32_c:
	return find_bsr_kernel_U<I, J, rocsparse_float_complex>(U_type);
      case rocsparse_datatype_f64_c:
	return find_bsr_kernel_U<I, J, rocsparse_double_complex>(U_type);
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I>
  static bsr_max_row_col_kernel_t find_bsr_kernel_J(rocsparse_indextype J_type,
						    rocsparse_datatype  T_type,
						    rocsparse_datatype  U_type)
  {
    switch(J_type)
      {
      case rocsparse_indextype_u16:
	return nullptr;
      case rocsparse_indextype_i32:
	return find_bsr_kernel_T<I, int32_t>(T_type, U_type);
      case rocsparse_indextype_i64:
	return find_bsr_kernel_T<I, int64_t>(T_type, U_type);
      }
    return nullptr;
  }
}

bsr_max_row_col_kernel_t find_bsr_max_row_col_kernel(rocsparse_indextype I_type,
						     rocsparse_indextype J_type,
						     rocsparse_datatype  T_type,
						     rocsparse_datatype  U_type)
{
  switch(I_type)
    {
    case rocsparse_indextype_u16:
      return nullptr;
    case rocsparse_indextype_i32:
      return find_bsr_kernel_J<int32_t>(J_type, T_type, U_type);
    case rocsparse_indextype_i64:
      return find_bsr_kernel_J<int64_t>(J_type, T_type, U_type);
    }
  return nullptr;
}

// ============================================================================
// BELL format (Blocked ELL)
// ============================================================================
//
// ELL at the block level. Each thread processes one block-row.
// For each block-ELL slot, it iterates over the block_dim x block_dim dense
// sub-block to compute row max and atomically update column max.
//
// Block-ELL layout: block-column-major across block-rows.
//   bell_col_ind[p * Mb + block_row]  gives the block-column index for
//   the p-th ELL entry of block_row (or -1 for padding).
//
// Parameters:
//   Mb            - number of block-rows (= rows / block_dim)
//   Nb            - number of block-columns (= cols / block_dim)
//   ell_width     - max number of blocks per block-row (= ell_cols / block_dim)
//   block_dim     - block dimension
//   dir           - storage direction within blocks
//   base          - index base
//   bell_col_ind  - block-column indices, size Mb * ell_width
//   bell_val      - block values, size Mb * ell_width * block_dim * block_dim
//   row_max       - output: max |val| per point-row, size Mb * block_dim
//   col_max       - output: max |val| per point-column, size Nb * block_dim
//
template <uint32_t BLOCKSIZE, typename I, typename T, typename U>
ROCSPARSE_DEVICE_ILF void bell_max_row_col_device(int64_t              Mb,
						   int64_t              Nb,
						   int64_t              ell_width,
						   int64_t              block_dim,
						   rocsparse_direction  dir,
						   rocsparse_index_base base,
						   const I* __restrict__ bell_col_ind,
						   const T* __restrict__ bell_val,
						   U* __restrict__       row_max,
						   U* __restrict__       col_max)
{
  const int64_t block_row = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(block_row < Mb)
    {
      for(int64_t bi = 0; bi < block_dim; ++bi)
	{
	  U rmax = static_cast<U>(0);
	  for(int64_t p = 0; p < ell_width; ++p)
	    {
	      const int64_t ell_idx   = p * Mb + block_row;
	      const I       block_col = bell_col_ind[ell_idx] - base;
	      if(block_col < 0 || block_col >= Nb)
		{
		  break;
		}
	      for(int64_t bj = 0; bj < block_dim; ++bj)
		{
		  const int64_t val_idx
		    = (dir == rocsparse_direction_row)
		    ? (block_dim * block_dim * ell_idx + bi * block_dim + bj)
		    : (block_dim * block_dim * ell_idx + bi + bj * block_dim);
		  const U aval = static_cast<U>(std::abs(bell_val[val_idx]));
		  rmax         = (aval > rmax) ? aval : rmax;
		  rocsparse::atomic_max(col_max + block_col * block_dim + bj, aval);
		}
	    }
	  row_max[block_row * block_dim + bi] = rmax;
	}
    }
}

template <uint32_t BLOCKSIZE, typename I, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void bell_max_row_col_kernel(int64_t              Mb,
			     int64_t              Nb,
			     int64_t              ell_width,
			     int64_t              block_dim,
			     rocsparse_direction  dir,
			     rocsparse_index_base base,
			     const void* __restrict__ bell_col_ind,
			     const void* __restrict__ bell_val,
			     int64_t     val_stride,
			     void* __restrict__       row_max,
			     int64_t     row_max_stride,
			     void* __restrict__       col_max,
			     int64_t     col_max_stride,
			     int32_t*    converged)
{
  const int64_t batch_index = hipBlockIdx_y;
  if(converged[batch_index] == 0)
    {
      bell_max_row_col_device<BLOCKSIZE>(
	Mb,
	Nb,
	ell_width,
	block_dim,
	dir,
	base,
	reinterpret_cast<const I* __restrict__>(bell_col_ind),
	reinterpret_cast<const T* __restrict__>(bell_val) + batch_index * val_stride,
	reinterpret_cast<U* __restrict__>(row_max) + batch_index * row_max_stride,
	reinterpret_cast<U* __restrict__>(col_max) + batch_index * col_max_stride);
    }
}

typedef void (*bell_max_row_col_kernel_t)(int64_t              Mb,
					  int64_t              Nb,
					  int64_t              ell_width,
					  int64_t              block_dim,
					  rocsparse_direction  dir,
					  rocsparse_index_base base,
					  const void* __restrict__ bell_col_ind,
					  const void* __restrict__ bell_val,
					  int64_t     val_stride,
					  void* __restrict__       row_max,
					  int64_t     row_max_stride,
					  void* __restrict__       col_max,
					  int64_t     col_max_stride,
					  int32_t*    converged);

namespace
{
  template <typename I, typename T>
  static bell_max_row_col_kernel_t find_bell_kernel_U(rocsparse_datatype U_type)
  {
    switch(U_type)
      {
      case rocsparse_datatype_f32_r:
	return bell_max_row_col_kernel<512, I, T, float>;
      case rocsparse_datatype_f64_r:
	return bell_max_row_col_kernel<512, I, T, double>;
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I>
  static bell_max_row_col_kernel_t find_bell_kernel_T(rocsparse_datatype T_type,
						      rocsparse_datatype U_type)
  {
    switch(T_type)
      {
      case rocsparse_datatype_f32_r:
	return find_bell_kernel_U<I, float>(U_type);
      case rocsparse_datatype_f64_r:
	return find_bell_kernel_U<I, double>(U_type);
      case rocsparse_datatype_f32_c:
	return find_bell_kernel_U<I, rocsparse_float_complex>(U_type);
      case rocsparse_datatype_f64_c:
	return find_bell_kernel_U<I, rocsparse_double_complex>(U_type);
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }
}

bell_max_row_col_kernel_t find_bell_max_row_col_kernel(rocsparse_indextype I_type,
						       rocsparse_datatype  T_type,
						       rocsparse_datatype  U_type)
{
  switch(I_type)
    {
    case rocsparse_indextype_u16:
      return nullptr;
    case rocsparse_indextype_i32:
      return find_bell_kernel_T<int32_t>(T_type, U_type);
    case rocsparse_indextype_i64:
      return find_bell_kernel_T<int64_t>(T_type, U_type);
    }
  return nullptr;
}

// ============================================================================
// SELL format (Sliced ELL)
// ============================================================================
//
// Each thread block processes one slice of sell_slice_size rows.
// Each thread handles one row within the slice.
//
// Within a slice s (rows [s*S, (s+1)*S)), data is stored row-interleaved:
//   index at sell_slice_offsets[s] + j * sell_slice_size + local_row
// where j iterates over the per-slice ELL width.
//
// Parameters:
//   m                  - number of rows
//   n                  - number of columns
//   sell_slice_size    - number of rows per slice
//   base               - index base
//   sell_slice_offsets - slice offset array, size nslices+1 (nslices = ceil(m/sell_slice_size))
//   sell_col_ind       - column indices, size sell_colval_size
//   sell_val           - values, size sell_colval_size
//   row_max            - output: max |val| per row, size m
//   col_max            - output: max |val| per column, size n
//
template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_DEVICE_ILF void sell_max_row_col_device(int64_t              m,
						   int64_t              n,
						   J                    sell_slice_size,
						   rocsparse_index_base base,
						   const I* __restrict__ sell_slice_offsets,
						   const J* __restrict__ sell_col_ind,
						   const T* __restrict__ sell_val,
						   U* __restrict__       row_max,
						   U* __restrict__       col_max)
{
  const int64_t slice_id  = hipBlockIdx_x;
  const int64_t local_row = hipThreadIdx_x;
  const int64_t row       = slice_id * sell_slice_size + local_row;

  if(local_row < sell_slice_size && row < m)
    {
      const I start = sell_slice_offsets[slice_id];
      const I end   = sell_slice_offsets[slice_id + 1];
      U       rmax  = static_cast<U>(0);

      for(I j = start + local_row; j < end; j += sell_slice_size)
	{
	  const J col = sell_col_ind[j] - base;
	  if(col >= 0 && col < n)
	    {
	      const U aval = static_cast<U>(std::abs(sell_val[j]));
	      rmax         = (aval > rmax) ? aval : rmax;
	      rocsparse::atomic_max(col_max + col, aval);
	    }
	}
      row_max[row] = rmax;
    }
}

template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void sell_max_row_col_kernel(int64_t              m,
			     int64_t              n,
			     int64_t              sell_slice_size,
			     rocsparse_index_base base,
			     const void* __restrict__ sell_slice_offsets,
			     const void* __restrict__ sell_col_ind,
			     const void* __restrict__ sell_val,
			     int64_t     val_stride,
			     void* __restrict__       row_max,
			     int64_t     row_max_stride,
			     void* __restrict__       col_max,
			     int64_t     col_max_stride,
			     int32_t*    converged)
{
  const int64_t batch_index = hipBlockIdx_y;
  if(converged[batch_index] == 0)
    {
      sell_max_row_col_device<BLOCKSIZE>(
	m,
	n,
	static_cast<J>(sell_slice_size),
	base,
	reinterpret_cast<const I* __restrict__>(sell_slice_offsets),
	reinterpret_cast<const J* __restrict__>(sell_col_ind),
	reinterpret_cast<const T* __restrict__>(sell_val) + batch_index * val_stride,
	reinterpret_cast<U* __restrict__>(row_max) + batch_index * row_max_stride,
	reinterpret_cast<U* __restrict__>(col_max) + batch_index * col_max_stride);
    }
}

typedef void (*sell_max_row_col_kernel_t)(int64_t              m,
					  int64_t              n,
					  int64_t              sell_slice_size,
					  rocsparse_index_base base,
					  const void* __restrict__ sell_slice_offsets,
					  const void* __restrict__ sell_col_ind,
					  const void* __restrict__ sell_val,
					  int64_t     val_stride,
					  void* __restrict__       row_max,
					  int64_t     row_max_stride,
					  void* __restrict__       col_max,
					  int64_t     col_max_stride,
					  int32_t*    converged);

namespace
{
  template <typename I, typename J, typename T>
  static sell_max_row_col_kernel_t find_sell_kernel_U(rocsparse_datatype U_type)
  {
    switch(U_type)
      {
      case rocsparse_datatype_f32_r:
	return sell_max_row_col_kernel<512, I, J, T, float>;
      case rocsparse_datatype_f64_r:
	return sell_max_row_col_kernel<512, I, J, T, double>;
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I, typename J>
  static sell_max_row_col_kernel_t find_sell_kernel_T(rocsparse_datatype T_type,
						      rocsparse_datatype U_type)
  {
    switch(T_type)
      {
      case rocsparse_datatype_f32_r:
	return find_sell_kernel_U<I, J, float>(U_type);
      case rocsparse_datatype_f64_r:
	return find_sell_kernel_U<I, J, double>(U_type);
      case rocsparse_datatype_f32_c:
	return find_sell_kernel_U<I, J, rocsparse_float_complex>(U_type);
      case rocsparse_datatype_f64_c:
	return find_sell_kernel_U<I, J, rocsparse_double_complex>(U_type);
      case rocsparse_datatype_f16_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_bf16_r:
	return nullptr;
      }
    return nullptr;
  }

  template <typename I>
  static sell_max_row_col_kernel_t find_sell_kernel_J(rocsparse_indextype J_type,
						      rocsparse_datatype  T_type,
						      rocsparse_datatype  U_type)
  {
    switch(J_type)
      {
      case rocsparse_indextype_u16:
	return nullptr;
      case rocsparse_indextype_i32:
	return find_sell_kernel_T<I, int32_t>(T_type, U_type);
      case rocsparse_indextype_i64:
	return find_sell_kernel_T<I, int64_t>(T_type, U_type);
      }
    return nullptr;
  }
}

sell_max_row_col_kernel_t find_sell_max_row_col_kernel(rocsparse_indextype I_type,
						       rocsparse_indextype J_type,
						       rocsparse_datatype  T_type,
						       rocsparse_datatype  U_type)
{
  switch(I_type)
    {
    case rocsparse_indextype_u16:
      return nullptr;
    case rocsparse_indextype_i32:
      return find_sell_kernel_J<int32_t>(J_type, T_type, U_type);
    case rocsparse_indextype_i64:
      return find_sell_kernel_J<int64_t>(J_type, T_type, U_type);
    }
  return nullptr;
}

// ============================================================================
// Unified launch function
// ============================================================================
//
// Launches the appropriate max-row-col kernel based on A->format.
// Both D_left (size m) and D_right (size n) must be zero-initialized
// before calling this function.
//
static constexpr uint32_t LAUNCH_BLOCKSIZE = 512;

rocsparse_status launch_max_row_col(rocsparse_handle           handle,
				    rocsparse_const_spmat_descr A,
				    rocsparse_dnvec_descr       D_left,
				    rocsparse_dnvec_descr       D_right,
				    void*                       converged)
{
  hipStream_t stream;
  rocsparse_get_stream(handle, &stream);

    if (D_left->data_type != D_right->data_type)
        return rocsparse_status_not_implemented;

  const rocsparse_datatype U_type = D_left->data_type;
if (A->format == rocsparse_format_csc)
  {
     RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_set_zero(handle,
							 D_left,
							 nullptr));
  }
  else
  {
  RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_set_zero(handle,
							 D_right,
							 nullptr));

  }

  switch(A->format)
    {
      // ---------------------------------------------------------------- CSR
    case rocsparse_format_csr:
      {
	auto kernel = find_csr_max_row_col_kernel(A->row_type,
						  A->col_type,
						  A->data_type,
						  U_type);
	if(kernel == nullptr)
	  return rocsparse_status_not_implemented;
      
	RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
	  kernel,
	  dim3((A->rows - 1) / LAUNCH_BLOCKSIZE + 1, A->batch_count),
	  dim3(LAUNCH_BLOCKSIZE),
	  0,
	  stream,
	  A->rows,
	  A->idx_base,
	  A->const_row_data,
	  A->const_col_data,
	  A->const_val_data,
	  A->columns_values_batch_stride,
	  D_left->values,
	  D_left->batch_stride,
	  D_right->values,
	  D_right->batch_stride,
	  reinterpret_cast<int32_t*>(converged));

      
	return rocsparse_status_success;
      }

      // ---------------------------------------------------------------- CSC
    case rocsparse_format_csc:
      {
	auto kernel = find_csc_max_row_col_kernel(A->col_type,
						  A->row_type,
						  A->data_type,
						  U_type);
	if(kernel == nullptr)
	  return rocsparse_status_not_implemented;
	RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
	  kernel,
	  dim3((A->cols - 1) / LAUNCH_BLOCKSIZE + 1, A->batch_count),
	  dim3(LAUNCH_BLOCKSIZE),
	  0,
	  stream,
	  A->cols,
	  A->idx_base,
	  A->const_col_data,
	  A->const_row_data,
	  A->const_val_data,
	  A->columns_values_batch_stride,
	  D_left->values,
	  D_left->batch_stride,
	  D_right->values,
	  D_right->batch_stride,
	  reinterpret_cast<int32_t*>(converged));
	return rocsparse_status_success;
      }

      // ---------------------------------------------------------------- ELL
    case rocsparse_format_ell:
      {
	auto kernel = find_ell_max_row_col_kernel(A->col_type,
						  A->data_type,
						  U_type);
	if(kernel == nullptr)
	  return rocsparse_status_not_implemented;
	RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
	  kernel,
	  dim3((A->rows - 1) / LAUNCH_BLOCKSIZE + 1, A->batch_count),
	  dim3(LAUNCH_BLOCKSIZE),
	  0,
	  stream,
	  A->rows,
	  A->cols,
	  A->ell_width,
	  A->idx_base,
	  A->const_col_data,
	  A->const_val_data,
	  A->columns_values_batch_stride,
	  D_left->values,
	  D_left->batch_stride,
	  D_right->values,
	  D_right->batch_stride,
	  reinterpret_cast<int32_t*>(converged));
	return rocsparse_status_success;
      }

      // ---------------------------------------------------------------- COO
    case rocsparse_format_coo:
      {
	auto kernel = find_coo_max_row_col_kernel(A->row_type,
						  A->data_type,
						  U_type);
	if(kernel == nullptr)
	  return rocsparse_status_not_implemented;
	RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
	  kernel,
	  dim3((A->nnz - 1) / LAUNCH_BLOCKSIZE + 1, A->batch_count),
	  dim3(LAUNCH_BLOCKSIZE),
	  0,
	  stream,
	  A->nnz,
	  A->idx_base,
	  A->const_row_data,
	  A->const_col_data,
	  A->const_val_data,
	  A->columns_values_batch_stride,
	  D_left->values,
	  D_left->batch_stride,
	  D_right->values,
	  D_right->batch_stride,
	  reinterpret_cast<int32_t*>(converged));
	return rocsparse_status_success;
      }

      // ---------------------------------------------------------------- COO_AOS
    case rocsparse_format_coo_aos:
      {
	auto kernel = find_coo_aos_max_row_col_kernel(A->row_type,
						      A->data_type,
						      U_type);
	if(kernel == nullptr)
	  return rocsparse_status_not_implemented;
	RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
	  kernel,
	  dim3((A->nnz - 1) / LAUNCH_BLOCKSIZE + 1, A->batch_count),
	  dim3(LAUNCH_BLOCKSIZE),
	  0,
	  stream,
	  A->nnz,
	  A->idx_base,
	  A->const_ind_data,
	  A->const_val_data,
	  A->columns_values_batch_stride,
	  D_left->values,
	  D_left->batch_stride,
	  D_right->values,
	  D_right->batch_stride,
	  reinterpret_cast<int32_t*>(converged));
	return rocsparse_status_success;
      }

      // ---------------------------------------------------------------- BSR
    case rocsparse_format_bsr:
      {
	auto kernel = find_bsr_max_row_col_kernel(A->row_type,
						  A->col_type,
						  A->data_type,
						  U_type);
	if(kernel == nullptr)
	  return rocsparse_status_not_implemented;
	const int64_t mb = A->rows;
	const int64_t nb = A->cols;
	RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
	  kernel,
	  dim3((mb - 1) / LAUNCH_BLOCKSIZE + 1, A->batch_count),
	  dim3(LAUNCH_BLOCKSIZE),
	  0,
	  stream,
	  mb,
	  nb,
	  A->block_dim,
	  A->block_dir,
	  A->idx_base,
	  A->const_row_data,
	  A->const_col_data,
	  A->const_val_data,
	  A->columns_values_batch_stride,
	  D_left->values,
	  D_left->batch_stride,
	  D_right->values,
	  D_right->batch_stride,
	  reinterpret_cast<int32_t*>(converged));
	return rocsparse_status_success;
      }

      // ---------------------------------------------------------------- BELL
    case rocsparse_format_bell:
      {
	auto kernel = find_bell_max_row_col_kernel(A->col_type,
						   A->data_type,
						   U_type);
	if(kernel == nullptr)
	  return rocsparse_status_not_implemented;
	const int64_t Mb        = A->rows / A->block_dim;
	const int64_t Nb        = A->cols / A->block_dim;
	const int64_t ell_width = A->ell_cols / A->block_dim;
	RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
	  kernel,
	  dim3((Mb - 1) / LAUNCH_BLOCKSIZE + 1, A->batch_count),
	  dim3(LAUNCH_BLOCKSIZE),
	  0,
	  stream,
	  Mb,
	  Nb,
	  ell_width,
	  A->block_dim,
	  A->block_dir,
	  A->idx_base,
	  A->const_col_data,
	  A->const_val_data,
	  A->columns_values_batch_stride,
	  D_left->values,
	  D_left->batch_stride,
	  D_right->values,
	  D_right->batch_stride,
	  reinterpret_cast<int32_t*>(converged));
	return rocsparse_status_success;
      }

      // ---------------------------------------------------------------- SELL
    case rocsparse_format_sell:
      {
	auto kernel = find_sell_max_row_col_kernel(A->row_type,
						   A->col_type,
						   A->data_type,
						   U_type);
	if(kernel == nullptr)
	  return rocsparse_status_not_implemented;
	const int64_t nslices = (A->rows + A->sell_slice_size - 1) / A->sell_slice_size;
	RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
	  kernel,
	  dim3(nslices, A->batch_count),
	  dim3(LAUNCH_BLOCKSIZE),
	  0,
	  stream,
	  A->rows,
	  A->cols,
	  A->sell_slice_size,
	  A->idx_base,
	  A->const_row_data,
	  A->const_col_data,
	  A->const_val_data,
	  A->columns_values_batch_stride,
	  D_left->values,
	  D_left->batch_stride,
	  D_right->values,
	  D_right->batch_stride,
	  reinterpret_cast<int32_t*>(converged));
	return rocsparse_status_success;
      }
    }

  return rocsparse_status_not_implemented;
}
