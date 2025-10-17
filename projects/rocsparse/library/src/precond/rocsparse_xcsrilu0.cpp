/*! \file */
/* ************************************************************************
 * Copyright (C) 2018-2025 Advanced Micro Devices, Inc. All rights Reserved.
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

#include "csrilu0_device.h"
#include "internal/precond/rocsparse_csrilu0.h"
#include "rocsparse_csrilu0.hpp"
#include "rocsparse_utility.hpp"

namespace rocsparse
{
    template <typename T>
    static rocsparse_status csrilu0_numeric_boost_template(rocsparse_handle   handle,
                                                           rocsparse_mat_info info,
                                                           int                enable_boost,
                                                           size_t             boost_tol_size,
                                                           const void*        boost_tol,
                                                           const T*           boost_val)
    {
        ROCSPARSE_ROUTINE_TRACE;

        ROCSPARSE_CHECKARG_HANDLE(0, handle);
        ROCSPARSE_CHECKARG_POINTER(1, info);

        // Reset boost
        info->boost_enable = 0;

        // Numeric boost
        if(enable_boost)
        {
            // Check pointer arguments
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

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */

extern "C" rocsparse_status rocsparse_scsrilu0_numeric_boost(rocsparse_handle   handle,
                                                             rocsparse_mat_info info,
                                                             int                enable_boost,
                                                             const float*       boost_tol,
                                                             const float*       boost_val)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrilu0_numeric_boost_template(
        handle, info, enable_boost, sizeof(float), boost_tol, boost_val));
    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status rocsparse_dcsrilu0_numeric_boost(rocsparse_handle   handle,
                                                             rocsparse_mat_info info,
                                                             int                enable_boost,
                                                             const double*      boost_tol,
                                                             const double*      boost_val)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrilu0_numeric_boost_template(
        handle, info, enable_boost, sizeof(double), boost_tol, boost_val));
    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status
    rocsparse_ccsrilu0_numeric_boost(rocsparse_handle               handle,
                                     rocsparse_mat_info             info,
                                     int                            enable_boost,
                                     const float*                   boost_tol,
                                     const rocsparse_float_complex* boost_val)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrilu0_numeric_boost_template(
        handle, info, enable_boost, sizeof(float), boost_tol, boost_val));
    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status
    rocsparse_zcsrilu0_numeric_boost(rocsparse_handle                handle,
                                     rocsparse_mat_info              info,
                                     int                             enable_boost,
                                     const double*                   boost_tol,
                                     const rocsparse_double_complex* boost_val)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrilu0_numeric_boost_template(
        handle, info, enable_boost, sizeof(double), boost_tol, boost_val));
    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status rocsparse_dscsrilu0_numeric_boost(rocsparse_handle   handle,
                                                              rocsparse_mat_info info,
                                                              int                enable_boost,
                                                              const double*      boost_tol,
                                                              const float*       boost_val)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrilu0_numeric_boost_template(
        handle, info, enable_boost, sizeof(double), boost_tol, boost_val));
    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status
    rocsparse_dccsrilu0_numeric_boost(rocsparse_handle               handle,
                                      rocsparse_mat_info             info,
                                      int                            enable_boost,
                                      const double*                  boost_tol,
                                      const rocsparse_float_complex* boost_val)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrilu0_numeric_boost_template(
        handle, info, enable_boost, sizeof(double), boost_tol, boost_val));
    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status rocsparse_csrilu0_clear(rocsparse_handle   handle,
                                                    rocsparse_mat_info info)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, info);

    // Logging
    rocsparse::log_trace(handle, "rocsparse_csrilu0_clear", (const void*&)info);

    info->clear_csrilu0_info();
    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status rocsparse_csrilu0_zero_pivot(rocsparse_handle   handle,
                                                         rocsparse_mat_info info,
                                                         rocsparse_int*     position)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);

    rocsparse::log_trace(
        handle, "rocsparse_csrilu0_zero_pivot", (const void*&)info, (const void*&)position);

    ROCSPARSE_CHECKARG_POINTER(1, info);
    ROCSPARSE_CHECKARG_POINTER(2, position);

    auto csrilu0_info = info->get_csrilu0_info();
    {
        auto status = csrilu0_info->copy_zero_pivot_async(handle->pointer_mode,
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

extern "C" rocsparse_status rocsparse_csrilu0_singular_pivot(rocsparse_handle   handle,
                                                             rocsparse_mat_info info,
                                                             rocsparse_int*     position)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);

    rocsparse::log_trace(
        handle, "rocsparse_csrilu0_singular_pivot", (const void*&)info, (const void*&)position);

    ROCSPARSE_CHECKARG_POINTER(1, info);
    ROCSPARSE_CHECKARG_POINTER(2, position);

    auto    csrilu0_info = info->get_csrilu0_info();
    int64_t zero_pivot_value;
    auto    status = csrilu0_info->copy_zero_pivot_async(rocsparse_pointer_mode_host,
                                                      rocsparse::get_indextype<int64_t>(),
                                                      &zero_pivot_value,
                                                      handle->stream);
    if(status != rocsparse_status_zero_pivot)
    {
        RETURN_IF_ROCSPARSE_ERROR(status);
    }

    int64_t singular_pivot_value;
    status = csrilu0_info->copy_singular_pivot_async(rocsparse_pointer_mode_host,
                                                     rocsparse::get_indextype<int64_t>(),
                                                     &singular_pivot_value,
                                                     handle->stream);

    if(status != rocsparse_status_zero_pivot)
    {
        RETURN_IF_ROCSPARSE_ERROR(status);
    }

    RETURN_IF_HIP_ERROR(hipStreamSynchronize(handle->stream));

    int64_t singular_pivot;
    if((singular_pivot_value == -1) || (zero_pivot_value == -1))
    {
        singular_pivot = ((singular_pivot_value == -1) ? zero_pivot_value : singular_pivot_value);
    }
    else
    {
        singular_pivot = std::min(zero_pivot_value, singular_pivot_value);
    }

    // Differentiate between pointer modes
    if(handle->pointer_mode == rocsparse_pointer_mode_device)
    {
        // rocsparse_pointer_mode_device
        RETURN_IF_HIP_ERROR(hipMemcpyAsync(position,
                                           &singular_pivot,
                                           sizeof(rocsparse_int),
                                           hipMemcpyHostToDevice,
                                           handle->stream));
        RETURN_IF_HIP_ERROR(hipStreamSynchronize(handle->stream));
    }
    else
    {
        // rocsparse_pointer_mode_host
        *position = singular_pivot;
    }

    return (rocsparse_status_success);
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status
    rocsparse_csrilu0_set_tolerance(rocsparse_handle handle, rocsparse_mat_info info, double tol)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, info);
    ROCSPARSE_CHECKARG(2, tol, (tol < 0), rocsparse_status_invalid_value);

    // Logging
    rocsparse::log_trace(handle, "rocsparse_csrilu0_set_tolerance", (const void*&)info, tol);
    auto csrilu0_info = info->get_csrilu0_info();
    csrilu0_info->set_singular_tol(tol);

    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status
    rocsparse_csrilu0_get_tolerance(rocsparse_handle handle, rocsparse_mat_info info, double* tol)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, info);
    ROCSPARSE_CHECKARG_POINTER(2, tol);

    // Logging
    rocsparse::log_trace(handle, "rocsparse_csrilu0_get_tolerance", (const void*&)info, tol);
    const auto csrilu0_info = info->get_csrilu0_info();
    *tol                    = csrilu0_info->get_singular_tol();

    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP
