
#include <rocblas/rocblas.h>
#include "rocsparse_itsol.hpp"
#include "bicgstab/rocsparse_itsol_bicgstab.hpp"
#include "cg/rocsparse_itsol_cg.hpp"
#include "fgmres/rocsparse_itsol_fgmres.hpp"
#include <iostream>
#if 0
extern "C" const char * rocsparse_itsol_request_get_name(rocsparse_itsol_request that)
{
  switch(that)
    {
    case   rocsparse_itsol_request_matrix_vector:
      return "rocsparse_itsol_request_matrix_vector";
    case   rocsparse_itsol_request_preconditioner:
      return "rocsparse_itsol_request_preconditioner";
    case   rocsparse_itsol_request_finished:      
      return "rocsparse_itsol_request_finished";
    case   rocsparse_itsol_request_error:
      return "rocsparse_itsol_request_error";
    }
  return "unknown";
}
#endif



rocsparse_itsol_alg rocsparse_itsol_impl_::get_alg()const {return this->m_alg;}
void rocsparse_itsol_impl_::set_alg(rocsparse_itsol_alg value){ this->m_alg = value;}
    void rocsparse_itsol_impl_::get_alg(void * data,size_t size) const
  {
    if (size == sizeof(rocsparse_itsol_alg))
      {
	*((rocsparse_itsol_alg*)data)= this->m_alg;
      }
    if (size == sizeof(int32_t))
      {
	 *((int32_t*)data)= this->m_alg;
      }
    else if (size == sizeof(int64_t))
      {
	 *((int64_t*)data)= this->m_alg;
      }
  }

void rocsparse_itsol_impl_::set_alg(const void * data,size_t size)
  {
    if (size == sizeof(rocsparse_datatype))
      {
	this->m_alg = *((const rocsparse_itsol_alg*)data);
      }
    if (size == sizeof(int32_t))
      {
	this->m_alg = (rocsparse_itsol_alg) *((const int32_t*)data);
      }
    else if (size == sizeof(int64_t))
      {
	this->m_alg = (rocsparse_itsol_alg) *((const int64_t*)data);
      }
  }

rocsparse_itsol_impl rocsparse_itsol_descr_::get_impl()
{
  const auto alg = this->m_inputs->get_alg();
  if (this->m_impl != nullptr &&  alg != this->m_impl->get_alg())
    {      
      delete this->m_impl;
      this->m_impl = nullptr;
    }
  
  switch(alg)
    {
    case rocsparse_itsol_alg_cg:
      {
	if (this->m_impl == nullptr)
	this->m_impl = new rocsparse_itsol_cg_descr_(this);
	return this->m_impl;
      }
    case rocsparse_itsol_alg_bicgstab:
      {
	if (this->m_impl == nullptr)
	this->m_impl = new rocsparse_itsol_bicgstab_descr_(this);
	return this->m_impl;
      }
    case rocsparse_itsol_alg_fgmres:
      {
	if (this->m_impl == nullptr)
	this->m_impl = new rocsparse_itsol_fgmres_descr_(this);
	return this->m_impl;
      }
    }
  return nullptr;
}

 rocsparse_status rocsparse_itsol_descr_::get_request(rocsparse_itsol_request * request) const
{
  request[0] = this->m_request;  
  return rocsparse_status_success;
}

 rocsparse_status rocsparse_itsol_descr_::set_request(rocsparse_itsol_request value)
{
  this->m_request = value;  
  return rocsparse_status_success;
}

 rocsparse_status rocsparse_itsol_descr_::get_request_input(	   void ** data)
{
  data[0] = this->m_in;  
  return rocsparse_status_success;
}

 rocsparse_status rocsparse_itsol_descr_::get_request_output(void ** data)
{
  data[0] = this->m_out;  
  return rocsparse_status_success;
}

   rocsparse_status rocsparse_itsol_descr_::set_input(	     rocsparse_itsol_input input,
							     const void * data,
							     size_t datasize)
{
  
    switch(input)
      {
	
#define CASE(NAME)						\
	case rocsparse_itsol_input_##NAME:			\
	  {							\
	    this->m_inputs->set_##NAME(data,datasize);		\
	    return rocsparse_status_success;    		\
	  }
	
	CASE(alg);
	CASE(dimension);
	CASE(datatype_rhs);
	CASE(datatype_sol);
	CASE(datatype_compute);
	CASE(tolerance);
	CASE(nmaxiter);
#undef CASE 
      }

    return rocsparse_status_invalid_value;
    
  }
  
    rocsparse_status rocsparse_itsol_descr_::get_input(	rocsparse_itsol_input input,
							void * data,
							size_t datasize)
  {
    switch(input)
      {
	
#define CASE(NAME)						\
	case rocsparse_itsol_input_##NAME:			\
	  {							\
	    this->m_inputs->get_##NAME(data,datasize);	\
	    return rocsparse_status_success;		\
	  }
	
	CASE(alg);
	CASE(dimension);
	CASE(datatype_rhs);
	CASE(datatype_sol);
	CASE(datatype_compute);
	CASE(tolerance);
	CASE(nmaxiter);
#undef CASE 
      }
    return rocsparse_status_invalid_value;
        
  }
    
 rocsparse_status rocsparse_itsol_descr_::get_output(rocsparse_itsol_output output,
							    void * data,
							    size_t size)
{
  switch(output)
    {	
#define CASE(NAME)							\
      case rocsparse_itsol_output_##NAME:				\
	{								\
	  this->m_outputs->get_##NAME(data,size);			\
	  return rocsparse_status_success;				\
	}
      
      CASE(niter);
#undef CASE 
    }
    return rocsparse_status_invalid_value;
    
}
#if 0
 rocsparse_status rocsparse_itsol_descr_::destroy_descr(rocsparse_itsol_descr descr)
{  
  if (descr != nullptr)
    {
      delete descr;
    }
  return rocsparse_status_success;  
}

 rocsparse_status rocsparse_itsol_descr_::create_descr(rocsparse_itsol_descr * descr)
{
  rocsparse_itsol_descr that = new rocsparse_itsol_descr_();
  descr[0] = that;  
}
#endif
rocsparse_itsol_descr_::rocsparse_itsol_descr_()
{
  this->m_inputs = new rocsparse_itsol_inputs_();
  this->m_outputs = new rocsparse_itsol_outputs_();

  rocblas_create_handle(&this->m_blas_handle);
  rocblas_set_pointer_mode(this->m_blas_handle, rocblas_pointer_mode_host);    

  this->m_outputs->set_niter(-1);
}
  
  // Destructor: free arrays
