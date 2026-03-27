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

#include "rocsparse_spequilibrate_descr.hpp"
#include "rocsparse_utility.hpp"

rocsparse_spequilibrate_stage _rocsparse_spequilibrate_ruiz_descr::get_stage() const
{
  return this->m_stage;
}

void _rocsparse_spequilibrate_ruiz_descr::set_stage(rocsparse_spequilibrate_stage value)
{
  this->m_stage= value;
}

rocsparse_spequilibrate_alg _rocsparse_spequilibrate_ruiz_descr::get_alg() const
{
    return this->m_alg;
}

void _rocsparse_spequilibrate_ruiz_descr::set_alg(rocsparse_spequilibrate_alg value)
{
  this->m_alg = value;
}

rocsparse_datatype _rocsparse_spequilibrate_ruiz_descr::get_compute_datatype() const
{
    return this->m_compute_datatype;
}

void _rocsparse_spequilibrate_ruiz_descr::set_compute_datatype(rocsparse_datatype value)
{
    this->m_compute_datatype = value;
}

void _rocsparse_spequilibrate_ruiz_descr::set_nrm(double value)
{
    this->m_nrm = value;
}

double _rocsparse_spequilibrate_ruiz_descr::get_nrm() const
{
  return this->m_nrm;
}

void _rocsparse_spequilibrate_ruiz_descr::set_tol(double value)
{
    this->m_tol = value;
}

double _rocsparse_spequilibrate_ruiz_descr::get_tol() const
{
  return this->m_tol;
}

void _rocsparse_spequilibrate_ruiz_descr::set_niter(int64_t value)
{
    this->m_niter = value;
}

int64_t _rocsparse_spequilibrate_ruiz_descr::get_niter() const
{
  return this->m_niter;
}

void _rocsparse_spequilibrate_ruiz_descr::set_nmaxiter(int64_t value)
{
    this->m_nmaxiter = value;
}

int64_t _rocsparse_spequilibrate_ruiz_descr::get_nmaxiter() const
{
  return this->m_nmaxiter;
}



_rocsparse_spequilibrate_ruiz_descr::~_rocsparse_spequilibrate_ruiz_descr()
{
  this->m_stage            = ((rocsparse_spequilibrate_stage)-1);
  this->m_alg              = ((rocsparse_spequilibrate_alg)-1);
  this->m_compute_datatype = ((rocsparse_datatype)-1);
  this->m_nmaxiter = -1;
  this->m_niter = -1;
  this->m_tol = -1.0;
  this->m_nrm = -1.0;

}

_rocsparse_spequilibrate_ruiz_descr::_rocsparse_spequilibrate_ruiz_descr()
  : m_stage((rocsparse_spequilibrate_stage)-1)
  , m_alg((rocsparse_spequilibrate_alg)-1)
  , m_compute_datatype((rocsparse_datatype)-1)
  , m_nmaxiter(-1)
  , m_niter(-1)
  , m_tol(-1.0)
  , m_nrm(-1.0)
{
}

rocsparse_spequilibrate_stage _rocsparse_spequilibrate_descr::get_stage() const
{
  return this->m_impl.get_stage();
}

void _rocsparse_spequilibrate_descr::set_stage(rocsparse_spequilibrate_stage value)
{
  this->m_impl.set_stage(value);
}

rocsparse_spequilibrate_alg _rocsparse_spequilibrate_descr::get_alg() const
{
    return this->m_impl.get_alg();
}

void _rocsparse_spequilibrate_descr::set_alg(rocsparse_spequilibrate_alg value)
{
    this->m_impl.set_alg (value);
}

rocsparse_datatype _rocsparse_spequilibrate_descr::get_compute_datatype() const
{
    return this->m_impl.get_compute_datatype();
}

void _rocsparse_spequilibrate_descr::set_compute_datatype(rocsparse_datatype value)
{
    this->m_impl.set_compute_datatype (value);
}

void _rocsparse_spequilibrate_descr::set_nrm(double value)
{
    this->m_impl.set_nrm (value);
}

double _rocsparse_spequilibrate_descr::get_nrm() const
{
  return this->m_impl.get_nrm();
}

void _rocsparse_spequilibrate_descr::set_tol(double value)
{
    this->m_impl.set_tol (value);
}

double _rocsparse_spequilibrate_descr::get_tol() const
{
  return this->m_impl.get_tol();
}

void _rocsparse_spequilibrate_descr::set_niter(int64_t value)
{
    this->m_impl.set_niter (value);
}

int64_t _rocsparse_spequilibrate_descr::get_niter() const
{
  return this->m_impl.get_niter();
}

void _rocsparse_spequilibrate_descr::set_nmaxiter(int64_t value)
{
    this->m_impl.set_nmaxiter (value);
}

int64_t _rocsparse_spequilibrate_descr::get_nmaxiter() const
{
  return this->m_impl.get_nmaxiter();
}



extern "C" rocsparse_status rocsparse_spequilibrate_descr_create(rocsparse_handle handle,
								 rocsparse_spequilibrate_descr*p_descr,
								 rocsparse_error*p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;
    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, p_descr);
    p_descr[0] = new _rocsparse_spequilibrate_descr();
    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status rocsparse_spequilibrate_descr_destroy(rocsparse_handle handle,
								  rocsparse_spequilibrate_descr descr,
								  rocsparse_error*p_error)
  try
    {
      ROCSPARSE_ROUTINE_TRACE;
      if(descr != nullptr)
	{
	  delete descr;
	}
      return rocsparse_status_success;
    // LCOV_EXCL_START
}
  catch(...)
    {
      RETURN_ROCSPARSE_EXCEPTION();
    }
// LCOV_EXCL_STOP


