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

#include "hipdss_control.hpp"
#include "hipdss_enum_utils.hpp"

template <>
bool hipdss::enum_utils::is_invalid(const hipdssConfigParam_t value)
{
    switch(value)
    {
    case HIPDSS_CONFIG_REORDERING_ALG:
    case HIPDSS_CONFIG_FACTORIZATION_ALG:
    case HIPDSS_CONFIG_SOLVE_ALG:
    case HIPDSS_CONFIG_MATCHING_ALG:
    case HIPDSS_CONFIG_SOLVE_MODE:
    case HIPDSS_CONFIG_IR_N_STEPS:
    case HIPDSS_CONFIG_IR_TOL:
    case HIPDSS_CONFIG_PIVOT_TYPE:
    case HIPDSS_CONFIG_PIVOT_THRESHOLD:
    case HIPDSS_CONFIG_PIVOT_EPSILON:
    case HIPDSS_CONFIG_MAX_LU_NNZ:
    case HIPDSS_CONFIG_HYBRID_MEMORY_MODE:
    case HIPDSS_CONFIG_HYBRID_DEVICE_MEMORY_LIMIT:
    case HIPDSS_CONFIG_USE_HIP_REGISTER_MEMORY:
    case HIPDSS_CONFIG_HOST_NTHREADS:
    case HIPDSS_CONFIG_HYBRID_EXECUTE_MODE:
    case HIPDSS_CONFIG_PIVOT_EPSILON_ALG:
    case HIPDSS_CONFIG_ND_NLEVELS:
    case HIPDSS_CONFIG_UBATCH_INDEX:
    case HIPDSS_CONFIG_USE_SUPERPANELS:
    case HIPDSS_CONFIG_DEVICE_COUNT:
    case HIPDSS_CONFIG_DEVICE_INDICES:
    case HIPDSS_CONFIG_SCHUR_MODE:
    case HIPDSS_CONFIG_DETERMINISTIC_MODE:
    case HIPDSS_CONFIG_ND_UBFACTOR:
    case HIPDSS_CONFIG_EMULATION_MANTISSA_BIT_COUNT:
    case HIPDSS_CONFIG_FORCE_NONSYMMETRIC_MATCHING:
    case HIPDSS_CONFIG_DOWNCAST_FACTORS:
    case HIPDSS_CONFIG_MEMORY_OPTIMIZATION_SETTING:
    case HIPDSS_CONFIG_UBATCH_SIZE:
    {
        return false;
    }
    }
    return true;
}

template <>
const char* hipdss::enum_utils::to_string(const hipdssConfigParam_t value)
{
    switch(value)
    {
    case HIPDSS_CONFIG_REORDERING_ALG:
        return "HIPDSS_CONFIG_REORDERING_ALG";
    case HIPDSS_CONFIG_FACTORIZATION_ALG:
        return "HIPDSS_CONFIG_FACTORIZATION_ALG";
    case HIPDSS_CONFIG_SOLVE_ALG:
        return "HIPDSS_CONFIG_SOLVE_ALG";
    case HIPDSS_CONFIG_MATCHING_ALG:
        return "HIPDSS_CONFIG_MATCHING_ALG";
    case HIPDSS_CONFIG_SOLVE_MODE:
        return "HIPDSS_CONFIG_SOLVE_MODE";
    case HIPDSS_CONFIG_IR_N_STEPS:
        return "HIPDSS_CONFIG_IR_N_STEPS";
    case HIPDSS_CONFIG_IR_TOL:
        return "HIPDSS_CONFIG_IR_TOL";
    case HIPDSS_CONFIG_PIVOT_TYPE:
        return "HIPDSS_CONFIG_PIVOT_TYPE";
    case HIPDSS_CONFIG_PIVOT_THRESHOLD:
        return "HIPDSS_CONFIG_PIVOT_THRESHOLD";
    case HIPDSS_CONFIG_PIVOT_EPSILON:
        return "HIPDSS_CONFIG_PIVOT_EPSILON";
    case HIPDSS_CONFIG_MAX_LU_NNZ:
        return "HIPDSS_CONFIG_MAX_LU_NNZ";
    case HIPDSS_CONFIG_HYBRID_MEMORY_MODE:
        return "HIPDSS_CONFIG_HYBRID_MEMORY_MODE";
    case HIPDSS_CONFIG_HYBRID_DEVICE_MEMORY_LIMIT:
        return "HIPDSS_CONFIG_HYBRID_DEVICE_MEMORY_LIMIT";
    case HIPDSS_CONFIG_USE_HIP_REGISTER_MEMORY:
        return "HIPDSS_CONFIG_USE_HIP_REGISTER_MEMORY";
    case HIPDSS_CONFIG_HOST_NTHREADS:
        return "HIPDSS_CONFIG_HOST_NTHREADS";
    case HIPDSS_CONFIG_HYBRID_EXECUTE_MODE:
        return "HIPDSS_CONFIG_HYBRID_EXECUTE_MODE";
    case HIPDSS_CONFIG_PIVOT_EPSILON_ALG:
        return "HIPDSS_CONFIG_PIVOT_EPSILON_ALG";
    case HIPDSS_CONFIG_ND_NLEVELS:
        return "HIPDSS_CONFIG_ND_NLEVELS";
    case HIPDSS_CONFIG_UBATCH_SIZE:
        return "HIPDSS_CONFIG_UBATCH_SIZE";
    case HIPDSS_CONFIG_UBATCH_INDEX:
        return "HIPDSS_CONFIG_UBATCH_INDEX";
    case HIPDSS_CONFIG_USE_SUPERPANELS:
        return "HIPDSS_CONFIG_USE_SUPERPANELS";
    case HIPDSS_CONFIG_DEVICE_COUNT:
        return "HIPDSS_CONFIG_DEVICE_COUNT";
    case HIPDSS_CONFIG_DEVICE_INDICES:
        return "HIPDSS_CONFIG_DEVICE_INDICES";
    case HIPDSS_CONFIG_SCHUR_MODE:
        return "HIPDSS_CONFIG_SCHUR_MODE";
    case HIPDSS_CONFIG_DETERMINISTIC_MODE:
        return "HIPDSS_CONFIG_DETERMINISTIC_MODE";
    case HIPDSS_CONFIG_ND_UBFACTOR:
        return "HIPDSS_CONFIG_ND_UBFACTOR";
    case HIPDSS_CONFIG_EMULATION_MANTISSA_BIT_COUNT:
        return "HIPDSS_CONFIG_EMULATION_MANTISSA_BIT_COUNT";
    case HIPDSS_CONFIG_FORCE_NONSYMMETRIC_MATCHING:
        return "HIPDSS_CONFIG_FORCE_NONSYMMETRIC_MATCHING";
    case HIPDSS_CONFIG_DOWNCAST_FACTORS:
        return "HIPDSS_CONFIG_DOWNCAST_FACTORS";
    case HIPDSS_CONFIG_MEMORY_OPTIMIZATION_SETTING:
        return "HIPDSS_CONFIG_MEMORY_OPTIMIZATION_SETTING";
    }
    // LCOV_EXCL_START
    THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    // LCOV_EXCL_STOP
}
