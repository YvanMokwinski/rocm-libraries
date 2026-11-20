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

#include "hipdssMatrixFormat_t.hpp"
#include "hipdss_control.hpp"

template <>
bool hipdss::enum_utils::is_invalid(const hipdssMatrixFormat_t value)
{
    constexpr int mask = HIPDSS_MFORMAT_DENSE | HIPDSS_MFORMAT_CSR | HIPDSS_MFORMAT_BATCH | HIPDSS_MFORMAT_DISTRIBUTED;
    return (static_cast<int>(value) & ~mask) != 0
           || static_cast<int>(value) == 0;
}

template <>
const char* hipdss::enum_utils::to_string(const hipdssMatrixFormat_t value)
{
    switch(value)
    {
    case HIPDSS_MFORMAT_DENSE:
        return "HIPDSS_MFORMAT_DENSE";
    case HIPDSS_MFORMAT_CSR:
        return "HIPDSS_MFORMAT_CSR";
    case HIPDSS_MFORMAT_BATCH:
        return "HIPDSS_MFORMAT_BATCH";
    case HIPDSS_MFORMAT_DISTRIBUTED:
        return "HIPDSS_MFORMAT_DISTRIBUTED";
    }
    if(!is_invalid(value))
    {
        return "hipdssMatrixFormat_t combination";
    }
    // LCOV_EXCL_START
    THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    // LCOV_EXCL_STOP
}
