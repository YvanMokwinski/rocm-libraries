#include "rocsparse_itsol_cg.hpp"
#include "rocsparse_blas.h"
#include "handle.h"
#include <iostream>
#define HIP_CHECK(call)						\
  do {								\
    hipError_t status = call;					\
    if (status != hipSuccess) {					\
      std::cerr << "HIP error: " << hipGetErrorString(status)	\
		<< " at line " << __LINE__ << std::endl;	\
      std::exit(1);						\
    }								\
  } while(0)

rocsparse_itsol_cg_descr_::rocsparse_itsol_cg_descr_(rocsparse_itsol_descr that)
  : rocsparse_itsol_impl_(rocsparse_itsol_alg_cg)
  , m_outputs(new rocsparse_cg_outputs_())
  , m_inputs(new rocsparse_cg_inputs_())
  , m_itsol(that)
{
  that->set_request(rocsparse_itsol_request_matrix_vector);
}


rocsparse_itsol_cg_descr_::~rocsparse_itsol_cg_descr_()
{
  if (this->m_outputs)
    delete this->m_outputs;
  this->m_outputs = nullptr;
  if (this->m_inputs)
    delete this->m_inputs;
  this->m_inputs = nullptr;
  
  (void)hipFree(this->r);
  (void)hipFree(this->p);
}

void rocsparse_itsol_descr_::axpy(rocsparse_handle handle,
				     const void* a,
				     const void* x,
				     void* y)  
{
  const auto in = this->get_inputs();
  const auto type = in->get_datatype_compute();
  const int64_t n = in->get_dimension();
  rocsparse::blas_axpy(handle->blas_handle, n, a,type, x, type,1,y, type, 1);
}

#include <rocblas/rocblas.h>
void rocsparse_itsol_descr_::nrm2(rocsparse_handle handle,
				     const void* a, void * r)
{
  const auto in = this->get_inputs();
  const auto type = in->get_datatype_compute();
  const int64_t n = in->get_dimension();
#if 0
  rocblas_handle h;
  rocblas_create_handle(&h);
  int32_t idx;
  rocblas_idamax(h,n,(const double*)a,1,&idx);
  rocblas_destroy_handle(h);
  double nrm;
  hipMemcpy(&nrm,((const double*)a)+idx,sizeof(double),hipMemcpyDefault);
  nrm = std::abs(nrm);
  memcpy(r,&nrm,sizeof(nrm));
#else
  rocsparse::blas_nrm2(handle->blas_handle,n, a, type, 1, r, type);
#endif
}

void rocsparse_itsol_descr_::dot_product( rocsparse_handle handle,
					     const void* a,
					     const void* b,
					     void*r)
{
  const auto in = this->get_inputs();
  const auto type = in->get_datatype_compute();
  const int64_t n = in->get_dimension();
  rocsparse::blas_dot(handle->blas_handle,n, a, type, 1, b, type, 1, r, type);
  hipDeviceSynchronize();
}

void rocsparse_itsol_descr_::scal(rocsparse_handle handle,
				     const void* a,
				      void* x)
{
  const auto in = this->get_inputs();
  const auto type = in->get_datatype_compute();
  const int64_t n = in->get_dimension();
  rocsparse::blas_scal(handle->blas_handle,n, a, type, x, type, 1);
}

void rocsparse_itsol_descr_::axpy2(rocsparse_handle handle,
				      const void* a,
				      const void* x,
				      const void * b,
				      void* y)
{
  this->scal(handle,b,y);
  this->axpy(handle,a,x,y);  
}

#if 0
void rocsparse_itsol_cg_descr_::axpy(const void* a,
				     const void* x,
				     void* y)  
{
  const auto inputs = this->m_itsol->get_inputs();
  const auto datatype = inputs->get_datatype_compute();
  const int64_t dimension = inputs->get_dimension();
  switch (datatype)
    {
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_u8_r:
	{
	  break;
	}
      case rocsparse_datatype_f32_r:
	{
	  rocblas_saxpy(this->m_itsol->get_blas_handle(), dimension, (const float*)a, (const float*)x, 1, (float*)y, 1);   
	  break;
	}
      case rocsparse_datatype_f64_r:
	{
	  rocblas_daxpy(this->m_itsol->get_blas_handle(), dimension,(const double*) a, (const double*)x, 1, (double*)y, 1);         
	  break;
	}
    }
}


