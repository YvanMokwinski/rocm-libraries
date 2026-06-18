#pragma once
#include <rocsparse/rocsparse-types.h>

struct rocsparse_scalar_
{
  double m_mem[2]{};
  rocsparse_datatype m_datatype{};
  rocsparse_scalar_();
  rocsparse_scalar_(rocsparse_datatype datatype_,void * data, size_t size);
  rocsparse_scalar_(const rocsparse_scalar_& that);
  rocsparse_scalar_(const float& that);
  rocsparse_scalar_(const rocsparse_float_complex& that);
  rocsparse_scalar_(const rocsparse_double_complex& that);
  rocsparse_scalar_(const double& that);

  template<typename T>
  operator const T*()const{ return (const T*)&this->m_mem[0];}
  template<typename T>
  operator T*(){ return (T*)&this->m_mem[0];}

  void set_datatype(rocsparse_datatype value);  
  void one();
  void zero();    
  void negative_one();
  void divide(const void * a, const void * b);
  void multiply(const void * a, const void * b);
  void negate();
  double sqrt();
  void get(const void*data,size_t datasize);
  void print(const char * msg)const;
};

