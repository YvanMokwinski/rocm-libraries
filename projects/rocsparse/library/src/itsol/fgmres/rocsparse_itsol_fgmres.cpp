#include "rocsparse_itsol_fgmres.hpp"
#include <iostream>
#include "control.h"
#define PRINT_VEC(MSG,H_)std::cout << MSG << std::endl;	    do {double tmp[2]{-7.-7}; \
	      hipMemcpy(tmp,H_,sizeof(double)*2,hipMemcpyDefault);\
	    std::cout << "   "#MSG"[0]" << *tmp << std::endl;\
	    std::cout << "   "#MSG"[1]" << tmp[1]<< std::endl;\
	    } while(false)

#define PRINT_VEC2(MSG,H_)std::cout << MSG << std::endl;	    do {double *tmp = new double[dimension]; \
	      hipMemcpy(tmp,H_,sizeof(double)*dimension,hipMemcpyDefault);\
	      for (int i=0;i<dimension;++i)std::cout << "   "#MSG"["<<i<<"]" << *(tmp+i) << std::endl; \
	    } while(false)

#undef PRINT_VEC
#define PRINT_VEC(MSG,H) (void)0
#undef PRINT_VEC2
#define PRINT_VEC2(MSG,H) (void)0

#define HIP_CHECK(call)						\
  do {								\
    hipError_t status = call;					\
    if (status != hipSuccess) {					\
      std::cerr << "HIP error: " << hipGetErrorString(status)	\
		<< " at line " << __LINE__ << std::endl;	\
      std::exit(1);						\
    }								\
  } while(0)

rocsparse_itsol_fgmres_descr_::rocsparse_itsol_fgmres_descr_(rocsparse_itsol_descr that)
  : rocsparse_itsol_impl_(rocsparse_itsol_alg_fgmres)
  , m_outputs(new rocsparse_fgmres_outputs_())
  , m_inputs(new rocsparse_fgmres_inputs_())
  , m_itsol(that)
{
  that->set_request(rocsparse_itsol_request_matrix_vector);
}


rocsparse_itsol_fgmres_descr_::~rocsparse_itsol_fgmres_descr_()
{
  if (this->m_outputs)
    delete this->m_outputs;
  this->m_outputs = nullptr;
  if (this->m_inputs)
    delete this->m_inputs;
  this->m_inputs = nullptr;
  
  (void)hipFree(this->m_c);
  (void)hipFree(this->m_s);
  (void)hipFree(this->m_r);
  (void)hipFree(this->m_H);
  (void)hipFree(this->m_z);
  (void)hipFree(this->m_v);
}


