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

#include "hipdssMemoryEstimateState_t.hpp"
#include "hipdss_control.hpp"

template <>
bool hipdss::enum_utils::is_invalid(const hipdssMemoryEstimateState_t value)
{
    switch(value)
    {
    case HIPDSS_MEMORY_ESTIMATE_STATE_NONE:
    case HIPDSS_MEMORY_ESTIMATE_STATE_FILL_KNOWN:
    case HIPDSS_MEMORY_ESTIMATE_STATE_FINAL:
    {
        return false;
    }
    }
    return true;
}

template <>
const char* hipdss::enum_utils::to_string(const hipdssMemoryEstimateState_t value)
{
    switch(value)
    {
    case HIPDSS_MEMORY_ESTIMATE_STATE_NONE:
        return "HIPDSS_MEMORY_ESTIMATE_STATE_NONE";
    case HIPDSS_MEMORY_ESTIMATE_STATE_FILL_KNOWN:
        return "HIPDSS_MEMORY_ESTIMATE_STATE_FILL_KNOWN";
    case HIPDSS_MEMORY_ESTIMATE_STATE_FINAL:
        return "HIPDSS_MEMORY_ESTIMATE_STATE_FINAL";
    }
    // LCOV_EXCL_START
    THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    // LCOV_EXCL_STOP
}
