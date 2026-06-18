#pragma once

#include <rocsparse/rocsparse-types.h>

typedef struct rocsparse_clients_generate_pointer_descr_
{
  rocsparse_format m_format;
  rocsparse_clients_generate_pointer_descr_(rocsparse_format format) : m_format(format)
  {
  }
} * rocsparse_clients_generate_pointer_descr;


