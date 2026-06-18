#pragma once
#include "rocsparse_clients_generate_descr.hpp"
#include "rocsparse_clients_generate_input_file_descr.hpp"
#include "rocsparse_clients_generate_output_file_descr.hpp"

typedef struct rocsparse_clients_generate_file_descr_ : rocsparse_clients_generate_descr_
{
  void generate(rocsparse_clients_generate_stage stage,
		size_t buffer_size,
		void * buffer);
  
  FILE * f{};
  
  char   m_data[16]{};
  int    m_symm{};

} * rocsparse_clients_generate_file_descr;

