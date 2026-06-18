#pragma once

#include "rocsparse_clients_generate_input_descr.hpp"
#include "rocsparse_clients_generate_output_descr.hpp"

#include "rocsparse_clients_generate_pointer_descr.hpp"

typedef struct rocsparse_clients_generate_descr_
{
  rocsparse_clients_generate_input_descr m_input{};  
  rocsparse_clients_generate_pointer_descr m_pointer{};
  rocsparse_clients_generate_output_descr m_output{};
} * rocsparse_clients_generate_descr;

typedef struct rocsparse_clients_generate_handle_
{
  rocsparse_clients_generate_descr m_descr;
} * rocsparse_clients_generate_handle;

