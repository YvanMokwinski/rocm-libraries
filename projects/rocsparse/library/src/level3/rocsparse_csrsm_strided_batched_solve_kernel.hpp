/*! \file */
/* ************************************************************************
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights Reserved.
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

#pragma once

#include "rocsparse_handle.hpp"

namespace rocsparse
{
    rocsparse_status
        csrsm_solve_strided_batched_kernel_launch(rocsparse_handle          handle,
                                                  rocsparse_operation       trans_B,
                                                  int64_t                   batch_count,
                                                  int64_t                   m,
                                                  int64_t                   nrhs,
                                                  rocsparse_datatype        alpha_datatype,
                                                  const void*               alpha_,
                                                  int64_t                   alpha_stride,
                                                  const rocsparse_mat_descr descr,
                                                  rocsparse_indextype       csr_row_ptr_indextype,
                                                  const void* __restrict__ csr_row_ptr_,
                                                  rocsparse_indextype csr_col_ind_indextype,
                                                  const void* __restrict__ csr_col_ind_,
                                                  rocsparse_datatype csr_val_datatype,
                                                  const void* __restrict__ csr_val_,
                                                  int64_t            csr_val_stride,
                                                  rocsparse_datatype B_datatype,
                                                  void* __restrict__ B_,
                                                  int64_t                      ldb,
                                                  int64_t                      B_stride,
                                                  rocsparse_mat_info           info,
                                                  rocsparse_fill_mode          fill_mode,
                                                  int32_t*                     done_array,
                                                  int64_t                      done_array_stride,
                                                  const rocsparse::trm_info_t* csrsm_info,
                                                  rocsparse::pivot_info_t*     pivot_info);

}
