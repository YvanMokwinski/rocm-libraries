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
#include "internal/level3/rocsparse_csrsm.h"
#include "rocsparse_csrsm_strided_batched.hpp"

#include "../level2/rocsparse_csrsv.hpp"
#include "rocsparse_common.hpp"
#include "rocsparse_control.hpp"
#include "rocsparse_utility.hpp"

rocsparse_status rocsparse::csrsm_analysis(rocsparse_handle            handle,
                                           rocsparse_operation         op_A,
                                           rocsparse_operation         op_B,
                                           rocsparse_datatype          alpha_datatype,
                                           int64_t                     alpha_stride,
                                           rocsparse_const_spmat_descr A,
                                           rocsparse_const_dnmat_descr B,
                                           rocsparse_analysis_policy   analysis,
                                           rocsparse_solve_policy      solve,
                                           rocsparse_csrsm_info*       p_csrsm_info,
                                           void*                       buffer)
{
    ROCSPARSE_ROUTINE_TRACE;
    const int64_t nrhs = (op_B == rocsparse_operation_none) ? B->cols : B->rows;
    if(A->rows == 0 || nrhs == 0 || A->batch_count == 0)
    {
        return rocsparse_status_success;
    }

    rocsparse_mat_descr descr = A->descr;
    ROCSPARSE_CHECKARG(
        5, descr, (descr->type != rocsparse_matrix_type_general), rocsparse_status_not_implemented);
    ROCSPARSE_CHECKARG(5,
                       descr,
                       (descr->storage_mode != rocsparse_storage_mode_sorted),
                       rocsparse_status_requires_sorted_storage);

    if(nrhs == 1)
    {
        //
        // Call csrsv.
        //
        RETURN_IF_ROCSPARSE_ERROR(
            rocsparse::csrsv_analysis(handle, op_A, A, analysis, solve, p_csrsm_info, buffer));
        return rocsparse_status_success;
    }

    auto csrsm_info = p_csrsm_info[0];

    // Differentiate the analysis policies
    if(analysis == rocsparse_analysis_policy_reuse)
    {
        auto info = A->info;
        //
        //
        //
        rocsparse::trm_info_t* p = nullptr;

        p = (p != nullptr) ? p : info->get_csrsm_info(op_A, descr->fill_mode);

        if((descr->fill_mode == rocsparse_fill_mode_lower) && (op_A == rocsparse_operation_none))
        {
            p = (p != nullptr) ? p : info->get_csrilu0_info(op_A, descr->fill_mode);
            p = (p != nullptr) ? p : info->get_csric0_info(op_A, descr->fill_mode);
        }

        p = (p != nullptr) ? p : info->get_csrsv_info(op_A, descr->fill_mode);
        if(p != nullptr)
        {
            info->set_csrsm_info(op_A, descr->fill_mode, p);
            return rocsparse_status_success;
        }
    }

    if(csrsm_info == nullptr)
    {
        csrsm_info      = new _rocsparse_csrsm_info();
        p_csrsm_info[0] = csrsm_info;
    }

    // Perform analysis
    RETURN_IF_ROCSPARSE_ERROR(csrsm_info->recreate(handle,
                                                   op_A,
                                                   A->rows,
                                                   A->nnz,
                                                   A->descr,
                                                   A->data_type,
                                                   A->val_data,
                                                   A->row_type,
                                                   A->row_data,
                                                   A->col_type,
                                                   A->col_data,
                                                   buffer));
    return rocsparse_status_success;
}
