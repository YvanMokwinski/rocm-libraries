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

#pragma once

#include "hipdss_control.hpp"

#include "../spic0/hipdss_spic0.hpp"
#include "../spildlt0/hipdss_spildlt0.hpp"
#include "../spilu0/hipdss_spilu0.hpp"

#include "../spreordering/hipdss_spreordering.hpp"
#include "../sptranspose/hipdss_sptranspose.hpp"
#include "../sptrsm/hipdss_sptrsm.hpp"
#include "hipdss_idvec_descr.hpp"

hipdssStatus_t rocsparse_status2hipdss_status(rocsparse_status that);

typedef struct _hipdss_rf_config_t* hipdss_rf_config_t;

typedef struct _hipdss_rf_descr* hipdss_rf_descr;
typedef enum
{
    hipdss_rf_stage_reordering             = 1,
    hipdss_rf_stage_symbolic_factorization = 2,
    hipdss_rf_stage_factorization          = 4,
    hipdss_rf_stage_refactorization        = 8,
    hipdss_rf_stage_solve_forward          = 16,
    hipdss_rf_stage_solve_diag             = 32,
    hipdss_rf_stage_solve_backward         = 64,
    hipdss_rf_stage_analysis               = hipdss_rf_stage_reordering | hipdss_rf_stage_symbolic_factorization,
    hipdss_rf_stage_solve                  = hipdss_rf_stage_solve_forward | hipdss_rf_stage_solve_diag | hipdss_rf_stage_solve_backward
} hipdss_rf_stage;

typedef enum
{
    hipdss_rf_factorization_alg_default,
} hipdss_rf_factorization_alg;

typedef enum
{
    hipdss_rf_solve_alg_default,
} hipdss_rf_solve_alg;

typedef enum
{
    hipdss_rf_input_data_row_permutation,
    hipdss_rf_input_data_column_permutation,
    hipdss_rf_input_data_row_scaling,
    hipdss_rf_input_data_column_scaling
} hipdss_rf_input_data;

typedef enum
{
    hipdss_rf_input_factorization_alg,
    hipdss_rf_input_solve_alg,
    hipdss_rf_input_reset_mode,
} hipdss_rf_input;

typedef enum
{
    rf_factorization_type_lu,
    rf_factorization_type_chol,
    rf_factorization_type_ldl
} rf_factorization_type;

struct _hipdss_rf_descr
{
    int64_t m_M{};
    int64_t m_N{};
    double  m_timing_reordering{};
    double  m_timing_symbolic_factorization{};
    double  m_timing_factorization{};
    double  m_timing_refactorization{};
    double  m_timing_analysis{};
    double  m_timing_solve_forward{};
    double  m_timing_solve_backward{};
    double  m_timing_solve_diag{};
    double  m_timing_solve{};

protected:
    void* m_reordering_config{};
    void* m_factorization_config{};
    void* m_symbolic_factorization_config{};
    void* m_symbolic_config{};
    void* m_scaling_config{};
    void* m_solve_config{};

    hipdss::spic0_config_t*    m_spic0_config{};
    hipdss::spildlt0_config_t* m_spildlt0_config{};
    hipdss::spilu0_config_t*   m_spilu0_config{};
    hipdss::sptrsm_config_t*   m_forward_sptrsm_config{};
    hipdss::sptrsm_config_t*   m_backward_sptrsm_config{};

    rocsparse_spic0_descr      m_spic0_descr{};
    rocsparse_spildlt0_descr   m_spildlt0_descr{};
    rocsparse_spilu0_descr     m_spilu0_descr{};
    rocsparse_sptrsm_descr     m_forward_sptrsm_descr{};
    rocsparse_sptrsm_descr     m_backward_sptrsm_descr{};
    hipdss::spreordering_descr m_spreordering_descr{};

    rocsparse_format      m_format{};
    rocsparse_dnvec_descr m_D_row{};
    rocsparse_dnvec_descr m_D_col{};

    hipdss::idvec_descr_t m_P_row{};
    hipdss::idvec_descr_t m_P_col{};
    hipdss::idvec_descr_t m_P_row_inverse{};
    hipdss::idvec_descr_t m_P_col_inverse{};

    hipdss::idvec_descr_t m_host_P_row{};
    hipdss::idvec_descr_t m_host_P_col{};
    hipdss::idvec_descr_t m_host_P_row_inverse{};
    hipdss::idvec_descr_t m_host_P_col_inverse{};

    hipdss::idvec_descr_t       m_A2F{};
    hipdssMatrix_t              m_permuted_rhs_matrix{};
    hipdssMatrix_t              m_permuted_solution_matrix{};
    hipdssMatrix_t              m_intermediate_solve_matrix{};
    hipdssMatrix_t              m_factorizationMatrix{};
    hipdssMatrix_t              m_At{};
    rocsparse_const_spvec_descr m_A2F_spvec{};
    rf_factorization_type       m_factorization_type{rf_factorization_type_lu};

