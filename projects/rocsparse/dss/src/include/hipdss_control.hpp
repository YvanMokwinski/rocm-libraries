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

#pragma once

#include "hipdss_argdescr.hpp"
#include "hipdss_log.hpp"
#include <hip/hip_runtime.h>
#include <iostream>
#include <rocsparse/rocsparse-types.h>
/*******************************************************************************
 * Definitions
 * this file to not include any others
 * thereby it can include top-level definitions included by all
 ******************************************************************************/

//
// @brief Macros for coverage exclusion
//
#define HIPDSS_COV_EXCL_START (void)("LCOV_EXCL_START")
#define HIPDSS_COV_EXCL_STOP (void)("LCOV_EXCL_STOP")

hipdssStatus_t rocsparse_status2hipdss_status(rocsparse_status);

namespace hipdss
{
    hipdssStatus_t exception_to_hipdss_status(std::exception_ptr e = std::current_exception());
    /*******************************************************************************
     * \brief convert hipError_t to hipdssStatus_t
     ******************************************************************************/
    hipdssStatus_t get_hipdss_status_for_hip_status(hipError_t status);
}

//
//
//
#define RETURN_IF_HIP_ERROR(INPUT_STATUS_FOR_CHECK)                                  \
    do                                                                               \
    {                                                                                \
        const hipError_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK);            \
        if(TMP_STATUS_FOR_CHECK != hipSuccess)                                       \
        {                                                                            \
            std::stringstream s;                                                     \
            s << "hip error detected: code '" << TMP_STATUS_FOR_CHECK << "', name '" \
              << hipGetErrorName(TMP_STATUS_FOR_CHECK) << "', description '"         \
              << hipGetErrorString(TMP_STATUS_FOR_CHECK) << "'";                     \
            HIPDSS_ERROR_MESSAGE(                                                    \
                hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK),      \
                s.str().c_str());                                                    \
            return hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK);   \
        }                                                                            \
    } while(false)

#define RETURN_WITH_MESSAGE_IF_HIP_ERROR(INPUT_STATUS_FOR_CHECK, MESSAGE)                   \
    do                                                                                      \
    {                                                                                       \
        const hipError_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK);                   \
        if(TMP_STATUS_FOR_CHECK != hipSuccess)                                              \
        {                                                                                   \
            std::stringstream s;                                                            \
            s << (MESSAGE) << ", hip error detected: code '" << TMP_STATUS_FOR_CHECK        \
              << "', name '" << hipGetErrorName(TMP_STATUS_FOR_CHECK) << "', description '" \
              << hipGetErrorString(TMP_STATUS_FOR_CHECK) << "'";                            \
            HIPDSS_ERROR_MESSAGE(                                                           \
                hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK),             \
                s.str().c_str());                                                           \
            return hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK);          \
        }                                                                                   \
    } while(false)

//
//
//

#define THROW_IF_HIP_ERROR(INPUT_STATUS_FOR_CHECK)                                              \
    do                                                                                          \
    {                                                                                           \
        const hipError_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK);                       \
        if(TMP_STATUS_FOR_CHECK != hipSuccess)                                                  \
        {                                                                                       \
            std::stringstream s;                                                                \
            s << "throwing exception due to hip error detected: code '" << TMP_STATUS_FOR_CHECK \
              << "', name '" << hipGetErrorName(TMP_STATUS_FOR_CHECK) << "', description '"     \
              << hipGetErrorString(TMP_STATUS_FOR_CHECK) << "'";                                \
            HIPDSS_ERROR_MESSAGE(                                                               \
                hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK),                 \
                s.str().c_str());                                                               \
            throw hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK);               \
        }                                                                                       \
    } while(false)

#define THROW_WITH_MESSAGE_IF_HIP_ERROR(INPUT_STATUS_FOR_CHECK, MESSAGE)                      \
    do                                                                                        \
    {                                                                                         \
        const hipError_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK);                     \
        if(TMP_STATUS_FOR_CHECK != hipSuccess)                                                \
        {                                                                                     \
            std::stringstream s;                                                              \
            s << (MESSAGE) << ", throwing exception due to hip error detected: code '"        \
              << TMP_STATUS_FOR_CHECK << "', name '" << hipGetErrorName(TMP_STATUS_FOR_CHECK) \
              << "', description '" << hipGetErrorString(TMP_STATUS_FOR_CHECK) << "'";        \
            HIPDSS_ERROR_MESSAGE(                                                             \
                hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK),               \
                s.str().c_str());                                                             \
            throw hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK);             \
        }                                                                                     \
    } while(false)