static hipError_t  hipMalloc(void**that,rocsparse_datatype datatype,size_t nelm)
{
     switch(datatype)
       {
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
}


static hipError_t  hipHostMalloc(void**that,rocsparse_datatype datatype,size_t nelm)
{
     switch(datatype)
       {
       case rocsparse_datatype_f32_r:
	 {
	   return hipHostMalloc(that,sizeof(float) *  nelm);
	  }
	case rocsparse_datatype_f32_c:
	  {
	    return hipHostMalloc(that,sizeof(float)*2*  nelm);
	  }
	case rocsparse_datatype_f64_r:
	  {
	    return hipHostMalloc(that,sizeof(double) *  nelm);
	  }
	case rocsparse_datatype_f64_c:
	  {
	    return hipHostMalloc(that,sizeof(double)*2*  nelm);
	  }
       case rocsparse_datatype_i32_r:
	 {
	    return hipHostMalloc(that,sizeof(int32_t)*  nelm);
	 }
       case rocsparse_datatype_u32_r:
	 {
	    return hipHostMalloc(that,sizeof(uint32_t)*  nelm);
	 }
       case rocsparse_datatype_i8_r:
	 {
	    return hipHostMalloc(that,sizeof(int8_t)*  nelm);
	 }
       case rocsparse_datatype_u8_r:
	 {
	    return hipHostMalloc(that,sizeof(uint8_t)*  nelm);
	 }
	}
}


static hipError_t  hipMemcpy(void*dst,const void * src,rocsparse_datatype datatype,size_t nelm)
{
     switch(datatype)
       {
       case rocsparse_datatype_f32_r:
	 {
	   return hipMemcpy(dst,src,sizeof(float) *  nelm,hipMemcpyDefault);
	  }
	case rocsparse_datatype_f32_c:
	  {
	    return hipMemcpy(dst,src,sizeof(float)*2*  nelm,hipMemcpyDefault);
	  }
	case rocsparse_datatype_f64_r:
	  {
	    return hipMemcpy(dst,src,sizeof(double) *  nelm,hipMemcpyDefault);
	  }
	case rocsparse_datatype_f64_c:
	  {
	    return hipMemcpy(dst,src,sizeof(double)*2*  nelm,hipMemcpyDefault);
	  }
       case rocsparse_datatype_i32_r:
	 {
	    return hipMemcpy(dst,src,sizeof(int32_t)*  nelm,hipMemcpyDefault);
	 }
       case rocsparse_datatype_u32_r:
	 {
	    return hipMemcpy(dst,src,sizeof(uint32_t)*  nelm,hipMemcpyDefault);
	 }
       case rocsparse_datatype_i8_r:
	 {
	    return hipMemcpy(dst,src,sizeof(int8_t)*  nelm,hipMemcpyDefault);
	 }
       case rocsparse_datatype_u8_r:
	 {
	   return hipMemcpy(dst,src,sizeof(uint8_t)*  nelm, hipMemcpyDefault);
	 }
	}
}


void * rocsparse_itsol_fgmres_descr_::get_c()
{
  if (this->m_c == nullptr)
    {
      const auto inputs = this->m_itsol->get_inputs();
      const auto datatype_compute = inputs->get_datatype_compute();
      const int64_t size_basis = this->get_inputs()->get_size_basis();
      THROW_IF_HIP_ERROR(hipHostMalloc(&this->m_c,datatype_compute, size_basis));
      hipMemset(this->m_c,0,sizeof(double)*(size_basis));

    }
  return this->m_c;
}
void * rocsparse_itsol_fgmres_descr_::get_s()
{
  if (this->m_s == nullptr)
    {
      const auto inputs = this->m_itsol->get_inputs();
      const auto datatype_compute = inputs->get_datatype_compute();
      const int64_t size_basis = this->get_inputs()->get_size_basis();
      THROW_IF_HIP_ERROR(hipHostMalloc(&this->m_s,datatype_compute, size_basis));
      hipMemset(this->m_s,0,sizeof(double)*(size_basis));
    }
  return this->m_s;
}

void * rocsparse_itsol_fgmres_descr_::get_r()
{
  if (this->m_r == nullptr)
    {
      const auto inputs = this->m_itsol->get_inputs();
      const auto datatype_compute = inputs->get_datatype_compute();
      const int64_t size_basis = this->get_inputs()->get_size_basis();
      THROW_IF_HIP_ERROR(hipHostMalloc(&this->m_r,datatype_compute, size_basis+1));
      hipMemset(this->m_r,0,sizeof(double)*(size_basis+1));
    }
  return this->m_r;
}

void * rocsparse_itsol_fgmres_descr_::get_H()
{
  if (this->m_H == nullptr)
    {
      const auto inputs = this->m_itsol->get_inputs();
      const auto datatype_compute = inputs->get_datatype_compute();
      const int64_t size_basis = this->get_inputs()->get_size_basis();
      THROW_IF_HIP_ERROR(hipHostMalloc(&this->m_H,datatype_compute, (size_basis+1)*size_basis));
      hipMemset(this->m_H,0,sizeof(double)*((size_basis+1)*size_basis));
    }
  return this->m_H;
}

void * rocsparse_itsol_fgmres_descr_::get_v()
{
  if (this->m_v == nullptr)
    {
      const int64_t dimension = this->m_itsol->get_inputs()->get_dimension();
      const auto inputs = this->m_itsol->get_inputs();
      const auto datatype_compute = inputs->get_datatype_compute();
      const int64_t size_basis = this->get_inputs()->get_size_basis();
      THROW_IF_HIP_ERROR(hipHostMalloc(&this->m_v,datatype_compute, (size_basis+1)*dimension));
      hipMemset(this->m_v,0,sizeof(double)*( (size_basis+1)*dimension));
    }
  return this->m_v;
}
  
void * rocsparse_itsol_fgmres_descr_::get_z()
{
  if (this->m_z == nullptr)
    {
      const auto inputs = this->m_itsol->get_inputs();
      const int64_t dimension = inputs->get_dimension();
      const auto datatype_compute = inputs->get_datatype_compute();
      const int64_t size_basis = this->get_inputs()->get_size_basis();
      THROW_IF_HIP_ERROR(hipHostMalloc(&this->m_z,datatype_compute, (size_basis+1)*dimension));
      hipMemset(this->m_v,0,sizeof(double)*( (size_basis+1)*dimension));
    }
  return this->m_z;
}


rocsparse_status rocsparse_itsol_fgmres_descr_::buffer_size(rocsparse_handle handle,
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
  

rocsparse_status rocsparse_itsol_fgmres_descr_::set_input(rocsparse_itsol_fgmres_input that,
						      const void * data,
						      size_t size)
{  
  return this->get_inputs()->set(that,data,size);
}


rocsparse_status rocsparse_itsol_fgmres_descr_::get_input(rocsparse_itsol_fgmres_input that,
						      void * data,
						      size_t size)
{  
  return this->get_inputs()->get(that,data,size);
}


rocsparse_status rocsparse_itsol_fgmres_descr_::set_output(rocsparse_itsol_fgmres_output that,
						       const void * data,
						       size_t size)
{  
  return this->m_outputs->set(that,data,size);
}

  
rocsparse_status rocsparse_itsol_fgmres_descr_::get_output(rocsparse_itsol_fgmres_output that,
						       void * data,
						       size_t size)
{  
  return this->m_outputs->get(that,data,size);
}

extern "C"   rocsparse_status rocsparse_itsol_fgmres_set_input(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
							   rocsparse_itsol_fgmres_input input,
							   const void * data,
							   size_t size)
{
  ((rocsparse_itsol_fgmres_descr_*)descr->get_impl())->set_input(input, data,size);
  return rocsparse_status_success;
}

extern "C"   rocsparse_status rocsparse_itsol_fgmres_get_input(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
							   rocsparse_itsol_fgmres_input input,
							   void * data,
							   size_t size)
{
  ((rocsparse_itsol_fgmres_descr_*)descr->get_impl())->get_input(input, data,size);
  return rocsparse_status_success;
}

extern "C" rocsparse_status rocsparse_itsol_fgmres_get_output(rocsparse_handle handle,
						rocsparse_itsol_descr descr,
							  rocsparse_itsol_fgmres_output output,
							  void * data,
							  size_t size)
{
  ((rocsparse_itsol_fgmres_descr_*)descr->get_impl())->get_output(output, data,size);
  return rocsparse_status_success;
}


#define H_IND(ai, aj, nrow, ncol) (ai) + (aj) * (nrow)


template<typename T>
void GenerateGivensRotation(T  dx,
			    T  dy,
			    T& c,
			    T& s)
{
  static constexpr T zero = static_cast<T>(0);
  static constexpr T one  = static_cast<T>(1);  
  if(dy == zero)
    {
      c = one;
      s = zero;
    }
  else if(dx == zero)
    {
      c = zero;
      s = one;
    }
  else if(std::abs(dy) > std::abs(dx))
    {
      T tmp = dx / dy;
      s             = one / std::sqrt(one + tmp * tmp);
      c             = tmp * s;
    }
  else
    {
      T tmp = dy / dx;
      c             = one / std::sqrt(one + tmp * tmp);
      s             = tmp * c;
    }
}

template<typename T>
void ApplyGivensRotation(T  c,
			 T  s,
			 T& dx,
			 T& dy)
{
  T temp = dx;
  //  dx             = std::conj(c) * dx + std::conj(s) * dy;
  dx             = c * dx + s * dy;
  dy             = -s * temp + c * dy;
}



#if 0
template<typename T>
void fgmres(rocsparse_handle handle,
	    rocsparse_itsol_descr descr,
	    int64_t M,
	    void * rhs_,
	    void*  x_,
	    int32_t size_basis,
	    double tolerance)
{
#if 0
  for (;;)
    {
      int32_t i = 0;
      do 
	{
	  {
	    this->sv();
	  }
	  
	  // 6. Compute residual z = b - Ax	  
	  this->mv(x, v);	  
        }  while(i < size);
    }
#endif  
}

#endif
 rocsparse_status rocsparse_itsol_fgmres_descr_::run(rocsparse_handle handle,
						     rocsparse_itsol_descr descr,
						     const void * b_,
						     void * x_,	    
						     size_t buffer_size,
						     void * buffer)
 {

   static constexpr bool verbose = false;
   const int64_t size_basis = this->get_inputs()->get_size_basis();
   double* H = (double*)this->get_H();

   if (this->m_initialized == false)
     {
       
       const rocsparse_datatype datatype = descr->get_inputs()->get_datatype_compute();
       this->m_one.set_datatype(datatype);
       this->m_negative_one.set_datatype(datatype);
       
       this->m_one.one();
       this->m_negative_one.negative_one();
       this->m_initialized = true;
       this->m_internal_state  = rocsparse_fgmres_internal_state_initial;
       
     }
   
   
   double * rhs= (double*) b_;
   double * x= (double*) x_;
   auto outputs = descr->get_outputs();
   auto inputs = descr->get_inputs();
   
   //  VectorType** z = this->z_;
   //  VectorType** v = this->v_;
     
   double* z = (double*)this->get_z();
   double* v = (double*)this->get_v();
   double* c = (double*)this->get_c();
   double* s = (double*)this->get_s();
   double* r = (double*)this->get_r();
   double * v0 = v;
   
   const int64_t v_ld =this->m_itsol->get_inputs()->get_dimension();
   const int64_t z_ld =this->m_itsol->get_inputs()->get_dimension();

   double tolerance = 1.0e-10;
   for (;;)
     {
       switch(this->m_internal_state)
	 {      
	 case rocsparse_fgmres_internal_state_initial:
	   {
	     outputs->set_niter(0);
	     return this->request(rocsparse_itsol_request_matrix_vector,
				  x,
				  v0,
				  rocsparse_fgmres_internal_state_init_mv);
	    
	   }
	  
	 case rocsparse_fgmres_internal_state_init_mv:
	   {
	     //
	     //
	     //
	     descr->scal(handle,  this->m_negative_one, v0);	    
	     descr->axpy(handle, this->m_one, rhs, v0);

	     //
	     // r = 0
	     //
	     hipMemset(r,0,(size_basis + 1) * sizeof(double));	  
	     descr->nrm2(handle, v0, r);	     
	     if (((const double*)r)[0] <= tolerance)
	       {		
		 descr->set_request(rocsparse_itsol_request_finished);
		 return rocsparse_status_success;		
	       }
	     this->m_internal_state = rocsparse_fgmres_internal_state_start;
	     break;
	   }
  
	 case rocsparse_fgmres_internal_state_start:
	   {
	     const double s = double(1) / ((const double*)r)[0];
	     descr->scal(handle, &s, v0);
	     this->arnoldi_iter = 0;
	     // request 1. preconditioner Mz_i = v_i
	     return this->request(rocsparse_itsol_request_preconditioner,
				  v+v_ld*this->arnoldi_iter,
				  z+z_ld*this->arnoldi_iter,
				  rocsparse_fgmres_internal_state_iter_prec);  
	   }
	  
	 case rocsparse_fgmres_internal_state_iter_prec:
	   {	    
	     // request v_i+1 = Az_i
	     return this->request(rocsparse_itsol_request_matrix_vector,
				  z+z_ld*this->arnoldi_iter,
				  v+v_ld*(this->arnoldi_iter+1),
				  rocsparse_fgmres_internal_state_iter_mv);  
	   }
	 case rocsparse_fgmres_internal_state_iter_mv:
	   {
	     {
	       for(int32_t k = 0; k <= this->arnoldi_iter; ++k)
		 {
		   const int32_t idx = H_IND(k, this->arnoldi_iter, size_basis + 1, size_basis);
		   // H_ki = <v_k,v_i+1>
		   descr->dot_product(handle,v+v_ld*k,v+v_ld*(this->arnoldi_iter + 1), &H[idx]);
		   // v_i+1  = v_i+1  - H_ki * v_k
		   const double cst = -H[idx];
		   descr->axpy(handle, &cst, v+v_ld*k,  v+v_ld*(this->arnoldi_iter + 1));
		 }
	    
	       // Precompute some indices
	       const int32_t ii   = H_IND(this->arnoldi_iter,
					      this->arnoldi_iter,
					      size_basis + 1,
					      size_basis);
	       const int32_t ip1i = H_IND((this->arnoldi_iter + 1),
					      this->arnoldi_iter,
					      size_basis + 1,
					      size_basis);
	      
	       // H_i+1i = ||v_i+1||
	       descr->nrm2(handle,v+v_ld*(this->arnoldi_iter + 1),&H[ip1i]);

	       // v_i+1 /= H_i+1i
	       { double tmp = double(1) / H[ip1i];
		 descr->scal(handle, &tmp,v+v_ld*(this->arnoldi_iter + 1)); }
	      
	       // Apply Givens rotation J(0),...,J(j-1) on (H(0,i),...,H(i,i))
	       for(int32_t k = 0; k < this->arnoldi_iter; ++k)
		 {
		   int32_t ki   = H_IND(k, this->arnoldi_iter, size_basis + 1, size_basis);
		   int32_t kp1i = H_IND(k + 1, this->arnoldi_iter, size_basis + 1, size_basis);
		   ApplyGivensRotation(c[k], s[k], H[ki], H[kp1i]);
		 }
	      
	       // Construct J(i)
	       GenerateGivensRotation(H[ii], H[ip1i], c[this->arnoldi_iter], s[this->arnoldi_iter]);
	    
	       // Apply J(i) to H(i,i) and H(i,i+1) such that H(i,i+1) = 0
	       ApplyGivensRotation(c[this->arnoldi_iter], s[this->arnoldi_iter], H[ii], H[ip1i]);
	       // Apply J(i) to the norm of the residual sg[i]
	       ApplyGivensRotation(c[this->arnoldi_iter], s[this->arnoldi_iter], r[this->arnoldi_iter], r[(this->arnoldi_iter + 1)]);
	       auto abs_r = std::abs(r[++this->arnoldi_iter]);
	       if (abs_r <= 1.0e-10)
		 {
		   this->m_internal_state = rocsparse_fgmres_internal_state_iter_mv_1;
		   break;
		 }
	     }
	    
	     if (this->arnoldi_iter >= size_basis)
	       {
		 this->m_internal_state = rocsparse_fgmres_internal_state_iter_mv_1;
		 break;
	       }
	     else
	       {
		 return this->request(rocsparse_itsol_request_preconditioner,
				      v+v_ld*this->arnoldi_iter,
				      z+z_ld*this->arnoldi_iter,
				      rocsparse_fgmres_internal_state_iter_prec);  
	       }
	   }
	  
	 case rocsparse_fgmres_internal_state_iter_mv_1:
	   {
	     // 4. Solve upper triangular system
	     for(int32_t j =  this->arnoldi_iter - 1; j >= 0; --j)
	       {
		 r[j] /= H[H_IND(j, j, size_basis + 1, size_basis)];	      
		 for(int32_t k = 0; k < j; ++k)
		   r[k] -= H[H_IND(k, j, size_basis + 1, size_basis)] * r[j];
	       }
	    
	     // 5. Update solution
	     descr->axpy(handle, r, z,  x);
	     for(int32_t j = 1; j < this->arnoldi_iter; ++j)
	       {
		 descr->axpy(handle, r+j,z + z_ld*j, x);
	       }
	     return this->request(rocsparse_itsol_request_matrix_vector,
				  x,
				  v,
				  rocsparse_fgmres_internal_state_iter_mv_2);  
	  
	   }
	  
	 case rocsparse_fgmres_internal_state_iter_mv_2:
	   {
	     descr->scal(handle, this->m_negative_one, v);
	     descr->axpy(handle, this->m_one, rhs, v);
	    
	     hipMemset(r,0,(size_basis + 1) * sizeof(double));	  
	     descr->nrm2(handle, v, r);
	     std::cout << outputs->get_niter() << " " << r[0] << std::endl;
	     if (r[0] <= tolerance)
	       {
		 outputs->set_niter( outputs->get_niter()+1);		
		 descr->set_request(rocsparse_itsol_request_finished);
		 return rocsparse_status_success;
	       }

	     outputs->set_niter( outputs->get_niter()+1);		
	     if(outputs->get_niter() >= inputs->get_nmaxiter())
	       {
		 descr->set_request(rocsparse_itsol_request_finished);
		 return rocsparse_status_success;
	       }
	    
	     this->m_internal_state = rocsparse_fgmres_internal_state_start;
	     break;
	   }
	 }
     }
  
   return rocsparse_status_success;
 }

