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
#include "../level2/rocsparse_csrsv_strided_batched.hpp"
#include "rocsparse_common.hpp"
#include "rocsparse_control.hpp"
#include "rocsparse_utility.hpp"

rocsparse_status
    rocsparse::csrsm_strided_batched_analysis(rocsparse_handle          handle,
                                              rocsparse_operation       trans_A,
                                              rocsparse_operation       trans_B,
                                              int64_t                   batch_count,
                                              int64_t                   m,
                                              int64_t                   nrhs,
                                              int64_t                   nnz,
                                              rocsparse_datatype        alpha_datatype,
                                              const void*               alpha,
                                              int64_t                   alpha_stride,
                                              const rocsparse_mat_descr descr,
                                              rocsparse_datatype        csr_val_datatype,
                                              const void*               csr_val,
                                              int64_t                   csr_val_stride,
                                              rocsparse_indextype       csr_row_ptr_indextype,
                                              const void*               csr_row_ptr,
                                              rocsparse_indextype       csr_col_ind_indextype,
                                              const void*               csr_col_ind,
                                              rocsparse_datatype        B_datatype,
                                              const void*               B,
                                              int64_t                   ldb,
                                              int64_t                   B_stride,
                                              rocsparse_order           order_B,
                                              rocsparse_mat_info        info,
                                              rocsparse_analysis_policy analysis,
                                              rocsparse_solve_policy    solve,
                                              void*                     temp_buffer)
{
    ROCSPARSE_ROUTINE_TRACE;

    if(m == 0 || nrhs == 0 || batch_count == 0)
    {
        return rocsparse_status_success;
    }

    ROCSPARSE_CHECKARG(
        7, descr, (descr->type != rocsparse_matrix_type_general), rocsparse_status_not_implemented);
    ROCSPARSE_CHECKARG(7,
                       descr,
                       (descr->storage_mode != rocsparse_storage_mode_sorted),
                       rocsparse_status_requires_sorted_storage);

    if(nrhs == 1)
    {
        //
        // Call csrsv.
        //
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_strided_batched_analysis(handle,
                                                                            trans_A,
                                                                            batch_count,
                                                                            m,
                                                                            nnz,
                                                                            descr,
                                                                            csr_val_datatype,
                                                                            csr_val,
                                                                            csr_val_stride,
                                                                            csr_row_ptr_indextype,
                                                                            csr_row_ptr,
                                                                            csr_col_ind_indextype,
                                                                            csr_col_ind,
                                                                            info,
                                                                            analysis,
                                                                            solve,
                                                                            temp_buffer));
        return rocsparse_status_success;
    }

    // Switch between lower and upper triangular analysis
    if(descr->fill_mode == rocsparse_fill_mode_upper)
    {
        // Differentiate the analysis policies
        if(analysis == rocsparse_analysis_policy_reuse)
        {
            // We try to re-use already analyzed upper part, if available.
            // It is the user's responsibility that this data is still valid,
            // since he passed the 'reuse' flag.

            // If csrsm meta data is already available, do nothing
            if(trans_A == rocsparse_operation_none && info->csrsm_upper_info != nullptr)
            {
                return rocsparse_status_success;
            }
            else if(trans_A == rocsparse_operation_transpose && info->csrsmt_upper_info != nullptr)
            {
                return rocsparse_status_success;
            }
            else if(trans_A == rocsparse_operation_conjugate_transpose
                    && info->csrsmt_upper_info != nullptr)
            {
                return rocsparse_status_success;
            }

            // Check for other upper analysis meta data

            if(trans_A == rocsparse_operation_none && info->csrsv_upper_info != nullptr)
            {
                // csrsv meta data
                info->csrsm_upper_info = info->csrsv_upper_info;
                return rocsparse_status_success;
            }

            if(trans_A == rocsparse_operation_transpose && info->csrsvt_upper_info != nullptr)
            {
                // csrsv meta data
                info->csrsmt_upper_info = info->csrsvt_upper_info;
                return rocsparse_status_success;
            }

            if(trans_A == rocsparse_operation_conjugate_transpose
               && info->csrsvt_upper_info != nullptr)
            {
                // csrsv meta data
                info->csrsmt_upper_info = info->csrsvt_upper_info;
                return rocsparse_status_success;
            }
        }

        // User is explicitly asking to force a re-analysis, or no valid data has been
        // found to be re-used

        rocsparse::trm_info_t** p_trm_info = (trans_A == rocsparse_operation_none)
                                                 ? &info->csrsm_upper_info
                                                 : &info->csrsmt_upper_info;

        rocsparse::trm_info_t::recreate(p_trm_info);

        // Perform analysis
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::gtrm_analysis(handle,
                                                           trans_A,
                                                           m,
                                                           nnz,
                                                           descr,
                                                           csr_val_datatype,
                                                           csr_val,
                                                           csr_row_ptr_indextype,
                                                           csr_row_ptr,
                                                           csr_col_ind_indextype,
                                                           csr_col_ind,
                                                           p_trm_info[0],
                                                           &info->zero_pivot,
                                                           temp_buffer));
    }
    else
    {
        // Differentiate the analysis policies
        if(analysis == rocsparse_analysis_policy_reuse)
        {
            // We try to re-use already analyzed lower part, if available.
            // It is the user's responsibility that this data is still valid,
            // since he passed the 'reuse' flag.

            // If csrsm meta data is already available, do nothing
            if(trans_A == rocsparse_operation_none && info->csrsm_lower_info != nullptr)
            {
                return rocsparse_status_success;
            }
            else if(trans_A == rocsparse_operation_transpose && info->csrsmt_lower_info != nullptr)
            {
                return rocsparse_status_success;
            }
            else if(trans_A == rocsparse_operation_conjugate_transpose
                    && info->csrsmt_lower_info != nullptr)
            {
                return rocsparse_status_success;
            }

            // Check for other lower analysis meta data

            if(trans_A == rocsparse_operation_none && info->csrilu0_info != nullptr)
            {
                // csrilu0 meta data
                info->csrsm_lower_info = info->csrilu0_info;
                return rocsparse_status_success;
            }
            else if(trans_A == rocsparse_operation_none && info->csric0_info != nullptr)
            {
                // csric0 meta data
                info->csrsm_lower_info = info->csric0_info;
                return rocsparse_status_success;
            }
            else if(trans_A == rocsparse_operation_none && info->csrsv_lower_info != nullptr)
            {
                // csrsv meta data
                info->csrsm_lower_info = info->csrsv_lower_info;
                return rocsparse_status_success;
            }

            if(trans_A == rocsparse_operation_transpose && info->csrsvt_lower_info != nullptr)
            {
                // csrsv meta data
                info->csrsm_upper_info = info->csrsvt_lower_info;
                return rocsparse_status_success;
            }

            if(trans_A == rocsparse_operation_conjugate_transpose
               && info->csrsvt_lower_info != nullptr)
            {
                // csrsv meta data
                info->csrsm_upper_info = info->csrsvt_lower_info;
                return rocsparse_status_success;
            }
        }

        // User is explicitly asking to force a re-analysis, or no valid data has been
        // found to be re-used

        rocsparse::trm_info_t** p_trm_info = (trans_A == rocsparse_operation_none)
                                                 ? &info->csrsm_lower_info
                                                 : &info->csrsmt_lower_info;

        rocsparse::trm_info_t::recreate(p_trm_info);

        // Perform analysis
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::gtrm_analysis(handle,
                                                           trans_A,
                                                           m,
                                                           nnz,
                                                           descr,
                                                           csr_val_datatype,
                                                           csr_val,
                                                           csr_row_ptr_indextype,
                                                           csr_row_ptr,
                                                           csr_col_ind_indextype,
                                                           csr_col_ind,
                                                           p_trm_info[0],
                                                           &info->zero_pivot,
                                                           temp_buffer));
    }

    return rocsparse_status_success;
}
