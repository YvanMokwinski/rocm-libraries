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
#include "hipdss_spequilibrate.h"
#include "hipdss_spequilibrate_descr.hpp"

extern "C" hipdssStatus_t hipdss_spequilibrate_set_input(hipdssHandle_t             handle,
                                                         hipdss_spequilibrate_descr descr,
                                                         hipdss_spequilibrate_input input,
                                                         const void*                data,
                                                         size_t                     data_size_in_bytes,
                                                         rocsparse_error*           p_error)
try
{
    HIPDSS_CHECKARG_HANDLE(0,
                           handle);
    HIPDSS_CHECKARG_POINTER(1,
                            descr);
    HIPDSS_CHECKARG_ENUM(2,
                         input);
    HIPDSS_CHECKARG_POINTER(3,
                            data);

    switch(input)
    {
    case hipdss_spequilibrate_input_alg:
    {
        RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(
            descr->get_stage() != ((hipdss_spequilibrate_stage)-1)
                ? HIPDSS_STATUS_INVALID_VALUE
                : HIPDSS_STATUS_SUCCESS,
            "hipdss_spequilibrate_set_input cannot modify the descriptor after any "
            "of the stages "
            "hipdss_spequilibrate_stage was executed");

        HIPDSS_CHECKARG(4,
                        data_size_in_bytes,
                        data_size_in_bytes != sizeof(hipdss_spequilibrate_alg),
                        HIPDSS_STATUS_INVALID_VALUE);

        const hipdss_spequilibrate_alg alg = *reinterpret_cast<const hipdss_spequilibrate_alg*>(data);
        descr->set_alg(alg);
        return HIPDSS_STATUS_SUCCESS;
    }

    case hipdss_spequilibrate_input_ruiz_nmaxiter:
    {
        HIPDSS_CHECKARG(4,
                        data_size_in_bytes,
                        data_size_in_bytes != sizeof(int64_t),
                        HIPDSS_STATUS_INVALID_VALUE);

        int64_t nmaxiter;
        RETURN_IF_HIP_ERROR(
            hipMemcpy(&nmaxiter,
                      data,
                      data_size_in_bytes,
                      hipMemcpyHostToHost));
        auto ruiz_descr = reinterpret_cast<_hipdss_spequilibrate_ruiz_descr*>(descr->get_impl());
        ruiz_descr->set_nmaxiter(nmaxiter);
        return HIPDSS_STATUS_SUCCESS;
    }

    case hipdss_spequilibrate_input_ruiz_tol:
    {

        HIPDSS_CHECKARG(4,
                        data_size_in_bytes,
                        data_size_in_bytes != sizeof(double),
                        HIPDSS_STATUS_INVALID_VALUE);

        auto   ruiz_descr = reinterpret_cast<_hipdss_spequilibrate_ruiz_descr*>(descr->get_impl());
        double tol;
        RETURN_IF_HIP_ERROR(
            hipMemcpy(&tol,
                      data,
                      data_size_in_bytes,
                      hipMemcpyHostToHost));

        ruiz_descr->set_tol(tol);
        return HIPDSS_STATUS_SUCCESS;
    }

        // LCOV_EXCL_START
    }
    RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
// LCOV_EXCL_STOP
