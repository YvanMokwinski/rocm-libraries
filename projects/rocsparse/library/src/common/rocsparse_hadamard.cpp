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

namespace rocsparse
{
  template <typename R, typename A, typename B>
  ROCSPARSE_DEVICE_ILF void hadamard_device(int64_t index_,
					    int64_t size_,
					    R* __restrict__ r_,
					    int64_t r_inc_,
					    const A* __restrict__ a_,
					    int64_t a_inc_,
					    const B* __restrict__ b_,
					    int64_t b_inc_)
  {
    if (index_ < size_)
      {
	r_ += index_ * r_inc_;
	a_ += index_ * a_inc_;
	b_ += index_ * b_inc_;
	auto a = *a_;
	auto b = *b_;
	*r = static_cast<R>(a * b);
      } 
  }
  
  template <uint32_t BLOCKSIZE, typename R, typename A, typename B>
  ROCSPARSE_KERNEL(BLOCKSIZE)
    void hadamard_kernel(int64_t size,
			 void* __restrict__ r_,
			 int64_t r_inc_,
			 int64_t r_stride_,
			 const void* __restrict__ a_,
			 int64_t a_inc_,
			 int64_t a_stride_,
			 const void* __restrict__ b_,
			 int64_t b_inc_,
			 int64_t b_stride_)
  {
    const auto batch_index = hipBlockIdx_y;
    const auto gid = hipThreadIdx_x + BLOCKSIZE * hipBlockIdx_x;
    if (gid < size)
      {
	hadamard_device(gid,
			reinterpret_cast<R*__restrict__>(r_) + r_stride_ * batch_index,
			r_inc_,
			reinterpret_cast<A*__restrict__>(a_) + a_stride_ * batch_index,
			a_inc_,
			reinterpret_cast<B*__restrict__>(b_) + b_stride_ * batch_index,
			b_inc_);
      } 
  }


  
  struct hadamar_t
  {
  public:
    static constexpr uint32_t BLOCKSIZE = 512;
    typedef  void (*kernel_intance_t)(int64_t size,
				      void* __restrict__ r_,
				      int64_t r_inc_,
				      int64_t r_stride_,
				      const void* __restrict__ a_,
				      int64_t a_inc_,
				      int64_t a_stride_,
				      const void* __restrict__ b_,
				      int64_t b_inc_,
				      int64_t b_stride_);
    
    struct kernel_t
    {
      kernel_intance_t instance;
      dim3 grid_blocks;
      dim3 grid_threads;
    };

  protected:
    template<typename R>
    static hadamard_kernel_t find_datatype(rocsparse_datatype datatype)
    {
      switch(datatype)
	{
	case rocsparse_datatype_f32_r:
	  {
	    return hadamard_kernel<BLOCKSIZE,float,float>;
	  }
	case rocsparse_datatype_f32_c:
	{
	  return hadamard_kernel<BLOCKSIZE,rocsparse_float_complex,rocsparse_float_complex>;
	}
	case rocsparse_datatype_f64_r:
	  {
	    return hadamard_kernel<BLOCKSIZE,double,double>;
	  }
	case rocsparse_datatype_f64_c:
	  {
	    return hadamard_kernel<BLOCKSIZE,rocsparse_double_complex,rocsparse_double_complex>;
	  }
	  
	default:
	  {
	    return nullptr;
	  }
	}
    }
    
  public:
    
    static rocsparse_status launch(kernel_t& kernel,
				   rocsparse_handle handle,
				   rocsparse_const_dnvec_descr A,
				   rocsparse_const_dnvec_descr B,
				   rocsparse_dnvec_descr R)
    {
      RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(kernel.instance,
					 kernel.grid_blocks,
					 kernel.grid_threads,
					 0,
					 handle->stream,
					 R->size,
					 R->get_values(),
					 R->get_inc(),
					 R->get_stride(),
					 A->get_const_values(),
					 A->get_inc(),
					 A->get_stride(),
					 B->get_const_values(),
					 B->get_inc(),
					 B->get_stride());
      return rocsparse_status_success;
    }
    
    static void find(kernel_t& kernel,
		     rocsparse_handle handle,
		     rocsparse_const_dnvec_descr A,
		     rocsparse_const_dnvec_descr B,
		     rocsparse_const_dnvec_descr R)
    {
      const auto datatype = r->get_datatype();
      kernel.grid_blocks = dim3( (R->nnz - 1) / BLOCKSIZE + 1, R->batch_count );
      kernel.grid_threads = dim3(BLOCKSIZE); 
      switch(datatype)
	{
	case rocsparse_datatype_f32_r:
	  {
	    kernel.instance = find_datatype<float>(A->data_type);
	    break;
	  }
	  
	case rocsparse_datatype_f32_c:
	  {
	    kernel.instance = find_datatype<rocsparse_float_complex>(A->data_type);
	    break;
	  }
	  
	case rocsparse_datatype_f64_r:
	  {
	    kernel.instance =  find_datatype<double>(A->data_type);
	  break;
	  }
	  
	case rocsparse_datatype_f64_c:
	  {
	    kernel.instance =  find_datatype<double>(A->data_type);
	    break;
	  }
	default:
	  {
	    break;
	  }
	}
    }
    

  };
  
  rocsparse_status rocsparse_dnvec_hadamard(rocsparse_handle handle,
					    rocsparse_const_dnvec_descr A,
					    rocsparse_const_dnvec_descr B,
					    rocsparse_dnvec_descr R,
					    rocsparse_error*p_error)
  {

    hadamar_t::kernel_t kernel;
    hadamar_t::find(kernel,handle,A,B,R);

    RETURN_IF_ROCSPARSE_ERROR(hadamar_t::launch(kernel,handle,A,B,R));
    return rocsparse_status_success;    
  }

