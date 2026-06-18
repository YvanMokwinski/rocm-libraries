#include <rocsparse/rocsparse.h>
#include <map>

#include "rocsparse_clients_generate_tridiagonal.h"
#include "rocsparse_clients_generate_descr.hpp"
#include "rocsparse_clients_generate_tridiagonal_descr.hpp"
#include "rocsparse_clients_generate_input_tridiagonal_descr.hpp"
#include "rocsparse_clients_generate_output_tridiagonal_descr.hpp"
#include "rocsparse_clients_generate_csr_pointer_descr.hpp"
#include <iostream>
template<rocsparse_indextype I>
struct rocsparse_indextype_traits_t;
template<>
struct rocsparse_indextype_traits_t<rocsparse_indextype_i32>
{
  using type_t = int32_t;
};
template<rocsparse_indextype I>
using rocsparse_indextype_type_t = typename rocsparse_indextype_traits_t<I>::type_t;

template<rocsparse_datatype T>
struct rocsparse_datatype_traits_t;
template<>
struct rocsparse_datatype_traits_t<rocsparse_datatype_f32_r>
{
  using type_t = float;
};
template<>
struct rocsparse_datatype_traits_t<rocsparse_datatype_f64_r>
{
  using type_t = double;
};
template<rocsparse_datatype T>
using rocsparse_datatype_type_t = typename rocsparse_datatype_traits_t<T>::type_t;


template<typename T,typename I, typename J>
static void generate_template(int64_t m,
			      int64_t nnz,
			      void*hptr_,
			      void*hind_,
			      void*hval_);



static std::map<std::tuple<rocsparse_datatype,
			   rocsparse_indextype,
			   rocsparse_indextype>,
		void (*)(int64_t ,
			 int64_t,
			 void*,
			 void*,
			 void*)> map {
  
#define SET(T_type,I_type,J_type)					\
  {{ T_type,								\
     I_type,								\
     J_type},								\
   generate_template<rocsparse_datatype_type_t<T_type>,			\
   rocsparse_indextype_type_t<I_type>,					\
   rocsparse_indextype_type_t<J_type>>}
  
  SET(rocsparse_datatype_f32_r,
      rocsparse_indextype_i32,
      rocsparse_indextype_i32),
  SET(rocsparse_datatype_f64_r,
      rocsparse_indextype_i32,
      rocsparse_indextype_i32),
  
};
#undef SET

template<typename T,typename I, typename J>
static void generate_template(int64_t m,
			      int64_t nnz,
			      void*hptr_,
			      void*hind_,
			      void*hval_)
{
  T * p_hval = (T*)hval_;
  J * p_hind = (J*)hind_;
  I * p_hptr = (I*)hptr_;
  
  p_hptr[0] = 0;
  p_hptr[1] = 2;
  const T sss = static_cast<T>(0.0);
  *p_hval++ = 4.0 + T(random()) / T(RAND_MAX) * sss;
  *p_hval++ = 1 + T(random()) / T(RAND_MAX) * sss;
  *p_hind++ = 0;
  *p_hind++ = 1;//IND(0,m-1);
  for (J i=1;i<m-1;++i)
    {
      *p_hind++ = i-1;
      *p_hind++ = i;
      *p_hind++ = i+1;
      *p_hval++ = 1.0 + T(random()) / T(RAND_MAX) * sss;
      *p_hval++ = 4.0 + T(random()) / T(RAND_MAX) * sss;
      *p_hval++ = 1.0 + T(random()) / T(RAND_MAX) * sss;
      p_hptr[i+1] = p_hptr[i]+3;
    }
    *p_hind++ = m-2;
    *p_hind++ = m-1;
    *p_hval++ = 1.0 + T(random()) / T(RAND_MAX) * sss;
    *p_hval++ = 4.0 + T(random()) / T(RAND_MAX) * sss;
    p_hptr[m] = p_hptr[m-1] + 2;
}

extern "C" void rocsparse_clients_generate_input_tridiagonal_set(rocsparse_clients_generate_handle self,
								 rocsparse_clients_generate_input_tridiagonal input_tridiagonal,
								 const void * data,
								 size_t data_size)
{
  switch(input_tridiagonal)
    {
    case rocsparse_clients_generate_input_tridiagonal_dvalue:
      {
	if (data_size == sizeof(float))
	  {
	    ((rocsparse_clients_generate_input_tridiagonal_descr)self->m_descr->m_input)->set_diag_value(*((const float*)data));
	  }
	else if (data_size == sizeof(double))
	  {
	    ((rocsparse_clients_generate_input_tridiagonal_descr)self->m_descr->m_input)->set_diag_value(*((const double*)data));
	  }
	break;
      }
    }  
}



void rocsparse_clients_generate_tridiagonal_descr_::generate(rocsparse_clients_generate_stage stage,
		size_t buffer_size,
		void * buffer)
  {
    switch(stage)
      {
      case rocsparse_clients_generate_stage_analysis:
	{

	  auto nrows = this->m_input->get_nrows();
	  if (nrows == -1)
	    {
	      nrows = 2;
	    }

	  const int64_t nnz =  int64_t(4) + int64_t(3)*(nrows-2);
	  this->m_output->m_m = nrows;	  
	  this->m_output->m_nnz = nnz;
	  this->m_output->m_n = nrows;
	  
	  this->m_output->m_ptr_indextype = this->m_input->get_ptr_indextype();
	  this->m_output->m_ind_indextype = this->m_input->get_ind_indextype();
	  this->m_output->m_val_datatype = this->m_input->get_val_datatype();

	  if (this->m_output->m_ind_indextype == (rocsparse_indextype)-1)
	    {
	      this->m_output->m_ind_indextype = (nrows <= std::numeric_limits<int32_t>::max() ) ? rocsparse_indextype_i32 : rocsparse_indextype_i64;
	    }
	  
	  if (this->m_output->m_ptr_indextype == (rocsparse_indextype)-1)
	    {
	      this->m_output->m_ptr_indextype = (nnz <= std::numeric_limits<int32_t>::max() ) ? rocsparse_indextype_i32 : rocsparse_indextype_i64;
	    }
	  
	  if (this->m_output->m_val_datatype == (rocsparse_datatype)-1)
	    {
	      this->m_output->m_val_datatype = rocsparse_datatype_f64_r;	  
	    }
	  break;
	}
	
      case rocsparse_clients_generate_stage_compute:
	{
	  auto f = map
	    [{this->m_output->get_val_datatype(),
		  this->m_output->get_ptr_indextype(),
		  this->m_output->get_ind_indextype()}];
	  
	  f(this->m_output->get_nrows(),
	    this->m_output->m_nnz,
	    reinterpret_cast<rocsparse_clients_generate_csr_pointer_descr>(this->m_pointer)->m_ptr,
	    reinterpret_cast<rocsparse_clients_generate_csr_pointer_descr>(this->m_pointer)->m_ind,
	    reinterpret_cast<rocsparse_clients_generate_csr_pointer_descr>(this->m_pointer)->m_val);
	  
	  break;
	}
      }
  }
