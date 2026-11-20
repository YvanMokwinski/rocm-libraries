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

#include "hipdss.hpp"
#include "hipdssDowncastFactorsMode_t.hpp"
#include "hipdssFactorizationAlg_t.hpp"
#include "hipdssMatchingAlg_t.hpp"
#include "hipdssPivotEpsilonAlg_t.hpp"
#include "hipdssPivotType_t.hpp"
#include "hipdssReorderingAlg_t.hpp"
#include "hipdssSolveAlg_t.hpp"
#include "hipdssSolveMode_t.hpp"
#include "hipdss_control.hpp"

extern "C" hipdssStatus_t hipdssConfigSet(hipdssConfig_t      config,
                                          hipdssConfigParam_t param,
                                          const void*         value,
                                          size_t              sizeInBytes)
try
{

    HIPDSS_CHECKARG_POINTER(0, config);
    HIPDSS_CHECKARG_ENUM(1, param);
    HIPDSS_CHECKARG_POINTER(2, value);

    switch(param)
    {
    case HIPDSS_CONFIG_REORDERING_ALG:
    {
        const auto enum_value = *static_cast<const hipdssReorderingAlg_t*>(value);
        HIPDSS_CHECKARG(2,
                        value,
                        hipdss::enum_utils::is_invalid(enum_value),
                        HIPDSS_STATUS_INVALID_VALUE);

        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(hipdssReorderingAlg_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_reordering_alg(enum_value);
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_FACTORIZATION_ALG:
    {
        const auto enum_value = *static_cast<const hipdssFactorizationAlg_t*>(value);
        HIPDSS_CHECKARG(2,
                        value,
                        hipdss::enum_utils::is_invalid(enum_value),
                        HIPDSS_STATUS_INVALID_VALUE);
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(hipdssFactorizationAlg_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_factorization_alg(enum_value);
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_SOLVE_ALG:
    {
        const auto enum_value = *static_cast<const hipdssSolveAlg_t*>(value);
        HIPDSS_CHECKARG(2,
                        value,
                        hipdss::enum_utils::is_invalid(enum_value),
                        HIPDSS_STATUS_INVALID_VALUE);
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(hipdssSolveAlg_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_solve_alg(enum_value);
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_MATCHING_ALG:
    {
        const auto enum_value = *static_cast<const hipdssMatchingAlg_t*>(value);
        HIPDSS_CHECKARG(2,
                        value,
                        hipdss::enum_utils::is_invalid(enum_value),
                        HIPDSS_STATUS_INVALID_VALUE);
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(hipdssMatchingAlg_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_matching_alg(enum_value);
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_SOLVE_MODE:
    {
        const auto enum_value = *static_cast<const hipdssSolveMode_t*>(value);
        HIPDSS_CHECKARG(2,
                        value,
                        hipdss::enum_utils::is_invalid(enum_value),
                        HIPDSS_STATUS_INVALID_VALUE);
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(hipdssSolveMode_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_solve_mode(enum_value);
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_IR_N_STEPS:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_ir_nsteps(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_IR_TOL:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(double), HIPDSS_STATUS_INVALID_VALUE);
        config->set_ir_tol(*static_cast<const double*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_PIVOT_TYPE:
    {
        const auto enum_value = *static_cast<const hipdssPivotType_t*>(value);
        HIPDSS_CHECKARG(2,
                        value,
                        hipdss::enum_utils::is_invalid(enum_value),
                        HIPDSS_STATUS_INVALID_VALUE);
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(hipdssPivotType_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_pivot_type(enum_value);
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_PIVOT_THRESHOLD:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(double), HIPDSS_STATUS_INVALID_VALUE);
        config->set_pivot_threshold(*static_cast<const double*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_PIVOT_EPSILON:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(double), HIPDSS_STATUS_INVALID_VALUE);
        config->set_pivot_epsilon(*static_cast<const double*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_MAX_LU_NNZ:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int64_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_max_lu_nnz(*static_cast<const int64_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_HYBRID_MEMORY_MODE:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_hybrid_memory_mode(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_HYBRID_DEVICE_MEMORY_LIMIT:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int64_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_hybrid_device_memory_limit(*static_cast<const int64_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_USE_HIP_REGISTER_MEMORY:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_use_hip_register_memory(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_HOST_NTHREADS:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_host_nthreads(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_HYBRID_EXECUTE_MODE:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_hybrid_execute_mode(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_PIVOT_EPSILON_ALG:
    {
        const auto enum_value = *static_cast<const hipdssPivotEpsilonAlg_t*>(value);
        HIPDSS_CHECKARG(2,
                        value,
                        hipdss::enum_utils::is_invalid(enum_value),
                        HIPDSS_STATUS_INVALID_VALUE);
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(hipdssPivotEpsilonAlg_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_pivot_epsilon_alg(enum_value);
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_ND_NLEVELS:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_nd_nlevels(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_UBATCH_SIZE:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_ubatch_size(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_UBATCH_INDEX:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_ubatch_index(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_USE_SUPERPANELS:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_use_superpanels(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_DEVICE_COUNT:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_device_count(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_SCHUR_MODE:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_schur_mode(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_DETERMINISTIC_MODE:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_deterministic_mode(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_ND_UBFACTOR:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_nd_ubfactor(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_EMULATION_MANTISSA_BIT_COUNT:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_emulation_mantissa_bit_count(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_FORCE_NONSYMMETRIC_MATCHING:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_force_nonsymmetric_matching(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_DOWNCAST_FACTORS:
    {
        const auto enum_value = *static_cast<const hipdssDowncastFactorsMode_t*>(value);
        HIPDSS_CHECKARG(2,
                        value,
                        hipdss::enum_utils::is_invalid(enum_value),
                        HIPDSS_STATUS_INVALID_VALUE);
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(hipdssDowncastFactorsMode_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_downcast_factors(enum_value);
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_MEMORY_OPTIMIZATION_SETTING:
    {
        HIPDSS_CHECKARG(3, sizeInBytes, sizeInBytes < sizeof(int32_t), HIPDSS_STATUS_INVALID_VALUE);
        config->set_memory_optimization_setting(*static_cast<const int32_t*>(value));
        return HIPDSS_STATUS_SUCCESS;
    }

    case HIPDSS_CONFIG_DEVICE_INDICES: /**< Array of GPU device indices (int*) */
    {
        HIPDSS_CHECKARG(3,
                        sizeInBytes,
                        sizeInBytes == 0 || sizeInBytes % sizeof(int32_t) != 0,
                        HIPDSS_STATUS_INVALID_VALUE);
        config->set_device_indices(static_cast<const int32_t*>(value),
                                   static_cast<int32_t>(sizeInBytes / sizeof(int32_t)));
        return HIPDSS_STATUS_SUCCESS;
    }
    }

    RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
