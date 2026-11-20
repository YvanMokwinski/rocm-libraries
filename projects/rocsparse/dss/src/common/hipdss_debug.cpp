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

#include "hipdss.h"
#include "hipdss_control.hpp"
#include <cstring>

#include <hip/hip_runtime_api.h>

extern "C" hipdssStatus_t rocdss_get_version(hipdssHandle_t handle, int* version)
try
{

    HIPDSS_CHECKARG_HANDLE(0, handle);
    *version = HIPDSS_VERSION_MAJOR * 100000 + HIPDSS_VERSION_MINOR * 100
               + HIPDSS_VERSION_PATCH;

    return HIPDSS_STATUS_SUCCESS;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
// LCOV_EXCL_STOP

/********************************************************************************
 * \brief Get rocdss git revision
 *******************************************************************************/
extern "C" hipdssStatus_t rocdss_get_git_rev(hipdssHandle_t handle, char* rev)
try
{

    HIPDSS_CHECKARG_HANDLE(0, handle);
    HIPDSS_CHECKARG_POINTER(1, rev);

    //    static constexpr char v[] = TO_STR(ROCDSS_VERSION_TWEAK);
    static constexpr char v[] = "unknown";
    memcpy(rev, v, sizeof(v));

    return HIPDSS_STATUS_SUCCESS;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
// LCOV_EXCL_STOP

#include <mutex>

#include "hipdss_debug.hpp"

static std::mutex s_mutex;

bool hipdss::debug_variables_st::get_debug() const
{
    return this->debug;
}

bool hipdss::debug_variables_st::get_debug_force_host_assert() const
{
    return this->debug_force_host_assert;
}

bool hipdss::debug_variables_st::get_debug_verbose() const
{
    return this->debug_verbose;
}

bool hipdss::debug_variables_st::get_debug_kernel_launch() const
{
    return this->debug_kernel_launch;
}

bool hipdss::debug_variables_st::get_debug_arguments() const
{
    return this->debug_arguments;
}

bool hipdss::debug_variables_st::get_debug_warnings() const
{
    return this->debug_warnings;
}

bool hipdss::debug_variables_st::get_debug_arguments_verbose() const
{
    return this->debug_arguments_verbose;
}

void hipdss::debug_variables_st::set_debug(bool value)
{
    if(value != this->debug)
    {
        s_mutex.lock();
        this->debug = value;
        s_mutex.unlock();
    }
}

void hipdss::debug_variables_st::set_debug_verbose(bool value)
{
    if(value != this->debug_verbose)
    {
        s_mutex.lock();
        this->debug_verbose = value;
        s_mutex.unlock();
    }
}

void hipdss::debug_variables_st::set_debug_warnings(bool value)
{
    if(value != this->debug_warnings)
    {
        s_mutex.lock();
        this->debug_warnings = value;
        s_mutex.unlock();
    }
}

void hipdss::debug_variables_st::set_debug_force_host_assert(bool value)
{
    if(value != this->debug_force_host_assert)
    {
        s_mutex.lock();
        this->debug_force_host_assert = value;
        s_mutex.unlock();
    }
}

void hipdss::debug_variables_st::set_debug_arguments(bool value)
{
    if(value != this->debug_arguments)
    {
        s_mutex.lock();
        this->debug_arguments = value;
        s_mutex.unlock();
    }
}

void hipdss::debug_variables_st::set_debug_kernel_launch(bool value)
{
    if(value != this->debug_kernel_launch)
    {
        s_mutex.lock();
        this->debug_kernel_launch = value;
        s_mutex.unlock();
    }
}

void hipdss::debug_variables_st::set_debug_arguments_verbose(bool value)
{
    if(value != this->debug_arguments_verbose)
    {
        s_mutex.lock();
        this->debug_arguments_verbose = value;
        s_mutex.unlock();
    }
}

extern "C" {

int rocdss_state_debug_arguments_verbose()
{
    return hipdss_debug_variables.get_debug_arguments_verbose() ? 1 : 0;
}

int rocdss_state_debug_verbose()
{
    return hipdss_debug_variables.get_debug_verbose() ? 1 : 0;
}

int rocdss_state_debug_force_host_assert()
{
    return hipdss_debug_variables.get_debug_force_host_assert() ? 1 : 0;
}

int rocdss_state_debug_kernel_launch()
{
    return hipdss_debug_variables.get_debug_kernel_launch() ? 1 : 0;
}

int rocdss_state_debug_arguments()
{
    return hipdss_debug_variables.get_debug_arguments() ? 1 : 0;
}

int rocdss_state_debug()
{
    return hipdss_debug_variables.get_debug() ? 1 : 0;
}

void rocdss_enable_debug_warnings()
{
    hipdss_debug_variables.set_debug_warnings(true);
}

void rocdss_disable_debug_warnings()
{
    hipdss_debug_variables.set_debug_warnings(false);
}

void rocdss_enable_debug_arguments_verbose()
{
    hipdss_debug_variables.set_debug_arguments_verbose(true);
}

void rocdss_disable_debug_arguments_verbose()
{
    hipdss_debug_variables.set_debug_arguments_verbose(false);
}

void rocdss_enable_debug_kernel_launch()
{
    hipdss_debug_variables.set_debug_kernel_launch(true);
}

void rocdss_disable_debug_kernel_launch()
{
    hipdss_debug_variables.set_debug_kernel_launch(false);
}

void rocdss_enable_debug_arguments()
{
    hipdss_debug_variables.set_debug_arguments(true);
    rocdss_enable_debug_arguments_verbose();
}

void rocdss_disable_debug_arguments()
{
    hipdss_debug_variables.set_debug_arguments(false);
    rocdss_disable_debug_arguments_verbose();
}

void rocdss_enable_debug_verbose()
{
    hipdss_debug_variables.set_debug_verbose(true);
    rocdss_enable_debug_arguments_verbose();
    rocdss_enable_debug_warnings();
}

void rocdss_disable_debug_verbose()
{
    hipdss_debug_variables.set_debug_verbose(false);
    rocdss_disable_debug_arguments_verbose();
    rocdss_disable_debug_warnings();
}

void rocdss_enable_debug_force_host_assert()
{
    hipdss_debug_variables.set_debug_force_host_assert(true);
}

void rocdss_disable_debug_force_host_assert()
{
    hipdss_debug_variables.set_debug_force_host_assert(false);
}

void rocdss_enable_debug()
{
    hipdss_debug_variables.set_debug(true);
    rocdss_enable_debug_verbose();
    rocdss_enable_debug_arguments();
}

void rocdss_disable_debug()
{
    hipdss_debug_variables.set_debug(false);
    rocdss_disable_debug_verbose();
    rocdss_disable_debug_arguments();
}
}
