#pragma once
#include "rocsparse_clients_generate.h"

typedef enum rocsparse_clients_generate_input_tridiagonal_
  {
    rocsparse_clients_generate_input_tridiagonal_dvalue
  }rocsparse_clients_generate_input_tridiagonal;

extern "C" void rocsparse_clients_generate_input_tridiagonal_set(rocsparse_clients_generate_handle handle,
								 rocsparse_clients_generate_input_tridiagonal input,
								 const void * data,
								 size_t data_size);
