#pragma once

#include "rocsparse_clients_generate_input_descr.hpp"

typedef struct rocsparse_clients_generate_input_tridiagonal_descr_ : rocsparse_clients_generate_input_descr_
{
private:
  double m_diagonal_value{0};
public:
  double get_diag_value() const { return this->m_diagonal_value; }
  void set_diag_value(double value) { this->m_diagonal_value=value; }
  
  rocsparse_clients_generate_input_tridiagonal_descr_()
  {
  }
} * rocsparse_clients_generate_input_tridiagonal_descr;
