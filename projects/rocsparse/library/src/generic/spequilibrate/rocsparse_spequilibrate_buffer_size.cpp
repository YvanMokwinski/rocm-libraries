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


namespace rocsparse
{
  static rocsparse_status spequilibrate_buffer_size(rocsparse_handle              handle,
						    rocsparse_spequilibrate_descr descr,
						    rocsparse_spequilibrate_stage stage,
						    rocsparse_const_spmat_descr   A,
						    rocsparse_const_dnvec_descr   D_left,
						    rocsparse_const_dnvec_descr   D_right,
						    size_t*                       buffer_size_in_bytes)
  {
    ROCSPARSE_ROUTINE_TRACE;
    const rocsparse_format    format      = A->format;
    const int64_t             batch_count = A->batch_count;
    if (format != rocsparse_format_csr)
      {
	RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
      }
    
    buffer_size_in_bytes[0] = std::numeric_limits<size_t>::max();    
    switch(stage)
      {	
      case rocsparse_spequilibrate_stage_analysis:
	{
	  buffer_size_in_bytes[0] = 0;
	  return rocsparse_status_success;
	}
	
      case rocsparse_spequilibrate_stage_compute:
	{
	  switch(descr->get_alg())
	    {
	    case rocsparse_spequilibrate_alg_default:
	    case rocsparse_spequilibrate_alg_ruiz:
	      {
		buffer_size_in_bytes[0] =
		  ( rocsparse::datatype_sizeof(D_left->data_type) * A->rows +
		    rocsparse::datatype_sizeof(D_right->data_type) * A->cols +
		    sizeof(double) * 2 )  * batch_count;
		return rocsparse_status_success;
	      }
	    }
	  RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
	}	
	// LCOV_EXCL_START      
      }
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
    // LCOV_EXCL_STOP    
  }


}
/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */
  extern "C" rocsparse_status rocsparse_spequilibrate_buffer_size(rocsparse_handle            handle,
								  rocsparse_spequilibrate_descr  descr,
								  rocsparse_const_spmat_descr A,
								  rocsparse_const_dnvec_descr D_left,
								  rocsparse_const_dnvec_descr D_right,
								  rocsparse_spequilibrate_stage  stage,
								  size_t*          buffer_size_in_bytes,
								  rocsparse_error* p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;
    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, descr);
    ROCSPARSE_CHECKARG_POINTER(2, A);
    ROCSPARSE_CHECKARG_POINTER(3, D_left);
    ROCSPARSE_CHECKARG_POINTER(4, D_right);
    ROCSPARSE_CHECKARG_ENUM(5, stage);
    ROCSPARSE_CHECKARG_POINTER(6, buffer_size_in_bytes);

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::spequilibrate_buffer_size(handle,
								   descr,
								   stage,
								   A,
								   D_left,
								   D_right,
								   buffer_size_in_bytes));
    return rocsparse_status_success;
    // LCOV_EXCL_START
 }
 catch(...)
   {
     RETURN_ROCSPARSE_EXCEPTION();
   }
// LCOV_EXCL_STOP

