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

hipDataType rocsparse_datatype2hipDataType(rocsparse_datatype dtype)
{
    switch(dtype)
    {
    case rocsparse_datatype_f32_r:
        return HIP_R_32F;
    case rocsparse_datatype_f64_r:
        return HIP_R_64F;
    case rocsparse_datatype_f32_c:
        return HIP_C_32F;
    case rocsparse_datatype_f64_c:
        return HIP_C_64F;
    case rocsparse_datatype_i32_r:
        return HIP_R_32I;

    case rocsparse_datatype_bf16_r:
    case rocsparse_datatype_f16_r:
    case rocsparse_datatype_i8_r:
    case rocsparse_datatype_u8_r:
    case rocsparse_datatype_u32_r:
    {
        THROW_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE,
                                           "invalid enumeration");
    }
    }
}
