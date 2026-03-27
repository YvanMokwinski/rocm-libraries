/*! \file */
/* ************************************************************************
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */

#include "internal/generic/rocsparse_spequilibrate.h"
#include "rocsparse_control.hpp"
#include "rocsparse_enum_utils.hpp"
#include "rocsparse_utility.hpp"
#include "rocsparse_spequilibrate_descr.hpp"


extern "C" rocsparse_status rocsparse_spequilibrate_set_input(rocsparse_handle       handle,
							      rocsparse_spequilibrate_descr descr,
							      rocsparse_spequilibrate_input input,
							      const void*            data,
							      size_t                 data_size_in_bytes,
							      rocsparse_error*       p_error)
  try
    {
      ROCSPARSE_ROUTINE_TRACE;      
      ROCSPARSE_CHECKARG_HANDLE(0, handle);
      ROCSPARSE_CHECKARG_POINTER(1, descr);
      ROCSPARSE_CHECKARG_ENUM(2, input);
      ROCSPARSE_CHECKARG_POINTER(3, data);
      
      switch(input)
	{
	case rocsparse_spequilibrate_input_alg:
	  {
	    RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(descr->get_stage() != ((rocsparse_spequilibrate_stage)-1)
						   ? rocsparse_status_invalid_value
						   : rocsparse_status_success,
						   "rocsparse_spequilibrate_set_input cannot modify the descriptor after any of the stages "
						   "rocsparse_spequilibrate_stage was executed");
	    
	    ROCSPARSE_CHECKARG(4,
			       data_size_in_bytes,
			       data_size_in_bytes != sizeof(rocsparse_spequilibrate_alg),
			       rocsparse_status_invalid_size);
	    
	    const rocsparse_spequilibrate_alg alg = *reinterpret_cast<const rocsparse_spequilibrate_alg*>(data);
	    descr->set_alg(alg);
	    return rocsparse_status_success;
	  }
	  
	case rocsparse_spequilibrate_input_ruiz_nmaxiter:
	  {
	    ROCSPARSE_CHECKARG(4,
			       data_size_in_bytes,
			       data_size_in_bytes != sizeof(int64_t),
			       rocsparse_status_invalid_size);

	    int64_t nmaxiter;
	    RETURN_IF_HIP_ERROR(hipMemcpy(&nmaxiter,
					  data,
					  data_size_in_bytes,
					  hipMemcpyHostToHost));

	    descr->set_nmaxiter(nmaxiter);
	    return rocsparse_status_success;
	  }
	  
	case rocsparse_spequilibrate_input_ruiz_tol:
	  {

	    ROCSPARSE_CHECKARG(4,
			       data_size_in_bytes,
			       data_size_in_bytes != sizeof(double),
			       rocsparse_status_invalid_size);

	    double tol;
	    RETURN_IF_HIP_ERROR(hipMemcpy(&tol,
					  data,
					  data_size_in_bytes,
					  hipMemcpyHostToHost));

	    descr->set_tol(tol);
	    return rocsparse_status_success;
	  }
	  
     // LCOV_EXCL_START
    }
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP
