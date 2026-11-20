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

#include "hipdss_working_row_t.hpp"
#include <iostream>

template <typename J>
J hipdss::working_row_t<J>::get(J i) const
{
    return this->m_select[i];
}

template <typename J>
J hipdss::working_row_t<J>::size() const
{
    return this->m_select_n;
}

template <typename J>
hipdss::working_row_t<J>::working_row_t(J m,
                                        J max_size_per_row)
{
    this->m_m                = m;
    this->m_max_size_per_row = max_size_per_row;

    this->m_marker = (J*)calloc(m, sizeof(J));

    this->m_select   = (J*)malloc(max_size_per_row * sizeof(J));
    this->m_select_n = 0;
}

template <typename J>
void hipdss::working_row_t<J>::print() const
{
    for(J i = 0; i < this->m_select_n; ++i)
    {
        std::cout << " working_row["
                  << i
                  << "] = "
                  << this->m_select[i]
                  << std::endl;
    }
}

template <typename J>
void hipdss::working_row_t<J>::sort()
{
    std::sort(this->m_select,
              this->m_select + this->m_select_n);
}

template <typename J>
bool hipdss::working_row_t<J>::has(J j) const
{
    return (this->m_marker[j] != 0);
}

template <typename J>
void hipdss::working_row_t<J>::add(J j)
{
    if(this->m_marker[j] == 0)
    {
        this->m_select[this->m_select_n++] = j;
        this->m_marker[j]                  = this->m_select_n;
    }
}

template <typename J>
void hipdss::working_row_t<J>::reset()
{
    for(J i = 0; i < this->m_select_n; ++i)
    {
        this->m_marker[this->m_select[i]] = 0;
    }
    this->m_select_n = 0;
}

template <typename J>
hipdss::working_row_t<J>::~working_row_t()
{
    if(m_marker)
    {
        free(m_marker);
        m_marker = nullptr;
    }

    if(m_select)
    {
        free(m_select);
        m_select = nullptr;
    }
}

template struct hipdss::working_row_t<int32_t>;
template struct hipdss::working_row_t<int64_t>;
