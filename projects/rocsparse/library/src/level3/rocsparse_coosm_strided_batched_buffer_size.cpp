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
#include "rocsparse_control.hpp"
#include "rocsparse_coosm.hpp"
#include "rocsparse_csrsm.hpp"
#include "rocsparse_utility.hpp"

#include "../conversion/rocsparse_coo2csr.hpp"

rocsparse_status rocsparse::coosm_buffer_size(rocsparse_handle            handle,
                                              rocsparse_operation         trans_A,
                                              rocsparse_operation         trans_B,
                                              rocsparse_datatype          alpha_datatype,
                                              int64_t                     alpha_stride,
                                              rocsparse_const_spmat_descr A,
                                              rocsparse_const_dnmat_descr B,
                                              rocsparse_solve_policy      policy,
                                              size_t*                     buffer_size)
{
    ROCSPARSE_ROUTINE_TRACE;

    const int64_t nrhs = (trans_B == rocsparse_operation_none) ? B->cols : B->rows;
    if(A->rows == 0 || nrhs == 0 || B->batch_count == 0 || A->batch_count == 0)
    {
        *buffer_size = 0;
        return rocsparse_status_success;
    }

    const bool  choose_i32 = (A->nnz <= std::numeric_limits<int32_t>::max());
    const void* ptr        = (const void*)0x4;

    // Trick since it is not used in csrsm_buffer_size, otherwise we need to create a proper ptr array for nothing.
    _rocsparse_spmat_descr csr(rocsparse_format_csr,
                               A->analysed,
                               B->batch_count,
                               A->rows,
                               A->cols,
                               A->nnz,
                               A->data_type,
                               A->const_val_data,
                               A->val_data,
                               A->batch_stride,
                               (choose_i32) ? rocsparse_indextype_i32 : rocsparse_indextype_i64,
                               ptr,
                               nullptr,
                               0,
                               A->col_type,
                               A->const_col_data,
                               A->col_data,
                               A->columns_values_batch_stride,
                               A->descr->base,
                               A->descr,
                               A->info);

    RETURN_IF_ROCSPARSE_ERROR((rocsparse::csrsm_buffer_size(
        handle, trans_A, trans_B, alpha_datatype, alpha_stride, &csr, B, policy, buffer_size)));
    return rocsparse_status_success;
}
