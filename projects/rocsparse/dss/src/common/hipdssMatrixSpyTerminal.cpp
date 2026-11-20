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

#include "hipdssContext.hpp"
#include "hipdssMatrix.hpp"
#include "hipdss_control.hpp"
#include "hipdss_datatype_utils.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

namespace hipdss
{
    template <typename I, typename J>
    static void csr_spy_terminal(int64_t           M,
                                 int64_t           N,
                                 const void*       Ap_,
                                 const void*       Ap_end_,
                                 const void*       Ai_,
                                 hipdssIndexBase_t base)
    {
        static constexpr int64_t max_tiles = 40;

        const I* Ap     = reinterpret_cast<const I*>(Ap_);
        const I* Ap_end = (Ap_end_) ? reinterpret_cast<const I*>(Ap_end_) : (Ap + 1);
        const J* Ai     = reinterpret_cast<const J*>(Ai_);

        const int64_t row_block     = (M - 1) / std::min(M, max_tiles) + 1;
        const int64_t col_block     = (N - 1) / std::min(N, max_tiles) + 1;
        const int64_t num_row_tiles = (M - 1) / row_block + 1;
        const int64_t num_col_tiles = (N - 1) / col_block + 1;

        bool filled[max_tiles][max_tiles]{};
        for(int64_t i = 0; i < M; ++i)
        {
            for(auto k = Ap[i] - base; k < Ap_end[i] - base; ++k)
            {
                const auto j                         = Ai[k] - base;
                filled[i / row_block][j / col_block] = true;
            }
        }

        for(int64_t i = 0; i < num_row_tiles; ++i)
        {
            for(int64_t j = 0; j < num_col_tiles; ++j)
            {
                std::cout << (filled[i][j] ? "x" : ".");
            }
            std::cout << std::endl;
        }
    }
}

hipdssStatus_t hipdssMatrixSpyTerminal(hipdssHandle_t handle,
                                       hipdssMatrix_t source)
{

    RETURN_IF_HIPDSS_ERROR(source->is_sparse()
                               ? HIPDSS_STATUS_SUCCESS
                               : HIPDSS_STATUS_NOT_SUPPORTED);

    const auto M = source->get_nrows();
    const auto N = source->get_ncols();

    const auto row_type = source->get_row_type();
    const auto col_type = source->get_col_type();
    const auto base     = source->get_index_base();

    auto row_data     = source->get_const_row_host_data();
    auto row_end_data = source->get_const_row_end_host_data();
    auto col_data     = source->get_const_col_host_data();

    const bool row64 = (row_type == HIP_R_64I);
    const bool col64 = (col_type == HIP_R_64I);
    const auto spy   = row64
                           ? (col64
                                  ? hipdss::csr_spy_terminal<int64_t, int64_t>
                                  : hipdss::csr_spy_terminal<int64_t, int32_t>)
                           : (col64
                                  ? hipdss::csr_spy_terminal<int32_t, int64_t>
                                  : hipdss::csr_spy_terminal<int32_t, int32_t>);

    spy(M,
        N,
        row_data,
        row_end_data,
        col_data,
        base);

    return HIPDSS_STATUS_SUCCESS;
}
