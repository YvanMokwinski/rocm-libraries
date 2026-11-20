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

#include "hipdss.hpp"
#include "rocsparse/rocsparse.h"

extern "C" hipdssStatus_t hipdssMatrixSetCsrPointers(hipdssMatrix_t matrix,
                                                     const void*    rowOffsets,
                                                     const void*    rowEnd,
                                                     const void*    colIndices,
                                                     const void*    values)
try
{
    HIPDSS_CHECKARG_POINTER(0, matrix);
    HIPDSS_CHECKARG_POINTER(0, matrix->get_sparse());
    HIPDSS_CHECKARG_POINTER(1, rowOffsets);
    HIPDSS_CHECKARG_POINTER(3, colIndices);
    HIPDSS_CHECKARG_POINTER(4, values);

    //
    // rocsparse_const_spmat_set_values does not exist
    //
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_values(std::remove_const_t<rocsparse_spmat_descr>(matrix->get_sparse()),
                                                         std::remove_const_t<void*>(values)));

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
