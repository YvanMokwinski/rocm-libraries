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

#include "hipdssReorderingAlg_t.hpp"
#include "hipdss_control.hpp"

template <>
bool hipdss::enum_utils::is_invalid(const hipdssReorderingAlg_t value)
{
    switch(value)
    {
    case HIPDSS_REORDERING_ALG_DEFAULT:
    case HIPDSS_REORDERING_ALG_BTF_COLAMD:
    case HIPDSS_REORDERING_ALG_COLAMD:
    case HIPDSS_REORDERING_ALG_AMD:
    case HIPDSS_REORDERING_ALG_NESTED_DISSECTION:
    case HIPDSS_REORDERING_ALG_NONE:
    case HIPDSSEXT_REORDERING_ALG_RCMK:
    {
        return false;
    }
    }
    return true;
}

template <>
const char* hipdss::enum_utils::to_string(const hipdssReorderingAlg_t value)
{
    switch(value)
    {
    case HIPDSS_REORDERING_ALG_DEFAULT:
        return "HIPDSS_REORDERING_ALG_DEFAULT";
    case HIPDSS_REORDERING_ALG_BTF_COLAMD:
        return "HIPDSS_REORDERING_ALG_BTF_COLAMD";
    case HIPDSS_REORDERING_ALG_COLAMD:
        return "HIPDSS_REORDERING_ALG_COLAMD";
    case HIPDSS_REORDERING_ALG_AMD:
        return "HIPDSS_REORDERING_ALG_AMD";
    case HIPDSS_REORDERING_ALG_NESTED_DISSECTION:
        return "HIPDSS_REORDERING_ALG_NESTED_DISSECTION";
    case HIPDSS_REORDERING_ALG_NONE:
        return "HIPDSS_REORDERING_ALG_NONE";
    case HIPDSSEXT_REORDERING_ALG_RCMK:
        return "HIPDSSEXT_REORDERING_ALG_RCMK";
    }
    // LCOV_EXCL_START
    THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    // LCOV_EXCL_STOP
}
