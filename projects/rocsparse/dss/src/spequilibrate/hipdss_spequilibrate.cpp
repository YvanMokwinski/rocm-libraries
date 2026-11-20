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

#include "hipdss_spequilibrate.h"
#include "hipdss_control.hpp"
#include "hipdss_enum_utils.hpp"
#include "hipdss_spequilibrate_descr.hpp"
#include "ruiz/hipdss_spequilibrate_ruiz.hpp"

namespace hipdss
{

    static hipdssStatus_t spequilibrate(hipdssHandle_t             handle,
                                        hipdss_spequilibrate_descr descr,
                                        hipdss_spequilibrate_stage stage,
                                        rocsparse_spmat_descr      A,
                                        rocsparse_dnvec_descr      D_left,
                                        rocsparse_dnvec_descr      D_right,
                                        size_t                     buffer_size_in_bytes,
                                        void*                      buffer)
    {
        rocsparse_format format;
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_get_format(A,
                                                             &format));

        const hipdss_spequilibrate_alg alg = descr->get_alg();
        HIPDSS_CHECKARG(1,
                        descr,
                        hipdss::enum_utils::is_invalid(alg),
                        HIPDSS_STATUS_INVALID_VALUE);

        const hipdss_spequilibrate_stage previous_stage = descr->get_stage();

        switch(stage)
        {
        case hipdss_spequilibrate_stage_analysis:
        {
            switch(previous_stage)
            {
            case hipdss_spequilibrate_stage_analysis:
            {
                RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE,
                                                    "invalid stage, the stage hipdss_spequilibrate_stage_analysis has "
                                                    "already "
                                                    "been "
                                                    "executed");
                // LCOV_EXCL_START
            }
                // LCOV_EXCL_STOP

            case hipdss_spequilibrate_stage_compute:
            {
                RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE,
                                                    "invalid stage, the stage hipdss_spequilibrate_stage_analysis cannot "
                                                    "be "
                                                    "called "
                                                    "after "
                                                    "the stage hipdss_spequilibrate_stage_compute");
                // LCOV_EXCL_START
            }
            }
            // LCOV_EXCL_STOP

            descr->set_stage(hipdss_spequilibrate_stage_analysis);
            return HIPDSS_STATUS_SUCCESS;
        }

        case hipdss_spequilibrate_stage_compute:
        {

            RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR((previous_stage == ((hipdss_spequilibrate_stage)-1))
                                                    ? HIPDSS_STATUS_INVALID_VALUE
                                                    : HIPDSS_STATUS_SUCCESS,
                                                "invalid stage, the stage hipdss_spequilibrate_stage_analysis must be "
                                                "executed "
                                                "before "
                                                "the stage hipdss_spequilibrate_stage_compute");

            switch(format)
            {
            case rocsparse_format_csr:
            {
                switch(alg)
                {
                case hipdss_spequilibrate_alg_default:
                case hipdss_spequilibrate_alg_ruiz:
                {
                    RETURN_IF_HIPDSS_ERROR(hipdss::spequilibrate_ruiz(handle,
                                                                      descr,
                                                                      stage,
                                                                      A,
                                                                      D_left,
                                                                      D_right,
                                                                      buffer_size_in_bytes,
                                                                      buffer));
                    break;
                }
                }
                descr->set_stage(hipdss_spequilibrate_stage_compute);
                return HIPDSS_STATUS_SUCCESS;
            }

            case rocsparse_format_coo:
            case rocsparse_format_csc:
            case rocsparse_format_ell:
            case rocsparse_format_sell:
            case rocsparse_format_coo_aos:
            case rocsparse_format_bsr:
            case rocsparse_format_bell:
            {
                // LCOV_EXCL_START
                RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_NOT_SUPPORTED);
            }
            }
        }
        }
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        // LCOV_EXCL_STOP
    }

} // namespace hipdss

extern "C" hipdssStatus_t
    hipdss_spequilibrate(hipdssHandle_t             handle, // 0
                         hipdss_spequilibrate_descr descr, // 1
                         rocsparse_spmat_descr      A, // 2
                         rocsparse_dnvec_descr      D_left, // 3
                         rocsparse_dnvec_descr      D_right, // 4
                         hipdss_spequilibrate_stage stage, // 5
                         size_t                     buffer_size_in_bytes, // 6
                         void*                      buffer, // 7
                         rocsparse_error*           p_error)
try
{
    HIPDSS_CHECKARG_HANDLE(0, handle);
    HIPDSS_CHECKARG_POINTER(1, descr);
    HIPDSS_CHECKARG_POINTER(2, A);
    HIPDSS_CHECKARG_POINTER(3, D_left);
    HIPDSS_CHECKARG_POINTER(4, D_right);

    HIPDSS_CHECKARG_ENUM(5, stage);

    HIPDSS_CHECKARG(6, buffer_size_in_bytes, (buffer_size_in_bytes == 0) && (buffer != nullptr), HIPDSS_STATUS_INVALID_VALUE);

    HIPDSS_CHECKARG(7, buffer, (buffer == nullptr) && (buffer_size_in_bytes != 0), HIPDSS_STATUS_INVALID_VALUE);

    RETURN_IF_HIPDSS_ERROR(hipdss::spequilibrate(
        handle, descr, stage, A, D_left, D_right, buffer_size_in_bytes, buffer));

    return HIPDSS_STATUS_SUCCESS;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
// LCOV_EXCL_STOP
