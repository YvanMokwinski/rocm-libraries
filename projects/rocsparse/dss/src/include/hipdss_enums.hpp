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

#ifndef HIPDSS_ENUMS_HPP
#define HIPDSS_ENUMS_HPP

#include "hipdss.hpp"
#include <rocsparse/rocsparse.h>

hipdssMatrixType_t     rocsparse2hipdssMatrixType_t(rocsparse_matrix_type value);
rocsparse_matrix_type  hipdssMatrixType_t2rocsparse(hipdssMatrixType_t value);
hipdssMatrixViewType_t rocsparse2hipdssMatrixViewType_t(rocsparse_fill_mode value);
rocsparse_fill_mode    hipdssMatrixViewType_t2rocsparse(hipdssMatrixViewType_t value);
rocsparse_datatype     hipDataType2rocsparse_datatype(hipDataType);
rocsparse_indextype    hipDataType2rocsparse_indextype(hipDataType);
hipDataType            rocsparse_datatype2hipDataType(rocsparse_datatype);
hipDataType            rocsparse_indextype2hipDataType(rocsparse_indextype);
rocsparse_order        hipdssLayout_t2rocsparse_order(hipdssLayout_t);
rocsparse_index_base   hipdssIndexBase_t2rocsparse_index_base(hipdssIndexBase_t);
hipdssIndexBase_t      rocsparse_index_base2hipdssIndexBase_t(rocsparse_index_base);
hipdssLayout_t         rocsparse_order2hipdssLayout_t(rocsparse_order);

std::ostream& operator<<(std::ostream& out, const hipDataType value);

#endif // HIPDSS_ENUMS_HPP