double rocsparse_itsol_cg_descr_::nrm2(const void* a)
{
  const auto inputs = this->m_itsol->get_inputs();
  const auto datatype = inputs->get_datatype_compute();
  const int64_t dimension = inputs->get_dimension();
  switch (datatype)
    {
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_u8_r:
	{
	double nrm = 0;
	//	rocblas_dnrm2(this->m_itsol->get_blas_handle(),dimension,(const double*)r,1,&nrm);
	return nrm;
	}
      
    case rocsparse_datatype_f64_r:
      {
	double nrm;
	rocblas_dnrm2(this->m_itsol->get_blas_handle(),dimension,(const double*)r,1,&nrm);
	return nrm;
      }
    case rocsparse_datatype_f64_c:
      {
	//	double nrm;
	//	rocblas_znrm2(this->m_itsol->get_blas_handle(),dimension,(const rocblas_double_complex*)r,1,&nrm);
	//	return nrm;
	break;
      }
    case rocsparse_datatype_f32_r:
      {
	float nrm;
	rocblas_snrm2(this->m_itsol->get_blas_handle(),dimension,(const float*)r,1,&nrm);
	return nrm;
      }
    case rocsparse_datatype_f32_c:
      {
	//	float nrm;
	//	rocblas_cnrm2(this->m_itsol->get_blas_handle(),dimension,(const rocblas_float_complex*)r,1,&nrm);
	//	return nrm;
	break;
      }
    }
  return 0;
}

void rocsparse_itsol_cg_descr_::dot_product( const void* a,
					     const void* b,
					     void*r)
{
  const auto inputs = this->m_itsol->get_inputs();
  const auto datatype = inputs->get_datatype_compute();
  const int64_t dimension = inputs->get_dimension();
  switch (datatype)
    {
      case rocsparse_datatype_f32_r:
	{
	  rocblas_sdot(this->m_itsol->get_blas_handle(), dimension,(const float*)a,1,(const float*)b,1,(float*)r);
	  break;
	}
	case rocsparse_datatype_f64_r:
	  {
	    rocblas_ddot(this->m_itsol->get_blas_handle(), dimension,(const double*)a,1,(const double*)b,1,(double*)r);
	    break;
	  }
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_u8_r:
	{
	  break;
	}
    }

}


void rocsparse_itsol_cg_descr_::axpy2(const void* a,
				      const void* x,
				      const void * b,
				      void* y)
{
  const auto inputs = this->m_itsol->get_inputs();
  const auto datatype = inputs->get_datatype_compute();
  const int64_t dimension = inputs->get_dimension();
  switch (datatype)
    {
      case rocsparse_datatype_f32_r:
	{
	  rocblas_sscal(this->m_itsol->get_blas_handle(), dimension,(const float*) b, (float*)y, 1);   
	  rocblas_saxpy(this->m_itsol->get_blas_handle(), dimension,(const float*) a, (const float*)x, 1, (float*)y, 1);
	  break;
	}
      case rocsparse_datatype_f64_r:
	{
	  rocblas_dscal(this->m_itsol->get_blas_handle(), dimension,(const double*) b, (double*)y, 1);   
	  rocblas_daxpy(this->m_itsol->get_blas_handle(), dimension,(const double*) a, (const double*)x, 1, (double*)y, 1);   
	  break;
	}
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_u8_r:
	{
	  break;
	}
    }
}
#endif

static hipError_t  hipMalloc(void**that,rocsparse_datatype datatype,size_t nelm)
{

     switch(datatype)
       {
	case rocsparse_datatype_f16_r:
	  {
	    return hipMalloc(that,(sizeof(float)/2) *  nelm);
	  }
       case rocsparse_datatype_f32_r:
	 {
	   return hipMalloc(that,sizeof(float) *  nelm);
	  }
	case rocsparse_datatype_f32_c:
	  {
	    return hipMalloc(that,sizeof(float)*2*  nelm);
	  }
	case rocsparse_datatype_f64_r:
	  {
	    return hipMalloc(that,sizeof(double) *  nelm);
	  }
	case rocsparse_datatype_f64_c:
	  {
	    return hipMalloc(that,sizeof(double)*2*  nelm);
	  }
       case rocsparse_datatype_i32_r:
	 {
	    return hipMalloc(that,sizeof(int32_t)*  nelm);
	 }
       case rocsparse_datatype_u32_r:
	 {
	    return hipMalloc(that,sizeof(uint32_t)*  nelm);
	 }
       case rocsparse_datatype_i8_r:
	 {
	    return hipMalloc(that,sizeof(int8_t)*  nelm);
	 }
       case rocsparse_datatype_u8_r:
	 {
	    return hipMalloc(that,sizeof(uint8_t)*  nelm);
	 }
	}
     return hipSuccess;
}


