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

#include "hipdss_rf_descr.hpp"
#include "hipdssContext.hpp"
#include <rocsparse/rocsparse-auxiliary.h>

hipdssStatus_t rocsparse_status2hipdss_status(rocsparse_status that)
{
    switch(that)
    {

    case rocsparse_status_success:
    {
        return HIPDSS_STATUS_SUCCESS;
    }

    case rocsparse_status_invalid_handle:
    case rocsparse_status_not_initialized:
    {
        return HIPDSS_STATUS_NOT_INITIALIZED;
    }

    case rocsparse_status_memory_error:
    {
        return HIPDSS_STATUS_ALLOC_FAILED;
    }

    case rocsparse_status_invalid_pointer:
    case rocsparse_status_invalid_size:
    case rocsparse_status_invalid_value:
    case rocsparse_status_type_mismatch:
    case rocsparse_status_requires_sorted_storage:
    {
        return HIPDSS_STATUS_INVALID_VALUE;
    }

    case rocsparse_status_not_implemented:
    case rocsparse_status_arch_mismatch:
    {
        return HIPDSS_STATUS_NOT_SUPPORTED;
    }

    case rocsparse_status_zero_pivot:
    {
        return HIPDSS_STATUS_EXECUTION_FAILED;
    }

    case rocsparse_status_thrown_exception:
    case rocsparse_status_internal_error:
    case rocsparse_status_continue:
    {
        return HIPDSS_STATUS_INTERNAL_ERROR;
    }
    }
    return HIPDSS_STATUS_INTERNAL_ERROR;
}

void* _hipdss_rf_descr::get_reordering_config() const
{
    return this->m_reordering_config;
}

hipdssMatrix_t _hipdss_rf_descr::get_factorizationMatrix()
{
    return this->m_factorizationMatrix;
}

void _hipdss_rf_descr::set_factorizationMatrix(hipdssMatrix_t pointer)
{
    this->m_factorizationMatrix = pointer;
}

void* _hipdss_rf_descr::get_factorization_config() const
{
    return this->m_factorization_config;
}

void* _hipdss_rf_descr::get_symbolic_factorization_config() const
{
    return this->m_symbolic_factorization_config;
}
void* _hipdss_rf_descr::get_symbolic_config() const
{
    return this->m_symbolic_config;
}
void* _hipdss_rf_descr::get_scaling_config() const
{
    return this->m_scaling_config;
}
void* _hipdss_rf_descr::get_solve_config() const
{
    return this->m_solve_config;
}

void _hipdss_rf_descr::set_reordering_config(void* pointer)
{
    this->m_reordering_config = pointer;
}
void _hipdss_rf_descr::set_factorization_config(void* pointer)
{
    this->m_factorization_config = pointer;
}
void _hipdss_rf_descr::set_symbolic_factorization_config(void* pointer)
{
    this->m_symbolic_factorization_config = pointer;
}
void _hipdss_rf_descr::set_symbolic_config(void* pointer)
{
    this->m_symbolic_config = pointer;
}
void _hipdss_rf_descr::set_scaling_config(void* pointer)
{
    this->m_scaling_config = pointer;
}
void _hipdss_rf_descr::set_solve_config(void* pointer)
{
    this->m_solve_config = pointer;
}

rocsparse_dnvec_descr _hipdss_rf_descr::get_F_val()
{
    return this->F_val;
}

void _hipdss_rf_descr::set_F_val(rocsparse_dnvec_descr that)
{
    this->F_val = that;
}

void _hipdss_rf_descr::set_A2F_spvec(rocsparse_const_spvec_descr that)
{
    this->m_A2F_spvec = that;
}

rocsparse_const_spvec_descr _hipdss_rf_descr::get_A2F_spvec()
{
    return this->m_A2F_spvec;
}

