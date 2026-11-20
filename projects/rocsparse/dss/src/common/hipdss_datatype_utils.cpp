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

#include "hipdss_datatype_utils.hpp"
#include "hipdss_control.hpp"
#include "hipdss_enum_utils.hpp"
#include <limits>

hipDataType hipdss::datatype_minimum(const int64_t size)
{
    return (size <= std::numeric_limits<int64_t>::max())
               ? HIP_R_32I
               : HIP_R_64I;
}

template <>
bool hipdss::enum_utils::is_invalid(const hipDataType value)
{
    switch(value)
    {
    case HIP_R_32F:
    case HIP_R_64F:
    case HIP_C_32F:
    case HIP_C_64F:
    case HIP_R_32I:
    case HIP_R_64I:
    {
        return false;
    }

    default:
    {
        return true;
    }
    }
}

size_t hipdss::datatype_sizeof(const hipDataType that)
{

    switch(that)
    {

    case HIP_R_32F:
    {
        return sizeof(float);
    }

    case HIP_R_64F:
    {
        return sizeof(double);
    }

    case HIP_C_32F:
    {
        return sizeof(float) * 2;
    }

    case HIP_C_64F:
    {
        return sizeof(double) * 2;
    }

    case HIP_R_32I:
    {
        return sizeof(int32_t);
    }

    case HIP_R_64I:
    {
        return sizeof(int64_t);
    }

    default:
    {
        THROW_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                           "unsupported datatype");
    }
    }
}

bool hipdss::datatype_is_complex(hipDataType that)
{
    switch(that)
    {

    case HIP_C_32F:
    case HIP_C_64F:
    {
        return true;
    }

    case HIP_R_32F:
    case HIP_R_64F:
    case HIP_R_32I:
    case HIP_R_64I:
    {
        return false;
    }

    default:
    {
        THROW_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                           "unsupported datatype");
    }
    }
}