void * rocsparse_itsol_cg_descr_::get_p()
{
  if (this->p == nullptr)
    {
      const auto inputs = this->m_itsol->get_inputs();
      const auto datatype_compute = inputs->get_datatype_compute();
      const int64_t dimension = inputs->get_dimension();
      HIP_CHECK(hipMalloc(&this->p,datatype_compute,  dimension));
    }
  return this->p;
}


void * rocsparse_itsol_cg_descr_::get_r()
{
  if (this->r == nullptr)
    {
      const auto inputs = this->m_itsol->get_inputs();
      const auto datatype_compute = inputs->get_datatype_compute();
      const int64_t dimension = inputs->get_dimension();
      HIP_CHECK(hipMalloc(&this->r,datatype_compute,  dimension));
    }
  return this->r;
}

rocsparse_status rocsparse_itsol_cg_descr_::buffer_size(rocsparse_handle handle,
							rocsparse_itsol_descr  descr,
							size_t*buffer_size)
{
      const auto inputs = this->m_itsol->get_inputs();
  const auto datatype_compute = inputs->get_datatype_compute();
  const int64_t dimension = inputs->get_dimension();
  
      switch(datatype_compute)
	{
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_u32_r:
	{
	  buffer_size[0] = dimension * sizeof(int32_t);
	  break;
	}
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u8_r:
	{
	  buffer_size[0] = dimension * sizeof(int8_t);
	  break;
	}
	case rocsparse_datatype_f32_r:
	  {
	    buffer_size[0] = dimension * sizeof(float);
	    break;
	  }
	case rocsparse_datatype_f16_r:
	  {
	    buffer_size[0] = dimension * sizeof(float) / 2;
	    break;
	  }
	case rocsparse_datatype_f32_c:
	  {
	    buffer_size[0] = dimension * sizeof(float)*2;
	    break;
	  }
	case rocsparse_datatype_f64_r:
	  {
	    buffer_size[0] = dimension * sizeof(double);
	    break;
	  }
	case rocsparse_datatype_f64_c:
	  {
	    buffer_size[0] = dimension * sizeof(double)*2;
	    break;
	  }
	}

  return rocsparse_status_success;
}

 rocsparse_status rocsparse_itsol_cg_descr_::run(rocsparse_handle handle,
							rocsparse_itsol_descr descr,
						 const void * b,
						 void * x,	    
						 size_t buffer_size,
						 void * buffer)
 {

  if (this->m_initialized == false)
    {
      const rocsparse_datatype datatype = descr->get_inputs()->get_datatype_compute();
      this->m_r_z.set_datatype(datatype);
      this->m_pAp.set_datatype(datatype);
      this->m_nrm2.set_datatype(datatype);
      this->m_scalar.set_datatype(datatype);
      this->m_next_r_z.set_datatype(datatype);
      this->m_alpha.set_datatype(datatype);
      this->m_beta.set_datatype(datatype);
      this->m_one.set_datatype(datatype);
      this->m_negative_one.set_datatype(datatype);
      
      this->m_one.one();
      this->m_negative_one.negative_one();
      this->m_initialized = true;
      this->m_internal_state  = rocsparse_cg_internal_state_initial;
    }
  void * r = this->get_r();
  void * p = this->get_p();
  auto output = descr->get_outputs();
  auto input = descr->get_inputs();
  static constexpr bool log = false;
  if (log)
    {
      //rocsparse_itsol_request_get_name(descr->get_request())
      rocsparse_itsol_request request;
      descr->get_request(&request);
      std::cout << "iter " <<   output->get_niter() << " "  <<  request << std::endl;
    }



  for (;;)
    {

      switch(this->m_internal_state)
	{      
	case rocsparse_cg_internal_state_initial:
	  {
	    descr->set_request(rocsparse_itsol_request_matrix_vector);	
	    descr->set_request_input(x);
	    descr->set_request_output(r);
	    this->m_internal_state = rocsparse_cg_internal_state_init_mv;
	    output->set_niter(0);
	    return rocsparse_status_success;
	  }
	  
	case rocsparse_cg_internal_state_iter_mv:
	  {
	    void * out{};
	    descr->get_request_output(&out);

	    descr->dot_product(handle,p, out, this->m_pAp);
	    this->m_alpha.divide(this->m_r_z,this->m_pAp);
	    descr->axpy(handle, this->m_alpha, p, x);	  
	    this->m_alpha.negate();
	    descr->axpy(handle, this->m_alpha,out, r);

	    
	    descr->nrm2(handle, r, this->m_nrm2);

	    double nrm{};
	    
	    this->m_nrm2.get(&nrm, sizeof(nrm) );
	    std::cout << output->get_niter()  << " " <<  nrm << std::endl;
	    if(nrm <= input->get_tolerance())
	      {
		descr->set_request(rocsparse_itsol_request_finished);
		return rocsparse_status_success;
	      }
	    
	    descr->set_request_input(r);
	    descr->set_request_output(buffer);
	    descr->set_request(rocsparse_itsol_request_preconditioner);
	    this->m_internal_state = rocsparse_cg_internal_state_iter_prec;
	    return rocsparse_status_success;
	  }
	  
	case rocsparse_cg_internal_state_iter_prec:
	  {
	    void * out{};
	    descr->get_request_output(&out);
	    
	    descr->dot_product(handle, r,out,this->m_next_r_z);
	    this->m_scalar.divide(this->m_next_r_z, this->m_r_z);	    
	    descr->axpy2(handle, this->m_one,out,this->m_scalar,this->p);
	    
	    this->m_r_z = this->m_next_r_z;
	    
	    descr->set_request_input(p);
	    descr->set_request_output(buffer);
	    descr->set_request(rocsparse_itsol_request_matrix_vector);	      
	    this->m_internal_state = rocsparse_cg_internal_state_post_iteration;
	    break;
	  }
	  
	case rocsparse_cg_internal_state_init_mv:
	  {
	    descr->axpy2( handle, this->m_one, b, this->m_negative_one, r);	    
	    descr->set_request_input(r);
	    descr->set_request_output(p);
	    descr->set_request(rocsparse_itsol_request_preconditioner);
	    this->m_internal_state = rocsparse_cg_internal_state_init_prec;
	    return rocsparse_status_success;
	  }
	  
	case rocsparse_cg_internal_state_init_prec:
	  {
	    descr->dot_product(handle, r, p, this->m_r_z);
	    descr->set_request(rocsparse_itsol_request_matrix_vector);
	    this->m_internal_state = rocsparse_cg_internal_state_post_iteration;
	    break;
	  }
	  
	case rocsparse_cg_internal_state_post_iteration:
	  {
	      output->set_niter(output->get_niter()+1);		
	      if(output->get_niter() >= input->get_nmaxiter())
		{
		  descr->set_request(rocsparse_itsol_request_finished);
		  return rocsparse_status_success;
		}
	      
	      descr->set_request_input(p);
	      descr->set_request_output(buffer);
	      descr->set_request(rocsparse_itsol_request_matrix_vector);
	      this->m_internal_state = rocsparse_cg_internal_state_iter_mv;
	    return rocsparse_status_success;
	  }
	}
    }

  return rocsparse_status_success;  
 

}




  

