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
#pragma once

#include "rocsparse_spequilibrate_descr.hpp"

namespace rocsparse
{
  rocsparse_status spequilibrate_ruiz(rocsparse_handle            handle,
				      rocsparse_spequilibrate_descr      descr,
				      rocsparse_spequilibrate_stage      stage,
				      rocsparse_spmat_descr A,
				      rocsparse_dnvec_descr D_left,
				      rocsparse_dnvec_descr D_right,
				      size_t                buffer_size_in_bytes,
				      void*                 buffer);
  struct ruiz_buffer_t
  {
  private:
    char  * buffer;
    size_t buffer_size_in_bytes;
    int64_t batch_count;
    double * nrms;
    size_t nrms_size_in_bytes;
    int32_t * converged;
    size_t converged_size_in_bytes;
    _rocsparse_dnvec_descr D_left;
    _rocsparse_dnvec_descr D_right;
  public:
    ruiz_buffer_t(int64_t M,
		  int64_t N,
		  int64_t batch_count_,
		  rocsparse_datatype datatype,
		  void  * buffer_,
		  size_t buffer_size_in_bytes_)
      : buffer(reinterpret_cast<char*>(buffer_))
      , buffer_size_in_bytes(buffer_size_in_bytes_)
      , batch_count(batch_count_)
      , nrms(reinterpret_cast<double*>(buffer + rocsparse::datatype_sizeof(datatype) * (M + N) * batch_count))
      , nrms_size_in_bytes(batch_count * sizeof(double))
      , converged(reinterpret_cast<int32_t*>(nrms + batch_count))
      , converged_size_in_bytes(batch_count * sizeof(int32_t))
      , D_left(batch_count,
	       M,
	       datatype,
	       buffer,
	       buffer,
	       1,
	       (batch_count > 1) ? M : 0)  
      , D_right(batch_count,
		N,
		datatype,
		buffer + M * rocsparse::datatype_sizeof(datatype) * batch_count,
		buffer + M * rocsparse::datatype_sizeof(datatype) * batch_count,
		1,
		(batch_count > 1) ? N : 0)
    {
    }

    size_t get_size_in_bytes()const {return this->buffer_size_in_bytes;}
    void * get_nrms(){ return this->nrms;};
    size_t get_nrms_size_in_bytes(){ return this->nrms_size_in_bytes;};
    void * get_converged(){ return this->converged;};
    size_t get_converged_size_in_bytes(){ return this->converged_size_in_bytes;};

    rocsparse_dnvec_descr get_left(){return &this->D_left;}
    rocsparse_dnvec_descr get_right(){return &this->D_left;}
  };
#if 0  

  typedef void (*spequilibrate_update_kernel_t)(int64_t m,
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
						double  * nrms);

  typedef void (*spequilibrate_compute_kernel_t)(int64_t m,
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
						 double  * nrms);

  spequilibrate_update_kernel_t spequilibrate_ruiz_find_update_kernel(rocsparse_handle handle,
								      rocsparse_spequilibrate_descr descr,
								      rocsparse_spmat_descr A,
								      rocsparse_const_dnvec_descr D_left,
								      rocsparse_const_dnvec_descr D_right);
  
  spequilibrate_compute_kernel_t spequilibrate_ruiz_find_compute_kernel(rocsparse_handle handle,
									rocsparse_spequilibrate_descr descr,
									rocsparse_spmat_descr A,
									rocsparse_const_dnvec_descr D_left,
									rocsparse_const_dnvec_descr D_right);
  
  rocsparse_status              spequilibrate_ruiz_update(rocsparse_handle handle,
							  rocsparse_spequilibrate_descr descr,
							  int64_t iter,
							  rocsparse_spmat_descr A,
							  rocsparse_dnvec_descr D_left,
							  rocsparse_dnvec_descr D_right,
							  spequilibrate_update_kernel_t kernel,
							  rocsparse::ruiz_buffer_t& buffer);
  
  rocsparse_status spequilibrate_ruiz_compute(rocsparse_handle handle,
					      rocsparse_spequilibrate_descr descr,
					      int64_t iter,
					      rocsparse_spmat_descr A,
					      rocsparse_dnvec_descr D_left,
					      rocsparse_dnvec_descr D_right,
					      spequilibrate_compute_kernel_t kernel,
					      rocsparse::ruiz_buffer_t& buffer);
#endif  

}
