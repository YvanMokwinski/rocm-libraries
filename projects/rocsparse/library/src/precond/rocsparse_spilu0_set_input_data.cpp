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
inline bool rocsparse::enum_utils::is_invalid(rocsparse_spilu0_input_data value)
{
    switch(value)
    {
    case rocsparse_spilu0_input_data_boost_tol:
    case rocsparse_spilu0_input_data_boost_val:
    {
        return false;
    }
    }
    return true;
};

extern "C" rocsparse_status rocsparse_spilu0_set_input_data(rocsparse_handle       handle,
                                                            rocsparse_spilu0_descr spilu0_descr,
                                                            rocsparse_spilu0_input_data input_data,
                                                            const void*                 data,
                                                            size_t           data_size_in_bytes,
                                                            rocsparse_error* p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, spilu0_descr);
    ROCSPARSE_CHECKARG_ENUM(2, input_data);
    ROCSPARSE_CHECKARG_POINTER(3, data);

    switch(input)
    {
    case rocsparse_spilu0_input_data_boost_val:
    {
        spilu0_descr->set_boost_val(data);
        return rocsparse_status_success;
    }
    case rocsparse_spilu0_input_data_boost_tol:
    {
        spilu0_descr->set_boost_tol(data);
        spilu0_descr->set_boost_tol_size(data_size_in_bytes);
        return rocsparse_status_success;
    }
    }
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
