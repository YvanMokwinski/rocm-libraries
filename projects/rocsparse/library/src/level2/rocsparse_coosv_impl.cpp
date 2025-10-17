/*! \file */
/* ************************************************************************
 * Copyright (C) 2021-2025 Advanced Micro Devices, Inc. All rights Reserved.
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
#include "rocsparse_coosv.hpp"
#include "rocsparse_csrsv.hpp"
#include "rocsparse_utility.hpp"

#include "../conversion/rocsparse_coo2csr.hpp"

rocsparse_status rocsparse::coosv_solve_buffer_size(rocsparse_handle          handle,
                                                    rocsparse_operation       trans,
                                                    int64_t                   m,
                                                    int64_t                   nnz,
                                                    const rocsparse_mat_descr descr,
                                                    rocsparse_datatype        coo_val_datatype,
                                                    const void*               coo_val,
                                                    rocsparse_indextype       coo_row_ind_indextype,
                                                    const void*               coo_row_ind,
                                                    rocsparse_indextype       coo_col_ind_indextype,
                                                    const void*               coo_col_ind,
                                                    rocsparse_mat_info        info,
                                                    size_t*                   buffer_size)
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_ENUM(1, trans);
    ROCSPARSE_CHECKARG_SIZE(2, m);
    ROCSPARSE_CHECKARG_SIZE(3, nnz);
    ROCSPARSE_CHECKARG_POINTER(4, descr);

    ROCSPARSE_CHECKARG_ARRAY(5, nnz, coo_val);
    ROCSPARSE_CHECKARG_ARRAY(6, nnz, coo_row_ind);
    ROCSPARSE_CHECKARG_ARRAY(7, nnz, coo_col_ind);
    ROCSPARSE_CHECKARG_POINTER(8, info);
    ROCSPARSE_CHECKARG_POINTER(9, buffer_size);

    // Quick return if possible
    if(m == 0)
    {
        *buffer_size = 0;
        return rocsparse_status_success;
    }

    ROCSPARSE_CHECKARG(4,
                       descr,
                       (descr->type != rocsparse_matrix_type_general
                        && descr->type != rocsparse_matrix_type_triangular),
                       rocsparse_status_not_implemented);
    ROCSPARSE_CHECKARG(4,
                       descr,
                       (descr->storage_mode != rocsparse_storage_mode_sorted),
                       rocsparse_status_requires_sorted_storage);

    *buffer_size                     = 0;
    const bool                use_32 = (nnz < std::numeric_limits<int32_t>::max());
    const rocsparse_indextype indextype
        = (use_32) ? rocsparse_indextype_i32 : rocsparse_indextype_i64;
    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsv_solve_buffer_size(handle,
                                                                 trans,
                                                                 m,
                                                                 nnz,
                                                                 descr,
                                                                 coo_val_datatype,
                                                                 coo_val,
                                                                 indextype,
                                                                 (const void*)0x4,
                                                                 indextype,
                                                                 coo_col_ind,
                                                                 info,
                                                                 buffer_size));

    return rocsparse_status_success;
}

rocsparse_status rocsparse::coosv_analysis(rocsparse_handle          handle,
                                           rocsparse_operation       trans,
                                           int64_t                   m,
                                           int64_t                   nnz,
                                           const rocsparse_mat_descr descr,
                                           rocsparse_datatype        coo_val_datatype,
                                           const void*               coo_val,
                                           rocsparse_indextype       coo_row_ind_indextype,
                                           const void*               coo_row_ind,
                                           rocsparse_indextype       coo_col_ind_indextype,
                                           const void*               coo_col_ind,
                                           rocsparse_mat_info        info,
                                           rocsparse_analysis_policy analysis,
                                           rocsparse_solve_policy    solve,
                                           rocsparse_csrsv_info*     p_csrsv_info,
                                           void*                     temp_buffer)
{
    ROCSPARSE_ROUTINE_TRACE;

    // Check for valid handle and matrix descriptor
    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_ENUM(1, trans);
    ROCSPARSE_CHECKARG_SIZE(2, m);
    ROCSPARSE_CHECKARG_SIZE(3, nnz);
    ROCSPARSE_CHECKARG_POINTER(4, descr);
    ROCSPARSE_CHECKARG_POINTER(8, info);
    ROCSPARSE_CHECKARG_ENUM(9, analysis);
    ROCSPARSE_CHECKARG_ENUM(10, solve);
    ROCSPARSE_CHECKARG_ARRAY(5, nnz, coo_val);
    ROCSPARSE_CHECKARG_ARRAY(6, nnz, coo_row_ind);
    ROCSPARSE_CHECKARG_ARRAY(7, nnz, coo_col_ind);
    ROCSPARSE_CHECKARG_ARRAY(11, (m > 0) ? 1 : 0, temp_buffer);

    // Check sizes

    // Check matrix type
    ROCSPARSE_CHECKARG(4,
                       descr,
                       (descr->type != rocsparse_matrix_type_general
                        && descr->type != rocsparse_matrix_type_triangular),
                       rocsparse_status_not_implemented);
    // Check matrix sorting mode
    ROCSPARSE_CHECKARG(4,
                       descr,
                       (descr->storage_mode != rocsparse_storage_mode_sorted),
                       rocsparse_status_requires_sorted_storage);
    // Quick return if possible
    if(m == 0)
    {
        return rocsparse_status_success;
    }

    // All must be null (zero matrix) or none null
    const bool                use_32 = nnz < std::numeric_limits<int32_t>::max();
    const rocsparse_indextype indextype
        = use_32 ? rocsparse_indextype_i32 : rocsparse_indextype_i64;

    // Buffer
    rocsparse::sorted_coo2csr_info_t* sorted_coo2csr_info = info->get_sorted_coo2csr_info();
    if(sorted_coo2csr_info == nullptr)
    {
        sorted_coo2csr_info = new rocsparse::sorted_coo2csr_info_t(m, indextype, handle->stream);

        //
        // Assign it first, because if an error occurs in calculate below, then we won't have a memory leak.
        //
        info->set_sorted_coo2csr_info(sorted_coo2csr_info);

        RETURN_IF_ROCSPARSE_ERROR(sorted_coo2csr_info->calculate(
            handle, nnz, coo_row_ind, coo_row_ind_indextype, descr->base));
    }

    const rocsparse_datatype  csr_val_datatype = coo_val_datatype;
    const void*               csr_val          = coo_val;
    const rocsparse_indextype csr_row_ptr_indextype
        = (use_32) ? rocsparse_indextype_i32 : rocsparse_indextype_i64;
    const void*               csr_row_ptr           = sorted_coo2csr_info->get_row_ptr();
    const rocsparse_indextype csr_col_ind_indextype = coo_col_ind_indextype;
    const void*               csr_col_ind           = coo_col_ind;
    RETURN_IF_ROCSPARSE_ERROR((rocsparse::csrsv_analysis(handle,
                                                         trans,
                                                         m,
                                                         nnz,
                                                         descr,
                                                         csr_val_datatype,
                                                         csr_val,
                                                         csr_row_ptr_indextype,
                                                         csr_row_ptr,
                                                         csr_col_ind_indextype,
                                                         csr_col_ind,
                                                         info,
                                                         analysis,
                                                         solve,
                                                         p_csrsv_info,
                                                         temp_buffer)));

    return rocsparse_status_success;
}

rocsparse_status rocsparse::coosv_solve(rocsparse_handle          handle,
                                        rocsparse_operation       trans,
                                        int64_t                   m,
                                        int64_t                   nnz,
                                        rocsparse_datatype        alpha_datatype,
                                        const void*               alpha,
                                        const rocsparse_mat_descr descr,
                                        rocsparse_datatype        coo_val_datatype,
                                        const void*               coo_val,
                                        rocsparse_indextype       coo_row_ind_indextype,
                                        const void*               coo_row_ind,
                                        rocsparse_indextype       coo_col_ind_indextype,
                                        const void*               coo_col_ind,
                                        rocsparse_mat_info        info,
                                        rocsparse_datatype        x_datatype,
                                        const void*               x,
                                        rocsparse_datatype        y_datatype,
                                        void*                     y,
                                        rocsparse_solve_policy    policy,
                                        rocsparse_csrsv_info      csrsv_info,
                                        void*                     temp_buffer)
{
    ROCSPARSE_ROUTINE_TRACE;

    // Check for valid handle and matrix descriptor
    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(5, descr);
    ROCSPARSE_CHECKARG_POINTER(9, info);

    ROCSPARSE_CHECKARG_ENUM(1, trans);
    ROCSPARSE_CHECKARG_ENUM(12, policy);

    // Check matrix type
    ROCSPARSE_CHECKARG(5,
                       descr,
                       (descr->type != rocsparse_matrix_type_general
                        && descr->type != rocsparse_matrix_type_triangular),
                       rocsparse_status_not_implemented);

    // Check matrix sorting mode
    ROCSPARSE_CHECKARG(5,
                       descr,
                       (descr->storage_mode != rocsparse_storage_mode_sorted),
                       rocsparse_status_requires_sorted_storage);

    // Check sizes
    ROCSPARSE_CHECKARG_SIZE(2, m);
    ROCSPARSE_CHECKARG_SIZE(3, nnz);

    // Quick return if possible
    if(m == 0)
    {
        return rocsparse_status_success;
    }

    // Check pointer arguments
    ROCSPARSE_CHECKARG_POINTER(4, alpha);
    ROCSPARSE_CHECKARG_POINTER(13, temp_buffer);
    ROCSPARSE_CHECKARG_ARRAY(10, m, x);
    ROCSPARSE_CHECKARG_ARRAY(11, m, y);

    // All must be null (zero matrix) or none null
    ROCSPARSE_CHECKARG_ARRAY(6, nnz, coo_val);
    ROCSPARSE_CHECKARG_ARRAY(7, nnz, coo_row_ind);
    ROCSPARSE_CHECKARG_ARRAY(8, nnz, coo_col_ind);

    rocsparse::sorted_coo2csr_info_t* sorted_coo2csr_info = info->get_sorted_coo2csr_info();
    if(sorted_coo2csr_info == nullptr)
    {
        RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
            rocsparse_status_internal_error,
            "sorted_coo2csr_info is not available, it looks like the analysis phase of this "
            "algorithm was not previously executed.");
    }

    const bool                use_32           = nnz < std::numeric_limits<int32_t>::max();
    const rocsparse_datatype  csr_val_datatype = coo_val_datatype;
    const void*               csr_val          = coo_val;
    const rocsparse_indextype csr_row_ptr_indextype
        = (use_32) ? rocsparse_indextype_i32 : rocsparse_indextype_i64;
    const void*               csr_row_ptr           = sorted_coo2csr_info->get_row_ptr();
    const rocsparse_indextype csr_col_ind_indextype = coo_col_ind_indextype;
    const void*               csr_col_ind           = coo_col_ind;
    RETURN_IF_ROCSPARSE_ERROR((rocsparse::csrsv_solve(handle,
                                                      trans,
                                                      m,
                                                      nnz,
                                                      alpha_datatype,
                                                      alpha,
                                                      descr,
                                                      csr_val_datatype,
                                                      csr_val,
                                                      csr_row_ptr_indextype,
                                                      csr_row_ptr,
                                                      csr_col_ind_indextype,
                                                      csr_col_ind,
                                                      info,
                                                      x_datatype,
                                                      x,
                                                      static_cast<int64_t>(1),
                                                      y_datatype,
                                                      y,
                                                      policy,
                                                      csrsv_info,
                                                      temp_buffer)));

    return rocsparse_status_success;
}
