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

#include "rocsparse_spic0_descr.hpp"
#include "rocsparse_control.hpp"
#include "rocsparse_logging.hpp"

void _rocsparse_spic0_descr::set_csric0_info(rocsparse_csric0_info value)
{
    this->m_csric0_info = std::shared_ptr<_rocsparse_csric0_info>(value);
}

rocsparse_csric0_info _rocsparse_spic0_descr::get_csric0_info()
{
    return this->m_csric0_info.get();
}
void _rocsparse_spic0_descr::set_shared_csric0_info(std::shared_ptr<_rocsparse_csric0_info> value)
{
    this->m_csric0_info = value;
}

void _rocsparse_spic0_descr::set_bsric0_info(rocsparse_bsric0_info value)
{
    this->m_bsric0_info = std::shared_ptr<_rocsparse_bsric0_info>(value);
}

rocsparse_bsric0_info _rocsparse_spic0_descr::get_bsric0_info()
{
    return this->m_bsric0_info.get();
}
void _rocsparse_spic0_descr::set_shared_bsric0_info(std::shared_ptr<_rocsparse_bsric0_info> value)
{
    this->m_bsric0_info = value;
}

_rocsparse_spic0_descr::~_rocsparse_spic0_descr()
{
    m_stage              = ((rocsparse_spic0_stage)-1);
    m_alg                = ((rocsparse_spic0_alg)-1);
    m_compute_datatype   = ((rocsparse_datatype)-1);
    m_analysis_policy    = ((rocsparse_analysis_policy)-1);
    m_boost_enable       = 0;
    m_boost_tol_datatype = (rocsparse_datatype)-1;
    m_boost_tol          = nullptr;
    m_boost_val          = nullptr;
    m_singular_tol       = 0;

    this->m_csric0_info.reset();
}

_rocsparse_spic0_descr::_rocsparse_spic0_descr()
    : m_stage((rocsparse_spic0_stage)-1)
    , m_alg((rocsparse_spic0_alg)-1)
    , m_compute_datatype((rocsparse_datatype)-1)
    , m_analysis_policy((rocsparse_analysis_policy)-1)
    , m_boost_enable(0)
    , m_boost_tol_datatype((rocsparse_datatype)-1)
    , m_boost_tol(nullptr)
    , m_boost_val(nullptr)
    , m_singular_tol(0)
{
}

rocsparse_spic0_stage _rocsparse_spic0_descr::get_stage() const
{
    return this->m_stage;
}

rocsparse_spic0_alg _rocsparse_spic0_descr::get_alg() const
{
    return this->m_alg;
}

rocsparse_analysis_policy _rocsparse_spic0_descr::get_analysis_policy() const
{
    return this->m_analysis_policy;
}

void _rocsparse_spic0_descr::set_analysis_policy(rocsparse_analysis_policy value)
{
    this->m_analysis_policy = value;
}

rocsparse_datatype _rocsparse_spic0_descr::get_compute_datatype() const
{
    return this->m_compute_datatype;
}

void _rocsparse_spic0_descr::set_stage(rocsparse_spic0_stage value)
{
    this->m_stage = value;
}

void _rocsparse_spic0_descr::set_alg(rocsparse_spic0_alg value)
{
    this->m_alg = value;
}

void _rocsparse_spic0_descr::set_compute_datatype(rocsparse_datatype value)
{
    this->m_compute_datatype = value;
}

int32_t _rocsparse_spic0_descr::get_boost_enable() const
{
    return this->m_boost_enable;
}
rocsparse_datatype _rocsparse_spic0_descr::get_boost_tol_datatype() const
{
    return this->m_boost_tol_datatype;
}
const void* _rocsparse_spic0_descr::get_boost_tol() const
{
    return this->m_boost_tol;
}
const void* _rocsparse_spic0_descr::get_boost_val() const
{
    return this->m_boost_val;
}

void _rocsparse_spic0_descr::set_boost_enable(int32_t value)
{
    this->m_boost_enable = value;
}
void _rocsparse_spic0_descr::set_boost_tol_datatype(rocsparse_datatype value)
{
    this->m_boost_tol_datatype = value;
}
void _rocsparse_spic0_descr::set_boost_tol(const void* value)
{
    this->m_boost_tol = value;
}
void _rocsparse_spic0_descr::set_boost_val(const void* value)
{
    this->m_boost_val = value;
}

double _rocsparse_spic0_descr::get_singular_tol() const
{
    return this->m_singular_tol;
}
void _rocsparse_spic0_descr::set_singular_tol(double value)
{
    this->m_singular_tol = value;
}

extern "C" rocsparse_status rocsparse_create_spic0_descr(rocsparse_handle       handle,
                                                         rocsparse_spic0_descr* descr,
                                                         rocsparse_error*       p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;
    ROCSPARSE_CHECKARG_POINTER(0, descr);
    *descr = new _rocsparse_spic0_descr();
    return rocsparse_status_success;
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}

extern "C" rocsparse_status rocsparse_destroy_spic0_descr(rocsparse_handle      handle,
                                                          rocsparse_spic0_descr descr,
                                                          rocsparse_error*      p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;
    if(descr != nullptr)
    {
        delete descr;
    }
    return rocsparse_status_success;
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
