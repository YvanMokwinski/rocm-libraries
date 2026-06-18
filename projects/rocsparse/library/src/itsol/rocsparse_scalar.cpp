
#include "rocsparse_scalar.hpp"
#include <iostream>

rocsparse_scalar_::rocsparse_scalar_(){}
rocsparse_scalar_::rocsparse_scalar_(rocsparse_datatype datatype_,void * data, size_t size)
    : m_datatype(datatype_)
  {
    memcpy(m_mem,data,size);
  }
  
rocsparse_scalar_::rocsparse_scalar_(const rocsparse_scalar_& that)
  {
    memcpy(this->m_mem,that.m_mem,sizeof(double)*2);
    this->m_datatype = that.m_datatype;
  }

rocsparse_scalar_::rocsparse_scalar_(const float& that)
  {
    memcpy(this->m_mem,&that,sizeof(float));
    this->m_datatype = rocsparse_datatype_f32_r;
    
  }

rocsparse_scalar_::rocsparse_scalar_(const rocsparse_float_complex& that)
  {
    memcpy(this->m_mem,&that,sizeof(float));
    this->m_datatype = rocsparse_datatype_f32_c;
  }
rocsparse_scalar_::rocsparse_scalar_(const rocsparse_double_complex& that)
  {
    memcpy(this->m_mem,&that,sizeof(double));
        this->m_datatype = rocsparse_datatype_f64_c;

  }
  
rocsparse_scalar_::rocsparse_scalar_(const double& that)
  {
    this->m_mem[0] = that;
    this->m_datatype = rocsparse_datatype_f64_r;
  }

  void rocsparse_scalar_::set_datatype(rocsparse_datatype value)    
  {
    this->m_datatype = value;
  }
  
void rocsparse_scalar_::print(const char * msg)const
  {
    std::cout << msg << " ";
    switch(this->m_datatype)
      {
      case rocsparse_datatype_f32_r:
	{
	  std::cout << *((const float*)&this->m_mem[0]);
	  break;
	}
	
      case rocsparse_datatype_f32_c:
	{
	  std::cout << *((const float*)&this->m_mem[0]) << " " <<  *(((const float*)&this->m_mem[0])+1);
	  break;
	}
      case rocsparse_datatype_f64_r:
	{
	  std::cout << *((const double*)&this->m_mem[0]);
	  break;
	}
	
      case rocsparse_datatype_f64_c:
	{
	std::cout << *((const double*)&this->m_mem[0]) << " " <<  *(((const double*)&this->m_mem[0])+1);
	break;
      }
    case rocsparse_datatype_i32_r:
      {
	std::cout << *((const int32_t*)&this->m_mem[0]);
	break;
      }
    case rocsparse_datatype_u32_r:
      {
	std::cout << *((const uint32_t*)&this->m_mem[0]);
	break;
      }
    case rocsparse_datatype_i8_r:
      {
	std::cout << *((const int8_t*)&this->m_mem[0]);
	break;
      }
    case rocsparse_datatype_u8_r:
      {
	std::cout << *((const uint8_t*)&this->m_mem[0]);
	break;
      }
      
    }
    std::cout << std::endl;
  }

  void rocsparse_scalar_::get(const void*data,size_t datasize)
  {
    switch(this->m_datatype)
      {
      case rocsparse_datatype_f32_r:
	{
	  if (datasize == sizeof(float))
	    {
	      *((float*)data) = float( *((float*)&this->m_mem) );
	    }
	  else if (datasize == sizeof(double))
	    {
	      *((double*)data) = double( *((float*)&this->m_mem) );
	    }
	  break;
	}
      case rocsparse_datatype_f64_r:
	{
	  if (datasize == sizeof(float))
	    {
	      *((float*)data) = float( *((double*)&this->m_mem) );
	    }
	  else if (datasize == sizeof(double))
	    {
	      *((double*)data) = double( *((double*)&this->m_mem) );
	    }
	  break;
	}
	
    case rocsparse_datatype_f32_c:      
    case rocsparse_datatype_f64_c:
    case rocsparse_datatype_i32_r:
    case rocsparse_datatype_u32_r:
    case rocsparse_datatype_i8_r:
    case rocsparse_datatype_u8_r:
      {
	break;
      }
      
    }

  }

