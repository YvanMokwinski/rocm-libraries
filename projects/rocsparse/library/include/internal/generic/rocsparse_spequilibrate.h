/*! \file */
/* ************************************************************************
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the Software), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */

#ifndef ROCSPARSE_SPEQUILIBRATE_H
#define ROCSPARSE_SPEQUILIBRATE_H

#include "../../rocsparse-types.h"
#include "rocsparse/rocsparse-export.h"

#ifdef __cplusplus
extern "C" {
#endif

  
  ROCSPARSE_EXPORT
  rocsparse_status rocsparse_spequilibrate_set_input(rocsparse_handle handle,
						     rocsparse_spequilibrate_descr descr,
						     rocsparse_spequilibrate_input value,
						     const void * data,
						     size_t  data_size_in_bytes,
						     rocsparse_error*p_error);
  
  ROCSPARSE_EXPORT
  rocsparse_status rocsparse_spequilibrate_get_output(rocsparse_handle handle,
						      rocsparse_spequilibrate_descr descr,
						      rocsparse_spequilibrate_output value,
						      void * data,
						      size_t  data_size_in_bytes,
						      rocsparse_error*p_error);
  
  ROCSPARSE_EXPORT
  rocsparse_status rocsparse_spequilibrate_descr_create(rocsparse_handle handle,
							rocsparse_spequilibrate_descr*p_descr,
							rocsparse_error*p_error);
  
  ROCSPARSE_EXPORT
  rocsparse_status rocsparse_spequilibrate_descr_destroy(rocsparse_handle handle,
							 rocsparse_spequilibrate_descr descr,
							 rocsparse_error*p_error);
  
  ROCSPARSE_EXPORT
  rocsparse_status rocsparse_spequilibrate_buffer_size(rocsparse_handle               handle,
						       rocsparse_spequilibrate_descr  descr,
						       rocsparse_const_spmat_descr    A,
						       rocsparse_const_dnvec_descr    D_left,
						       rocsparse_const_dnvec_descr    D_right,
						       rocsparse_spequilibrate_stage  stage,
						       size_t*                        p_buffer_size_in_bytes,
						       rocsparse_error*               p_error);
  
  ROCSPARSE_EXPORT
  rocsparse_status rocsparse_spequilibrate(rocsparse_handle              handle,
					   rocsparse_spequilibrate_descr descr,
					   rocsparse_spmat_descr         A,
					   rocsparse_dnvec_descr         D_left,
					   rocsparse_dnvec_descr         D_right,
					   rocsparse_spequilibrate_stage stage,
					   size_t                        buffer_size_in_bytes,
					   void*                         buffer,
					   rocsparse_error*              p_error);
  
#ifdef __cplusplus
}
#endif

#endif /* ROCSPARSE_SPEQUILIBRATE_H */
