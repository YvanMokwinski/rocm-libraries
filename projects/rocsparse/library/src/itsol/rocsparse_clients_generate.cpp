#include <rocsparse/rocsparse.h>
#include <map>
#include <iostream>

#include "rocsparse_clients_generate_tridiagonal_descr.hpp"
#include "rocsparse_clients_generate_file_descr.hpp"
#include "rocsparse_clients_generate_descr.hpp"
#include "rocsparse_clients_generate_csr_pointer_descr.hpp"


extern "C" void rocsparse_clients_generate_output_get(rocsparse_clients_generate_handle  handle,
						      rocsparse_clients_generate_output output,
						      void * data,
						      size_t data_size)
{
  switch(output)
    {
    case rocsparse_clients_generate_output_ptr_indextype:
      {
	*((rocsparse_indextype*)data) = handle->m_descr->m_output->m_ptr_indextype;
	break;
      }
    case rocsparse_clients_generate_output_ind_indextype:
      {
	*((rocsparse_indextype*)data) = handle->m_descr->m_output->m_ind_indextype;
	break;
      }
    case rocsparse_clients_generate_output_val_datatype:
      {
	*((rocsparse_datatype*)data) = handle->m_descr->m_output->m_val_datatype;
	break;
      }

    case rocsparse_clients_generate_output_m:
      {

	if (data_size == sizeof(int32_t))
	  {
	    *((int32_t*)data) = handle->m_descr->m_output->m_m;
	  }
	else
	  {
	    *((int64_t*)data) = handle->m_descr->m_output->m_m;
	  }
	break;
      }

    case rocsparse_clients_generate_output_n:
      {

	if (data_size == sizeof(int32_t))
	  {
	    *((int32_t*)data) = handle->m_descr->m_output->m_n;
	  }
	else
	  {
	    *((int64_t*)data) = handle->m_descr->m_output->m_n;
	  }
	break;
      }

    case rocsparse_clients_generate_output_nnz:
      {
	if (data_size == sizeof(int32_t))
	  {
	    *((int32_t*)data) = handle->m_descr->m_output->m_nnz;
	  }
	else
	  {
	    *((int64_t*)data) = handle->m_descr->m_output->m_nnz;
	  }
	break;
      }
    }
}

extern "C" void  rocsparse_clients_create_generate_handle(rocsparse_clients_generate_handle * handle)
{
  handle[0] = new rocsparse_clients_generate_handle_();
  
}

extern "C" void  rocsparse_clients_destroy_generate_handle(rocsparse_clients_generate_handle  handle)
{
  if (handle)
    {
      
      delete handle;
    }
}

extern "C" void rocsparse_clients_generate_input_set(rocsparse_clients_generate_handle self,
						     rocsparse_clients_generate_input input,
						     const void * data,
						     size_t data_size)
{
  //
  //
  //

  switch(input)
    {
    case rocsparse_clients_generate_input_alg:
      {
	auto alg = *((const rocsparse_clients_generate_alg*)data);
	switch(alg)
	  {
	  case rocsparse_clients_generate_alg_file:
	    {
	      if (self->m_descr)
		delete self->m_descr;
	      self->m_descr = new rocsparse_clients_generate_file_descr_();
	      
	      self->m_descr->m_input = new rocsparse_clients_generate_input_file_descr_();	      
	      self->m_descr->m_output = new rocsparse_clients_generate_output_file_descr_();	     	      
	      break;
	    }
	  case rocsparse_clients_generate_alg_tridiagonal:
	    {
	      if (self->m_descr)
		delete self->m_descr;
	      self->m_descr = new rocsparse_clients_generate_tridiagonal_descr_();
	      self->m_descr->m_input = new rocsparse_clients_generate_input_tridiagonal_descr_();	      
	      self->m_descr->m_output = new rocsparse_clients_generate_output_tridiagonal_descr_();	      
	      break;
	    }
	  }
	self->m_descr->m_input->set_alg(alg);	
	break;
      }
      
    case rocsparse_clients_generate_input_format:
      {
	rocsparse_format format = *((const rocsparse_format*)data);
	switch(format)
	  {
	  case rocsparse_format_csr:
	    {
	      self->m_descr->m_pointer = new  rocsparse_clients_generate_csr_pointer_descr_();
	      break;
	    }
	  case rocsparse_format_csc:
	  case rocsparse_format_coo:
	  case rocsparse_format_coo_aos:
	  case rocsparse_format_ell:
	  case rocsparse_format_bell:
	  case rocsparse_format_bsr:
	    {
	      self->m_descr->m_pointer = nullptr;
	      break;
	    }
	  }

	break;
      }
    case rocsparse_clients_generate_input_ptr_indextype:
      {

	self->m_descr->m_input->set_ptr_indextype(*((const rocsparse_indextype*)data));
	break;
      }
    case rocsparse_clients_generate_input_ind_indextype:
      {

	self->m_descr->m_input->set_ind_indextype(*((const rocsparse_indextype*)data));
	break;
      }
    case rocsparse_clients_generate_input_val_datatype:
      {

	self->m_descr->m_input->set_val_datatype(*((const rocsparse_datatype*)data));
	break;
      }
    case rocsparse_clients_generate_input_m:
      {

	if (sizeof(data_size) == sizeof(int32_t))
	  {
	    self->m_descr->m_input->set_nrows(*((const int32_t*)data));
	  }
	else
	  {
	    self->m_descr->m_input->set_nrows(*((const int64_t*)data));
	  }
	break;
      }
    case rocsparse_clients_generate_input_n:
      {

	if (sizeof(data_size) == sizeof(int32_t))
	  {
	    self->m_descr->m_input->set_ncols(*((const int32_t*)data));	    
	  }
	else
	  {
	    self->m_descr->m_input->set_ncols(*((const int64_t*)data));	    	    
	  }
	break;
      }
    }
}

