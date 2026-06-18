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

#ifndef ROCSPARSE_ITSOL_H
#define ROCSPARSE_ITSOL_H

#include "rocsparse-types.h"
#include "rocsparse/rocsparse-export.h"

#ifdef __cplusplus
extern "C" {
#endif
  
  // -------------------------------------------------------------------------
  // RCI request codes
  // -------------------------------------------------------------------------
  typedef enum rocsparse_itsol_alg_ {
    rocsparse_itsol_alg_cg = 1,
    rocsparse_itsol_alg_bicgstab,
    rocsparse_itsol_alg_fgmres,
  } rocsparse_itsol_alg;
  
  // -------------------------------------------------------------------------
  // RCI request codes
  // -------------------------------------------------------------------------
  typedef enum rocsparse_itsol_request_ {
    rocsparse_itsol_request_matrix_vector=1, // The solver wants A*v
    rocsparse_itsol_request_preconditioner, // The solver wants P^{-1}*v
    rocsparse_itsol_request_finished,      // The solver converged or hit max iterations
    rocsparse_itsol_request_error          // The solver encountered an error
  } rocsparse_itsol_request;
  
  typedef enum rocsparse_itsol_output_ {
    rocsparse_itsol_output_niter=1
  } rocsparse_itsol_output;
  
  typedef enum rocsparse_itsol_input_ {
    rocsparse_itsol_input_alg=1,
    rocsparse_itsol_input_dimension,
    rocsparse_itsol_input_datatype_rhs,
    rocsparse_itsol_input_datatype_sol,
    rocsparse_itsol_input_datatype_compute,
    rocsparse_itsol_input_nmaxiter,
    rocsparse_itsol_input_tolerance
  } rocsparse_itsol_input;
  
  // -------------------------------------------------------------------------
  // A structure to hold the solver state (using raw pointers for arrays).
  // -------------------------------------------------------------------------
  typedef struct rocsparse_itsol_descr_ * rocsparse_itsol_descr;
  
  ROCSPARSE_EXPORT   rocsparse_status rocsparse_itsol_set_input(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
								rocsparse_itsol_input input,
								const void * input_data,
								size_t input_datasize);
  
  ROCSPARSE_EXPORT   rocsparse_status rocsparse_itsol_get_input(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
								rocsparse_itsol_input input,
								void * input_data,
								size_t input_datasize);

  ROCSPARSE_EXPORT rocsparse_status rocsparse_itsol_get_output(rocsparse_handle handle,
							       rocsparse_itsol_descr descr,
							       rocsparse_itsol_output output,
							       void * output_data,
							       size_t output_datasize);

  ROCSPARSE_EXPORT rocsparse_status rocsparse_itsol_get_request(rocsparse_handle handle,
								rocsparse_itsol_descr descr,
								rocsparse_itsol_request * request);

  ROCSPARSE_EXPORT rocsparse_status rocsparse_itsol_get_request_input(rocsparse_handle handle,
								      rocsparse_itsol_descr descr,
								      void ** data);

  ROCSPARSE_EXPORT rocsparse_status rocsparse_itsol_get_request_output(rocsparse_handle handle,
								       rocsparse_itsol_descr descr,
								       void ** data);


  ROCSPARSE_EXPORT rocsparse_status rocsparse_destroy_itsol_descr(rocsparse_itsol_descr descr);

  ROCSPARSE_EXPORT rocsparse_status rocsparse_create_itsol_descr(rocsparse_handle handle,
								 rocsparse_itsol_descr * descr);

  ROCSPARSE_EXPORT rocsparse_status rocsparse_itsol_buffer_size(rocsparse_handle handle,
								rocsparse_itsol_descr  descr,
								size_t*buffer_size);

  ROCSPARSE_EXPORT rocsparse_status rocsparse_itsol(rocsparse_handle handle,
						    rocsparse_itsol_descr st,
						    const void * b,
						    void * x,	    
						    size_t buffer_size,
						    void * buffer);

  ROCSPARSE_EXPORT rocsparse_status rocsparse_itsol_request_get_name(rocsparse_handle handle,
								     rocsparse_itsol_request that,
								     char * name);

#ifdef __cplusplus
}
#endif

#endif
