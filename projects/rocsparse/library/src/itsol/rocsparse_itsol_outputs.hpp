#pragma once

#include "internal/itsol/rocsparse_itsol.h"

typedef struct rocsparse_itsol_outputs_
{
private:
  int64_t niter;
public:
  
  int64_t get_niter()const;
  void   set_niter(int64_t value);
  void get_niter(void * data,size_t size) const;
  void set_niter(const void * data,size_t size) ;
  void get(rocsparse_itsol_output output,void * data,size_t size) const;
  void set(rocsparse_itsol_output output,const void * data,size_t size);
} *rocsparse_itsol_outputs;
