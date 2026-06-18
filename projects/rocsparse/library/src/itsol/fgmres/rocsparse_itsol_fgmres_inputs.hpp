#pragma once

#include "internal/itsol/rocsparse_itsol_fgmres.h"

typedef struct rocsparse_fgmres_inputs_
{
private:
  int64_t size_basis;
public:
  int64_t get_size_basis()const;
  void set_size_basis(int64_t value);
  
  rocsparse_status set(rocsparse_itsol_fgmres_input input,
		       const void * value,
		       size_t size);

  rocsparse_status get(rocsparse_itsol_fgmres_input input,
		       void * value,
		       size_t size)const;
  
}* rocsparse_fgmres_inputs;

