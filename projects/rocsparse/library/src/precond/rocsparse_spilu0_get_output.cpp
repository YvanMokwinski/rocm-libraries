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

#include "rocsparse_spilu0.h"
#include "rocsparse_spilu0_descr.hpp"
#include "rocsparse_utility.hpp"

template <>
inline bool rocsparse::enum_utils::is_invalid(rocsparse_spilu0_output value)
{
    switch(value)
    {
    case rocsparse_spilu0_output_singular_pivot:
    {
        return false;
    }
    }
    return true;
};

extern "C" rocsparse_status rocsparse_spilu0_get_output(rocsparse_handle        handle,
                                                        rocsparse_spilu0_descr  spilu0_descr,
                                                        rocsparse_spilu0_output output,
                                                        void*                   data,
                                                        size_t                  data_size_in_bytes,
                                                        rocsparse_error*        p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;
    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, spilu0_descr);
    ROCSPARSE_CHECKARG_ENUM(2, output);
    ROCSPARSE_CHECKARG_POINTER(3, data);
    ROCSPARSE_CHECKARG(
        4, data_size_in_bytes, data_size_in_bytes == 0, rocsparse_status_invalid_size);

    switch(output)
    {

    case rocsparse_spilu0_output_singular_pivot:
    {
        ROCSPARSE_CHECKARG(4,
                           data_size_in_bytes,
                           data_size_in_bytes != sizeof(int64_t),
                           rocsparse_status_invalid_size);

        auto    csrilu0_info = spilu0_descr->get_csrilu0_info();
        int64_t singular_pivot_value;
        auto    status = csrilu0_info->copy_singular_pivot_async(rocsparse_pointer_mode_host,
                                                              rocsparse::get_indextype<int64_t>(),
                                                              &singular_pivot_value,
                                                              handle->stream);
        RETURN_IF_HIP_ERROR(hipStreamSynchronize(handle->stream));
        *reinterpret_cast<int64_t*>(data) = singular_pivot_value;
        if(status == rocsparse_status_zero_pivot)
        {
            return status;
        }
        RETURN_IF_ROCSPARSE_ERROR(status);
        return rocsparse_status_success;
    }
    }
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
