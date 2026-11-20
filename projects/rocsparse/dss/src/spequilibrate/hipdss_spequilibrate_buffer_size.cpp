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

#include "hipdss_control.hpp"
#include "hipdss_spequilibrate.h"
#include "hipdss_spequilibrate_descr.hpp"

namespace hipdss
{
    static hipdssStatus_t spequilibrate_buffer_size(
        hipdssHandle_t              handle,
        hipdss_spequilibrate_descr  descr,
        hipdss_spequilibrate_stage  stage,
        rocsparse_const_spmat_descr A,
        rocsparse_const_dnvec_descr D_left,
        rocsparse_const_dnvec_descr D_right,
        size_t*                     buffer_size_in_bytes)
    {

        rocsparse_format format;
        int64_t          M, N, nnz;
        int32_t          batch_count;

        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_get_format(A, &format));
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_get_size(A, &M, &N, &nnz));
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_get_strided_batch(A, &batch_count));
        if(format != rocsparse_format_csr)
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }

        buffer_size_in_bytes[0] = std::numeric_limits<size_t>::max();
        switch(stage)
        {
        case hipdss_spequilibrate_stage_analysis:
        {
            buffer_size_in_bytes[0] = 0;
            return HIPDSS_STATUS_SUCCESS;
        }

        case hipdss_spequilibrate_stage_compute:
        {
            switch(descr->get_alg())
            {
            case hipdss_spequilibrate_alg_default:
            case hipdss_spequilibrate_alg_ruiz:
            {
                buffer_size_in_bytes[0] = ((batch_count * sizeof(int32_t) - 1) / 256 + 1) * 256;
                buffer_size_in_bytes[0] += ((std::max(M, N) * sizeof(double) - 1) / 256 + 1) * 256; //
                buffer_size_in_bytes[0] += ((batch_count * sizeof(double) - 1) / 256 + 1) * 256;
                buffer_size_in_bytes[0] += ((sizeof(double) - 1) / 256 + 1) * 256;

                return HIPDSS_STATUS_SUCCESS;
            }
            }
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
            // LCOV_EXCL_START
        }
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        // LCOV_EXCL_STOP
    }

} // namespace hipdss
/*
   * ===========================================================================
   *    C wrapper
   * ===========================================================================
   */
extern "C" hipdssStatus_t hipdss_spequilibrate_buffer_size(
    hipdssHandle_t handle, hipdss_spequilibrate_descr descr, rocsparse_const_spmat_descr A, rocsparse_const_dnvec_descr D_left, rocsparse_const_dnvec_descr D_right, hipdss_spequilibrate_stage stage, size_t* buffer_size_in_bytes, rocsparse_error* p_error)
try
{

    HIPDSS_CHECKARG_HANDLE(0, handle);
    HIPDSS_CHECKARG_POINTER(1, descr);
    HIPDSS_CHECKARG_POINTER(2, A);
    HIPDSS_CHECKARG_POINTER(3, D_left);
    HIPDSS_CHECKARG_POINTER(4, D_right);
    HIPDSS_CHECKARG_ENUM(5, stage);
    HIPDSS_CHECKARG_POINTER(6, buffer_size_in_bytes);

    RETURN_IF_HIPDSS_ERROR(hipdss::spequilibrate_buffer_size(
        handle, descr, stage, A, D_left, D_right, buffer_size_in_bytes));
    return HIPDSS_STATUS_SUCCESS;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
// LCOV_EXCL_STOP
