#pragma once

#include "rocsparse_clients_generate.h"

typedef struct rocsparse_clients_generate_input_descr_
{
private:
  int64_t m_m{-1};
  int64_t m_n{-1};
  rocsparse_indextype m_ptr_indextype{};
  rocsparse_indextype m_ind_indextype{};
  rocsparse_datatype m_val_datatype{};
  rocsparse_clients_generate_alg m_alg{};
  rocsparse_format m_format{rocsparse_format_csr};
public:
  int64_t get_nrows() const { return this->m_m; }
  void set_nrows(int64_t value) { this->m_m=value; }
  int64_t get_ncols() const { return this->m_n; }
  void set_ncols(int64_t value) { this->m_n=value; }
  rocsparse_indextype get_ptr_indextype() const { return this->m_ptr_indextype; }
  void set_ptr_indextype(rocsparse_indextype value)  { this->m_ptr_indextype=value; }
  rocsparse_format get_format() const { return this->m_format; }
  void set_format(rocsparse_format value)  { this->m_format=value; }
  rocsparse_indextype get_ind_indextype() const { return this->m_ind_indextype; }
  void set_ind_indextype(rocsparse_indextype value)  { this->m_ind_indextype=value; }
  rocsparse_datatype get_val_datatype() const { return this->m_val_datatype; }
  void set_val_datatype(rocsparse_datatype value)  { this->m_val_datatype=value; }
  
  rocsparse_clients_generate_alg get_alg() const { return this->m_alg; }  
  void set_alg(rocsparse_clients_generate_alg value)  { this->m_alg=value; }
  
  rocsparse_clients_generate_input_descr_()
  {
    m_ptr_indextype = (rocsparse_indextype)-1;
    m_ind_indextype = (rocsparse_indextype)-1;
    m_val_datatype = (rocsparse_datatype)-1;    
    m_alg = (rocsparse_clients_generate_alg)-1;
  }
} * rocsparse_clients_generate_input_descr;
