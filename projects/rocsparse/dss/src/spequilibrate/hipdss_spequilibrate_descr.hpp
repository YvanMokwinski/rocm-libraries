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
#include "hipdss_spequilibrate.h"

struct _hipdss_spequilibrate_descr
{
protected:
    void*                    m_impl{};
    hipdss_spequilibrate_alg m_alg;

public:
    ~_hipdss_spequilibrate_descr() = default;
    _hipdss_spequilibrate_descr()  = default;

    const void* get_impl() const
    {
        return this->m_impl;
    }
    void* get_impl()
    {
        return this->m_impl;
    }
    void set_impl(void* pointer)
    {
        this->m_impl = pointer;
    }

    hipdss_spequilibrate_stage get_stage() const;
    void                       set_stage(hipdss_spequilibrate_stage value);

    hipdss_spequilibrate_alg get_alg() const;
    void                     set_alg(hipdss_spequilibrate_alg value);

#if 0
  void  set_batch_tolerances(rocsparse_const_dnvec_descr that);
  rocsparse_const_dnvec_descr get_batch_tolerances()const;

  double  get_tol() const;
  void    set_tol(double value);

  int64_t get_niter() const;
  void    set_niter(int64_t niter);

  double  get_nrm() const;
  void                   set_nrm(double value);


  rocsparse_datatype     get_compute_datatype() const;

  int64_t                get_nmaxiter() const;
  void                   set_nmaxiter(int64_t value);

  void                   set_compute_datatype(rocsparse_datatype value);
#endif
};

struct _hipdss_spequilibrate_ruiz_descr
{
protected:
    hipdss_spequilibrate_stage m_stage;

    rocsparse_datatype          m_compute_datatype{};
    int64_t                     m_nmaxiter;
    int64_t                     m_niter{};
    double                      m_tol{};
    double                      m_nrm{};
    double*                     dtolerance{};
    rocsparse_dnvec_descr       batch_tolerances{nullptr};
    rocsparse_const_dnvec_descr p_batch_tolerances{nullptr};

public:
    _hipdss_spequilibrate_ruiz_descr();
    ~_hipdss_spequilibrate_ruiz_descr();

    //
    hipdssStatus_t create(hipdssHandle_t handle);
    hipdssStatus_t destroy(hipdssHandle_t handle);

    //
    void                        set_batch_tolerances(rocsparse_const_dnvec_descr that);
    rocsparse_const_dnvec_descr get_batch_tolerances() const;

    //
    hipdss_spequilibrate_stage get_stage() const;
    void                       set_stage(hipdss_spequilibrate_stage value);

    //
    double get_tol() const;
    void   set_tol(double value);

    //
    int64_t get_niter() const;
    void    set_niter(int64_t niter);

    //
    double get_nrm() const;
    void   set_nrm(double value);

    //
    hipdss_spequilibrate_alg get_alg() const;
    void                     set_alg(hipdss_spequilibrate_alg value);

    //
    int64_t get_nmaxiter() const;
    void    set_nmaxiter(int64_t value);

    //
    rocsparse_datatype get_compute_datatype() const;
    void               set_compute_datatype(rocsparse_datatype value);
};
