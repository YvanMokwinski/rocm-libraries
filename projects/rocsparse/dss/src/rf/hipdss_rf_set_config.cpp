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

hipdssStatus_t hipdss_rf_set_config(hipdssHandle_t   handle,
                                    hipdss_rf_descr  rf_descr,
                                    hipdss_rf_stage  stage,
                                    void*            config,
                                    rocsparse_error* p_error)
{
    switch(stage)
    {
    case hipdss_rf_stage_reordering:
    {
        rf_descr->set_reordering_config(config);
        return HIPDSS_STATUS_SUCCESS;
    }

    case hipdss_rf_stage_symbolic_factorization:
    {
        rf_descr->set_symbolic_factorization_config(config);
        return HIPDSS_STATUS_SUCCESS;
    }

    case hipdss_rf_stage_factorization:
    {
        rf_descr->set_factorization_config(config);
        return HIPDSS_STATUS_SUCCESS;
    }

    case hipdss_rf_stage_solve:
    {
        rf_descr->set_solve_config(config);
        return HIPDSS_STATUS_SUCCESS;
    }
    case hipdss_rf_stage_analysis:
    case hipdss_rf_stage_solve_diag:
    case hipdss_rf_stage_solve_backward:
    case hipdss_rf_stage_solve_forward:
    case hipdss_rf_stage_refactorization:
    {
        return HIPDSS_STATUS_SUCCESS;
    }
    }
    RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
}
