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
#include "hipdss_spequilibrate.h"
#include "hipdss_spequilibrate_descr.hpp"

template <>
inline bool hipdss::enum_utils::is_invalid(hipdss_spequilibrate_output value)
{
    switch(value)
    {
    case hipdss_spequilibrate_output_ruiz_iter:
    {
        return false;
    }
    case hipdss_spequilibrate_output_ruiz_nrm:
    {
        return false;
    }
    }
    return true;
};

extern "C" hipdssStatus_t hipdss_spequilibrate_get_output(
    hipdssHandle_t handle, hipdss_spequilibrate_descr descr, hipdss_spequilibrate_output output, void* data, size_t data_size_in_bytes, rocsparse_error* p_error)
try
{
    HIPDSS_CHECKARG_HANDLE(0, handle);
    HIPDSS_CHECKARG_POINTER(1, descr);
    HIPDSS_CHECKARG_ENUM(2, output);
    HIPDSS_CHECKARG_POINTER(3, data);
    HIPDSS_CHECKARG(4, data_size_in_bytes, data_size_in_bytes == 0, HIPDSS_STATUS_INVALID_VALUE);

    switch(output)
    {

    case hipdss_spequilibrate_output_ruiz_nrm:
    {
        HIPDSS_CHECKARG(4, data_size_in_bytes, data_size_in_bytes != sizeof(double), HIPDSS_STATUS_INVALID_VALUE);

        auto ruiz_descr = reinterpret_cast<_hipdss_spequilibrate_ruiz_descr*>(descr->get_impl());

        double nrm = ruiz_descr->get_nrm();
        RETURN_IF_HIP_ERROR(
            hipMemcpy(data, &nrm, data_size_in_bytes, hipMemcpyHostToHost));
        return HIPDSS_STATUS_SUCCESS;
    }

    case hipdss_spequilibrate_output_ruiz_iter:
    {
        HIPDSS_CHECKARG(4, data_size_in_bytes, data_size_in_bytes != sizeof(int64_t), HIPDSS_STATUS_INVALID_VALUE);
        auto    ruiz_descr = reinterpret_cast<_hipdss_spequilibrate_ruiz_descr*>(descr->get_impl());
        int64_t niter      = ruiz_descr->get_niter();
        RETURN_IF_HIP_ERROR(
            hipMemcpy(data, &niter, data_size_in_bytes, hipMemcpyHostToHost));
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
