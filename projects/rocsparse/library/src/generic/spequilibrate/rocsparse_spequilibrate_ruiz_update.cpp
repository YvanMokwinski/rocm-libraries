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
#include "rocsparse_spequilibrate_ruiz.hpp"

namespace rocsparse
{

  template <uint32_t BLOCKSIZE, typename T, typename I, typename J, typename U>
  ROCSPARSE_DEVICE_ILF void ruiz_csrscale_device(int64_t m,
						 int64_t n,
						 rocsparse_index_base base,
						 const I* __restrict__ ptr,
						 const J* __restrict__ ind,
						 T* __restrict__ val,
						 U* __restrict__ tmp_D_left, 
						 U* __restrict__ tmp_D_right,
						 U* __restrict__ D_left,
						 U* __restrict__ D_right)
  {
    static constexpr U s_one = static_cast<U>(1);
    const auto tid = hipThreadIdx_x;
    const auto gid = tid + BLOCKSIZE * hipBlockIdx_x;
    if (gid < n)
      {
	D_right += gid;
	tmp_D_right += gid;
	*D_right *= *tmp_D_right;
      }
    
    if (gid < m)
      {
	D_left += gid;
	tmp_D_left += gid;
	*D_left *= *tmp_D_left;
      }    
  }

  template <uint32_t BLOCKSIZE, typename T, typename I, typename J, typename U>
  ROCSPARSE_DEVICE_ILF void ruiz_update_device(int64_t m,
					       int64_t n,
					       rocsparse_index_base base,
					       const I* __restrict__ ptr,
					       const J* __restrict__ ind,
					       T* __restrict__ val,
					       U* __restrict__ tmp_D_left, 
					       U* __restrict__ tmp_D_right,
					       U* __restrict__ D_left,
					       U* __restrict__ D_right)
  {
    static constexpr U s_one = static_cast<U>(1);
    const auto tid = hipThreadIdx_x;
    const auto gid = tid + BLOCKSIZE * hipBlockIdx_x;
    if (gid < n)
      {
	D_right += gid;
	tmp_D_right += gid;
	*D_right *= *tmp_D_right;
      }
    
    if (gid < m)
      {
	D_left += gid;
	tmp_D_left += gid;
	*D_left *= *tmp_D_left;
      }    
  }


  
  template <uint32_t BLOCKSIZE, typename T, typename I, typename J, typename U>
  ROCSPARSE_KERNEL(BLOCKSIZE) void ruiz_update_kernel(int64_t m,
						      int64_t n,
						      rocsparse_index_base base,
						      const void* __restrict__ ptr,
						      const void* __restrict__ ind,
						      void* __restrict__ val,
						      void* __restrict__ tmp_D_left,
						      int64_t tmp_D_left_stride,
						      void* __restrict__ tmp_D_right,
						      int64_t tmp_D_right_stride,
						      void* __restrict__       D_left,
						      int64_t D_left_stride,
						      void* __restrict__       D_right,
						      int64_t D_right_stride,
						      int32_t * converged,
						      double  * nrms)
  {
    const int64_t batch_index = hipBlockIdx_y;  
    if (converged[batch_index] == 0)
      {
	if (iter > 0)
	  {
	    ruiz_update_device<BLOCKSIZE>(m,
					  n,
					  base,
					  ptr,
					  ind,
					  val,
					  tmp_D_left + batch_index * tmp_D_left_stride,
					  tmp_D_right + batch_index * tmp_D_right_stride,
					  D_left + batch_index * D_left_stride,
					  D_right + batch_index * D_right_stride);
	  }
	nrms[batch_index] = 0;
      }
  }


  template <uint32_t ...PARAMS,typename T, typename I>
  static auto find_ruiz_update_kernel_j(rocsparse_indextype jtype)
  {
    switch(itype)
      {
      case rocsparse_indextype_i32:
	{
	  return ruiz_update_kernel<PARAMS...,T,I,int32_t,T>(jtype);
	}
      case rocsparse_indextype_i64:
	{
	  return ruiz_update_kernel<PARAMS...,T,I,int64_t,T>(jtype);
	}
      case rocsparse_indextype_i32:
	{
	  RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
	}	
      }
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
  }

  template <uint32_t ...PARAMS,typename T>
  static auto find_ruiz_update_kernel_i(rocsparse_indextype itype,
				   rocsparse_indextype jtype)
  {
    switch(itype)
      {
      case rocsparse_indextype_i32:
	{
	  return find_ruiz_update_kernel_j<PARAMS...,T,int32_t>(jtype);
	}
      case rocsparse_indextype_i64:
	{
	  return find_ruiz_update_kernel_j<PARAMS...,T,int64_t>(jtype);
	}
      case rocsparse_indextype_i32:
	{
	  RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
	}	
      }
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
  }


  spequilibrate_ruiz_update_kernel_t spequilibrate_ruiz_find_update_kernel(rocsparse_datatype datatype,
								      rocsparse_indextype itype,
								      rocsparse_indextype jtype)
  {
    switch(datatype)
      {
      case rocsparse_datatype_f32_r:
	{
	  return find_ruiz_update_kernel_i<512, float>(itype,jtype);
	}
      case rocsparse_datatype_f32_c:
	{
	  return find_ruiz_update_kernel_i<512,  rocsparse_float_complex>(itype,jtype);
	}
      case rocsparse_datatype_f64_r:
	{
	  return find_ruiz_update_kernel_i<512, double>(itype,jtype);
	}
      case rocsparse_datatype_f64_c:
	{
	  return find_ruiz_update_kernel_i<512,  rocsparse_double_complex>(itype,jtype);
	}	
      }
  }


  rocsparse_status spequilibrate_ruiz_update(spequilibrate_update_kernel_t kernel,
					     int64_t iter,
					     rocsparse_spmat_descr A,
					     rocsparse_const_dnvec_descr D_left,
					     rocsparse_const_dnvec_descr D_right,
					     rocsparse::ruiz_buffer_t& buffer,
					     spequilibrate_update_kernel_t kernel)
  {
    RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(kernel,
				       dim3( (A->nnz - 1) / 512 + 1, A->batch_count ),
				       dim3( 512 ),
				       0,
				       stream,
				       iter,
				       A->rows,
				       A->cols,
				       A->idx_base,
				       A->const_row_data,
				       A->const_col_data,
				       A->val_data,
				       A->batch_stride,
				       R->const_values,
				       R->batch_stride,
				       C->const_values,
				       C->batch_stride,
				       buffer.get_D_left(),
				       buffer.get_D_left()->batch_stride,
				       buffer.get_D_right(),
				       buffer.get_D_right()->batch_stride,
				       D_left(),
				       D_left()->batch_stride,
				       D_right(),
				       D_right()->batch_stride,
				       buffer.get_converged(),
				       buffer.get_nrms());
    return rocsparse_status_success;
  }
}
  