    enum class data_t
    {
        D_left,
        D_right,
        P,
        Q,
        F_row,
        F_col,
        F_val
    };

    static constexpr int32_t ndata = 7;
    void*                    m_data[ndata]{};
    size_t                   m_data_size_in_bytes[ndata]{};
    rocsparse_dnvec_descr    F_val{};

public:
    void* get_reordering_config() const;
    void* get_factorization_config() const;
    void* get_symbolic_factorization_config() const;
    void* get_symbolic_config() const;
    void* get_scaling_config() const;
    void* get_solve_config() const;

    void set_reordering_config(void*);
    void set_factorization_config(void*);
    void set_symbolic_factorization_config(void*);
    void set_symbolic_config(void*);
    void set_scaling_config(void*);
    void set_solve_config(void*);

    hipdss::spreordering_alg get_ordering_alg() const;
    void                     set_ordering_alg(hipdss::spreordering_alg);

    hipdss::idvec_descr_t get_host_A_ptr();
    hipdss::idvec_descr_t get_host_A_ind();
    hipdss::idvec_descr_t get_host_At_ptr();
    hipdss::idvec_descr_t get_host_At_ind();

    hipdss::idvec_descr_t get_A_ptr();
    hipdss::idvec_descr_t get_A_ind();
    hipdss::idvec_descr_t get_At_ptr();
    hipdss::idvec_descr_t get_At_ind();
    rocsparse_dnvec_descr get_F_val();

    int64_t get_M() const;
    void    set_M(int64_t);

    int64_t get_N() const;
    void    set_N(int64_t);

    void                        set_F_val(rocsparse_dnvec_descr);
    void                        set_A2F_spvec(rocsparse_const_spvec_descr);
    rocsparse_const_spvec_descr get_A2F_spvec();
    rocsparse_const_spvec_descr get_A2F_spvec(rocsparse_const_spmat_descr A);

    hipdss::spreordering_descr get_spreordering_descr();
    void                       set_spreordering_descr(hipdss::spreordering_descr);

    rocsparse_spic0_descr get_spic0_descr();
    void                  set_spic0_descr(rocsparse_spic0_descr);

    rocsparse_spildlt0_descr get_spildlt0_descr();
    void                     set_spildlt0_descr(rocsparse_spildlt0_descr);

    rocsparse_spilu0_descr get_spilu0_descr();
    void                   set_spilu0_descr(rocsparse_spilu0_descr);

    rocsparse_sptrsm_descr get_forward_sptrsm_descr();
    void                   set_forward_sptrsm_descr(rocsparse_sptrsm_descr);

    rocsparse_sptrsm_descr get_backward_sptrsm_descr();
    void                   set_backward_sptrsm_descr(rocsparse_sptrsm_descr);

    hipdss::idvec_descr_t get_A2F();

    hipdss::spic0_config_t* get_spic0_config();
    void                    set_spic0_config(hipdss::spic0_config_t*);

    hipdss::spildlt0_config_t* get_spildlt0_config();
    void                       set_spildlt0_config(hipdss::spildlt0_config_t*);

    hipdss::spilu0_config_t* get_spilu0_config();
    void                     set_spilu0_config(hipdss::spilu0_config_t*);

    hipdss::sptrsm_config_t* get_forward_sptrsm_config();
    void                     set_forward_sptrsm_config(hipdss::sptrsm_config_t*);

    hipdss::sptrsm_config_t* get_backward_sptrsm_config();
    void                     set_backward_sptrsm_config(hipdss::sptrsm_config_t*);

    rocsparse_dnvec_descr get_DR();
    rocsparse_dnvec_descr get_DC();

    hipdss::idvec_descr_t get_host_Q();
    void                  set_host_Q(hipdss::idvec_descr_t);

    hipdss::idvec_descr_t get_host_P();
    void                  set_host_P(hipdss::idvec_descr_t);

    hipdss::idvec_descr_t get_Q();
    void                  set_Q(hipdss::idvec_descr_t);

    hipdss::idvec_descr_t get_P();
    void                  set_P(hipdss::idvec_descr_t);

    hipdss::idvec_descr_t get_P_inverse();
    void                  set_P_inverse(hipdss::idvec_descr_t);

    hipdss::idvec_descr_t get_host_P_inverse();
    void                  set_host_P_inverse(hipdss::idvec_descr_t);

    hipdss::idvec_descr_t get_Q_inverse();
    void                  set_Q_inverse(hipdss::idvec_descr_t);

    hipdssMatrix_t get_permuted_rhs_matrix();
    void           set_permuted_rhs_matrix(hipdssMatrix_t);

