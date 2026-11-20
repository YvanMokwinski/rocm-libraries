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

#include "hipdssMatrix.hpp"
#include "hipdss_control.hpp"
#include "hipdss_datatype_utils.hpp"
#include <iostream>

namespace hipdss
{

    template <typename T>
    static void dense_matrix_print(int64_t        nrows,
                                   int64_t        ncols,
                                   hipdssLayout_t layout,
                                   const void*    values)
    {
        const T* v = reinterpret_cast<const T*>(values);
        for(int64_t i = 0; i < nrows; ++i)
        {
            for(int64_t j = 0; j < ncols; ++j)
            {
                std::cout << v[(HIPDSS_LAYOUT_COL_MAJOR == layout) ? (j * nrows + i) : (j * ncols + i)];
            }
            std::cout << std::endl;
        }
    }

}

hipdssStatus_t hipdssMatrixPrint(hipdssMatrix_t pointer)
{
    if(false == pointer->is_dense())
    {
        return HIPDSS_STATUS_INTERNAL_ERROR;
    }

    const auto datatype = pointer->get_val_type();
    auto       print    = hipdss::dense_matrix_print<float>;
    switch(datatype)
    {
    case HIP_R_32F:
    {
        print = hipdss::dense_matrix_print<float>;
        break;
    }

    case HIP_R_64F:
    {
        print = hipdss::dense_matrix_print<double>;
        break;
    }

    case HIP_C_32F:
    {
        print = hipdss::dense_matrix_print<rocsparse_float_complex>;
        break;
    }

    case HIP_C_64F:
    {
        print = hipdss::dense_matrix_print<rocsparse_double_complex>;
        break;
    }

    case HIP_R_32I:
    {
        print = hipdss::dense_matrix_print<int32_t>;
        break;
    }

    case HIP_R_64I:
    {
        print = hipdss::dense_matrix_print<int64_t>;
        break;
    }

    default:
    {
        RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                            "unsupported datatype");
    }
    }

    const size_t nbytes = hipdss::datatype_sizeof(datatype) * pointer->get_nrows() * pointer->get_ncols();
    void*        v      = malloc(nbytes);
    RETURN_IF_HIP_ERROR(hipMemcpy(v,
                                  pointer->get_val_data(),
                                  nbytes,
                                  hipMemcpyDeviceToHost));

    print(pointer->get_nrows(),
          pointer->get_ncols(),
          pointer->get_layout(),
          v);

    free(v);

    return HIPDSS_STATUS_SUCCESS;
}
