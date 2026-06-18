#pragma once

#include "rocsparse_clients_generate_input_descr.hpp"
#include <string>

typedef struct rocsparse_clients_generate_input_file_descr_ : rocsparse_clients_generate_input_descr_
{
private:
  std::string m_filename;

public:
  const char * get_filename() const { return this->m_filename.c_str(); }
  void set_filename(const char * value) { this->m_filename = value; }
  
  rocsparse_clients_generate_input_file_descr_()
  {
  }
} * rocsparse_clients_generate_input_file_descr;