void rocsparse_scalar_::one()
  {
    switch(this->m_datatype)
      {
      case rocsparse_datatype_f32_r:
	{
	    const float f = 1;
	    memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
      
    case rocsparse_datatype_f32_c:
      {
	const float f[2] = {1,0};
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
      case rocsparse_datatype_f64_r:
	{
	    const double f = 1;
	    memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
      
    case rocsparse_datatype_f64_c:
      {
	const double f[2] = {1,0};
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
    case rocsparse_datatype_i32_r:
      {
	const int32_t f = 1;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
    case rocsparse_datatype_u32_r:
      {
	const uint32_t f = 1;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
    case rocsparse_datatype_i8_r:
      {
	const int8_t f = 1;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
    case rocsparse_datatype_u8_r:
      {
	const uint8_t f = 1;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
      
    }
  }
  
    void rocsparse_scalar_::zero()
    {
      switch(this->m_datatype)
	{
      case rocsparse_datatype_f32_r:
	{
	  memset(m_mem,0,sizeof(float));
	break;
      }
      
    case rocsparse_datatype_f32_c:
      {
	memset(m_mem,0,sizeof(rocsparse_float_complex));
	break;
      }
      case rocsparse_datatype_f64_r:
	{
	  memset(m_mem,0,sizeof(double));

	break;
      }
      
    case rocsparse_datatype_f64_c:
      {
	memset(m_mem,0,sizeof(rocsparse_double_complex));
	break;
      } 
    case rocsparse_datatype_i32_r:
      {
	const int32_t f = 0;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
    case rocsparse_datatype_u32_r:
      {
	const uint32_t f = 0;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
    case rocsparse_datatype_i8_r:
      {
	const int8_t f = 0;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
    case rocsparse_datatype_u8_r:
      {
	const uint8_t f = 0;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
     
    }
  }    
    
    void rocsparse_scalar_::negative_one()
  {
    switch(this->m_datatype)
      {
      case rocsparse_datatype_f32_r:
	{
	    const float f = -1;
	    memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
      
    case rocsparse_datatype_f32_c:
      {
	const float f[2] = {-1,0};
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
      case rocsparse_datatype_f64_r:
	{
	    const double f = -1;
	    memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
      
    case rocsparse_datatype_f64_c:
      {
	const double f[2] = {-1,0};
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }


          case rocsparse_datatype_i32_r:
      {
	const int32_t f = -1;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
    case rocsparse_datatype_u32_r:
      {
	const uint32_t f = -1;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
    case rocsparse_datatype_i8_r:
      {
	const int8_t f = -1;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }
    case rocsparse_datatype_u8_r:
      {
	const uint8_t f = -1;
	memcpy(&m_mem[0],&f,sizeof(f));
	break;
      }

    }
  }
  
   void rocsparse_scalar_::divide(const void * a, const void * b)
  {
    switch(this->m_datatype)
      {
      case rocsparse_datatype_f64_r:
	{
	  *((double*)*this) = *((const double*)a) / *((const double*)b);	  
	  break;
	}
      case rocsparse_datatype_f64_c:
	{
	*((rocsparse_double_complex*)*this) = *((const rocsparse_double_complex*)a) / *((const rocsparse_double_complex*)b);	  
	  break;
	}
      case rocsparse_datatype_f32_r:
	{
	*((float*)*this) = *((const float*)a) / *((const float*)b);	  
	  break;
	}
      case rocsparse_datatype_f32_c:
	{
	*((rocsparse_float_complex*)*this) = *((const rocsparse_float_complex*)a) / *((const rocsparse_float_complex*)b);	  
	  break;
	}
    case rocsparse_datatype_i32_r:
      {
	*((int32_t*)*this) = *((const int32_t*)a) / *((const int32_t*)b);	  
	break;
      }
    case rocsparse_datatype_u32_r:
      {
	*((uint32_t*)*this) = *((const uint32_t*)a) / *((const uint32_t*)b);	  
	break;
      }
    case rocsparse_datatype_i8_r:
      {
	*((int8_t*)*this) = *((const int8_t*)a) / *((const int8_t*)b);	  
	break;
      }
    case rocsparse_datatype_u8_r:
      {
	*((uint8_t*)*this) = *((const uint8_t*)a) / *((const uint8_t*)b);	  
	break;
      }
      }
  }



   void rocsparse_scalar_::multiply(const void * a, const void * b)
  {
    switch(this->m_datatype)
      {
      case rocsparse_datatype_f64_r:
	{
	  *((double*)*this) = *((const double*)a) * *((const double*)b);	  
	  break;
	}
      case rocsparse_datatype_f64_c:
	{
	*((rocsparse_double_complex*)*this) = *((const rocsparse_double_complex*)a) * *((const rocsparse_double_complex*)b);	  
	  break;
	}
      case rocsparse_datatype_f32_r:
	{
	*((float*)*this) = *((const float*)a) * *((const float*)b);	  
	  break;
	}
      case rocsparse_datatype_f32_c:
	{
	*((rocsparse_float_complex*)*this) = *((const rocsparse_float_complex*)a) * *((const rocsparse_float_complex*)b);	  
	  break;
	}
    case rocsparse_datatype_i32_r:
      {
	*((int32_t*)*this) = *((const int32_t*)a) * *((const int32_t*)b);	  
	break;
      }
    case rocsparse_datatype_u32_r:
      {
	*((uint32_t*)*this) = *((const uint32_t*)a) * *((const uint32_t*)b);	  
	break;
      }
    case rocsparse_datatype_i8_r:
      {
	*((int8_t*)*this) = *((const int8_t*)a) * *((const int8_t*)b);	  
	break;
      }
    case rocsparse_datatype_u8_r:
      {
	*((uint8_t*)*this) = *((const uint8_t*)a) * *((const uint8_t*)b);	  
	break;
      }
      }
  }


   void rocsparse_scalar_::negate()
  {
    switch(m_datatype)
      {
    case rocsparse_datatype_i32_r:
      {
	  const auto tmp = *((int32_t*)*this);
	  *((int32_t*)*this) = -tmp;
	break;
      }
    case rocsparse_datatype_u32_r:
      {
	  const auto tmp = *((uint32_t*)*this);
	  *((uint32_t*)*this) = -tmp;
	break;
      }
    case rocsparse_datatype_i8_r:
      {
	  const auto tmp = *((int8_t*)*this);
	  *((int8_t*)*this) = -tmp;
	break;
      }
    case rocsparse_datatype_u8_r:
      {
	  const auto tmp = *((uint8_t*)*this);
	  *((uint8_t*)*this) = -tmp;
	break;
      }
      case rocsparse_datatype_f64_r:
	{
	  const auto tmp = *((double*)*this);
	  *((double*)*this) = -tmp;
	  break;
	}
      case rocsparse_datatype_f64_c:
	{
	  const auto tmp = *((rocsparse_double_complex*)*this);
	  *((rocsparse_double_complex*)*this) = -tmp;
	  break;
	}
      case rocsparse_datatype_f32_r:
	{
	const auto tmp = *((float*)*this);
	*((float*)*this) = -tmp;
	  break;
	}
      case rocsparse_datatype_f32_c:
	{
	  const auto tmp = *((rocsparse_float_complex*)*this);
	  *((rocsparse_float_complex*)*this) = -tmp;
	  break;
	}
      }
  }
  
   double rocsparse_scalar_::sqrt()
  {
    switch(this->m_datatype)
      {
      case rocsparse_datatype_f32_c:
      case rocsparse_datatype_f64_c:
      case rocsparse_datatype_i32_r:
      case rocsparse_datatype_i8_r:
      case rocsparse_datatype_u32_r:
      case rocsparse_datatype_u8_r:
	{
	  return 0;
	}
      case rocsparse_datatype_f64_r:
	{
	return std::sqrt(*((const double*)*this));
	}
      case rocsparse_datatype_f32_r:
	{
	return std::sqrt(*((const float*)*this));
	}
      }
    return 0;
  }
