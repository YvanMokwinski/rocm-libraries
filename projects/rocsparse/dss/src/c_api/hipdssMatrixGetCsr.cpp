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

#include "rocsparse_indextype_utils.hpp"

#include "hipdss_enums.hpp"

extern "C" hipdssStatus_t hipdssMatrixGetCsr(const hipdssMatrix_t    matrix,
                                             int64_t*                nrows,
                                             int64_t*                ncols,
                                             int64_t*                nnz,
                                             void**                  rowStart,
                                             void**                  rowEnd,
                                             void**                  colIndices,
                                             void**                  values,
                                             hipDataType*            offsetType,
                                             hipDataType*            indexType,
                                             hipDataType*            valueType,
                                             hipdssMatrixType_t*     mtype,
                                             hipdssMatrixViewType_t* mview,
                                             hipdssIndexBase_t*      indexBase)
try
{

    HIPDSS_CHECKARG_POINTER(0, matrix);
    HIPDSS_CHECKARG_POINTER(0, matrix->get_sparse());

    auto sparse = matrix->get_sparse();

    int64_t local_nrows;
    int64_t local_ncols;
    int64_t local_nnz;

    const void* local_rowStart;
    const void* local_colIndices;
    const void* local_values;

    rocsparse_datatype   local_value_type;
    rocsparse_index_base local_idx_base;
    rocsparse_indextype  local_row_ptr_type;
    rocsparse_indextype  local_col_ind_type;

    if(!matrix)
    {
        return HIPDSS_STATUS_INVALID_VALUE;
    }

    if(!sparse)
    {
        return HIPDSS_STATUS_INVALID_VALUE;
    }

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_const_csr_get(sparse,
                                                      &local_nrows,
                                                      &local_ncols,
                                                      &local_nnz,
                                                      &local_rowStart,
                                                      &local_colIndices,
                                                      &local_values,
                                                      &local_row_ptr_type,
                                                      &local_col_ind_type,
                                                      &local_idx_base,
                                                      &local_value_type));

    if(nrows)
    {
        *nrows = local_nrows;
    }

    if(ncols)
    {
        *ncols = local_ncols;
    }

    if(nnz)
    {
        *nnz = local_nnz;
    }

    if(rowStart)
    {
        *rowStart = const_cast<void*>(local_rowStart);
    }

    if(rowEnd)
    {
        *rowEnd = reinterpret_cast<char*>(const_cast<void*>(local_rowStart)) + rocsparse::indextype_sizeof(local_row_ptr_type);
    }

    if(values)
    {
        *values = const_cast<void*>(local_values);
    }

    if(colIndices)
    {
        *colIndices = const_cast<void*>(local_colIndices);
    }

    if(indexType)
    {
        *indexType = rocsparse_indextype2hipDataType(local_row_ptr_type);
    }

    if(valueType)
    {
        *valueType = rocsparse_datatype2hipDataType(local_value_type);
    }

    if(indexBase)
    {
        *indexBase = rocsparse_index_base2hipdssIndexBase_t(local_idx_base);
    }

    if(mtype)
    {
        rocsparse_matrix_type matrix_type;
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_get_attribute(sparse,
                                                                rocsparse_spmat_matrix_type,
                                                                &matrix_type,
                                                                sizeof(matrix_type)));
        *mtype = rocsparse2hipdssMatrixType_t(matrix_type);
    }

    if(mview)
    {
        rocsparse_fill_mode fill_mode;
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_get_attribute(sparse,
                                                                rocsparse_spmat_fill_mode,
                                                                &fill_mode,
                                                                sizeof(fill_mode)));
        *mview = rocsparse2hipdssMatrixViewType_t(fill_mode);
    }

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
