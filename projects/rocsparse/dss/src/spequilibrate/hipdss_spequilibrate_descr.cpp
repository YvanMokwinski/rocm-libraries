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

#include "hipdss_spequilibrate_descr.hpp"
#include "hipdssContext.hpp"
#include "hipdss_control.hpp"

#if 0
void  _hipdss_spequilibrate_descr::set_batch_tolerances(rocsparse_const_dnvec_descr that) {  this->m_impl.set_batch_tolerances(that); }
rocsparse_const_dnvec_descr _hipdss_spequilibrate_descr::get_batch_tolerances()const {  return  this->m_impl.get_batch_tolerances();  }
#endif

void _hipdss_spequilibrate_ruiz_descr::set_batch_tolerances(
    rocsparse_const_dnvec_descr that)
{
    this->p_batch_tolerances = that;
}
rocsparse_const_dnvec_descr
    _hipdss_spequilibrate_ruiz_descr::get_batch_tolerances() const
{
    return this->p_batch_tolerances;
}

hipdss_spequilibrate_stage _hipdss_spequilibrate_ruiz_descr::get_stage() const
{
    return this->m_stage;
}

void _hipdss_spequilibrate_ruiz_descr::set_stage(
    hipdss_spequilibrate_stage value)
{
    this->m_stage = value;
}

rocsparse_datatype
    _hipdss_spequilibrate_ruiz_descr::get_compute_datatype() const
{
    return this->m_compute_datatype;
}

void _hipdss_spequilibrate_ruiz_descr::set_compute_datatype(
    rocsparse_datatype value)
{
    this->m_compute_datatype = value;
}

void _hipdss_spequilibrate_ruiz_descr::set_nrm(double value)
{
    this->m_nrm = value;
}

double _hipdss_spequilibrate_ruiz_descr::get_nrm() const
{
    return this->m_nrm;
}

void _hipdss_spequilibrate_ruiz_descr::set_tol(double value)
{
    std::cout << "this->dtolerance " << this->dtolerance << std::endl;
    THROW_IF_HIP_ERROR(hipMemcpy(this->dtolerance, &value, sizeof(double), hipMemcpyHostToDevice));

    auto status = rocsparse_create_dnvec_descr(
        &this->batch_tolerances, 1, this->dtolerance, rocsparse_datatype_f64_r);
    if(status != rocsparse_status_success)
    {
        THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
    }
    this->p_batch_tolerances = this->batch_tolerances;

#if 0
  this->p_batch_tolerances = &;
  this->batch_tolerances.init=true;
  this->batch_tolerances.size = 1;
  this->batch_tolerances.values = nullptr;
  this->batch_tolerances.const_values = this->dtolerance;
  this->batch_tolerances.data_type = rocsparse_datatype_f64_r;
  this->batch_tolerances.batch_stride = 0;
  this->batch_tolerances.batch_count  = 1;
  this->batch_tolerances.inc  = 1;
#endif
    this->m_tol = value;
}

double _hipdss_spequilibrate_ruiz_descr::get_tol() const
{
    return this->m_tol;
}

void _hipdss_spequilibrate_ruiz_descr::set_niter(int64_t value)
{
    this->m_niter = value;
}

int64_t _hipdss_spequilibrate_ruiz_descr::get_niter() const
{
    return this->m_niter;
}

void _hipdss_spequilibrate_ruiz_descr::set_nmaxiter(int64_t value)
{
    this->m_nmaxiter = value;
}

int64_t _hipdss_spequilibrate_ruiz_descr::get_nmaxiter() const
{
    return this->m_nmaxiter;
}

