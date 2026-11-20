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

#include <rocsparse/rocsparse-types.h>

namespace hipdss
{

    struct spilu0_config_t
    {
    protected:
        rocsparse_spilu0_alg      m_alg{};
        rocsparse_analysis_policy m_analysis_policy{};
        rocsparse_datatype        m_compute_datatype{};
        rocsparse_pointer_mode    m_singularity_tolerance_pointer_mode{};
        const void*               m_singularity_tolerance{};
        double                    m_singularity_tolerance_host_value;

        int32_t                m_boost_enable{};
        rocsparse_pointer_mode m_boost_tolerance_pointer_mode{};
        const void*            m_boost_tolerance{};
        rocsparse_pointer_mode m_boost_value_pointer_mode{};
        const void*            m_boost_value{};

    public:
        rocsparse_spilu0_alg      get_alg() const;
        rocsparse_analysis_policy get_analysis_policy() const;
        rocsparse_datatype        get_compute_datatype() const;
        rocsparse_pointer_mode    get_singularity_tolerance_pointer_mode() const;
        const void*               get_singularity_tolerance() const;
        int32_t                   get_boost_enable() const;
        rocsparse_pointer_mode    get_boost_tolerance_pointer_mode() const;
        const void*               get_boost_tolerance() const;
        rocsparse_pointer_mode    get_boost_value_pointer_mode() const;
        const void*               get_boost_value() const;
        const void*               get_singularity_tolerance_host_value() const;

        void set_alg(rocsparse_spilu0_alg);
        void set_analysis_policy(rocsparse_analysis_policy);
        void set_compute_datatype(rocsparse_datatype);
        void set_singularity_tolerance_pointer_mode(rocsparse_pointer_mode);
        void set_singularity_tolerance(const void*);
        void set_boost_enable(int32_t);
        void set_boost_tolerance_pointer_mode(rocsparse_pointer_mode);
        void set_boost_tolerance(const void*);
        void set_boost_value_pointer_mode(rocsparse_pointer_mode);
        void set_boost_value(const void*);
        void set_singularity_tolerance_host_value(double);
    };

} // namespace hipdss