    hipdssMatrix_t get_permuted_solution_matrix();
    void           set_permuted_solution_matrix(hipdssMatrix_t);

    hipdssMatrix_t get_factorizationMatrix();

    hipdssMatrix_t get_At();
    void           set_At(hipdssMatrix_t);

    hipdssMatrix_t get_intermediate_solve_matrix();
    void           set_intermediate_solve_matrix(hipdssMatrix_t);

    void set_A2F(hipdss::idvec_descr_t);

    void set_factorizationMatrix(hipdssMatrix_t);

    rf_factorization_type get_factorization_type() const;
    void                  set_factorization_type(rf_factorization_type);

    void           set_timing(hipdss_rf_stage, double);
    hipdssStatus_t create_P_inverse(hipdss::idvec_descr_t*);
    hipdssStatus_t create_Q_inverse(hipdss::idvec_descr_t*);
    hipdssStatus_t create_P(hipdss::idvec_descr_t*);
    hipdssStatus_t create_Q(hipdss::idvec_descr_t*);

    hipdssStatus_t destroy(hipdssHandle_t);
};

hipdssStatus_t hipdss_rf_descr_create(hipdssHandle_t   handle,
                                      hipdss_rf_descr* p_rf_descr);

#if 0
hipdssStatus_t hipdss_rf_descr_create_from_config(hipdssHandle_t   handle,
                                                  hipdss_rf_descr*   p_rf_descr,
                                                  hipdss_rf_config_t rf_config,
                                                  rocsparse_error*   p_error);


hipdssStatus_t hipdss_rf_config_create_cmdline(hipdssHandle_t    handle,
                                               hipdss_rf_config_t* p_config,
                                               int32_t*            argc,
                                               const char**        argv,
                                               rocsparse_error*    p_error);
#endif

hipdssStatus_t hipdss_rf_info(hipdssHandle_t  handle,
                              hipdss_rf_descr descr,
                              FILE*           out);

hipdssStatus_t hipdss_rf_descr_destroy(hipdssHandle_t  handle,
                                       hipdss_rf_descr rf_descr);

hipdssStatus_t hipdss_rf_set_config(hipdssHandle_t  handle,
                                    hipdss_rf_descr rf_descr,
                                    hipdss_rf_stage stage,
                                    void*           config);

hipdssStatus_t hipdss_rf_set_input(hipdssHandle_t  handle,
                                   hipdss_rf_descr p_rf_descr,
                                   hipdss_rf_input rf_input,
                                   const void*     data,
                                   size_t          data_size_in_bytes);

namespace hipdss
{

    hipdssStatus_t rf_scaling(hipdssHandle_t        handle,
                              hipdss_rf_descr       rf_descr,
                              rocsparse_spmat_descr A);

    hipdssStatus_t rf_reordering(hipdssHandle_t  handle,
                                 hipdss_rf_descr rf_descr,
                                 hipdssConfig_t  solverConfig,
                                 hipdssData_t    solverData,
                                 hipdssMatrix_t  A);

    hipdssStatus_t rf_symbolic_factorization(hipdssHandle_t  handle,
                                             hipdss_rf_descr rf_descr,
                                             hipdssConfig_t  solverConfig,
                                             hipdssData_t    solverData,
                                             hipdssMatrix_t  A);

    hipdssStatus_t rf_factorization(hipdssHandle_t  handle,
                                    hipdss_rf_descr rf_descr,
                                    hipdssConfig_t  solverConfig,
                                    hipdssData_t    solverData,
                                    hipdssMatrix_t  inputMatrix);

    hipdssStatus_t rf_refactorization(hipdssHandle_t  handle,
                                      hipdss_rf_descr rf_descr,
                                      hipdssConfig_t  solverConfig,
                                      hipdssData_t    solverData,
                                      hipdssMatrix_t  inputMatrix);

    hipdssStatus_t rf_solve_forward(hipdssHandle_t  handle,
                                    hipdss_rf_descr rf_descr,
                                    hipdssMatrix_t  rhs,
                                    hipdssMatrix_t  x);

    hipdssStatus_t rf_solve_diagonal(hipdssHandle_t  handle,
                                     hipdss_rf_descr rf_descr,
                                     hipdssMatrix_t  rhs,
                                     hipdssMatrix_t  x);

    hipdssStatus_t rf_solve_backward(hipdssHandle_t  handle,
                                     hipdss_rf_descr rf_descr,
                                     hipdssMatrix_t  rhs,
                                     hipdssMatrix_t  x);

    hipdssStatus_t rf_solve(hipdssHandle_t  handle,
                            hipdss_rf_descr rf_descr,
                            hipdssMatrix_t  rhs,
                            hipdssMatrix_t  x);

}
