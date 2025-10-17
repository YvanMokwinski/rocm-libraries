/*! \file */
/* ************************************************************************
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights Reserved.
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

#include "internal/level2/rocsparse_csrsv.h"
#include "internal/precond/rocsparse_bsrilu0.h"

#include "rocsparse_bsrilu0.hpp"

#include "../level2/rocsparse_csrsv.hpp"
#include "bsrilu0_device.h"
#include "rocsparse_control.hpp"
#include "rocsparse_utility.hpp"

namespace rocsparse
{
    template <typename T>
    rocsparse_status bsrilu0_numeric_boost_template(rocsparse_handle   handle,
                                                    rocsparse_mat_info info,
                                                    int                enable_boost,
                                                    size_t             boost_tol_size,
                                                    const void*        boost_tol,
                                                    const T*           boost_val)
    {
        ROCSPARSE_ROUTINE_TRACE;

        ROCSPARSE_CHECKARG_HANDLE(0, handle);

        rocsparse::log_trace(handle,
                             rocsparse::replaceX<T>("rocsparse_Xbsrilu0_numeric_boost"),
                             (const void*&)info,
                             enable_boost,
                             (const void*&)boost_tol,
                             (const void*&)boost_val);

        ROCSPARSE_CHECKARG_POINTER(1, info);

        // Reset boost
        info->boost_enable = 0;

        if(enable_boost)
        {
            ROCSPARSE_CHECKARG_POINTER(3, boost_tol);
            ROCSPARSE_CHECKARG_POINTER(4, boost_val);

            info->boost_enable   = enable_boost;
            info->boost_tol_size = boost_tol_size;
            info->boost_tol      = boost_tol;
            info->boost_val      = reinterpret_cast<const void*>(boost_val);
        }

        return rocsparse_status_success;
    }
}

extern "C" rocsparse_status rocsparse_bsrilu0_clear(rocsparse_handle   handle,
                                                    rocsparse_mat_info info)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    // Logging
    rocsparse::log_trace(handle, "rocsparse_bsrilu0_clear", (const void*&)info);

    ROCSPARSE_CHECKARG_POINTER(1, info);

    info->clear_bsrilu0_info();

    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

#define CIMPL(NAME, U, V)                                                    \
    extern "C" rocsparse_status NAME(rocsparse_handle   handle,              \
                                     rocsparse_mat_info info,                \
                                     int                enable_boost,        \
                                     const U*           boost_tol,           \
                                     const V*           boost_val)           \
    try                                                                      \
    {                                                                        \
        ROCSPARSE_ROUTINE_TRACE;                                             \
        RETURN_IF_ROCSPARSE_ERROR(rocsparse::bsrilu0_numeric_boost_template( \
            handle, info, enable_boost, sizeof(U), boost_tol, boost_val));   \
        return rocsparse_status_success;                                     \
    }                                                                        \
    catch(...)                                                               \
    {                                                                        \
        RETURN_ROCSPARSE_EXCEPTION();                                        \
    }

CIMPL(rocsparse_sbsrilu0_numeric_boost, float, float);
CIMPL(rocsparse_dbsrilu0_numeric_boost, double, double);
CIMPL(rocsparse_cbsrilu0_numeric_boost, float, rocsparse_float_complex);
CIMPL(rocsparse_zbsrilu0_numeric_boost, double, rocsparse_double_complex);
CIMPL(rocsparse_dsbsrilu0_numeric_boost, double, float);
CIMPL(rocsparse_dcbsrilu0_numeric_boost, double, rocsparse_float_complex);
#undef CIMPL

extern "C" rocsparse_status rocsparse_bsrilu0_zero_pivot(rocsparse_handle   handle,
                                                         rocsparse_mat_info info,
                                                         rocsparse_int*     position)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);

    // Logging
    rocsparse::log_trace(
        handle, "rocsparse_bsrilu0_zero_pivot", (const void*&)info, (const void*&)position);

    ROCSPARSE_CHECKARG_POINTER(1, info);
    ROCSPARSE_CHECKARG_POINTER(2, position);

    auto bsrilu0_info = info->get_bsrilu0_info();
    {
        auto status = bsrilu0_info->copy_zero_pivot_async(handle->pointer_mode,
                                                          rocsparse::get_indextype<rocsparse_int>(),
                                                          position,
                                                          handle->stream);
        if(status == rocsparse_status_zero_pivot)
        {
            return status;
        }
        RETURN_IF_ROCSPARSE_ERROR(status);
    }
    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP
