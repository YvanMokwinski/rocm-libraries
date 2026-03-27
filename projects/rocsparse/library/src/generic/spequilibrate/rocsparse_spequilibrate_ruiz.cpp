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
#include "rocsparse_spequilibrate_descr.hpp"
#include "rocsparse_spequilibrate_ruiz.hpp"
#include "rocsparse_assign_async.hpp"

static rocsparse_status rocsparse_dnvec_set_one(rocsparse_handle handle,
						rocsparse_dnvec_descr that,
						rocsparse_error * p_error)
  
{
  RETURN_IF_ROCSPARSE_ERROR(rocsparse::gassign_async_one(that->batch_count,
							 that->size,
							 that->data_type,
							 that->values,
							 that->inc,
							 handle->stream));
  return rocsparse_status_success;
}
    
static rocsparse_status rocsparse_dnvec_set_zero(rocsparse_handle handle,
						 rocsparse_dnvec_descr that,
						 rocsparse_error * p_error)
{
  RETURN_IF_ROCSPARSE_ERROR(rocsparse::gassign_async_zero(that->batch_count,
							  that->size,
							  that->data_type,
							  that->values,
							  that->inc,
							  handle->stream));
  return rocsparse_status_success;
}


// ============================================================================
// CSR format
// ============================================================================
//
// Scale CSR matrix values in place: A = diag(D_left) * A * diag(D_right).
// Each thread processes one row:
//   val[k] *= D_left[row] * D_right[ind[k] - base]
//
template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_DEVICE_ILF void csr_update_device(int64_t              m,
					     rocsparse_index_base base,
					     const I* __restrict__ ptr,
					     const J* __restrict__ ind,
					     T* __restrict__       val,
					     const U* __restrict__ D_left,
					     const U* __restrict__ D_right)
{
  const int64_t row = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(row < m)
    {
      const I row_begin = ptr[row] - base;
      const I row_end   = ptr[row + 1] - base;
      const U dl        = D_left[row];
      for(I k = row_begin; k < row_end; ++k)
	{
	  val[k] = dl * val[k] * D_right[ind[k] - base];
	}
    }
}

template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_KERNEL(BLOCKSIZE)
void csr_update_kernel(int64_t              m,
		       rocsparse_index_base base,
		       const void* __restrict__ ptr,
		       const void* __restrict__ ind,
		       void* __restrict__       val,
		       int64_t     val_stride,
		       const void* __restrict__ D_left,
		       int64_t     D_left_stride,
		       const void* __restrict__ D_right,
		       int64_t     D_right_stride,
		       int32_t*    converged)
{
  const int64_t batch_index = hipBlockIdx_y;
  if(converged[batch_index] == 0)
    {
      csr_update_device<BLOCKSIZE>(
	m,
	base,
	reinterpret_cast<const I* __restrict__>(ptr),
	reinterpret_cast<const J* __restrict__>(ind),
	reinterpret_cast<T* __restrict__>(val) + batch_index * val_stride,
	reinterpret_cast<const U* __restrict__>(D_left) + batch_index * D_left_stride,
	reinterpret_cast<const U* __restrict__>(D_right) + batch_index * D_right_stride);
    }
}

#if 0
template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_DEVICE_ILF void ruiz_part1_device(int64_t              nnz,
					    rocsparse_index_base base,
					    const J* __restrict__ ind,
					    const T* __restrict__ val,
					    U*__restrict__ col_max)
{
  const int64_t gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(gid < nnz)
    {
      rocsparse::atomic_max(col_max + ind[k] - base, std::abs(val[gid]));
    }  
}


template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_DEVICE_ILF void ruiz_part2_device(int64_t              n
					    rocsparse_index_base base,
					    const J* __restrict__ ind,
					    const T* __restrict__ val,
					    U*__restrict__ col_max)
{
  const int64_t gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
  if(gid < n)
    {
      const double s = (max_col[gid] > 0) ? static_cast<double>(1) / rocsparse::sqrt(double(max_col[gid])) :  static_cast<double>(1);
      const double c = (max_col[gid] > 0) ? (s_one - max_col[gid] * s) :  static_cast<double>(0);
      D_right[gid] *= s;
      rocsparse::atomic_max(nrm, c);
    }  
}


