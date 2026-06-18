#pragma once

#include "rocsparse_clients_generate_pointer_descr.hpp"

typedef struct rocsparse_clients_generate_csr_pointer_descr_ : rocsparse_clients_generate_pointer_descr_
{
  void * m_ptr{};
  void * m_ind{};
  void * m_val{};
  rocsparse_clients_generate_csr_pointer_descr_() : rocsparse_clients_generate_pointer_descr_(rocsparse_format_csr)
  {
  }
} * rocsparse_clients_generate_csr_pointer_descr;


