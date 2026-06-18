#include "rocsparse_itsol_inputs.hpp"

rocsparse_datatype rocsparse_itsol_inputs_::get_datatype_compute()const {return datatype_compute;}
void rocsparse_itsol_inputs_::set_datatype_compute(rocsparse_datatype value){ datatype_compute = value;}

rocsparse_datatype rocsparse_itsol_inputs_::get_datatype_sol()const {return datatype_sol;}
void rocsparse_itsol_inputs_::set_datatype_sol(rocsparse_datatype value){ datatype_sol = value;}

rocsparse_datatype rocsparse_itsol_inputs_::get_datatype_rhs()const {return datatype_rhs;}
void rocsparse_itsol_inputs_::set_datatype_rhs(rocsparse_datatype value){ datatype_rhs = value;}


int64_t rocsparse_itsol_inputs_::get_dimension()const {return dimension;}
void rocsparse_itsol_inputs_::set_dimension(int64_t value){ dimension = value;}
int64_t rocsparse_itsol_inputs_::get_nmaxiter()const {return nmaxiter;}
void rocsparse_itsol_inputs_::set_nmaxiter(int64_t value){ nmaxiter = value;}
double rocsparse_itsol_inputs_::get_tolerance()const {return tolerance;}
void rocsparse_itsol_inputs_::set_tolerance(double value){ tolerance = value;}


void rocsparse_itsol_inputs_::set_datatype_rhs(const void * data,size_t size)
  {
    if (size == sizeof(rocsparse_datatype))
      {
	this->datatype_rhs = *((const rocsparse_datatype*)data);
      }
    if (size == sizeof(int32_t))
      {
	this->datatype_rhs = (rocsparse_datatype) *((const int32_t*)data);
      }
    else if (size == sizeof(int64_t))
      {
	this->datatype_rhs = (rocsparse_datatype) *((const int64_t*)data);
      }
  }


rocsparse_itsol_alg rocsparse_itsol_inputs_::get_alg()const {return alg;}
void rocsparse_itsol_inputs_::set_alg(rocsparse_itsol_alg value){ alg = value;}
    void rocsparse_itsol_inputs_::get_alg(void * data,size_t size) const
  {
    if (size == sizeof(rocsparse_itsol_alg))
      {
	*((rocsparse_itsol_alg*)data)= this->alg;
      }
    if (size == sizeof(int32_t))
      {
	 *((int32_t*)data)= this->alg;
      }
    else if (size == sizeof(int64_t))
      {
	 *((int64_t*)data)= this->alg;
      }
  }

void rocsparse_itsol_inputs_::set_alg(const void * data,size_t size)
  {
    if (size == sizeof(rocsparse_datatype))
      {
	this->alg = *((const rocsparse_itsol_alg*)data);
      }
    if (size == sizeof(int32_t))
      {
	this->alg = (rocsparse_itsol_alg) *((const int32_t*)data);
      }
    else if (size == sizeof(int64_t))
      {
	this->alg = (rocsparse_itsol_alg) *((const int64_t*)data);
      }
  }

void rocsparse_itsol_inputs_::set_datatype_compute(const void * data,size_t size)
{
  if (size == sizeof(rocsparse_datatype))
    {
      this->datatype_compute = *((const rocsparse_datatype*)data);
    }
  if (size == sizeof(int32_t))
    {
      this->datatype_compute = (rocsparse_datatype) *((const int32_t*)data);
    }
  else if (size == sizeof(int64_t))
    {
      this->datatype_compute = (rocsparse_datatype) *((const int64_t*)data);
    }
  }

