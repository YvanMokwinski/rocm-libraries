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

#include "hipdss.h"
#include <vector>

struct hipdssConfig
{
protected:
    hipdssReorderingAlg_t       m_reordering_alg;
    hipdssFactorizationAlg_t    m_factorization_alg;
    hipdssSolveAlg_t            m_solve_alg;
    hipdssMatchingAlg_t         m_matching_alg;
    hipdssSolveMode_t           m_solve_mode;
    int32_t                     m_ir_nsteps;
    double                      m_ir_tol;
    hipdssPivotType_t           m_pivot_type;
    double                      m_pivot_threshold;
    double                      m_pivot_epsilon;
    int64_t                     m_max_lu_nnz; // BTF_COLAMD/COLAMD
    int32_t                     m_hybrid_memory_mode;
    int64_t                     m_hybrid_device_memory_limit;
    int32_t                     m_use_hip_register_memory;
    int32_t                     m_host_nthreads;
    int32_t                     m_hybrid_execute_mode;
    hipdssPivotEpsilonAlg_t     m_pivot_epsilon_alg;
    int32_t                     m_nd_nlevels;
    int32_t                     m_ubatch_size;
    int32_t                     m_ubatch_index;
    int32_t                     m_use_superpanels;
    int32_t                     m_device_count;
    std::vector<int32_t>        m_device_indices;
    int32_t                     m_schur_mode;
    int32_t                     m_deterministic_mode;
    int32_t                     m_nd_ubfactor;
    int32_t                     m_emulation_mantissa_bit_count;
    int32_t                     m_force_nonsymmetric_matching;
    hipdssDowncastFactorsMode_t m_downcast_factors;
    int32_t                     m_memory_optimization_setting;

public:
    hipdssConfig();

    hipdssReorderingAlg_t       get_reordering_alg() const;
    hipdssFactorizationAlg_t    get_factorization_alg() const;
    hipdssSolveAlg_t            get_solve_alg() const;
    hipdssMatchingAlg_t         get_matching_alg() const;
    hipdssSolveMode_t           get_solve_mode() const;
    int32_t                     get_ir_nsteps() const;
    double                      get_ir_tol() const;
    hipdssPivotType_t           get_pivot_type() const;
    double                      get_pivot_threshold() const;
    double                      get_pivot_epsilon() const;
    int64_t                     get_max_lu_nnz() const; // BTF_COLAMD/COLAMD
    int32_t                     get_hybrid_memory_mode() const;
    int64_t                     get_hybrid_device_memory_limit() const;
    int32_t                     get_use_hip_register_memory() const;
    int32_t                     get_host_nthreads() const;
    int32_t                     get_hybrid_execute_mode() const;
    hipdssPivotEpsilonAlg_t     get_pivot_epsilon_alg() const;
    int32_t                     get_nd_nlevels() const;
    int32_t                     get_ubatch_size() const;
    int32_t                     get_ubatch_index() const;
    int32_t                     get_use_superpanels() const;
    int32_t                     get_device_count() const;
    const std::vector<int32_t>& get_device_indices() const;
    int32_t                     get_schur_mode() const;
    int32_t                     get_deterministic_mode() const;
    int32_t                     get_nd_ubfactor() const;
    int32_t                     get_emulation_mantissa_bit_count() const;
    int32_t                     get_force_nonsymmetric_matching() const;
    hipdssDowncastFactorsMode_t get_downcast_factors() const;
    int32_t                     get_memory_optimization_setting() const;

    void set_reordering_alg(hipdssReorderingAlg_t value);
    void set_factorization_alg(hipdssFactorizationAlg_t value);
    void set_solve_alg(hipdssSolveAlg_t value);
    void set_matching_alg(hipdssMatchingAlg_t value);
    void set_solve_mode(hipdssSolveMode_t value);
    void set_ir_nsteps(int32_t value);
    void set_ir_tol(double value);
    void set_pivot_type(hipdssPivotType_t value);
    void set_pivot_threshold(double value);
    void set_pivot_epsilon(double value);
    void set_max_lu_nnz(int64_t value);
    void set_hybrid_memory_mode(int32_t value);
    void set_hybrid_device_memory_limit(int64_t value);
    void set_use_hip_register_memory(int32_t value);
    void set_host_nthreads(int32_t value);
    void set_hybrid_execute_mode(int32_t value);
    void set_pivot_epsilon_alg(hipdssPivotEpsilonAlg_t value);
    void set_nd_nlevels(int32_t value);
    void set_ubatch_size(int32_t value);
    void set_ubatch_index(int32_t value);
    void set_use_superpanels(int32_t value);
    void set_device_count(int32_t value);
    void set_device_indices(const int32_t* indices, int32_t count);
    void set_schur_mode(int32_t value);
    void set_deterministic_mode(int32_t value);
    void set_nd_ubfactor(int32_t value);
    void set_emulation_mantissa_bit_count(int32_t value);
    void set_force_nonsymmetric_matching(int32_t value);
    void set_downcast_factors(hipdssDowncastFactorsMode_t value);
    void set_memory_optimization_setting(int32_t value);
};
