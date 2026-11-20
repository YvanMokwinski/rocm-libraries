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

#include "hipdss_rf.hpp"
#include "hipdssContext.hpp"
#include "hipdssMatrix.hpp"
#include "hipdss_timer.hpp"

hipdssStatus_t hipdss_rf(hipdssHandle_t  handle, // 0
                         hipdss_rf_descr rf_descr, // 1
                         hipdssConfig_t  solverConfig, // 2
                         hipdssData_t    solverData, // 3
                         hipdssMatrix_t  A, // 4
                         hipdssMatrix_t  RHS, // 5
                         hipdssMatrix_t  X, // 6
                         hipdss_rf_stage stage) // 7

{
    HIPDSS_CHECKARG_HANDLE(0, handle);
    HIPDSS_CHECKARG_POINTER(1, rf_descr);
    HIPDSS_CHECKARG_POINTER(2, solverConfig);
    HIPDSS_CHECKARG_POINTER(3, solverData);
    HIPDSS_CHECKARG_POINTER(4, A);
    HIPDSS_CHECKARG_POINTER(5, RHS);
    HIPDSS_CHECKARG_POINTER(6, X);

    hipdss::timer t(handle->get_stream());
    t.start();

    switch(stage)
    {
    case hipdss_rf_stage_analysis:
    {
        break;
    }

    case hipdss_rf_stage_symbolic_factorization:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::rf_symbolic_factorization(handle,
                                                                 rf_descr,
                                                                 solverConfig,
                                                                 solverData,
                                                                 A));
        break;
    }

    case hipdss_rf_stage_factorization:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::rf_factorization(handle,
                                                        rf_descr,
                                                        solverConfig,
                                                        solverData,
                                                        A));
        break;
    }

    case hipdss_rf_stage_refactorization:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::rf_refactorization(handle,
                                                          rf_descr,
                                                          solverConfig,
                                                          solverData,
                                                          A));
        break;
    }

    case hipdss_rf_stage_reordering:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::rf_reordering(handle,
                                                     rf_descr,
                                                     solverConfig,
                                                     solverData,
                                                     A));
        break;
    }

    case hipdss_rf_stage_solve:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::rf_solve(handle,
                                                rf_descr,
                                                RHS,
                                                X));

        break;
    }

    case hipdss_rf_stage_solve_backward:
    {

        RETURN_IF_HIPDSS_ERROR(hipdss::rf_solve_backward(handle,
                                                         rf_descr,
                                                         RHS,
                                                         X));

        break;
    }

    case hipdss_rf_stage_solve_diag:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::rf_solve_diagonal(handle,
                                                         rf_descr,
                                                         RHS,
                                                         X));
        break;
    }

    case hipdss_rf_stage_solve_forward:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::rf_solve_forward(handle,
                                                        rf_descr,
                                                        RHS,
                                                        X));
        break;
    }
    }

    rf_descr->set_timing(stage,
                         t.stop());

    return HIPDSS_STATUS_SUCCESS;
}
