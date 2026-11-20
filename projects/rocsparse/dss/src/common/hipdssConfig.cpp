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

#include "hipdssConfig.hpp"

hipdssConfig::hipdssConfig()
    : m_reordering_alg(HIPDSS_REORDERING_ALG_DEFAULT)
    , m_factorization_alg(HIPDSS_FACTORIZATION_ALG_DEFAULT)
    , m_solve_alg(HIPDSS_SOLVE_ALG_DEFAULT)
    , m_matching_alg(HIPDSS_MATCHING_ALG_NONE)
    , m_solve_mode(HIPDSS_SOLVE_MODE_NORMAL)
    , m_ir_nsteps(2)
    , m_ir_tol(0.0)
    , m_pivot_type(HIPDSS_PIVOT_AUTO)
    , m_pivot_threshold(1.0)
    , m_pivot_epsilon(0.0)
    , m_max_lu_nnz(-1)
    , m_hybrid_memory_mode(0)
    , m_hybrid_device_memory_limit(0)
    , m_use_hip_register_memory(1)
    , m_host_nthreads(0)
    , m_hybrid_execute_mode(0)
    , m_pivot_epsilon_alg(HIPDSS_PIVOT_EPSILON_ALG_DEFAULT)
    , m_nd_nlevels(10)
    , m_ubatch_size(0)
    , m_ubatch_index(-1)
    , m_use_superpanels(1)
    , m_device_count(1)
    , m_schur_mode(0)
    , m_deterministic_mode(0)
    , m_nd_ubfactor(0)
    , m_emulation_mantissa_bit_count(0)
    , m_force_nonsymmetric_matching(0)
    , m_downcast_factors(HIPDSS_DOWNCAST_FACTORS_MODE_DISABLED)
    , m_memory_optimization_setting(HIPDSS_MEMORY_OPTIMIZATION_DEFAULT)
{
}

hipdssReorderingAlg_t hipdssConfig::get_reordering_alg() const
{
    return this->m_reordering_alg;
}

hipdssFactorizationAlg_t hipdssConfig::get_factorization_alg() const
{
    return this->m_factorization_alg;
}

hipdssSolveAlg_t hipdssConfig::get_solve_alg() const
{
    return this->m_solve_alg;
}

hipdssMatchingAlg_t hipdssConfig::get_matching_alg() const
{
    return this->m_matching_alg;
}

hipdssSolveMode_t hipdssConfig::get_solve_mode() const
{
    return this->m_solve_mode;
}

int32_t hipdssConfig::get_ir_nsteps() const
{
    return this->m_ir_nsteps;
}

double hipdssConfig::get_ir_tol() const
{
    return this->m_ir_tol;
}

hipdssPivotType_t hipdssConfig::get_pivot_type() const
{
    return this->m_pivot_type;
}

double hipdssConfig::get_pivot_threshold() const
{
    return this->m_pivot_threshold;
}

double hipdssConfig::get_pivot_epsilon() const
{
    return this->m_pivot_epsilon;
}

int64_t hipdssConfig::get_max_lu_nnz() const
{
    return this->m_max_lu_nnz;
}

int32_t hipdssConfig::get_hybrid_memory_mode() const
{
    return this->m_hybrid_memory_mode;
}

int64_t hipdssConfig::get_hybrid_device_memory_limit() const
{
    return this->m_hybrid_device_memory_limit;
}

int32_t hipdssConfig::get_use_hip_register_memory() const
{
    return this->m_use_hip_register_memory;
}

int32_t hipdssConfig::get_host_nthreads() const
{
    return this->m_host_nthreads;
}

int32_t hipdssConfig::get_hybrid_execute_mode() const
{
    return this->m_hybrid_execute_mode;
}

hipdssPivotEpsilonAlg_t hipdssConfig::get_pivot_epsilon_alg() const
{
    return this->m_pivot_epsilon_alg;
}

int32_t hipdssConfig::get_nd_nlevels() const
{
    return this->m_nd_nlevels;
}

int32_t hipdssConfig::get_ubatch_size() const
{
    return this->m_ubatch_size;
}

int32_t hipdssConfig::get_ubatch_index() const
{
    return this->m_ubatch_index;
}

int32_t hipdssConfig::get_use_superpanels() const
{
    return this->m_use_superpanels;
}

int32_t hipdssConfig::get_device_count() const
{
    return this->m_device_count;
}

const std::vector<int32_t>& hipdssConfig::get_device_indices() const
{
    return this->m_device_indices;
}

