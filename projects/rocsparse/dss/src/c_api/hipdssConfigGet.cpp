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

#include "hipdss.h"
#include "hipdss_control.hpp"

extern "C" hipdssStatus_t hipdssConfigGet(hipdssConfig_t      config,
                                          hipdssConfigParam_t param,
                                          void*               value,
                                          size_t              sizeInBytes,
                                          size_t*             sizeWritten)
try
{

    HIPDSS_CHECKARG_POINTER(0, config);
    HIPDSS_CHECKARG_ENUM(1, param);
    HIPDSS_CHECKARG_POINTER(2, value);
    HIPDSS_CHECKARG_POINTER(4, sizeWritten);

    switch(param)
    {
    case HIPDSS_CONFIG_REORDERING_ALG: /**< Reordering algorithm (hipdssReorderingAlg_t) */
    case HIPDSS_CONFIG_FACTORIZATION_ALG: /**< Factorization algorithm (hipdssFactorizationAlg_t) */
    case HIPDSS_CONFIG_SOLVE_ALG: /**< Solve algorithm (hipdssSolveAlg_t) */
    case HIPDSS_CONFIG_MATCHING_ALG: /**< Matching algorithm (hipdssMatchingAlg_t) */
    case HIPDSS_CONFIG_SOLVE_MODE: /**< Solve mode: 0:A, 1:A^T, 2:A^H */
    case HIPDSS_CONFIG_IR_N_STEPS: /**< Iterative refinement steps */
    case HIPDSS_CONFIG_IR_TOL: /**< IR tolerance (not supported) */
    case HIPDSS_CONFIG_PIVOT_TYPE: /**< Pivoting type (hipdssPivotType_t) */
    case HIPDSS_CONFIG_PIVOT_THRESHOLD: /**< Pivoting threshold (double) */
    case HIPDSS_CONFIG_PIVOT_EPSILON: /**< Pivoting epsilon (double) */
    case HIPDSS_CONFIG_MAX_LU_NNZ: /**< Max factor NNZ (int64_t) -- BTF_COLAMD/COLAMD */
    case HIPDSS_CONFIG_HYBRID_MEMORY_MODE: /**< Hybrid memory mode (int: 0:off) */
    case HIPDSS_CONFIG_HYBRID_DEVICE_MEMORY_LIMIT: /**< GPU memory limit in hybrid mode */
    case HIPDSS_CONFIG_USE_HIP_REGISTER_MEMORY: /**< Use HIP registered memory (int: 1:on) */
    case HIPDSS_CONFIG_HOST_NTHREADS: /**< Number of CPU threads (int) */
    case HIPDSS_CONFIG_HYBRID_EXECUTE_MODE: /**< Hybrid execute mode (int: 0:off) */
    case HIPDSS_CONFIG_PIVOT_EPSILON_ALG: /**< Pivot epsilon algorithm (hipdssPivotEpsilonAlg_t) */
    case HIPDSS_CONFIG_ND_NLEVELS: /**< Nested dissection levels (int) */
    case HIPDSS_CONFIG_UBATCH_SIZE: /**< Uniform batch size (int) */
    case HIPDSS_CONFIG_UBATCH_INDEX: /**< Uniform batch index (int) */
    case HIPDSS_CONFIG_USE_SUPERPANELS: /**< Superpanel optimisation (int: 0/1) */
    case HIPDSS_CONFIG_DEVICE_COUNT: /**< Number of GPUs for MG mode (int) */
    case HIPDSS_CONFIG_DEVICE_INDICES: /**< Array of GPU device indices (int*) */
    case HIPDSS_CONFIG_SCHUR_MODE: /**< Schur complement mode (int) */
    case HIPDSS_CONFIG_DETERMINISTIC_MODE: /**< Deterministic execution (int: 0/1) */
    case HIPDSS_CONFIG_ND_UBFACTOR: /**< Nested dissection unbalance factor (percent, int) */
    case HIPDSS_CONFIG_EMULATION_MANTISSA_BIT_COUNT:
    case HIPDSS_CONFIG_FORCE_NONSYMMETRIC_MATCHING:
    case HIPDSS_CONFIG_DOWNCAST_FACTORS:
    case HIPDSS_CONFIG_MEMORY_OPTIMIZATION_SETTING:
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_NOT_SUPPORTED);
    }
    }

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
