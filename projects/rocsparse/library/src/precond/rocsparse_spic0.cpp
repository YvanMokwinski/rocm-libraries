/* ************************************************************************
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights Reserved.
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

#include <map>
#include <sstream>

#include "rocsparse_control.hpp"
#include "rocsparse_enum_utils.hpp"
#include "rocsparse_handle.hpp"
#include "rocsparse_utility.hpp"

#include "../conversion/rocsparse_convert_array.hpp"

#include "rocsparse_bsric0.hpp"
#include "rocsparse_csric0.hpp"
#include "rocsparse_spic0.h"
#include "rocsparse_spic0_descr.hpp"

template <>
inline bool rocsparse::enum_utils::is_invalid(rocsparse_spic0_stage value)
{
    switch(value)
    {
    case rocsparse_spic0_stage_analysis:
    case rocsparse_spic0_stage_compute:
    {
        return false;
    }
    }
    return true;
};

namespace rocsparse
{
    static rocsparse_status spic0(rocsparse_handle      handle,
                                  rocsparse_spic0_descr spic0_descr,
                                  rocsparse_spmat_descr A,
                                  rocsparse_spic0_stage spic0_stage,
                                  size_t                buffer_size_in_bytes,
                                  void*                 buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;
        const rocsparse_format      format         = A->format;
        const rocsparse_spic0_stage previous_stage = spic0_descr->get_stage();
        switch(spic0_stage)
        {
        case rocsparse_spic0_stage_analysis:
        {
            switch(previous_stage)
            {
            case rocsparse_spic0_stage_analysis:
            {
                RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
                    rocsparse_status_invalid_value,
                    "invalid stage, the stage rocsparse_spic0_stage_analysis has already "
                    "been "
                    "executed");
            }

            case rocsparse_spic0_stage_compute:
            {
                RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
                    rocsparse_status_invalid_value,
                    "invalid stage, the stage rocsparse_spic0_stage_analysis cannot be "
                    "called "
                    "after "
                    "the stage rocsparse_spic0_stage_compute");
            }
            }

            const rocsparse_analysis_policy analysis_policy = spic0_descr->get_analysis_policy();
            if(rocsparse::enum_utils::is_invalid(analysis_policy))
            {
                RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value,
                                                       "invalid analysis_policy");
            }

            switch(format)
            {
            case rocsparse_format_csr:
            {
                rocsparse_csric0_info csric0_info{};
                switch(analysis_policy)
                {
                case rocsparse_analysis_policy_reuse:
                {
                    spic0_descr->set_shared_csric0_info(A->info->get_shared_csric0_info());
                    csric0_info = spic0_descr->get_csric0_info();
                    break;
                }
                case rocsparse_analysis_policy_force:
                {
                    csric0_info = nullptr;
                    break;
                }
                }

                RETURN_IF_ROCSPARSE_ERROR((rocsparse::csric0_analysis(handle,
                                                                      A,
                                                                      analysis_policy,
                                                                      rocsparse_solve_policy_auto,
                                                                      &csric0_info,
                                                                      buffer)));
                switch(analysis_policy)
                {
                case rocsparse_analysis_policy_reuse:
                {
                    break;
                }
                case rocsparse_analysis_policy_force:
                {
                    spic0_descr->set_csric0_info(csric0_info);
                    break;
                }
                }
                spic0_descr->set_stage(rocsparse_spic0_stage_analysis);
                return rocsparse_status_success;
            }

            case rocsparse_format_bsr:
            {
                rocsparse_bsric0_info bsric0_info{};
                switch(analysis_policy)
                {
                case rocsparse_analysis_policy_reuse:
                {
                    spic0_descr->set_shared_bsric0_info(A->info->get_shared_bsric0_info());
                    bsric0_info = spic0_descr->get_bsric0_info();
                    break;
                }
                case rocsparse_analysis_policy_force:
                {
                    bsric0_info = nullptr;
                    break;
                }
                }

                RETURN_IF_ROCSPARSE_ERROR((rocsparse::bsric0_analysis(handle,
                                                                      A,
                                                                      analysis_policy,
                                                                      rocsparse_solve_policy_auto,
                                                                      &bsric0_info,
                                                                      buffer)));
                switch(analysis_policy)
                {
                case rocsparse_analysis_policy_reuse:
                {
                    break;
                }
                case rocsparse_analysis_policy_force:
                {
                    spic0_descr->set_bsric0_info(bsric0_info);
                    break;
                }
                }
                spic0_descr->set_stage(rocsparse_spic0_stage_analysis);
                return rocsparse_status_success;
            }

            case rocsparse_format_coo:
            case rocsparse_format_csc:
            case rocsparse_format_ell:
            case rocsparse_format_bell:
            case rocsparse_format_coo_aos:
            {
                // LCOV_EXCL_START
                RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_not_implemented);
                // LCOV_EXCL_STOP
            }
            }
        }
        case rocsparse_spic0_stage_compute:
        {

            if(previous_stage == ((rocsparse_spic0_stage)-1))
            {
                RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
                    rocsparse_status_invalid_value,
                    "invalid stage, the stage rocsparse_spic0_stage_analysis must be executed "
                    "before "
                    "the stage rocsparse_spic0_stage_compute");
            }

            switch(format)
            {
            case rocsparse_format_csr:
            {
                RETURN_IF_ROCSPARSE_ERROR(rocsparse::csric0_solve(handle,
                                                                  A,
                                                                  rocsparse_solve_policy_auto,
                                                                  spic0_descr->get_csric0_info(),
                                                                  buffer));
                spic0_descr->set_stage(rocsparse_spic0_stage_compute);
                return rocsparse_status_success;
            }

            case rocsparse_format_bsr:
            {
                RETURN_IF_ROCSPARSE_ERROR(rocsparse::bsric0_solve(handle,
                                                                  A,
                                                                  rocsparse_solve_policy_auto,
                                                                  spic0_descr->get_bsric0_info(),
                                                                  buffer));

                spic0_descr->set_stage(rocsparse_spic0_stage_compute);
                return rocsparse_status_success;
            }

            case rocsparse_format_coo:
            case rocsparse_format_csc:
            case rocsparse_format_ell:
            case rocsparse_format_bell:
            case rocsparse_format_coo_aos:
            {
                // LCOV_EXCL_START
                RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_not_implemented);
                // LCOV_EXCL_STOP
            }
            }
        }
        }
        // LCOV_EXCL_START
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
        // LCOV_EXCL_STOP
    }
}

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */

