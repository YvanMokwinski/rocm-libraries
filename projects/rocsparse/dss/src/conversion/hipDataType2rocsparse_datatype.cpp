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

#include "hipdss_enums.hpp"

std::ostream& operator<<(std::ostream& out, const hipDataType value)
{
    switch(value)
    {

    case HIP_R_32F:
    {
        out << "HIP_R_32F";
        return out;
    }

    case HIP_R_64F:
    {
        out << "HIP_R_64F";
        return out;
    }

    case HIP_C_32F:
    {
        out << "HIP_C_32F";
        return out;
    }

    case HIP_C_64F:
    {
        out << "HIP_C_64F";
        return out;
    }

    case HIP_R_32I:
    {
        out << "HIP_R_32I";
        return out;
    }

    case HIP_R_64I:
    {
        out << "HIP_R_64I";
        return out;
    }

    default:
    {
        THROW_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                           "unsupported datatype");
    }
    }

    return out;
}

rocsparse_datatype hipDataType2rocsparse_datatype(hipDataType dtype)
{
    switch(dtype)
    {
    case HIP_R_32F:
    {
        return rocsparse_datatype_f32_r;
    }

    case HIP_R_64F:
    {
        return rocsparse_datatype_f64_r;
    }

    case HIP_C_32F:
    {
        return rocsparse_datatype_f32_c;
    }

    case HIP_C_64F:
    {
        return rocsparse_datatype_f64_c;
    }

    case HIP_R_32I:
    {
        return rocsparse_datatype_i32_r;
    }

    case HIP_R_64I:
    {
        break;
    }

    default:
    {
        break;
    }
    }

    return rocsparse_datatype_f32_r;
}