hipdssStatus_t _hipdss_spequilibrate_ruiz_descr::destroy(hipdssHandle_t handle)
{
    this->m_stage            = ((hipdss_spequilibrate_stage)-1);
    this->m_compute_datatype = ((rocsparse_datatype)-1);
    this->m_nmaxiter         = -1;
    this->m_niter            = -1;
    this->m_tol              = -1.0;
    this->m_nrm              = -1.0;
    if(this->dtolerance)
    {
        RETURN_IF_HIP_ERROR(handle->device_free(this->dtolerance));
        this->dtolerance = nullptr;
    }
    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t _hipdss_spequilibrate_ruiz_descr::create(hipdssHandle_t handle)
{
    this->m_stage            = (hipdss_spequilibrate_stage)-1;
    this->m_compute_datatype = (rocsparse_datatype)-1;
    this->m_nmaxiter         = -1;
    this->m_niter            = -1;
    this->m_tol              = -1.0;
    this->m_nrm              = -1.0;
    RETURN_IF_HIP_ERROR(
        handle->device_malloc(&this->dtolerance, sizeof(double)));
    return HIPDSS_STATUS_SUCCESS;
}

_hipdss_spequilibrate_ruiz_descr::~_hipdss_spequilibrate_ruiz_descr() {}

_hipdss_spequilibrate_ruiz_descr::_hipdss_spequilibrate_ruiz_descr()
    : m_stage((hipdss_spequilibrate_stage)-1)
    , m_compute_datatype((rocsparse_datatype)-1)
    , m_nmaxiter(-1)
    , m_niter(-1)
    , m_tol(-1.0)
    , m_nrm(-1.0)
{
}

hipdss_spequilibrate_stage _hipdss_spequilibrate_descr::get_stage() const
{
    const auto alg = this->get_alg();
    switch(alg)
    {
    case hipdss_spequilibrate_alg_default:
    case hipdss_spequilibrate_alg_ruiz:
    {
        return reinterpret_cast<_hipdss_spequilibrate_ruiz_descr*>(this->m_impl)
            ->get_stage();
    }
    }
    THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
}

void _hipdss_spequilibrate_descr::set_stage(hipdss_spequilibrate_stage value)
{
    const auto alg = this->get_alg();
    switch(alg)
    {
    case hipdss_spequilibrate_alg_default:
    case hipdss_spequilibrate_alg_ruiz:
    {
        reinterpret_cast<_hipdss_spequilibrate_ruiz_descr*>(this->m_impl)
            ->set_stage(value);
        return;
    }
    }
    THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
}

hipdss_spequilibrate_alg _hipdss_spequilibrate_descr::get_alg() const
{
    return this->m_alg;
}

void _hipdss_spequilibrate_descr::set_alg(hipdss_spequilibrate_alg value)
{
    this->m_alg = value;
}

#if 0
rocsparse_datatype _hipdss_spequilibrate_descr::get_compute_datatype() const
{
    return this->m_impl.get_compute_datatype();
}

void _hipdss_spequilibrate_descr::set_compute_datatype(rocsparse_datatype value)
{
    this->m_impl.set_compute_datatype (value);
}

void _hipdss_spequilibrate_descr::set_nrm(double value)
{
    this->m_impl.set_nrm (value);
}

double _hipdss_spequilibrate_descr::get_nrm() const
{
  return this->m_impl.get_nrm();
}

void _hipdss_spequilibrate_descr::set_tol(double value)
{

    this->m_impl.set_tol (value);
}

double _hipdss_spequilibrate_descr::get_tol() const
{
  return this->m_impl.get_tol();
}

void _hipdss_spequilibrate_descr::set_niter(int64_t value)
{
    this->m_impl.set_niter (value);
}

int64_t _hipdss_spequilibrate_descr::get_niter() const
{
  return this->m_impl.get_niter();
}

void _hipdss_spequilibrate_descr::set_nmaxiter(int64_t value)
{
    this->m_impl.set_nmaxiter (value);
}

int64_t _hipdss_spequilibrate_descr::get_nmaxiter() const
{
  return this->m_impl.get_nmaxiter();
}

#endif

extern "C" hipdssStatus_t
    hipdss_spequilibrate_descr_create(hipdssHandle_t              handle,
                                      hipdss_spequilibrate_descr* p_descr,
                                      rocsparse_error*            p_error)
try
{

    HIPDSS_CHECKARG_HANDLE(0, handle);
    HIPDSS_CHECKARG_POINTER(1, p_descr);
    p_descr[0] = new _hipdss_spequilibrate_descr();
    return HIPDSS_STATUS_SUCCESS;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" hipdssStatus_t
    hipdss_spequilibrate_descr_destroy(hipdssHandle_t             handle,
                                       hipdss_spequilibrate_descr descr,
                                       rocsparse_error*           p_error)
try
{
    if(descr != nullptr)
    {
        delete descr;
    }
    return HIPDSS_STATUS_SUCCESS;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
// LCOV_EXCL_STOP