void rocsparse_itsol_inputs_::set_datatype_sol(const void * data,size_t size)
{
  if (size == sizeof(rocsparse_datatype))
    {
      this->datatype_sol = *((const rocsparse_datatype*)data);
      }
  if (size == sizeof(int32_t))
    {
      this->datatype_sol = (rocsparse_datatype) *((const int32_t*)data);
      }
  else if (size == sizeof(int64_t))
    {
      this->datatype_sol = (rocsparse_datatype) *((const int64_t*)data);
      }
  }

  void rocsparse_itsol_inputs_::set_nmaxiter(const void * data,size_t size)
  {
    if (size == sizeof(int32_t))
      {
	this->nmaxiter = *((const int32_t*)data);
      }
    else if (size == sizeof(int64_t))
      {
	this->nmaxiter = *((const int64_t*)data);
      }
  }
  
  void rocsparse_itsol_inputs_::set_dimension(const void * data,size_t size)
  {
    if (size == sizeof(int32_t))
      {
	this->dimension = *((const int32_t*)data);
      }
    else if (size == sizeof(int64_t))
      {
	this->dimension = *((const int64_t*)data);
      }
  }
  
  void rocsparse_itsol_inputs_::set_tolerance(const void * data,size_t size)
  {
    memcpy(&this->tolerance, data, size);
  }


    void rocsparse_itsol_inputs_::get_datatype_rhs(void * data,size_t size) const
  {
    if (size == sizeof(rocsparse_datatype))
      {
	*((rocsparse_datatype*)data)= this->datatype_rhs;
      }
    if (size == sizeof(int32_t))
      {
	 *((int32_t*)data)= this->datatype_rhs;
      }
    else if (size == sizeof(int64_t))
      {
	 *((int64_t*)data)= this->datatype_rhs;
      }
  }

    void rocsparse_itsol_inputs_::get_datatype_compute(void * data,size_t size) const
  {
    if (size == sizeof(rocsparse_datatype))
      {
	*((rocsparse_datatype*)data) = this->datatype_compute;
      }
    if (size == sizeof(int32_t))
      {
	*((int32_t*)data)=this->datatype_compute;
      }
    else if (size == sizeof(int64_t))
      {
	*((int64_t*)data)=this->datatype_compute;
      }
  }

    void rocsparse_itsol_inputs_::get_datatype_sol(void * data,size_t size) const
  {
    if (size == sizeof(rocsparse_datatype))
      {
	*((rocsparse_datatype*)data) = this->datatype_sol;
      }
    if (size == sizeof(int32_t))
      {
	*((int32_t*)data) = this->datatype_sol;
      }
    else if (size == sizeof(int64_t))
      {
	*((int64_t*)data) = this->datatype_sol;
      }
  }

  void rocsparse_itsol_inputs_::get_nmaxiter(void * data,size_t size) const
  {
    if (size == sizeof(int32_t))
      {
	*((int32_t*)data)=this->nmaxiter;
      }
    else if (size == sizeof(int64_t))
      {
	*((int64_t*)data) = this->nmaxiter;
      }
  }

void rocsparse_itsol_inputs_::get_dimension(void * data,size_t size) const
  {
    if (size == sizeof(int32_t))
      {
	*((int32_t*)data)=this->dimension;
      }
    else if (size == sizeof(int64_t))
      {
	*((int64_t*)data) = this->dimension;
      }
  }
  
  void rocsparse_itsol_inputs_::get_tolerance(void * data,size_t size) const
  {
    memcpy(data,&this->tolerance, size);
  }

  void rocsparse_itsol_inputs_::set(rocsparse_itsol_input that,
	   const void * data,
	   size_t size) 
  {
    switch(that)
      {
      case rocsparse_itsol_input_dimension:
	{
	  this->set_dimension(data,size);
	  break;
	}	
      case rocsparse_itsol_input_alg:
	{
	  this->set_alg(data,size);
	  break;
	}	
      case rocsparse_itsol_input_nmaxiter:
	{
	  this->set_nmaxiter(data,size);
	  break;
	}
      case rocsparse_itsol_input_tolerance:
	{
	  this->set_tolerance(data,size);
	  break;
	}
      case rocsparse_itsol_input_datatype_rhs:
	{
	  this->set_datatype_rhs(data,size);
	  break;
	}
      case rocsparse_itsol_input_datatype_sol:
	{
	  this->set_datatype_sol(data,size);
	  break;
	}
      case rocsparse_itsol_input_datatype_compute:
	{
	  this->set_datatype_compute(data,size);
	  break;
	}
      }
  }


void rocsparse_itsol_inputs_::get(rocsparse_itsol_input that,
				  void * data,
				  size_t size) const
  {
    switch(that)
      {
      case rocsparse_itsol_input_alg:
	{
	  this->get_alg(data,size);
	  break;
	}	
      case rocsparse_itsol_input_dimension:
	{
	  this->get_dimension(data,size);
	  break;
	}	
      case rocsparse_itsol_input_nmaxiter:
	{
	  this->get_nmaxiter(data,size);
	  break;
	}
      case rocsparse_itsol_input_tolerance:
	{
	  this->get_tolerance(data,size);
	  break;
	}
      case rocsparse_itsol_input_datatype_rhs:
	{
	  this->get_datatype_rhs(data,size);
	  break;
	}
      case rocsparse_itsol_input_datatype_sol:
	{
	  this->get_datatype_sol(data,size);
	  break;
	}
      case rocsparse_itsol_input_datatype_compute:
	{
	  this->get_datatype_compute(data,size);
	  break;
	}
      }
  }

