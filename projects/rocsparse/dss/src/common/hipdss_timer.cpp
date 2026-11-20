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

#include "hipdss_timer.hpp"

hipdss::timer::timer(hipStream_t stream)
    : m_stream(stream)
{
    auto status = hipEventCreate(&this->m_start);
    if(status != hipSuccess)
    {
        throw status;
    }
    status = hipEventCreate(&this->m_stop);
    if(status != hipSuccess)
    {
        throw status;
    }
}

void hipdss::timer::start()
{

    auto status = hipEventRecord(this->m_start, this->m_stream);
    if(hipSuccess != status)
    {
        throw status;
    }
}

float hipdss::timer::stop()
{
    float time;
    auto  status = hipEventRecord(this->m_stop, this->m_stream);
    if(status != hipSuccess)
    {
        throw status;
    }
    status = hipEventSynchronize(this->m_stop);
    if(status != hipSuccess)
    {
        throw status;
    }
    status = hipEventElapsedTime(&time, this->m_start, this->m_stop);
    if(status != hipSuccess)
    {
        throw status;
    }
    return time;
}

hipdss::timer::~timer()
{
    std::ignore = hipEventDestroy(this->m_start);
    std::ignore = hipEventDestroy(this->m_stop);
}
