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

hipdssStatus_t hipdss_rf_info(hipdssHandle_t  handle,
                              hipdss_rf_descr descr,
                              FILE*           out)
{
    fprintf(out, "hipdss info timings\n");

    fprintf(out, "  reordering             : %5.3f ms\n", descr->m_timing_reordering);

    fprintf(out, "  symbolic_factorization : %5.3f ms\n", descr->m_timing_symbolic_factorization);

    fprintf(out, "  factorization          : %5.3f ms\n", descr->m_timing_factorization);

    fprintf(out, "  refactorization        : %5.3f ms\n", descr->m_timing_refactorization);

    fprintf(out, "  analysis               : %5.3f ms\n", descr->m_timing_analysis);

    fprintf(out, "  solve_forward          : %5.3f ms\n", descr->m_timing_solve_forward);

    fprintf(out, "  solve_backward         : %5.3f ms\n", descr->m_timing_solve_backward);

    fprintf(out, "  solve_diag             : %5.3f ms\n", descr->m_timing_solve_diag);

    fprintf(out, "  solve                  : %5.3f ms\n", descr->m_timing_solve);

    return HIPDSS_STATUS_SUCCESS;
}