extern "C" void rocsparse_clients_generate_get_output(rocsparse_clients_generate_handle  handle,
						      rocsparse_clients_generate_output output,
						      void * data,
						      size_t data_size)
{
  switch(output)
    {
    case rocsparse_clients_generate_output_ptr_indextype:
      {
	*((rocsparse_indextype*)data) = handle->m_descr->m_output->m_ptr_indextype;
	break;
      }
    case rocsparse_clients_generate_output_ind_indextype:
      {
	*((rocsparse_indextype*)data) = handle->m_descr->m_output->m_ind_indextype;
	break;
      }
    case rocsparse_clients_generate_output_val_datatype:
      {
	*((rocsparse_datatype*)data) = handle->m_descr->m_output->m_val_datatype;
	break;
      }

    case rocsparse_clients_generate_output_m:
      {

	if (data_size == sizeof(int32_t))
	  {
	    *((int32_t*)data) = handle->m_descr->m_output->m_m;
	  }
	else
	  {
	    *((int64_t*)data) = handle->m_descr->m_output->m_m;
	  }
	break;
      }
    case rocsparse_clients_generate_output_n:
      {

	if (data_size == sizeof(int32_t))
	  {
	    *((int32_t*)data) = handle->m_descr->m_output->m_n;
	  }
	else
	  {
	    *((int64_t*)data) = handle->m_descr->m_output->m_n;
	  }
	break;
      }
    case rocsparse_clients_generate_output_nnz:
      {
	if (data_size == sizeof(int32_t))
	  {
	    *((int32_t*)data) = handle->m_descr->m_output->m_nnz;
	  }
	else
	  {
	    *((int64_t*)data) = handle->m_descr->m_output->m_nnz;
	  }
	break;
      }
    }
}

extern "C" void rocsparse_clients_generate_csr_pointer_set(rocsparse_clients_generate_handle  handle,
							   rocsparse_clients_generate_csr_pointer pointer,
							   void * data,
							   size_t data_size_in_bytes)
{
  switch(pointer)
    {
    case rocsparse_clients_generate_csr_pointer_ptr:
      {
	reinterpret_cast<rocsparse_clients_generate_csr_pointer_descr>(handle->m_descr->m_pointer)->m_ptr = data;
	break;
      }
    case rocsparse_clients_generate_csr_pointer_ind:
      {
	reinterpret_cast<rocsparse_clients_generate_csr_pointer_descr>(handle->m_descr->m_pointer)->m_ind = data;
	break;
      }
    case rocsparse_clients_generate_csr_pointer_val:
      {
	reinterpret_cast<rocsparse_clients_generate_csr_pointer_descr>(handle->m_descr->m_pointer)->m_val = data;
	break;
      }
    }
}

extern "C" void rocsparse_clients_generate_buffer_size(rocsparse_clients_generate_handle handle,
						       rocsparse_clients_generate_stage stage,
						       size_t* buffer_size)
{
  buffer_size[0] = 0;
}



extern "C" void  rocsparse_clients_generate(rocsparse_clients_generate_handle handle_,
					    rocsparse_clients_generate_stage stage,
					    size_t buffer_size,
					    void * buffer)
{
  auto alg = handle_->m_descr->m_input->get_alg();
  switch(alg)    
    {
    case rocsparse_clients_generate_alg_tridiagonal:
      {
	rocsparse_clients_generate_tridiagonal_descr descr = (rocsparse_clients_generate_tridiagonal_descr)handle_->m_descr;
	descr->generate(stage, buffer_size, buffer);
	return;
      }
    case rocsparse_clients_generate_alg_file:
      {
	rocsparse_clients_generate_file_descr descr = (rocsparse_clients_generate_file_descr)handle_->m_descr;
	descr->generate(stage, buffer_size, buffer);
	return;
      }
    }  
}
