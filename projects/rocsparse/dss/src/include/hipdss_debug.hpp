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

#include "hipdss_envariables.hpp"

namespace hipdss
{
    ///
    /// @brief Structure to store debug global variables.
    ///
    struct debug_variables_st
    {
    private:
        bool debug;
        bool debug_arguments;
        bool debug_verbose;
        bool debug_arguments_verbose;
        bool debug_kernel_launch;
        bool debug_force_host_assert;
        bool debug_warnings;

    public:
        bool get_debug() const;
        bool get_debug_verbose() const;
        bool get_debug_kernel_launch() const;
        bool get_debug_arguments() const;
        bool get_debug_arguments_verbose() const;
        bool get_debug_force_host_assert() const;
        bool get_debug_warnings() const;

        void set_debug(bool value);
        void set_debug_verbose(bool value);
        void set_debug_arguments(bool value);
        void set_debug_kernel_launch(bool value);
        void set_debug_arguments_verbose(bool value);
        void set_debug_force_host_assert(bool value);
        void set_debug_warnings(bool value);
    };

    struct debug_st
    {
    private:
        debug_variables_st m_var{};

    public:
        static debug_st& instance()
        {
            static debug_st self;
            return self;
        }

        static debug_variables_st& var()
        {
            return instance().m_var;
        }

        ~debug_st() = default;

    private:
        debug_st()
        {
            const bool debug = HIPDSS_ENVARIABLES.get(hipdss::envariables::DEBUG);
            m_var.set_debug(debug);

            const bool debug_arguments
                = (!getenv(hipdss::envariables::names[hipdss::envariables::DEBUG_ARGUMENTS]))
                      ? debug
                      : HIPDSS_ENVARIABLES.get(hipdss::envariables::DEBUG);
            m_var.set_debug_arguments(debug_arguments);

            m_var.set_debug_verbose(
                (!getenv(hipdss::envariables::names[hipdss::envariables::DEBUG_VERBOSE]))
                    ? debug
                    : HIPDSS_ENVARIABLES.get(hipdss::envariables::DEBUG_VERBOSE));
            m_var.set_debug_arguments_verbose(
                (!getenv(
                    hipdss::envariables::names[hipdss::envariables::DEBUG_ARGUMENTS_VERBOSE]))
                    ? debug_arguments
                    : HIPDSS_ENVARIABLES.get(hipdss::envariables::DEBUG_ARGUMENTS_VERBOSE));

            m_var.set_debug_force_host_assert(
                (!getenv(
                    hipdss::envariables::names[hipdss::envariables::DEBUG_FORCE_HOST_ASSERT]))
                    ? debug
                    : HIPDSS_ENVARIABLES.get(hipdss::envariables::DEBUG_FORCE_HOST_ASSERT));

            m_var.set_debug_verbose(
                (!getenv(hipdss::envariables::names[hipdss::envariables::DEBUG_WARNINGS]))
                    ? debug
                    : HIPDSS_ENVARIABLES.get(hipdss::envariables::DEBUG_WARNINGS));

            const bool debug_kernel_launch
                = HIPDSS_ENVARIABLES.get(hipdss::envariables::DEBUG_KERNEL_LAUNCH);
            m_var.set_debug_kernel_launch(debug_kernel_launch);
        };
    };

#define hipdss_debug_variables hipdss::debug_st::instance().var()
}