//
//
//
#define WARNING_IF_HIP_ERROR(INPUT_STATUS_FOR_CHECK)                      \
    do                                                                    \
    {                                                                     \
        const hipError_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK); \
        if(TMP_STATUS_FOR_CHECK != hipSuccess)                            \
        {                                                                 \
            HIPDSS_WARNING_MESSAGE("hip error detected");                 \
        }                                                                 \
    } while(false)

#define FORWARD_IF_HIP_ERROR(INPUT_STATUS_FOR_CHECK)                                 \
    do                                                                               \
    {                                                                                \
        const hipError_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK);            \
        if(TMP_STATUS_FOR_CHECK != hipSuccess)                                       \
        {                                                                            \
            std::stringstream s;                                                     \
            s << "hip error detected: code '" << TMP_STATUS_FOR_CHECK << "', name '" \
              << hipGetErrorName(TMP_STATUS_FOR_CHECK) << "', description '"         \
              << hipGetErrorString(TMP_STATUS_FOR_CHECK) << "'";                     \
            HIPDSS_ERROR_MESSAGE(                                                    \
                hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK),      \
                s.str().c_str());                                                    \
            return TMP_STATUS_FOR_CHECK;                                             \
        }                                                                            \
    } while(false)

#define FORWARD_WITH_MESSAGE_IF_HIP_ERROR(INPUT_STATUS_FOR_CHECK, MESSAGE)                  \
    do                                                                                      \
    {                                                                                       \
        const hipError_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK);                   \
        if(TMP_STATUS_FOR_CHECK != hipSuccess)                                              \
        {                                                                                   \
            std::stringstream s;                                                            \
            s << (MESSAGE) << ", hip error detected: code '" << TMP_STATUS_FOR_CHECK        \
              << "', name '" << hipGetErrorName(TMP_STATUS_FOR_CHECK) << "', description '" \
              << hipGetErrorString(TMP_STATUS_FOR_CHECK) << "'";                            \
            HIPDSS_ERROR_MESSAGE(                                                           \
                hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK),             \
                s.str().c_str());                                                           \
            return TMP_STATUS_FOR_CHECK;                                                    \
        }                                                                                   \
    } while(false)

#define THROW_IF_ROCSPARSE_ERROR(INPUT_STATUS_FOR_CHECK)                                        \
    do                                                                                          \
    {                                                                                           \
        const auto TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK);                             \
        if(TMP_STATUS_FOR_CHECK != rocsparse_status_success)                                    \
        {                                                                                       \
            HIPDSS_ERROR_MESSAGE(rocsparse_status2hipdss_status(TMP_STATUS_FOR_CHECK), "none"); \
            throw TMP_STATUS_FOR_CHECK;                                                         \
        }                                                                                       \
    } while(false)

//
//
//
#define RETURN_IF_ROCSPARSE_ERROR(INPUT_STATUS_FOR_CHECK)                                       \
    do                                                                                          \
    {                                                                                           \
        const rocsparse_status TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK);                 \
        if(TMP_STATUS_FOR_CHECK != rocsparse_status_success)                                    \
        {                                                                                       \
            HIPDSS_ERROR_MESSAGE(rocsparse_status2hipdss_status(TMP_STATUS_FOR_CHECK), "none"); \
            return HIPDSS_STATUS_INTERNAL_ERROR;                                                \
        }                                                                                       \
    } while(false)

#define RETURN_IF_HIPDSS_ERROR(INPUT_STATUS_FOR_CHECK)                        \
    do                                                                        \
    {                                                                         \
        const hipdssStatus_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK); \
        if(TMP_STATUS_FOR_CHECK != HIPDSS_STATUS_SUCCESS)                     \
        {                                                                     \
            HIPDSS_ERROR_MESSAGE(TMP_STATUS_FOR_CHECK, "none");               \
            return TMP_STATUS_FOR_CHECK;                                      \
        }                                                                     \
    } while(false)

#define WARNING_IF_HIPDSS_ERROR(INPUT_STATUS_FOR_CHECK)                          \
    do                                                                           \
    {                                                                            \
        const hipdssStatus_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK);    \
        if(TMP_STATUS_FOR_CHECK != HIPDSS_STATUS_SUCCESS)                        \
        {                                                                        \
            std::stringstream s;                                                 \
            s << "HIPDSS error detected: code '" << TMP_STATUS_FOR_CHECK << "'"; \
            HIPDSS_WARNING_MESSAGE(s.str().c_str());                             \
        }                                                                        \
    } while(false)

