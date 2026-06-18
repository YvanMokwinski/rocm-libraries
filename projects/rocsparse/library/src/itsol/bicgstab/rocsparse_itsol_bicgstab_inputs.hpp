#pragma once

#include "internal/itsol/rocsparse_itsol_bicgstab.h"

typedef struct rocsparse_bicgstab_inputs_
{
private:
  int64_t scal;
public:
  int64_t get_scal()const;
  void set_scal(int64_t value);
  
  rocsparse_status set(rocsparse_itsol_bicgstab_input input,
		       const void * value,
		       size_t size);

  rocsparse_status get(rocsparse_itsol_bicgstab_input input,
		       void * value,
		       size_t size)const;
  
}* rocsparse_bicgstab_inputs;