rocsparse_const_spvec_descr
    _hipdss_rf_descr::get_A2F_spvec(rocsparse_const_spmat_descr A)
{
    const void* values{};
    rocsparse_const_spmat_get_values(A, &values);
    rocsparse_spvec_set_values((rocsparse_spvec_descr)this->m_A2F_spvec, (void*)values);
    return this->m_A2F_spvec;
}

hipdss::spreordering_descr _hipdss_rf_descr::get_spreordering_descr()
{
    return this->m_spreordering_descr;
}
void _hipdss_rf_descr::set_spreordering_descr(hipdss::spreordering_descr that)
{
    this->m_spreordering_descr = that;
}

rocsparse_spildlt0_descr _hipdss_rf_descr::get_spildlt0_descr()
{
    return this->m_spildlt0_descr;
}

rocsparse_spic0_descr _hipdss_rf_descr::get_spic0_descr()
{
    return this->m_spic0_descr;
}

void _hipdss_rf_descr::set_spic0_descr(rocsparse_spic0_descr that)
{
    this->m_spic0_descr = that;
}

void _hipdss_rf_descr::set_spildlt0_descr(rocsparse_spildlt0_descr that)
{
    this->m_spildlt0_descr = that;
}

rocsparse_spilu0_descr _hipdss_rf_descr::get_spilu0_descr()
{
    return this->m_spilu0_descr;
}
void _hipdss_rf_descr::set_spilu0_descr(rocsparse_spilu0_descr that)
{
    this->m_spilu0_descr = that;
}

rocsparse_sptrsm_descr _hipdss_rf_descr::get_forward_sptrsm_descr()
{
    return this->m_forward_sptrsm_descr;
}
void _hipdss_rf_descr::set_forward_sptrsm_descr(rocsparse_sptrsm_descr that)
{
    this->m_forward_sptrsm_descr = that;
}

rocsparse_sptrsm_descr _hipdss_rf_descr::get_backward_sptrsm_descr()
{
    return this->m_backward_sptrsm_descr;
}
void _hipdss_rf_descr::set_backward_sptrsm_descr(rocsparse_sptrsm_descr that)
{
    this->m_backward_sptrsm_descr = that;
}

hipdss::idvec_descr_t _hipdss_rf_descr::get_A2F()
{
    return this->m_A2F;
}

hipdss::spic0_config_t* _hipdss_rf_descr::get_spic0_config()
{
    return this->m_spic0_config;
}

hipdss::spilu0_config_t* _hipdss_rf_descr::get_spilu0_config()
{
    return this->m_spilu0_config;
}

hipdss::sptrsm_config_t* _hipdss_rf_descr::get_forward_sptrsm_config()
{
    return this->m_forward_sptrsm_config;
}

hipdss::sptrsm_config_t* _hipdss_rf_descr::get_backward_sptrsm_config()
{
    return this->m_backward_sptrsm_config;
}

void _hipdss_rf_descr::set_spic0_config(hipdss::spic0_config_t* that)
{
    this->m_spic0_config = that;
}

void _hipdss_rf_descr::set_spildlt0_config(hipdss::spildlt0_config_t* that)
{
    this->m_spildlt0_config = that;
}

void _hipdss_rf_descr::set_spilu0_config(hipdss::spilu0_config_t* that)
{
    this->m_spilu0_config = that;
}

void _hipdss_rf_descr::set_forward_sptrsm_config(hipdss::sptrsm_config_t* that)
{
    this->m_forward_sptrsm_config = that;
}

void _hipdss_rf_descr::set_backward_sptrsm_config(hipdss::sptrsm_config_t* that)
{
    this->m_backward_sptrsm_config = that;
}

rocsparse_dnvec_descr _hipdss_rf_descr::get_DR()
{
    return this->m_D_row;
}
rocsparse_dnvec_descr _hipdss_rf_descr::get_DC()
{
    return this->m_D_col;
}

hipdss::idvec_descr_t _hipdss_rf_descr::get_Q()
{
    return this->m_P_col;
}
void _hipdss_rf_descr::set_Q(hipdss::idvec_descr_t that)
{
    this->m_P_col = that;
}

