#pragma once
#include "rocsparse_clients_generate_descr.hpp"
#include "rocsparse_clients_generate_input_tridiagonal_descr.hpp"
#include "rocsparse_clients_generate_output_tridiagonal_descr.hpp"

typedef struct rocsparse_clients_generate_tridiagonal_descr_ : rocsparse_clients_generate_descr_
{
  void generate(rocsparse_clients_generate_stage stage,
		size_t buffer_size,
		void * buffer);
  
  
} * rocsparse_clients_generate_tridiagonal_descr;

