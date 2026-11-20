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

extern "C" hipdssStatus_t hipdssMatrixGetBatchDn(hipdssMatrix_t  matrix,
                                                 int64_t*        batchCount,
                                                 void**          nrows,
                                                 void**          ncols,
                                                 void**          ld,
                                                 void***         values,
                                                 hipDataType*    indexType,
                                                 hipDataType*    valueType,
                                                 hipdssLayout_t* layout)
try
{

    HIPDSS_CHECKARG_POINTER(0, matrix);
    HIPDSS_CHECKARG_POINTER(0, matrix->get_dense());

    auto dense = matrix->get_dense();

    //
    // Many things need to be clarified.
    //
    RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_NOT_SUPPORTED);

    rocsparse_datatype local_datatype{};
    rocsparse_order    local_order{};
    int64_t            local_nrows{};
    int64_t            local_ncols{};
    int64_t            local_ld{};
    const void*        local_values{};

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_const_dnmat_get(dense,
                                                        &local_nrows,
                                                        &local_ncols,
                                                        &local_ld,
                                                        &local_values,
                                                        &local_datatype,
                                                        &local_order));

    rocsparse_int batch_count{};
    int64_t       batch_stride{};

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnmat_get_strided_batch(dense,
                                                                &batch_count,
                                                                &batch_stride));

    if(batchCount)
    {
        batchCount[0] = batch_count;
    }

    if(nrows)
    {
        int64_t** p_nrows = reinterpret_cast<int64_t**>(nrows);
        for(int64_t i = 0; i < batch_count; ++i)
        {
            p_nrows[i][0] = local_nrows;
        }
    }

    if(ncols)
    {
        int64_t** p_ncols = reinterpret_cast<int64_t**>(ncols);
        for(int64_t i = 0; i < batch_count; ++i)
        {
            p_ncols[i][0] = local_ncols;
        }
    }

    if(ld)
    {
        int64_t** p_ld = reinterpret_cast<int64_t**>(ld);
        for(int64_t i = 0; i < batch_count; ++i)
        {
            p_ld[i][0] = local_ld;
        }
    }

    if(values)
    {
        //
        //
        //
        values[0][0] = const_cast<void*>(local_values);
    }

    if(valueType)
    {
        valueType[0] = rocsparse_datatype2hipDataType(local_datatype);
    }

    if(layout)
    {
        layout[0] = rocsparse_order2hipdssLayout_t(local_order);
    }

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