hipdss::idvec_descr_t _hipdss_rf_descr::get_P()
{
    return this->m_P_row;
}
void _hipdss_rf_descr::set_P(hipdss::idvec_descr_t that)
{
    this->m_P_row = that;
}

hipdss::idvec_descr_t _hipdss_rf_descr::get_P_inverse()
{
    return this->m_P_row_inverse;
}
void _hipdss_rf_descr::set_P_inverse(hipdss::idvec_descr_t that)
{
    this->m_P_row_inverse = that;
}

hipdss::idvec_descr_t _hipdss_rf_descr::get_Q_inverse()
{
    return this->m_P_col_inverse;
}
void _hipdss_rf_descr::set_Q_inverse(hipdss::idvec_descr_t that)
{
    this->m_P_col_inverse = that;
}

hipdssMatrix_t _hipdss_rf_descr::get_permuted_rhs_matrix()
{
    return this->m_permuted_rhs_matrix;
}

void _hipdss_rf_descr::set_permuted_rhs_matrix(hipdssMatrix_t that)
{
    this->m_permuted_rhs_matrix = that;
}

hipdssMatrix_t _hipdss_rf_descr::get_permuted_solution_matrix()
{
    return this->m_permuted_solution_matrix;
}
void _hipdss_rf_descr::set_permuted_solution_matrix(hipdssMatrix_t that)
{
    this->m_permuted_solution_matrix = that;
}

//

//

hipdssMatrix_t _hipdss_rf_descr::get_At()
{
    return this->m_At;
}

void _hipdss_rf_descr::set_At(hipdssMatrix_t p)
{
    this->m_At = p;
}
//
hipdssMatrix_t _hipdss_rf_descr::get_intermediate_solve_matrix()
{
    return this->m_intermediate_solve_matrix;
}

void _hipdss_rf_descr::set_intermediate_solve_matrix(hipdssMatrix_t that)
{
    this->m_intermediate_solve_matrix = that;
}
//
void _hipdss_rf_descr::set_timing(hipdss_rf_stage stage, double value)
{
    switch(stage)
    {
    case hipdss_rf_stage_reordering:
    {
        this->m_timing_reordering = value;
        break;
    }

    case hipdss_rf_stage_symbolic_factorization:
    {
        this->m_timing_symbolic_factorization = value;
        break;
    }

    case hipdss_rf_stage_analysis:
    {
        this->m_timing_analysis = value;
        break;
    }

    case hipdss_rf_stage_factorization:
    {
        this->m_timing_factorization = value;
        break;
    }

    case hipdss_rf_stage_refactorization:
    {
        this->m_timing_refactorization = value;
        break;
    }

    case hipdss_rf_stage_solve_diag:
    {
        this->m_timing_solve_diag = value;
        break;
    }

    case hipdss_rf_stage_solve_backward:
    {
        this->m_timing_solve_backward = value;
        break;
    }

    case hipdss_rf_stage_solve_forward:
    {
        this->m_timing_solve_forward = value;
        break;
    }

    case hipdss_rf_stage_solve:
    {
        this->m_timing_solve = value;
        break;
    }
    }
}

void _hipdss_rf_descr::set_factorization_type(rf_factorization_type value)
{
    this->m_factorization_type = value;
}

rf_factorization_type _hipdss_rf_descr::get_factorization_type() const
{
    return this->m_factorization_type;
}

void _hipdss_rf_descr::set_A2F(hipdss::idvec_descr_t that)
{
    this->m_A2F = that;
}

hipdssStatus_t _hipdss_rf_descr::destroy(hipdssHandle_t handle)
{
    for(int i = 0; i < this->ndata; ++i)
    {
        RETURN_IF_HIP_ERROR(handle->device_free(this->m_data[i]));
        this->m_data[i] = nullptr;
    }
    return HIPDSS_STATUS_SUCCESS;
}
