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

#include "hipdssPhase_t.hpp"
#include "hipdss_control.hpp"

template <>
bool hipdss::enum_utils::is_invalid(const hipdssPhase_t value)
{
    constexpr int mask = HIPDSS_PHASE_REORDERING | HIPDSS_PHASE_SYMBOLIC_FACTORIZATION | HIPDSS_PHASE_ANALYSIS | HIPDSS_PHASE_FACTORIZATION | HIPDSS_PHASE_REFACTORIZATION | HIPDSS_PHASE_SOLVE_FWD_PERM | HIPDSS_PHASE_SOLVE_FWD | HIPDSS_PHASE_SOLVE_DIAG | HIPDSS_PHASE_SOLVE_BWD | HIPDSS_PHASE_SOLVE_BWD_PERM | HIPDSS_PHASE_SOLVE_REFINEMENT | HIPDSS_PHASE_SOLVE;
    return (static_cast<int>(value) & ~mask) != 0
           || static_cast<int>(value) == 0;
}

template <>
const char* hipdss::enum_utils::to_string(const hipdssPhase_t value)
{
    switch(value)
    {
    case HIPDSS_PHASE_REORDERING:
        return "HIPDSS_PHASE_REORDERING";
    case HIPDSS_PHASE_SYMBOLIC_FACTORIZATION:
        return "HIPDSS_PHASE_SYMBOLIC_FACTORIZATION";
    case HIPDSS_PHASE_ANALYSIS:
        return "HIPDSS_PHASE_ANALYSIS";
    case HIPDSS_PHASE_FACTORIZATION:
        return "HIPDSS_PHASE_FACTORIZATION";
    case HIPDSS_PHASE_REFACTORIZATION:
        return "HIPDSS_PHASE_REFACTORIZATION";
    case HIPDSS_PHASE_SOLVE_FWD_PERM:
        return "HIPDSS_PHASE_SOLVE_FWD_PERM";
    case HIPDSS_PHASE_SOLVE_FWD:
        return "HIPDSS_PHASE_SOLVE_FWD";
    case HIPDSS_PHASE_SOLVE_DIAG:
        return "HIPDSS_PHASE_SOLVE_DIAG";
    case HIPDSS_PHASE_SOLVE_BWD:
        return "HIPDSS_PHASE_SOLVE_BWD";
    case HIPDSS_PHASE_SOLVE_BWD_PERM:
        return "HIPDSS_PHASE_SOLVE_BWD_PERM";
    case HIPDSS_PHASE_SOLVE_REFINEMENT:
        return "HIPDSS_PHASE_SOLVE_REFINEMENT";
    case HIPDSS_PHASE_SOLVE:
        return "HIPDSS_PHASE_SOLVE";
    }

    if(!is_invalid(value))
    {
        return "hipdssPhase_t combination";
    }
    // LCOV_EXCL_START
    THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    // LCOV_EXCL_STOP
}
