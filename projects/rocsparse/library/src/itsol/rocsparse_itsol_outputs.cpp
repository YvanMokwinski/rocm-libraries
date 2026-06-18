
#include "rocsparse_itsol_outputs.hpp"
#include <string.h>
int64_t rocsparse_itsol_outputs_::get_niter()const {return this->niter;}
void   rocsparse_itsol_outputs_::set_niter(int64_t value){ this->niter = value;}
void rocsparse_itsol_outputs_::get_niter(void * data,size_t size) const
{
  if (size == sizeof(int32_t))
    {
      const int32_t d = this->niter;
      memcpy(data, &d, size);
    }
  else if (size == sizeof(int64_t))
    {
      memcpy(data, &this->niter, size);
    }
}

void rocsparse_itsol_outputs_::set_niter(const void * data,size_t size) 
{
  if (size == sizeof(int32_t))
    {
      int32_t d;
      memcpy( &d,data, size);
      this->niter = d;
    }
  else if (size == sizeof(int64_t))
    {
      memcpy(&this->niter,data, size);
    }
}

void rocsparse_itsol_outputs_::get(rocsparse_itsol_output output,void * data,size_t size)const
{
  switch(output)
    {
    case rocsparse_itsol_output_niter:
      {
	this->get_niter(data,size);
	break;
      }
    }
}

void rocsparse_itsol_outputs_::set(rocsparse_itsol_output output,const void * data,size_t size)
{
  switch(output)
    {
    case rocsparse_itsol_output_niter:
      {
	this->set_niter(data,size);
	break;
      }
    }
}
