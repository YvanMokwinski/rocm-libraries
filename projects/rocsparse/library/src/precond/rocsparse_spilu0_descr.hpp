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

#include "rocsparse-types.h"
#include "rocsparse_bsrilu0_info.hpp"
#include "rocsparse_csrilu0_info.hpp"
#include "rocsparse_spilu0.h"
#include <memory>

struct _rocsparse_spilu0_descr
{
protected:
    rocsparse_spilu0_stage                   m_stage;
    rocsparse_spilu0_alg                     m_alg;
    rocsparse_datatype                       m_compute_datatype;
    rocsparse_analysis_policy                m_analysis_policy;
    std::shared_ptr<_rocsparse_csrilu0_info> m_csrilu0_info;
    std::shared_ptr<_rocsparse_bsrilu0_info> m_bsrilu0_info;

    int32_t            m_boost_enable{};
    rocsparse_datatype m_boost_tol_datatype{};
    const void*        m_boost_tol{};
    const void*        m_boost_val{};
    double             m_singular_tol{};

public:
    ~_rocsparse_spilu0_descr();
    _rocsparse_spilu0_descr();

    int32_t            get_boost_enable() const;
    rocsparse_datatype get_boost_tol_datatype() const;
    const void*        get_boost_tol() const;
    const void*        get_boost_val() const;

    void set_boost_enable(int32_t value);
    void set_boost_tol_datatype(rocsparse_datatype value);
    void set_boost_tol(const void* value);
    void set_boost_val(const void* value);

    double get_singular_tol() const;
    void   set_singular_tol(double value);

    rocsparse_spilu0_stage get_stage() const;
    rocsparse_spilu0_alg   get_alg() const;
    rocsparse_datatype     get_compute_datatype() const;
    void                   set_stage(rocsparse_spilu0_stage value);
    void                   set_alg(rocsparse_spilu0_alg value);
    void                   set_compute_datatype(rocsparse_datatype value);

    rocsparse_analysis_policy get_analysis_policy() const;
    void                      set_analysis_policy(rocsparse_analysis_policy value);

    rocsparse_csrilu0_info get_csrilu0_info();
    void                   set_csrilu0_info(rocsparse_csrilu0_info value);
    void                   set_shared_csrilu0_info(std::shared_ptr<_rocsparse_csrilu0_info> value);

    rocsparse_bsrilu0_info get_bsrilu0_info();
    void                   set_bsrilu0_info(rocsparse_bsrilu0_info value);
    void                   set_shared_bsrilu0_info(std::shared_ptr<_rocsparse_bsrilu0_info> value);
};
