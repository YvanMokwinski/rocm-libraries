#pragma once
#include "rocsparse_clients_generate.h"


typedef enum rocsparse_clients_generate_input_file_
  {
    rocsparse_clients_generate_input_filename
  }rocsparse_clients_generate_input_file;

extern "C" void rocsparse_clients_generate_input_file_set(rocsparse_clients_generate_handle  handle,
							  rocsparse_clients_generate_input_file input,
							  const void * data,
							  size_t data_size);
