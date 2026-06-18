#pragma once

#include "internal/itsol/rocsparse_itsol_fgmres.h"

typedef struct rocsparse_fgmres_outputs_
{
  double alpha;
  double get_alpha()const;
  void set_alpha(double value);
  rocsparse_status set(rocsparse_itsol_fgmres_output output,
		       const void * value,
		       size_t size);
  rocsparse_status get(rocsparse_itsol_fgmres_output output,
		       void * value,
		       size_t size)const;
  
} * rocsparse_fgmres_outputs;