extern "C" rocsparse_status rocsparse_spic0(rocsparse_handle      handle, // 0
                                            rocsparse_spic0_descr spic0_descr, // 1
                                            rocsparse_spmat_descr A, // 2
                                            rocsparse_spic0_stage spic0_stage, // 3
                                            size_t                buffer_size_in_bytes, // 4
                                            void*                 buffer, // 5
                                            rocsparse_error*      p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;
    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, spic0_descr);
    ROCSPARSE_CHECKARG_POINTER(2, A);
    ROCSPARSE_CHECKARG_ENUM(3, spic0_stage);

    ROCSPARSE_CHECKARG(4,
                       buffer_size_in_bytes,
                       (buffer_size_in_bytes == 0) && (buffer != nullptr),
                       rocsparse_status_invalid_size);

    ROCSPARSE_CHECKARG(5,
                       buffer,
                       (buffer == nullptr) && (buffer_size_in_bytes != 0),
                       rocsparse_status_invalid_pointer);

    // Check if descriptors are initialized
    // Basically this never happens, but I let it here.
    // LCOV_EXCL_START
    ROCSPARSE_CHECKARG(2, A, (A->init == false), rocsparse_status_not_initialized);
    // LCOV_EXCL_STOP

    // Check for matching types while we do not support mixed precision computation
    ROCSPARSE_CHECKARG(2,
                       A,
                       (A->data_type != spic0_descr->get_compute_datatype()),
                       rocsparse_status_not_implemented);

    RETURN_IF_ROCSPARSE_ERROR(
        rocsparse::spic0(handle, spic0_descr, A, spic0_stage, buffer_size_in_bytes, buffer));
    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP
