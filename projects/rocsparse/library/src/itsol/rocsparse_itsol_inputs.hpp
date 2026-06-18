#pragma once

#include "internal/itsol/rocsparse_itsol.h"

typedef struct rocsparse_itsol_inputs_
{
  rocsparse_itsol_alg alg;
  int64_t dimension;
  int64_t nmaxiter;
  double tolerance;
  rocsparse_datatype datatype_rhs;
  rocsparse_datatype datatype_sol;
  rocsparse_datatype datatype_compute;
  
  rocsparse_itsol_alg get_alg()const;
  void set_alg(rocsparse_itsol_alg alg);
  int64_t get_dimension()const;
  void set_dimension(int64_t value);
  int64_t get_nmaxiter()const;
  void set_nmaxiter(int64_t value);
  double get_tolerance()const;
  void set_tolerance(double value);

  rocsparse_datatype get_datatype_compute()const;
  void set_datatype_compute(rocsparse_datatype value);

  rocsparse_datatype get_datatype_sol()const;
  void set_datatype_sol(rocsparse_datatype value);

  rocsparse_datatype get_datatype_rhs()const;
  void set_datatype_rhs(rocsparse_datatype value);

  void set(rocsparse_itsol_input that,
	   const void * data,
	   size_t size);
  
  void get(rocsparse_itsol_input that,
	   void * data,
	   size_t size) const;

  
  void set_datatype_rhs(const void * data,size_t size);
  void set_datatype_compute(const void * data,size_t size);
  void set_datatype_sol(const void * data,size_t size);
  void set_nmaxiter(const void * data,size_t size);
  void set_tolerance(const void * data,size_t size);
  void set_dimension(const void * data,size_t size);
  void set_alg(const void * data,size_t size);

  void get_datatype_rhs(void * data,size_t size) const;
  void get_datatype_compute(void * data,size_t size) const;
  void get_datatype_sol(void * data,size_t size) const;
  void get_nmaxiter(void * data,size_t size) const;
  void get_tolerance(void * data,size_t size) const;
  void get_dimension(void * data,size_t size) const;
  void get_alg(void * data,size_t size) const;

} * rocsparse_itsol_inputs;