int32_t hipdssConfig::get_schur_mode() const
{
    return this->m_schur_mode;
}

int32_t hipdssConfig::get_deterministic_mode() const
{
    return this->m_deterministic_mode;
}

int32_t hipdssConfig::get_nd_ubfactor() const
{
    return this->m_nd_ubfactor;
}

int32_t hipdssConfig::get_emulation_mantissa_bit_count() const
{
    return this->m_emulation_mantissa_bit_count;
}

int32_t hipdssConfig::get_force_nonsymmetric_matching() const
{
    return this->m_force_nonsymmetric_matching;
}

hipdssDowncastFactorsMode_t hipdssConfig::get_downcast_factors() const
{
    return this->m_downcast_factors;
}

int32_t hipdssConfig::get_memory_optimization_setting() const
{
    return this->m_memory_optimization_setting;
}

void hipdssConfig::set_reordering_alg(hipdssReorderingAlg_t value)
{
    this->m_reordering_alg = value;
}

void hipdssConfig::set_factorization_alg(hipdssFactorizationAlg_t value)
{
    this->m_factorization_alg = value;
}

void hipdssConfig::set_solve_alg(hipdssSolveAlg_t value)
{
    this->m_solve_alg = value;
}

void hipdssConfig::set_matching_alg(hipdssMatchingAlg_t value)
{
    this->m_matching_alg = value;
}

void hipdssConfig::set_solve_mode(hipdssSolveMode_t value)
{
    this->m_solve_mode = value;
}

void hipdssConfig::set_ir_nsteps(int32_t value)
{
    this->m_ir_nsteps = value;
}

void hipdssConfig::set_ir_tol(double value)
{
    this->m_ir_tol = value;
}

void hipdssConfig::set_pivot_type(hipdssPivotType_t value)
{
    this->m_pivot_type = value;
}

void hipdssConfig::set_pivot_threshold(double value)
{
    this->m_pivot_threshold = value;
}

void hipdssConfig::set_pivot_epsilon(double value)
{
    this->m_pivot_epsilon = value;
}

void hipdssConfig::set_max_lu_nnz(int64_t value)
{
    this->m_max_lu_nnz = value;
}

void hipdssConfig::set_hybrid_memory_mode(int32_t value)
{
    this->m_hybrid_memory_mode = value;
}

void hipdssConfig::set_hybrid_device_memory_limit(int64_t value)
{
    this->m_hybrid_device_memory_limit = value;
}

void hipdssConfig::set_use_hip_register_memory(int32_t value)
{
    this->m_use_hip_register_memory = value;
}

void hipdssConfig::set_host_nthreads(int32_t value)
{
    this->m_host_nthreads = value;
}

void hipdssConfig::set_hybrid_execute_mode(int32_t value)
{
    this->m_hybrid_execute_mode = value;
}

void hipdssConfig::set_pivot_epsilon_alg(hipdssPivotEpsilonAlg_t value)
{
    this->m_pivot_epsilon_alg = value;
}

void hipdssConfig::set_nd_nlevels(int32_t value)
{
    this->m_nd_nlevels = value;
}

void hipdssConfig::set_ubatch_size(int32_t value)
{
    this->m_ubatch_size = value;
}

void hipdssConfig::set_ubatch_index(int32_t value)
{
    this->m_ubatch_index = value;
}

void hipdssConfig::set_use_superpanels(int32_t value)
{
    this->m_use_superpanels = value;
}

void hipdssConfig::set_device_count(int32_t value)
{
    this->m_device_count = value;
}

void hipdssConfig::set_device_indices(const int32_t* indices, int32_t count)
{
    this->m_device_indices.assign(indices, indices + count);
}

void hipdssConfig::set_schur_mode(int32_t value)
{
    this->m_schur_mode = value;
}

void hipdssConfig::set_deterministic_mode(int32_t value)
{
    this->m_deterministic_mode = value;
}

void hipdssConfig::set_nd_ubfactor(int32_t value)
{
    this->m_nd_ubfactor = value;
}

void hipdssConfig::set_emulation_mantissa_bit_count(int32_t value)
{
    this->m_emulation_mantissa_bit_count = value;
}

void hipdssConfig::set_force_nonsymmetric_matching(int32_t value)
{
    this->m_force_nonsymmetric_matching = value;
}

void hipdssConfig::set_downcast_factors(hipdssDowncastFactorsMode_t value)
{
    this->m_downcast_factors = value;
}

void hipdssConfig::set_memory_optimization_setting(int32_t value)
{
    this->m_memory_optimization_setting = value;
}