rocsparse_itsol_descr_::~rocsparse_itsol_descr_(){};



extern "C" rocsparse_status rocsparse_itsol_buffer_size(rocsparse_handle handle,
								rocsparse_itsol_descr  descr,
							size_t*buffer_size)
{
  return descr->get_impl()->buffer_size(handle, descr,buffer_size);
}


extern "C" rocsparse_status rocsparse_itsol_get_request(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
							 rocsparse_itsol_request * request)
{
  return descr->get_request( request);
}


extern "C"   rocsparse_status rocsparse_itsol_get_request_input(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
								 void ** data)
{
  return descr->get_request_input( data);
}

extern "C" rocsparse_status rocsparse_itsol_get_request_output(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
								void ** data)
{
  return descr->get_request_output( data);
}

extern "C"   rocsparse_status rocsparse_itsol_set_input(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
							 rocsparse_itsol_input input,
							 const void * input_data,
							 size_t input_datasize)
{
  return descr->set_input( input,input_data,input_datasize);
}

extern "C"   rocsparse_status rocsparse_itsol_get_input(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
							 rocsparse_itsol_input input,
							 void * input_data,
							 size_t input_datasize)
{
  return descr->get_input( input,input_data,input_datasize);
}


extern "C" rocsparse_status rocsparse_itsol_get_output(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
							rocsparse_itsol_output output,
							void * output_data,
							size_t output_datasize)
{
  return descr->get_output( output,output_data,output_datasize);
}

extern "C" rocsparse_status rocsparse_destroy_itsol_descr(rocsparse_itsol_descr descr)
{
  if (descr)
  delete descr;
  return rocsparse_status_success;
}

extern "C" rocsparse_status rocsparse_create_itsol_descr(rocsparse_handle handle,
								rocsparse_itsol_descr * descr)
{
  descr[0] = new rocsparse_itsol_descr_();


  return rocsparse_status_success;
}

extern "C" rocsparse_status rocsparse_itsol(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
					    const void * b,
					    void * x,	    
					    size_t buffer_size,
					    void * buffer)
{
  return descr->get_impl()->run(handle,descr,b,x,buffer_size,buffer);
}

