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
#include "hipdss_control.hpp"

#include "hipdss_rf.hpp"

namespace hipdss
{
    hipdss_rf_stage hipdss_phase2hipdss_rf_stage(int phase);
}

extern "C" hipdssStatus_t hipdssExecute(hipdssHandle_t handle,
                                        int            phase_,
                                        hipdssConfig_t solverConfig,
                                        hipdssData_t   solverData,
                                        hipdssMatrix_t inputMatrix,
                                        hipdssMatrix_t solution,
                                        hipdssMatrix_t rhs)
try
{
    HIPDSS_CHECKARG_HANDLE(0,
                           handle);

    HIPDSS_CHECKARG_POINTER(2,
                            solverConfig);

    HIPDSS_CHECKARG_POINTER(3,
                            solverData);

    HIPDSS_CHECKARG_POINTER(4,
                            inputMatrix);

    HIPDSS_CHECKARG_POINTER(5,
                            solution);

    HIPDSS_CHECKARG_POINTER(6,
                            rhs);

    auto A        = inputMatrix;
    auto rf_descr = handle->get_rf_descr();

    const hipdssPhase_t phase = static_cast<hipdssPhase_t>(phase_);

    const auto stage = hipdss::hipdss_phase2hipdss_rf_stage(phase);

    switch(phase)
    {
    case HIPDSS_PHASE_REORDERING:
    {

        if(handle->reordering_phase_completed)
        {
            RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                                "reordering phase has already been completed.");
        }

        //
        // Make sure data are on host.
        //
        RETURN_IF_HIPDSS_ERROR(A->to_host(handle));

        RETURN_IF_HIPDSS_ERROR(hipdss_rf(handle,
                                         rf_descr,
                                         solverConfig,
                                         solverData,
                                         A,
                                         rhs,
                                         solution,
                                         stage));

        handle->reordering_phase_completed = true;
        break;
    }

    case HIPDSS_PHASE_SYMBOLIC_FACTORIZATION:
    {

        if(handle->symbolic_factorization_phase_completed)
        {
            RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                                "symbolic factorization phase has already been completed.");
        }

        if(handle->reordering_phase_completed == false)
        {
            RETURN_IF_HIPDSS_ERROR(A->to_host(handle));
        }

        RETURN_IF_HIPDSS_ERROR(hipdss_rf(handle,
                                         rf_descr,
                                         solverConfig,
                                         solverData,
                                         A,
                                         rhs,
                                         solution,
                                         stage));

        //
        // Free host data.
        //
        RETURN_IF_HIPDSS_ERROR(A->free_host(handle));

        auto F = rf_descr->get_factorizationMatrix();

        //
        // Move the sparsity pattern to device.
        //
        RETURN_IF_HIPDSS_ERROR(F->to_device(handle));

        //
        // Allocate the numeric part.
        //
        RETURN_IF_HIPDSS_ERROR(F->device_numeric_allocation(handle, A->get_val_type()));

        //
        // Release the host memory.
        //
        RETURN_IF_HIPDSS_ERROR(F->free_host(handle));

        handle->symbolic_factorization_phase_completed = true;
        break;
    }

    case HIPDSS_PHASE_FACTORIZATION:
    {
        if(handle->symbolic_factorization_phase_completed == false)
        {
            RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                                "symbolic factorization phase has not been completed, and is required by the factorization phase.");
        }

        if(handle->factorization_phase_completed)
        {
            RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                                "factorization phase has already been completed, refactorization phase is required for subsequent factorization.");
        }

        RETURN_IF_HIPDSS_ERROR(hipdss_rf(handle,
                                         rf_descr,
                                         solverConfig,
                                         solverData,
                                         A,
                                         rhs,
                                         solution,
                                         stage));
        handle->factorization_phase_completed = true;
        break;
    }

    case HIPDSS_PHASE_REFACTORIZATION:
    {
        if(handle->factorization_phase_completed == false)
        {
            RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                                "factorization phase has not been completed, and is required by the refactorization phase.");
        }

        RETURN_IF_HIPDSS_ERROR(hipdss_rf(handle,
                                         rf_descr,
                                         solverConfig,
                                         solverData,
                                         A,
                                         rhs,
                                         solution,
                                         stage));

        break;
    }

    case HIPDSS_PHASE_SOLVE:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss_rf(handle,
                                         rf_descr,
                                         solverConfig,
                                         solverData,
                                         A,
                                         rhs,
                                         solution,
                                         stage));
        break;
    }

    case HIPDSS_PHASE_SOLVE_FWD:
    case HIPDSS_PHASE_SOLVE_DIAG:
    case HIPDSS_PHASE_SOLVE_BWD:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss_rf(handle,
                                         rf_descr,
                                         solverConfig,
                                         solverData,
                                         A,
                                         rhs,
                                         solution,
                                         stage));
        break;
    }

    case HIPDSS_PHASE_ANALYSIS:
    {

        RETURN_IF_HIPDSS_ERROR(hipdssExecute(handle,
                                             HIPDSS_PHASE_REORDERING,
                                             solverConfig,
                                             solverData,
                                             A,
                                             solution,
                                             rhs));

        RETURN_IF_HIPDSS_ERROR(hipdssExecute(handle,
                                             HIPDSS_PHASE_SYMBOLIC_FACTORIZATION,
                                             solverConfig,
                                             solverData,
                                             A,
                                             solution,
                                             rhs));

        break;
    }

    case HIPDSS_PHASE_SOLVE_REFINEMENT:
    {
        break;
    }
    case HIPDSS_PHASE_SOLVE_BWD_PERM:
    {
        break;
    }

    case HIPDSS_PHASE_SOLVE_FWD_PERM:
    {
        break;
    }
    }

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
