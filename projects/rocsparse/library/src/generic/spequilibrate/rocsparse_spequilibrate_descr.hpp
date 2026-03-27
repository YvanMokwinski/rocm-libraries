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
#include "internal/generic/rocsparse_spequilibrate.h"
#include "rocsparse_dnvec_descr.hpp"
#include <variant>

struct ruiz_buffer_t
{
private:
  char  * buffer;
  size_t buffer_size_in_bytes;
    int64_t batch_count;
    void * nrms;
  size_t nrms_size_in_bytes;
  void * converged;
  size_t converged_size_in_bytes;
    _rocsparse_dnvec_descr D_left;
  _rocsparse_dnvec_descr D_right;
public:
    ruiz_buffer_t(int64_t M,
		  int64_t N,
		  int64_t batch_count_,
		  rocsparse_datatype datatype,
		  void  * buffer_,
		  size_t buffer_size_in_bytes_);
  ~ruiz_buffer_t() = default;
  void * get_nrms(){ return this->nrms;};
  size_t get_nrms_size_in_bytes(){ return this->nrms_size_in_bytes;};
  void * get_converged(){ return this->converged;};
  size_t get_converged_size_in_bytes(){ return this->converged_size_in_bytes;};
  rocsparse_dnvec_descr get_left(){return &this->D_left;}
  rocsparse_dnvec_descr get_right(){return &this->D_left;}
};

struct _rocsparse_spequilibrate_ruiz_descr
{
protected:
  rocsparse_spequilibrate_stage  m_stage;
  rocsparse_spequilibrate_alg    m_alg;
  rocsparse_datatype             m_compute_datatype{};  
  int64_t                        m_nmaxiter;
  int64_t                        m_niter{};
  double                         m_tol;
  double                         m_nrm{};
public:
  ~_rocsparse_spequilibrate_ruiz_descr();
  _rocsparse_spequilibrate_ruiz_descr();
  
  rocsparse_spequilibrate_stage get_stage() const;
  void                   set_stage(rocsparse_spequilibrate_stage value);
  
  double  get_tol() const;  
  void    set_tol(double value);
  
  int64_t get_niter() const;
  void    set_niter(int64_t niter);
  
  double  get_nrm() const;
  void                   set_nrm(double value);
  
  rocsparse_spequilibrate_alg   get_alg() const;
  void                   set_alg(rocsparse_spequilibrate_alg value);
  
  rocsparse_operation    get_operation() const;
  rocsparse_datatype     get_compute_datatype() const;
  
  int64_t                get_nmaxiter() const; 
  void                   set_nmaxiter(int64_t value);
  
  void                   set_compute_datatype(rocsparse_datatype value);
};


struct _rocsparse_spequilibrate_descr
{
private:
  _rocsparse_spequilibrate_ruiz_descr m_impl{};

public:
  ~_rocsparse_spequilibrate_descr() = default;
  _rocsparse_spequilibrate_descr() = default;

  rocsparse_spequilibrate_stage get_stage() const;
  void                   set_stage(rocsparse_spequilibrate_stage value);
  
  double  get_tol() const;  
  void    set_tol(double value);
  
  int64_t get_niter() const;
  void    set_niter(int64_t niter);
  
  double  get_nrm() const;
  void                   set_nrm(double value);
  
  rocsparse_spequilibrate_alg   get_alg() const;
  void                   set_alg(rocsparse_spequilibrate_alg value);
  
  rocsparse_operation    get_operation() const;
  rocsparse_datatype     get_compute_datatype() const;
  
  int64_t                get_nmaxiter() const; 
  void                   set_nmaxiter(int64_t value);
  
  void                   set_compute_datatype(rocsparse_datatype value);
};
