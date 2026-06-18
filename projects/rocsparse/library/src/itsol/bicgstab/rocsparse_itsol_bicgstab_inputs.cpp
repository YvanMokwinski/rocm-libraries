

#include "rocsparse_itsol_bicgstab_inputs.hpp"

int64_t rocsparse_bicgstab_inputs_::get_scal()const {return this->scal;}
void rocsparse_bicgstab_inputs_::set_scal(int64_t value){ this->scal = value;}
rocsparse_status rocsparse_bicgstab_inputs_::set(rocsparse_itsol_bicgstab_input input,
					   const void * value,
					   size_t size)
{
    switch(input)
    {
    case rocsparse_itsol_bicgstab_input_scal:
      {
	if (size == sizeof(int32_t))
	  {
	    this->set_scal(*((const int32_t*)value));
	    return rocsparse_status_success;
	  }
	else if (size == sizeof(int64_t))
	  {
	    this->set_scal(*((const int64_t*)value));
	    return rocsparse_status_success;
	  }
	return rocsparse_status_invalid_value;
      }
    }
    return rocsparse_status_invalid_value;
  }

  rocsparse_status rocsparse_bicgstab_inputs_::get(rocsparse_itsol_bicgstab_input input,
		        void * value,
		       size_t size)const
  {
    switch(input)
    {
    case rocsparse_itsol_bicgstab_input_scal:
      {
	if (size == sizeof(int32_t))
	  {
	    *(( int32_t*)value) = this->get_scal();
	    return rocsparse_status_success;
	  }
	else if (size == sizeof(int64_t))
	  {
	    *((int64_t*)value) = this->get_scal();
	    return rocsparse_status_success;
	  }
	return rocsparse_status_invalid_value;
      }
    }
    return rocsparse_status_invalid_value;
  }
  
