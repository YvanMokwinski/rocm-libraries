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

namespace hipdss
{
    hipdss_rf_stage hipdss_phase2hipdss_rf_stage(int phase)
    {
        switch(phase)
        {
        case HIPDSS_PHASE_REORDERING:
            return hipdss_rf_stage_reordering;
        case HIPDSS_PHASE_SYMBOLIC_FACTORIZATION:
            return hipdss_rf_stage_symbolic_factorization;
        case HIPDSS_PHASE_FACTORIZATION:
            return hipdss_rf_stage_factorization;
        case HIPDSS_PHASE_REFACTORIZATION:
            return hipdss_rf_stage_refactorization;
        case HIPDSS_PHASE_SOLVE_FWD:
            return hipdss_rf_stage_solve_forward;
        case HIPDSS_PHASE_SOLVE_DIAG:
            return hipdss_rf_stage_solve_diag;
        case HIPDSS_PHASE_SOLVE_BWD:
            return hipdss_rf_stage_solve_backward;
        case HIPDSS_PHASE_ANALYSIS:
            return hipdss_rf_stage_analysis;
        case HIPDSS_PHASE_SOLVE:
            return hipdss_rf_stage_solve;
        }
        THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }
}
