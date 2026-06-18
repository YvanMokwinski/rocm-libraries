

#include "rocsparse_itsol_bicgstab_outputs.hpp"

  double rocsparse_bicgstab_outputs_::get_alpha()const {return this->alpha;}
  void rocsparse_bicgstab_outputs_::set_alpha(double value){ this->alpha = value;}
    rocsparse_status rocsparse_bicgstab_outputs_::set(rocsparse_itsol_bicgstab_output output,
		       const void * value,
		       size_t size)
  {
    switch(output)
    {
    case rocsparse_itsol_bicgstab_output_alpha:
      {
	if (size == sizeof(int32_t))
	  {
	    this->set_alpha(*((const int32_t*)value));
	    return rocsparse_status_success;
	  }
	else if (size == sizeof(int64_t))
	  {
	    this->set_alpha(*((const int64_t*)value));
	    return rocsparse_status_success;
	  }
	return rocsparse_status_invalid_value;
      }
    }
    return rocsparse_status_invalid_value;
  }

  rocsparse_status rocsparse_bicgstab_outputs_::get(rocsparse_itsol_bicgstab_output output,
					      void * value,
					      size_t size)const
  {
    switch(output)
      {
      case rocsparse_itsol_bicgstab_output_alpha:
	{
	  if (size == sizeof(int32_t))
	  {
	    *(( int32_t*)value) = this->get_alpha();
	    return rocsparse_status_success;
	  }
	else if (size == sizeof(int64_t))
	  {
	    *((int64_t*)value) = this->get_alpha();
	    return rocsparse_status_success;
	  }
	return rocsparse_status_invalid_value;
      }
    }
    return rocsparse_status_invalid_value;
  }
