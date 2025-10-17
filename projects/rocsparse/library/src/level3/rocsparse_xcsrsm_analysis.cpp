/*! \file */
/* ************************************************************************
 * Copyright (C) 2020-2025 Advanced Micro Devices, Inc. All rights Reserved.
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
#include "rocsparse_csrsm.hpp"

#include "../level2/rocsparse_csrsv.hpp"
#include "rocsparse_common.hpp"
#include "rocsparse_control.hpp"
#include "rocsparse_utility.hpp"

namespace rocsparse
{
    static rocsparse_status xcsrsm_analysis_checkarg(rocsparse_handle          handle, //0
                                                     rocsparse_operation       trans_A, //1
                                                     rocsparse_operation       trans_B, //2
                                                     int64_t                   m, //3
                                                     int64_t                   nrhs, //4
                                                     int64_t                   nnz, //5
                                                     const void*               alpha, //6
                                                     const rocsparse_mat_descr descr, //7
                                                     const void*               csr_val, //8
                                                     const void*               csr_row_ptr, //9
                                                     const void*               csr_col_ind, //10
                                                     const void*               B, //11
                                                     int64_t                   ldb, //12
                                                     rocsparse_mat_info        info, //13
                                                     rocsparse_analysis_policy analysis, //14
                                                     rocsparse_solve_policy    solve, //15
                                                     void*                     temp_buffer) //16
    {
        ROCSPARSE_ROUTINE_TRACE;

        ROCSPARSE_CHECKARG_HANDLE(0, handle);
        ROCSPARSE_CHECKARG_ENUM(1, trans_A);
        ROCSPARSE_CHECKARG_ENUM(2, trans_B);
        ROCSPARSE_CHECKARG_SIZE(3, m);
        ROCSPARSE_CHECKARG_SIZE(4, nrhs);
        ROCSPARSE_CHECKARG_SIZE(5, nnz);
        ROCSPARSE_CHECKARG(12,
                           ldb,
                           (trans_B == rocsparse_operation_none && ldb < m),
                           rocsparse_status_invalid_size);
        ROCSPARSE_CHECKARG(12,
                           ldb,
                           ((trans_B == rocsparse_operation_transpose
                             || trans_B == rocsparse_operation_conjugate_transpose)
                            && ldb < nrhs),
                           rocsparse_status_invalid_size);

        ROCSPARSE_CHECKARG_POINTER(7, descr);
        ROCSPARSE_CHECKARG_ARRAY(8, nnz, csr_val);
        ROCSPARSE_CHECKARG_ARRAY(9, m, csr_row_ptr);
        ROCSPARSE_CHECKARG_ARRAY(10, nnz, csr_col_ind);
        ROCSPARSE_CHECKARG_POINTER(13, info);
        ROCSPARSE_CHECKARG_ENUM(14, analysis);
        ROCSPARSE_CHECKARG_ENUM(15, solve);
        ROCSPARSE_CHECKARG_POINTER(6, alpha);
        ROCSPARSE_CHECKARG_POINTER(11, B);
        ROCSPARSE_CHECKARG_POINTER(16, temp_buffer);
        return rocsparse_status_success;
    }

    template <typename T>
    static rocsparse_status xcsrsm_analysis(rocsparse_handle          handle,
                                            rocsparse_operation       trans_A,
                                            rocsparse_operation       trans_B,
                                            rocsparse_int             m,
                                            rocsparse_int             nrhs,
                                            rocsparse_int             nnz,
                                            const T*                  alpha,
                                            const rocsparse_mat_descr descr,
                                            const T*                  csr_val,
                                            const rocsparse_int*      csr_row_ptr,
                                            const rocsparse_int*      csr_col_ind,
                                            const T*                  B,
                                            int64_t                   ldb,
                                            rocsparse_mat_info        info,
                                            rocsparse_analysis_policy analysis,
                                            rocsparse_solve_policy    solve,
                                            void*                     temp_buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;

        RETURN_IF_ROCSPARSE_ERROR(rocsparse::xcsrsm_analysis_checkarg(handle,
                                                                      trans_A,
                                                                      trans_B,
                                                                      m,
                                                                      nrhs,
                                                                      nnz,
                                                                      alpha,
                                                                      descr,
                                                                      csr_val,
                                                                      csr_row_ptr,
                                                                      csr_col_ind,
                                                                      B,
                                                                      ldb,
                                                                      info,
                                                                      analysis,
                                                                      solve,
                                                                      temp_buffer));
        rocsparse_csrsm_info csrsm_info = info->get_csrsm_info();

        _rocsparse_spmat_descr csr(rocsparse_format_csr,
                                   false,
                                   static_cast<int64_t>(1),
                                   m,
                                   m,
                                   nnz,
                                   rocsparse::get_datatype<T>(),
                                   csr_val,
                                   nullptr,
                                   static_cast<int64_t>(0),
                                   rocsparse::get_indextype<rocsparse_int>(),
                                   csr_row_ptr,
                                   nullptr,
                                   static_cast<int64_t>(0),
                                   rocsparse::get_indextype<rocsparse_int>(),
                                   csr_col_ind,
                                   nullptr,
                                   static_cast<int64_t>(0),
                                   descr->base,
                                   descr,
                                   info);

        _rocsparse_dnmat_descr dnmat(static_cast<int64_t>(1),
                                     (trans_B == rocsparse_operation_none) ? m : nrhs,
                                     (trans_B == rocsparse_operation_none) ? nrhs : m,
                                     ldb,
                                     rocsparse_order_column,
                                     rocsparse::get_datatype<T>(),
                                     B,
                                     nullptr,
                                     static_cast<int64_t>(0));

        RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_analysis(handle,
                                                            trans_A,
                                                            trans_B,
                                                            rocsparse::get_datatype<T>(),
                                                            static_cast<int64_t>(0),
                                                            &csr,
                                                            &dnmat,
                                                            analysis,
                                                            solve,
                                                            &csrsm_info,
                                                            temp_buffer));
        return rocsparse_status_success;
    }
}

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */
#define C_IMPL(NAME, T)                                                        \
    extern "C" rocsparse_status NAME(rocsparse_handle          handle,         \
                                     rocsparse_operation       trans_A,        \
                                     rocsparse_operation       trans_B,        \
                                     rocsparse_int             m,              \
                                     rocsparse_int             nrhs,           \
                                     rocsparse_int             nnz,            \
                                     const T*                  alpha,          \
                                     const rocsparse_mat_descr descr,          \
                                     const T*                  csr_val,        \
                                     const rocsparse_int*      csr_row_ptr,    \
                                     const rocsparse_int*      csr_col_ind,    \
                                     const T*                  B,              \
                                     rocsparse_int             ldb,            \
                                     rocsparse_mat_info        info,           \
                                     rocsparse_analysis_policy analysis,       \
                                     rocsparse_solve_policy    solve,          \
                                     void*                     temp_buffer)    \
    try                                                                        \
    {                                                                          \
        ROCSPARSE_ROUTINE_TRACE;                                               \
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::xcsrsm_analysis<T>(handle,        \
                                                                trans_A,       \
                                                                trans_B,       \
                                                                m,             \
                                                                nrhs,          \
                                                                nnz,           \
                                                                alpha,         \
                                                                descr,         \
                                                                csr_val,       \
                                                                csr_row_ptr,   \
                                                                csr_col_ind,   \
                                                                B,             \
                                                                ldb,           \
                                                                info,          \
                                                                analysis,      \
                                                                solve,         \
                                                                temp_buffer)); \
        return rocsparse_status_success;                                       \
    }                                                                          \
    catch(...)                                                                 \
    {                                                                          \
        RETURN_ROCSPARSE_EXCEPTION();                                          \
    }

C_IMPL(rocsparse_scsrsm_analysis, float);
C_IMPL(rocsparse_dcsrsm_analysis, double);
C_IMPL(rocsparse_ccsrsm_analysis, rocsparse_float_complex);
C_IMPL(rocsparse_zcsrsm_analysis, rocsparse_double_complex);

#undef C_IMPL
