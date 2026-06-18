#pragma once

#include "rocsparse_clients_generate.h"

typedef struct rocsparse_clients_generate_output_descr_
{
  int64_t m_m{-1};
  int64_t m_n{-1};
  int64_t m_nnz{-1};
  rocsparse_indextype m_ptr_indextype{};
  rocsparse_indextype m_ind_indextype{};
  rocsparse_datatype m_val_datatype{};
  
  rocsparse_indextype get_ptr_indextype()const{return m_ptr_indextype;}
  rocsparse_indextype get_ind_indextype()const{return m_ind_indextype;}
  rocsparse_datatype get_val_datatype()const{return m_val_datatype;}  
  int64_t get_nrows()const {return this->m_m;}
  int64_t get_ncols()const {return this->m_n;}
  int64_t get_nnz()const {return this->m_nnz;}
} * rocsparse_clients_generate_output_descr;

