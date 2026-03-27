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
#include "rocsparse_enum_utils.hpp"
#include "rocsparse_utility.hpp"
#include "rocsparse_spequilibrate_descr.hpp"

template <>
inline bool rocsparse::enum_utils::is_invalid(rocsparse_spequilibrate_output value)
{
    switch(value)
    {
    case rocsparse_spequilibrate_output_ruiz_iter:
    {
        return false;
    }
    case rocsparse_spequilibrate_output_ruiz_nrm:
    {
        return false;
    }
    }
    return true;
};

extern "C" rocsparse_status rocsparse_spequilibrate_get_output(rocsparse_handle        handle,
							       rocsparse_spequilibrate_descr  descr,
							       rocsparse_spequilibrate_output output,
							       void*                   data,
							       size_t                  data_size_in_bytes,
							       rocsparse_error*        p_error)
  try
    {
      ROCSPARSE_ROUTINE_TRACE;
      ROCSPARSE_CHECKARG_HANDLE(0, handle);
      ROCSPARSE_CHECKARG_POINTER(1, descr);
      ROCSPARSE_CHECKARG_ENUM(2, output);
      ROCSPARSE_CHECKARG_POINTER(3, data);
      ROCSPARSE_CHECKARG(
			 4, data_size_in_bytes, data_size_in_bytes == 0, rocsparse_status_invalid_size);

      switch(output)
	{
	
	case rocsparse_spequilibrate_output_ruiz_nrm:
	  {
	    ROCSPARSE_CHECKARG(4,
			       data_size_in_bytes,
			       data_size_in_bytes != sizeof(double),
			       rocsparse_status_invalid_size);

	    double nrm = descr->get_nrm();
	    RETURN_IF_HIP_ERROR(hipMemcpy(data,
					  &nrm,
					  data_size_in_bytes,
					  hipMemcpyHostToHost));	  
	    return rocsparse_status_success;
	  }
	
	case rocsparse_spequilibrate_output_ruiz_iter:
	  {
	    ROCSPARSE_CHECKARG(4,
			       data_size_in_bytes,
			       data_size_in_bytes != sizeof(int64_t),
			       rocsparse_status_invalid_size);
	    int64_t niter = descr->get_niter();
	    RETURN_IF_HIP_ERROR(hipMemcpy(data,
					  &niter,
					  data_size_in_bytes,
					  hipMemcpyHostToHost));
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
