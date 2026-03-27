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

#include "internal/generic/rocsparse_spequilibrate.h"
#include "rocsparse_control.hpp"
#include "rocsparse_enum_utils.hpp"
#include "rocsparse_utility.hpp"
#include "rocsparse_spequilibrate_descr.hpp"
#include "rocsparse_assign_async.hpp"



namespace rocsparse
{


	 template <uint32_t BLOCKSIZE, typename T, typename T_nrm, typename T_tol>
    ROCSPARSE_KERNEL(BLOCKSIZE)
    void convergence_device(size_t size_x,
                       const T* __restrict__ x,
					   int64_t x_inc,
					   size_t size_y,
                       const T* __restrict__ y,
                       int64_t y_inc,
                       T_nrm* __restrict__ nrm,
					   int32_t* __restrict__ converged,
					   T_tol * __restrict__ tol)
    {
        auto    tid = hipThreadIdx_x;
        auto gid = tid + BLOCKSIZE * hipBlockIdx_x;

        __shared__ T shared[BLOCKSIZE*2];
        
    	shared[2*tid] = (gid < size_x) ? std::abs(x[gid * x_inc]) : 0;
    	shared[2*tid+1] = (gid < size_y) ? std::abs(y[gid * y_inc]) : 0;   

        __syncthreads();

        rocsparse::blockreduce_max<BLOCKSIZE*2>(tid, shared);

        if(tid == 0)
        {
			rocsparse::atomic_max(nrm, static_cast<T_nrm>(shared[0]));
			rocsparse::atomic_min(converged, (shared[0] <= tol[0]) ? int32_t(1) : int32_t(0));	
        }
    }



    template <uint32_t BLOCKSIZE, typename I,typename T, typename U>
  ROCSPARSE_KERNEL(BLOCKSIZE)
    void convergence_kernel(int64_t m,
			int64_t n,
			rocsparse_index_base base,
			void * __restrict__ dleft,
			int64_t dleft_stride,
			void * __restrict__ dright,
			int64_t dright_stride,
				double * __restrict__ nrm,
				int64_t nrm_inc,
			int64_t nrm_stride,
			int32_t * converged,
			int43_t converged_inc,
			int64_t converged_stride,
			double * tol,
			int64_t tol_inc,
			int64_t tol_stride)
    {
      const auto batch_index = hipBlockIdx_y;
	  if (converged[batch_index * converged_stride] == 0)
	  {	
      convergence_device<BLOCKSIZE,U>(m,	
				      n,
				      reinterpret_cast<U*>(dleft) + batch_index * dleft_stride,
				      reinterpret_cast<U*>(dright) + batch_index * dright_stride,
				      reinterpret_cast<double*>(nrm) + batch_index * nrm_stride,
					  nrm_inc,
				      converged + batch_index * converged_stride,
					  converged_inc,
				      tol + batch_index * tol_stride,
					  tol_inc); 
    }
}

  compute_kernel_t compute_kernel_launch_find(rocsparse_spequilibrate_descr descr,
					  rocsparse_spmat_descr A,
					  rocsparse_dnvec_descr R,
					  rocsparse_const_dnvec_descr C,
					  dim3& grid_blocks,
					  dim3& grid_threads)
  {
    static constexpr uint32_t BLOCKSIZE = 512;
    compute_kernel_t kernel = nullptr;
   
     kernel = part2<BLOCKSIZE,int32_t,float, float >;
    
    grid_blocks = dim3( (std::max(A->rows,A->cols) - 1) / BLOCKSIZE + 1 );
    grid_threads = dim3( BLOCKSIZE );
    return kernel;
  }
  
  rocsparse_status compute_kernel_launch(rocsparse_spequilibrate_descr descr,
					 rocsparse_spmat_descr A,
					 rocsparse_dnvec_descr R,
					 rocsparse_const_dnvec_descr C,
					 compute_kernel_t  kernel,
					 dim3& grid_blocks,
					 dim3& grid_threads,
					 void * __restrict__ nrm,
					 int32_t * converged,
					 double tol,
					 hipStream_t stream)
  {
    RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(kernel,
				       grid_blocks,
				       grid_threads,
				       0,
				       stream,
				       //
				       A->rows,
				       A->cols,
				       A->idx_base,
				       A->const_row_data,
				       A->val_data,
				       R->values,
				       C->const_values,
				       nrm,
				       converged,
				       tol);    
    return rocsparse_status_success;
  }


  
  template <uint32_t BLOCKSIZE, typename I,typename T, typename U>
  ROCSPARSE_KERNEL(BLOCKSIZE)
  void convergence_kernel(int64_t batch_count,
			  U * __restrict__ nrm,
			  int32_t * batch_converged,
			  int32_t * converged,
			  U tol)
  {
    const uint32_t tid = hipThreadIdx_x;
    const uint32_t gid = tid + BLOCKSIZE * hipBlockIdx_x;
    __shared__ int32_t shared[BLOCKSIZE];

    if (gid < batch_count)
      {
	batch_converged[gid] = (nrm[gid] > tol) ? 1 : 0;
      }
    
    shared[tid] = (gid < batch_count) ? batch_converged[gid] : 0;    
    __syncthreads();  
    rocsparse::blockreduce_max<BLOCKSIZE>(tid, shared);    
    if(tid == 0)
      {
	rocsparse::atomic_max(converged, batch_converged);	
      }
  }
  
  
  template <uint32_t BLOCKSIZE, typename U>
  ROCSPARSE_KERNEL(BLOCKSIZE)
    void convergence(int64_t m,
		     int64_t n,
		     U * __restrict__ dleft,
		     int64_t dleft_stride,
		     const U * __restrict__ dright,
		     int64_t dright_stride,
		     U * __restrict__ nrm,
		     int64_t nrm_stride,
		     int32_t * converged,
		     int64_t converged_stride,
		     U tol)
  {
    const int64_t batch_index = hipBlockIdx_y;
    if (converged[batch_index * converged_stride ] == false)
      {
	convergence_device(m,
			   n,
			   reinterpret_cast<T>(dleft) + batch_index * dleft_stride,
			   dright + batch_index * dright_stride,
			   nrm + batch_index * nrm_stride,
			   converged + batch_index * converged_stride,
			   tol);	
      }
  }