//
//
//
#define THROW_IF_HIPDSS_ERROR(INPUT_STATUS_FOR_CHECK)                          \
    do                                                                         \
    {                                                                          \
        const hipdssStatus_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK);  \
        if(TMP_STATUS_FOR_CHECK != HIPDSS_STATUS_SUCCESS)                      \
        {                                                                      \
            HIPDSS_ERROR_MESSAGE(TMP_STATUS_FOR_CHECK,                         \
                                 "HIPDSS error detected, throwing exception"); \
            throw TMP_STATUS_FOR_CHECK;                                        \
        }                                                                      \
    } while(false)

//
//
//
#define RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(INPUT_STATUS_FOR_CHECK, MSG)      \
    do                                                                        \
    {                                                                         \
        const hipdssStatus_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK); \
        if(TMP_STATUS_FOR_CHECK != HIPDSS_STATUS_SUCCESS)                     \
        {                                                                     \
            HIPDSS_ERROR_MESSAGE(TMP_STATUS_FOR_CHECK, MSG);                  \
            return TMP_STATUS_FOR_CHECK;                                      \
        }                                                                     \
    } while(false)

//
//
//
#define THROW_WITH_MESSAGE_IF_HIPDSS_ERROR(INPUT_STATUS_FOR_CHECK, MESSAGE)   \
    do                                                                        \
    {                                                                         \
        const hipdssStatus_t TMP_STATUS_FOR_CHECK = (INPUT_STATUS_FOR_CHECK); \
        if(TMP_STATUS_FOR_CHECK != HIPDSS_STATUS_SUCCESS)                     \
        {                                                                     \
            HIPDSS_ERROR_MESSAGE(TMP_STATUS_FOR_CHECK, MESSAGE);              \
            throw TMP_STATUS_FOR_CHECK;                                       \
        }                                                                     \
    } while(false)

//
//
//
#define RETURN_HIPDSS_EXCEPTION()                                               \
    do                                                                          \
    {                                                                           \
        const hipdssStatus_t TMP_STATUS = hipdss::exception_to_hipdss_status(); \
        HIPDSS_ERROR_MESSAGE(TMP_STATUS, "exception detected");                 \
        return TMP_STATUS;                                                      \
    } while(false)

//
//
//
#define THROW_IF_HIPLAUNCHKERNELGGL_ERROR(...)                                                 \
    do                                                                                         \
    {                                                                                          \
        if(false == hipdss_debug_variables.get_debug_kernel_launch())                          \
        {                                                                                      \
            hipLaunchKernelGGL(__VA_ARGS__);                                                   \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            THROW_WITH_MESSAGE_IF_HIP_ERROR(hipGetLastError(), "prior to hipLaunchKernelGGL"); \
            hipLaunchKernelGGL(__VA_ARGS__);                                                   \
            THROW_IF_HIP_ERROR(hipGetLastError());                                             \
        }                                                                                      \
    } while(false)

#define RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(...)                                                 \
    do                                                                                          \
    {                                                                                           \
        if(false == hipdss_debug_variables.get_debug_kernel_launch())                           \
        {                                                                                       \
            hipLaunchKernelGGL(__VA_ARGS__);                                                    \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            RETURN_WITH_MESSAGE_IF_HIP_ERROR(hipGetLastError(), "prior to hipLaunchKernelGGL"); \
            hipLaunchKernelGGL(__VA_ARGS__);                                                    \
            RETURN_IF_HIP_ERROR(hipGetLastError());                                             \
        }                                                                                       \
    } while(false)

#define RETURN_HIPDSS_ERROR_IF(STATUS, INPUT_STATUS_FOR_CHECK)     \
    do                                                             \
    {                                                              \
        if(INPUT_STATUS_FOR_CHECK)                                 \
        {                                                          \
            HIPDSS_ERROR_MESSAGE(STATUS, #INPUT_STATUS_FOR_CHECK); \
            return STATUS;                                         \
        }                                                          \
    } while(false)

#define PRINT_IF_HIP_ERROR(INPUT_STATUS_FOR_CHECK)                                   \
    {                                                                                \
        hipError_t TMP_STATUS_FOR_CHECK = INPUT_STATUS_FOR_CHECK;                    \
        if(TMP_STATUS_FOR_CHECK != hipSuccess)                                       \
        {                                                                            \
            std::stringstream s;                                                     \
            s << "hip error detected: code '" << TMP_STATUS_FOR_CHECK << "', name '" \
              << hipGetErrorName(TMP_STATUS_FOR_CHECK) << "', description '"         \
              << hipGetErrorString(TMP_STATUS_FOR_CHECK) << "'";                     \
            HIPDSS_ERROR_MESSAGE(                                                    \
                hipdss::get_hipdss_status_for_hip_status(TMP_STATUS_FOR_CHECK),      \
                s.str().c_str());                                                    \
        }                                                                            \
    }
