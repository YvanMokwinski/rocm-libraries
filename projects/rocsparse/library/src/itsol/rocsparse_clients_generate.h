#pragma once
#include <rocsparse/rocsparse.h>
#include <map>

typedef enum rocsparse_clients_generate_alg_
  {
    rocsparse_clients_generate_alg_tridiagonal,
    rocsparse_clients_generate_alg_file
  }rocsparse_clients_generate_alg;

typedef enum rocsparse_clients_generate_csr_pointer_
  {
    rocsparse_clients_generate_csr_pointer_ptr,
    rocsparse_clients_generate_csr_pointer_ind,
    rocsparse_clients_generate_csr_pointer_val,
  } rocsparse_clients_generate_csr_pointer;

typedef enum rocsparse_clients_generate_input_
  {
    rocsparse_clients_generate_input_m,
    rocsparse_clients_generate_input_n,
    rocsparse_clients_generate_input_alg,
    rocsparse_clients_generate_input_format,
    rocsparse_clients_generate_input_ptr_indextype,
    rocsparse_clients_generate_input_ind_indextype,
    rocsparse_clients_generate_input_val_datatype,
  }rocsparse_clients_generate_input;

typedef enum rocsparse_clients_generate_output_
  {
    rocsparse_clients_generate_output_nnz,
    rocsparse_clients_generate_output_m,
    rocsparse_clients_generate_output_n,
    rocsparse_clients_generate_output_ptr_indextype,
    rocsparse_clients_generate_output_ind_indextype,
    rocsparse_clients_generate_output_val_datatype,
  }rocsparse_clients_generate_output;

typedef enum rocsparse_clients_generate_stage_
  {
    rocsparse_clients_generate_stage_analysis,
    rocsparse_clients_generate_stage_compute
  }rocsparse_clients_generate_stage;

typedef struct rocsparse_clients_generate_handle_ * rocsparse_clients_generate_handle;


extern "C" void rocsparse_clients_create_generate_handle(rocsparse_clients_generate_handle * handle);
extern "C" void rocsparse_clients_destroy_generate_handle(rocsparse_clients_generate_handle  handle);


extern "C" void rocsparse_clients_generate_input_set(rocsparse_clients_generate_handle  handle,
						     rocsparse_clients_generate_input input,
						     const void * data,
						     size_t data_size);

extern "C" void rocsparse_clients_generate_output_get(rocsparse_clients_generate_handle  handle,
						      rocsparse_clients_generate_output output,
						      void * data,
						      size_t data_size);

extern "C" void rocsparse_clients_generate_csr_pointer_set(rocsparse_clients_generate_handle  handle,
							   rocsparse_clients_generate_csr_pointer pointer,
							   void * data,
							   size_t data_size_in_bytes);


extern "C" void rocsparse_clients_generate_buffer_size(rocsparse_clients_generate_handle handle,
						       rocsparse_clients_generate_stage stage,
						       size_t* buffer_size);

extern "C" void rocsparse_clients_generate(rocsparse_clients_generate_handle handle,
					   rocsparse_clients_generate_stage stage,
					   size_t buffer_size,
					   void * buffer);

