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
#include "rocsparse_csrsm_info.hpp"
#include <memory>

struct _rocsparse_sptrsm_descr
{
protected:
    rocsparse_sptrsm_stage                 m_stage;
    rocsparse_sptrsm_alg                   m_alg;
    rocsparse_operation                    m_operation_A;
    rocsparse_operation                    m_operation_B;
    rocsparse_datatype                     m_B_datatype;
    rocsparse_datatype                     m_C_datatype;
    rocsparse_order                        m_B_order;
    rocsparse_order                        m_C_order;
    rocsparse_datatype                     m_scalar_datatype;
    rocsparse_datatype                     m_compute_datatype;
    int64_t                                m_nrhs;
    const void*                            m_scalar_alpha;
    rocsparse_analysis_policy              m_analysis_policy;
    std::shared_ptr<_rocsparse_csrsm_info> m_csrsm_info{};
    float                                  m_local_host_alpha_value[4];

public:
    void* get_local_host_alpha()
    {
        return &this->m_local_host_alpha_value[0];
    }
    rocsparse_csrsm_info get_csrsm_info();
    void                 set_csrsm_info(rocsparse_csrsm_info value);
    void                 set_shared_csrsm_info(std::shared_ptr<_rocsparse_csrsm_info> value);

    ~_rocsparse_sptrsm_descr();

    _rocsparse_sptrsm_descr();
    rocsparse_analysis_policy get_analysis_policy() const;
    void                      set_analysis_policy(rocsparse_analysis_policy value);

    rocsparse_sptrsm_stage get_stage() const;
    rocsparse_sptrsm_alg   get_alg() const;

    int64_t             get_nrhs() const;
    rocsparse_operation get_operation_A() const;

    rocsparse_operation get_operation_B() const;

    rocsparse_datatype get_scalar_datatype() const;
    const void*        get_scalar_alpha() const;

    rocsparse_datatype get_compute_datatype() const;

    rocsparse_datatype get_B_datatype() const;

    rocsparse_datatype get_C_datatype() const;

    rocsparse_order get_B_order() const;

    rocsparse_order get_C_order() const;

    void set_nrhs(int64_t);
    void set_stage(rocsparse_sptrsm_stage value);
    void set_alg(rocsparse_sptrsm_alg value);
    void set_scalar_alpha(const void* value);

    void set_operation_A(rocsparse_operation value);

    void set_operation_B(rocsparse_operation value);

    void set_scalar_datatype(rocsparse_datatype value);
    void set_compute_datatype(rocsparse_datatype value);

    void set_B_datatype(rocsparse_datatype value);
    void set_C_datatype(rocsparse_datatype value);

    void set_B_order(rocsparse_order value);
    void set_C_order(rocsparse_order value);
};
