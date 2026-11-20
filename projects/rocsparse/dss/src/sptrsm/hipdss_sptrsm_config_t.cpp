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

#include "hipdss_sptrsm_config_t.hpp"

rocsparse_sptrsm_alg hipdss::sptrsm_config_t::get_alg() const
{
    return this->m_alg;
}

rocsparse_operation hipdss::sptrsm_config_t::get_op_A() const
{
    return this->m_op_A;
}

rocsparse_operation hipdss::sptrsm_config_t::get_op_X() const
{
    return this->m_op_X;
}

rocsparse_analysis_policy hipdss::sptrsm_config_t::get_analysis_policy() const
{
    return this->m_analysis_policy;
}

rocsparse_datatype hipdss::sptrsm_config_t::get_compute_datatype() const
{
    return this->m_compute_datatype;
}

rocsparse_datatype hipdss::sptrsm_config_t::get_scalar_datatype() const
{
    return this->m_scalar_datatype;
}

void hipdss::sptrsm_config_t::set_alg(rocsparse_sptrsm_alg value)
{
    this->m_alg = value;
}

void hipdss::sptrsm_config_t::set_op_A(rocsparse_operation value)
{
    this->m_op_A = value;
}

void hipdss::sptrsm_config_t::set_op_X(rocsparse_operation value)
{
    this->m_op_X = value;
}

void hipdss::sptrsm_config_t::set_analysis_policy(rocsparse_analysis_policy value)
{
    this->m_analysis_policy = value;
}

void hipdss::sptrsm_config_t::set_compute_datatype(rocsparse_datatype value)
{
    this->m_compute_datatype = value;
}

void hipdss::sptrsm_config_t::set_scalar_datatype(rocsparse_datatype value)
{
    this->m_scalar_datatype = value;
}
