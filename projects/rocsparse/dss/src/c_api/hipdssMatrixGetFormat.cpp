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

extern "C" hipdssStatus_t hipdssMatrixGetFormat(hipdssMatrix_t matrix,
                                                int32_t*       format)
try
{
    HIPDSS_CHECKARG_POINTER(0, matrix);
    HIPDSS_CHECKARG_POINTER(1, format);

    if(matrix->get_dense())
    {
        format[0] = static_cast<int32_t>(HIPDSS_MFORMAT_DENSE);
        return HIPDSS_STATUS_SUCCESS;
    }
    else if(matrix->get_sparse())
    {
        rocsparse_format spmat_format;
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_get_format(matrix->get_sparse(),
                                                             &spmat_format));

        switch(spmat_format)
        {
        case rocsparse_format_csr:
        {
            format[0] = static_cast<int32_t>(HIPDSS_MFORMAT_CSR);
            return HIPDSS_STATUS_SUCCESS;
        }
        case rocsparse_format_csc:
        case rocsparse_format_ell:
        case rocsparse_format_sell:
        case rocsparse_format_bell:
        case rocsparse_format_bsr:
        case rocsparse_format_coo:
        case rocsparse_format_coo_aos:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
        }
        }
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
    }

    RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
