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

extern "C" hipdssStatus_t hipdssMatrixCreateCsr(hipdssMatrix_t*        matrix, // 0
                                                int64_t                nrows, // 1
                                                int64_t                ncols, // 2
                                                int64_t                nnz, // 3
                                                const void*            rowStart, // 4
                                                const void*            rowEnd, // 5
                                                const void*            colIndices, // 6
                                                const void*            values, // 7
                                                hipDataType            offsetType, // 8
                                                hipDataType            indexType, // 9
                                                hipDataType            valueType, // 10
                                                hipdssMatrixType_t     mtype, // 11
                                                hipdssMatrixViewType_t mview, // 12
                                                hipdssIndexBase_t      indexBase) // 13
try
{

    HIPDSS_CHECKARG_POINTER(0,
                            matrix);

    HIPDSS_CHECKARG_SIZE(1, nrows);
    HIPDSS_CHECKARG_SIZE(2, ncols);
    HIPDSS_CHECKARG_SIZE(3, nnz);
    HIPDSS_CHECKARG_POINTER(4, rowStart);
    HIPDSS_CHECKARG_POINTER(6, colIndices);
    HIPDSS_CHECKARG_POINTER(7, values);
    HIPDSS_CHECKARG_ENUM(8, offsetType);
    HIPDSS_CHECKARG_ENUM(9, indexType);
    HIPDSS_CHECKARG_ENUM(10, valueType);
    HIPDSS_CHECKARG_ENUM(11, mtype);
    HIPDSS_CHECKARG_ENUM(12, mview);
    HIPDSS_CHECKARG_ENUM(13, indexBase);

    RETURN_IF_HIPDSS_ERROR(hipdssMatrix::create_sparse_csr(nullptr,
                                                           matrix,
                                                           nrows,
                                                           ncols,
                                                           nnz,
                                                           rowStart,
                                                           nullptr,
                                                           rowEnd,
                                                           nullptr,
                                                           colIndices,
                                                           nullptr,
                                                           values,
                                                           nullptr,
                                                           offsetType,
                                                           indexType,
                                                           valueType,
                                                           mtype,
                                                           mview,
                                                           indexBase));

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
