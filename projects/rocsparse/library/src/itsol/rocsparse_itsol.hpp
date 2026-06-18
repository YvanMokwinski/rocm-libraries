#pragma once
#include "internal/itsol/rocsparse_itsol.h"
#include <rocblas/rocblas.h>

#include "rocsparse_itsol_inputs.hpp"
#include "rocsparse_itsol_outputs.hpp"


typedef struct rocsparse_itsol_impl_ 
{
private:
  rocsparse_itsol_alg m_alg;
public:

  rocsparse_itsol_alg get_alg()const;
  void set_alg(rocsparse_itsol_alg alg);
  void get_alg(void * data,size_t size) const;
  void set_alg(const void * data,size_t size);

  virtual rocsparse_status buffer_size(rocsparse_handle handle,
				       rocsparse_itsol_descr  descr,
				       size_t*buffer_size) = 0;
  
  virtual rocsparse_status run(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
			       const void * b,
			       void * x,	    
			       size_t buffer_size,
			       void * buffer) = 0;
protected:
  rocsparse_itsol_impl_(  rocsparse_itsol_alg alg) : m_alg(alg) {}
public:
  virtual ~rocsparse_itsol_impl_(){}    
} * rocsparse_itsol_impl;

struct rocsparse_itsol_descr_ 
{
protected:
  
  rocsparse_itsol_outputs  m_outputs{};
  rocsparse_itsol_inputs   m_inputs{};  
  void * m_in{};
  void * m_out{};
  rocsparse_itsol_request m_request{};
  rocsparse_itsol_impl m_impl{};
  rocblas_handle m_blas_handle{};
public:
  rocsparse_itsol_outputs  get_outputs() { return this->m_outputs;}
  rocsparse_itsol_inputs  get_inputs() { return this->m_inputs;}

  rocblas_handle get_blas_handle() const { return this->m_blas_handle; }
  void set_blas_handle(rocblas_handle h) { this->m_blas_handle  =h; }
  void * get_in() {return m_in;}
  void * get_out() {return m_out;}
  rocsparse_itsol_impl get_impl();
  rocsparse_status get_request(rocsparse_itsol_request * request)const;  
  rocsparse_status set_request(rocsparse_itsol_request  request);  
  rocsparse_status get_request_input(	     void ** data);  
  rocsparse_status get_request_output(void ** data);
  rocsparse_status set_request_input(	     void * data){ this->m_in=data; return rocsparse_status_success;}  
  rocsparse_status set_request_output(	     void * data){ this->m_out = data; return rocsparse_status_success;}
  rocsparse_status set_input(rocsparse_itsol_input input,
			     const void * data,
			     size_t size);
  
  rocsparse_status get_input(	     rocsparse_itsol_input input,
				     void * data,
				     size_t size);
    
  rocsparse_status get_output(rocsparse_itsol_output output,
			      void * data,
			      size_t size);
  
  rocsparse_status buffer_size(rocsparse_handle handle,
			       size_t*buffer_size)
  {
    return this->get_impl()->buffer_size(handle,this,buffer_size);
  }
  
  rocsparse_status run(rocsparse_handle handle,
		       const void * b,
		       void * x,	    
		       size_t buffer_size,
		       void * buffer)
  {
    return this->get_impl()->run(handle,this,b,x,buffer_size,buffer);
  }

    void axpy2(rocsparse_handle handle,
		    const void* a,
	     const void* x,
	     const void * b,
	     void* y);
  
  void axpy(rocsparse_handle handle,
		    const void* a,
	    const void* x,
	    void* y);
  void scal(rocsparse_handle handle,
	    const void* a,
	    void* y);
  
  void dot_product( rocsparse_handle handle,
		    const void* a,
		    const void* b,
		    void*r);
  
  void nrm2(rocsparse_handle handle,
	    const void* a,
	    void * r);

  rocsparse_itsol_descr_();
  
  // Destructor: free arrays
  ~rocsparse_itsol_descr_();
    
};



