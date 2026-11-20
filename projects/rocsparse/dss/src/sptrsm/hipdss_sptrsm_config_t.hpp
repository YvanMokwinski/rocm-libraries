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

    struct sptrsm_config_t
    {
    protected:
        rocsparse_sptrsm_alg      m_alg{};
        rocsparse_operation       m_op_A{};
        rocsparse_operation       m_op_X{};
        rocsparse_analysis_policy m_analysis_policy{};
        rocsparse_datatype        m_compute_datatype{};
        rocsparse_datatype        m_scalar_datatype{};

    public:
        //
        rocsparse_sptrsm_alg get_alg() const;
        void                 set_alg(rocsparse_sptrsm_alg);

        //
        rocsparse_operation get_op_A() const;
        void                set_op_A(rocsparse_operation);

        //
        rocsparse_operation get_op_X() const;
        void                set_op_X(rocsparse_operation);

        //
        rocsparse_analysis_policy get_analysis_policy() const;
        void                      set_analysis_policy(rocsparse_analysis_policy);

        //
        rocsparse_datatype get_compute_datatype() const;
        void               set_compute_datatype(rocsparse_datatype);

        //
        rocsparse_datatype get_scalar_datatype() const;
        void               set_scalar_datatype(rocsparse_datatype);
    };

} // namespace hipdss