template <uint32_t BLOCKSIZE, typename I, typename J, typename T, typename U>
ROCSPARSE_DEVICE_ILF void ruiz_part3_device(int64_t              m,
						 int64_t              n,
						 rocsparse_index_base base,
						 const I* __restrict__ ptr,
						 const J* __restrict__ ind,
						 const T* __restrict__ val,
						 U*__restrict__ nrm)
{
  static constexpr U s_one = static_cast<U>(1);
  const int64_t gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;  
  if(row < m)
    {
      const I row_begin = ptr[row] - base;
      const I row_end   = ptr[row + 1] - base;
      U       rmax      = static_cast<U>(0);
      for(I k = row_begin; k < row_end; ++k)
	{
	  const U aval = static_cast<U>(std::abs(val[k]));
	  rmax         = (aval > rmax) ? aval : rmax;


	  //	  rocsparse::atomic_max(col_max + ind[k] - base, aval);
	}

      const double s = (rmax > 0) ? static_cast<double>(1) / rocsparse::sqrt(double(rmax)) :  static_cast<double>(1);
      const double c = (rmax > 0) ? (s_one - rmax * s) :  static_cast<double>(0);
      //
      // Update.
      //
      D_left[row] *= s;
      for(I k = row_begin; k < row_end; ++k)
	{
	  val[k] *= s * D_right[ind[k] - base];      	  
	}
      rocsparse::atomic_max(nrm, c);
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
			    int64_t              n,
			    rocsparse_index_base base,
			    const void* __restrict__ ptr,
			    const void* __restrict__ ind,
			    const void* __restrict__ val,
			    int64_t     val_stride,
			    void* __restrict__       D_left,
			    int64_t                  D_left_stride,
			    void* __restrict__       D_right,
			    int64_t                  D_right_stride,
			    void* __restrict__       col_max,
			    int64_t                  col_max_stride,
			    int32_t*                 converged,
			    int32_t                  converged_inc)
{
  const int64_t batch_index = hipBlockIdx_y;
  if(converged[batch_index] == 0)
    {
      csr_max_row_col_device<BLOCKSIZE>(m,
					n,
					base,
					reinterpret_cast<const I* __restrict__>(ptr),
					reinterpret_cast<const J* __restrict__>(ind),
					reinterpret_cast<const T* __restrict__>(val) + batch_index * val_stride,
					reinterpret_cast<U* __restrict__>(D_left) + batch_index * D_left_stride,
					reinterpret_cast<U* __restrict__>(D_right) + batch_index * D_right_stride,
					reinterpret_cast<U* __restrict__>(col_max) + batch_index * col_max_stride,
					converged + batch_index * converged_inc);
    }
}

#endif

namespace rocsparse
{
  
  rocsparse_status spequilibrate_ruiz(rocsparse_handle handle,
				      rocsparse_spequilibrate_descr descr,
				      rocsparse_spequilibrate_stage      stage,
				      rocsparse_spmat_descr A,
				      rocsparse_dnvec_descr D_left,
				      rocsparse_dnvec_descr D_right,
				      size_t buffer_size_in_bytes,
				      void * buffer)  
  {
#if 0
    RETURN_IF_ROCSPARSE_ERROR((( buffer_size_in_bytes < descr->get_buffer_size_in_bytes())
			       ? rocsparse_status_invalid_size
			       : rocsparse_status_success));
#endif			    
    RETURN_IF_ROCSPARSE_ERROR((D_left->data_type != D_right->data_type)
			    ? rocsparse_status_invalid_value
			    : rocsparse_status_success);

  const rocsparse_datatype D_datatype = D_left->data_type;
  const size_t D_datatype_sizeof  = rocsparse::datatype_sizeof(D_datatype);
  
  hipStream_t stream;
  rocsparse_get_stream(handle,&stream);


  //  auto found_compute_kernel = rocsparse::spequilibrate_ruiz_find_compute_kernel(A, D_left, D_right);
  //  auto found_update_kernel = rocsparse::spequilibrate_ruiz_find_update_kernel(A, D_left, D_right);

  rocsparse::ruiz_buffer_t ruiz_buffer(A->rows,
				       A->cols,
				       A->batch_count,
				       D_left->data_type,
				       buffer,
				       buffer_size_in_bytes);

  const int64_t nmaxiter = descr->get_nmaxiter();
  descr->set_niter(nmaxiter);
  
  RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_set_one(handle,D_left, nullptr));
  RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_set_one(handle,D_right, nullptr));

  int32_t * converged = reinterpret_cast<int32_t*>(ruiz_buffer.get_converged());

  auto b_left = ruiz_buffer.get_left();
  auto b_right = ruiz_buffer.get_right();
  for(int64_t iter = 0; iter < nmaxiter; ++iter)
    {
      if (iter > 0)
	{
#if 0
        //
        // D_left = D_left * tmp_D_left
        //
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_hadamard(handle, b_left, D_left));

	//
	// The calculation of D_right is using atomics.
	// 
        //
        // D_right = D_right * tmp_D_right
        //
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnvec_hadamard(handle,
							   b_right,
							   D_right,
							   converged));

	//
	// A = D_left * A * D_right
	//
        RETURN_IF_ROCSPARSE_ERROR(launch_scale(handle,
					       A,
					       b_left,
					       b_right,
					       onverged));
	
#endif
		
	}
#if 0
      //
      // tmp_D_left = max_row(A)
      // tmp_D_right = max_col(A)
      // 
      RETURN_IF_ROCSPARSE_ERROR(rocsparse::launch_max_row_col(handle, A, ruiz_buffer.get_left(), ruiz_buffer.get_right(), nullptr));
      
      double nrm = 0;
      int32_t converged = 0;      
      RETURN_IF_ROCSPARSE_ERROR(rocsparse::convergence(handle,
						       iter,
						       ruiz_buffer.get_left(),
						       ruiz_buffer.get_right(),
						       &nrm,
						       &converged,
						       nullptr));
      RETURN_IF_HIP_ERROR(hipStreamSynchronize(stream));
      if (converged)
	{
	  descr->set_niter(iter);
	  break;
	}
#endif
    }
  
  return rocsparse_status_success;
}
  



}
