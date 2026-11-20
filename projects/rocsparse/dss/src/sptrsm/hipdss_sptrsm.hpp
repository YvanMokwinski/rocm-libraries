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

#pragma once

#include "hipdss_data_types.h"
#include "hipdss_sptrsm_config_t.hpp"

namespace hipdss
{
    hipdssStatus_t sptrsm_create_and_configure(hipdssHandle_t          handle,
                                               rocsparse_sptrsm_descr* p_sptrsm_descr,
                                               const sptrsm_config_t*  config);

    hipdssStatus_t sptrsm_analysis(hipdssHandle_t              handle,
                                   rocsparse_sptrsm_descr      sptrsm_descr,
                                   rocsparse_const_spmat_descr F,
                                   rocsparse_const_dnmat_descr rhs,
                                   rocsparse_dnmat_descr       x);

    hipdssStatus_t sptrsm_compute(hipdssHandle_t              handle,
                                  rocsparse_sptrsm_descr      sptrsm_descr,
                                  rocsparse_spmat_descr       F,
                                  rocsparse_const_dnmat_descr rhs,
                                  rocsparse_dnmat_descr       x);

} // namespace hipdss
