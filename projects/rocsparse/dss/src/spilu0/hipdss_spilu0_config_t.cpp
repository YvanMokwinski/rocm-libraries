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

#include "hipdss_spilu0_config_t.hpp"

rocsparse_spilu0_alg hipdss::spilu0_config_t::get_alg() const
{
    return this->m_alg;
}

rocsparse_analysis_policy hipdss::spilu0_config_t::get_analysis_policy() const
{
    return this->m_analysis_policy;
}

rocsparse_datatype hipdss::spilu0_config_t::get_compute_datatype() const
{
    return this->m_compute_datatype;
}

rocsparse_pointer_mode
    hipdss::spilu0_config_t::get_singularity_tolerance_pointer_mode() const
{
    return this->m_singularity_tolerance_pointer_mode;
}

const void* hipdss::spilu0_config_t::get_singularity_tolerance() const
{
    return this->m_singularity_tolerance;
}

int32_t hipdss::spilu0_config_t::get_boost_enable() const
{
    return this->m_boost_enable;
}

rocsparse_pointer_mode
    hipdss::spilu0_config_t::get_boost_tolerance_pointer_mode() const
{
    return this->m_boost_tolerance_pointer_mode;
}

const void* hipdss::spilu0_config_t::get_boost_tolerance() const
{
    return this->m_boost_tolerance;
}

rocsparse_pointer_mode
    hipdss::spilu0_config_t::get_boost_value_pointer_mode() const
{
    return this->m_boost_value_pointer_mode;
}

const void* hipdss::spilu0_config_t::get_boost_value() const
{
    return this->m_boost_value;
}

const void* hipdss::spilu0_config_t::get_singularity_tolerance_host_value() const
{
    return &this->m_singularity_tolerance_host_value;
}

void hipdss::spilu0_config_t::set_alg(rocsparse_spilu0_alg value)
{
    this->m_alg = value;
}

void hipdss::spilu0_config_t::set_analysis_policy(rocsparse_analysis_policy value)
{
    this->m_analysis_policy = value;
}

void hipdss::spilu0_config_t::set_compute_datatype(rocsparse_datatype value)
{
    this->m_compute_datatype = value;
}

void hipdss::spilu0_config_t::set_singularity_tolerance_pointer_mode(rocsparse_pointer_mode value)
{
    this->m_singularity_tolerance_pointer_mode = value;
}

void hipdss::spilu0_config_t::set_singularity_tolerance(const void* value)
{
    this->m_singularity_tolerance = value;
}

void hipdss::spilu0_config_t::set_boost_enable(int32_t value)
{
    this->m_boost_enable = value;
}

void hipdss::spilu0_config_t::set_boost_tolerance_pointer_mode(rocsparse_pointer_mode value)
{
    this->m_boost_tolerance_pointer_mode = value;
}

void hipdss::spilu0_config_t::set_boost_tolerance(const void* value)
{
    this->m_boost_tolerance = value;
}

void hipdss::spilu0_config_t::set_boost_value_pointer_mode(rocsparse_pointer_mode value)
{
    this->m_boost_value_pointer_mode = value;
}

void hipdss::spilu0_config_t::set_boost_value(const void* value)
{
    this->m_boost_value = value;
}

void hipdss::spilu0_config_t::set_singularity_tolerance_host_value(double value)
{
    this->m_singularity_tolerance_host_value = value;
}
