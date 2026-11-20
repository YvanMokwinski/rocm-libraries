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

extern "C" hipdssStatus_t hipdssMatrixGetDn(hipdssMatrix_t  matrix,
                                            int64_t*        nrows,
                                            int64_t*        ncols,
                                            int64_t*        ld,
                                            void**          values,
                                            hipDataType*    type,
                                            hipdssLayout_t* layout)
try
{
    HIPDSS_CHECKARG_POINTER(0, matrix);
    HIPDSS_CHECKARG_POINTER(0, matrix->get_dense());

    if(nrows)
    {
        nrows[0] = matrix->get_nrows();
    }

    if(ncols)
    {
        ncols[0] = matrix->get_ncols();
    }

    if(ld)
    {
        ld[0] = matrix->get_ld();
    }

    if(values)
    {
        //
        // Because hipdssMatrixCreateDn takes a const pointer !! ...
        //
        values[0] = std::remove_const_t<void*>(matrix->get_const_val_data());
    }

    if(type)
    {
        type[0] = matrix->get_val_type();
    }

    if(layout)
    {
        layout[0] = matrix->get_layout();
    }

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
