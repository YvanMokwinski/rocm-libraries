

#include "rocsparse_itsol_fgmres_inputs.hpp"

int64_t rocsparse_fgmres_inputs_::get_size_basis()const {return this->size_basis;}
void rocsparse_fgmres_inputs_::set_size_basis(int64_t value){ this->size_basis = value;}
rocsparse_status rocsparse_fgmres_inputs_::set(rocsparse_itsol_fgmres_input input,
					   const void * value,
					   size_t size)
{
    switch(input)
    {
    case rocsparse_itsol_fgmres_input_size_basis:
      {
	if (size == sizeof(int32_t))
	  {
	    this->set_size_basis(*((const int32_t*)value));
	    return rocsparse_status_success;
	  }
	else if (size == sizeof(int64_t))
	  {
	    this->set_size_basis(*((const int64_t*)value));
	    return rocsparse_status_success;
	  }
	return rocsparse_status_invalid_value;
      }
    }
    return rocsparse_status_invalid_value;
  }

  rocsparse_status rocsparse_fgmres_inputs_::get(rocsparse_itsol_fgmres_input input,
		        void * value,
		       size_t size)const
  {
    switch(input)
    {
    case rocsparse_itsol_fgmres_input_size_basis:
      {
	if (size == sizeof(int32_t))
	  {
	    *(( int32_t*)value) = this->get_size_basis();
	    return rocsparse_status_success;
	  }
	else if (size == sizeof(int64_t))
	  {
	    *((int64_t*)value) = this->get_size_basis();
	    return rocsparse_status_success;
	  }
	return rocsparse_status_invalid_value;
      }
    }
    return rocsparse_status_invalid_value;
  }
  
