#pragma once

#include "rocsparse_clients_generate.h"
typedef struct rocsparse_clients_generate_output_file_descr_ : rocsparse_clients_generate_output_descr_
{
  int64_t m_zero{-1};
} * rocsparse_clients_generate_output_file_descr;
