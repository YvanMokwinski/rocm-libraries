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

#include "hipdss.h"
#include "hipdss_enums.hpp"

extern "C" hipdssStatus_t hipdssMatrixCreateDn(hipdssMatrix_t* p_matrix,
                                               int64_t         nrows,
                                               int64_t         ncols,
                                               int64_t         ld,
                                               const void*     values,
                                               hipDataType     valueType,
                                               hipdssLayout_t  layout)
try
{
    HIPDSS_CHECKARG_POINTER(0, p_matrix);
    HIPDSS_CHECKARG_SIZE(1, nrows);
    HIPDSS_CHECKARG_SIZE(2, ncols);
    HIPDSS_CHECKARG_SIZE(3, ld);
    HIPDSS_CHECKARG_POINTER(4, values);
    HIPDSS_CHECKARG_ENUM(5, valueType);
    HIPDSS_CHECKARG_ENUM(6, layout);

    RETURN_IF_HIPDSS_ERROR(hipdssMatrix::create_dense(nullptr,
                                                      p_matrix,
                                                      layout,
                                                      valueType,
                                                      1,
                                                      nrows,
                                                      ncols,
                                                      ld,
                                                      std::remove_const_t<void*>(values)));

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