rocsparse_status rocsparse_itsol_cg_descr_::set_input(rocsparse_itsol_cg_input that,
						      const void * data,
						      size_t size)
{  
  return this->get_inputs()->set(that,data,size);
}


rocsparse_status rocsparse_itsol_cg_descr_::get_input(rocsparse_itsol_cg_input that,
						      void * data,
						      size_t size)
{  
  return this->get_inputs()->get(that,data,size);
}


rocsparse_status rocsparse_itsol_cg_descr_::set_output(rocsparse_itsol_cg_output that,
						       const void * data,
						       size_t size)
{  
  return this->m_outputs->set(that,data,size);
}

  
rocsparse_status rocsparse_itsol_cg_descr_::get_output(rocsparse_itsol_cg_output that,
						       void * data,
						       size_t size)
{  
  return this->m_outputs->get(that,data,size);
}

extern "C"   rocsparse_status rocsparse_itsol_cg_set_input(rocsparse_handle handle,
							   rocsparse_itsol_descr descr,
							   rocsparse_itsol_cg_input input,
							   const void * data,
							   size_t size)
{
  ((rocsparse_itsol_cg_descr_*)descr->get_impl())->set_input(input, data,size);
  return rocsparse_status_success;
}

extern "C"   rocsparse_status rocsparse_itsol_cg_get_input(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
							   rocsparse_itsol_cg_input input,
							   void * data,
							   size_t size)
{
  ((rocsparse_itsol_cg_descr_*)descr->get_impl())->get_input(input, data,size);
  return rocsparse_status_success;
}

extern "C" rocsparse_status rocsparse_itsol_cg_get_output(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
							  rocsparse_itsol_cg_output output,
							  void * data,
							  size_t size)
{
  ((rocsparse_itsol_cg_descr_*)descr->get_impl())->get_output(output, data,size);
  return rocsparse_status_success;
}

 

