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

#include "hipdss_idvec_descr.hpp"
#include "hipdss_rf.hpp"
#include "hipdss_rf_descr.hpp"
#include "rocdss_timer.hpp"

struct _rocdss_spmat_import_config
{
    const char* filename;
};

extern "C" hipdssStatus_t rocdss_spmat_import_config_cmdline(
    hipdssHandle_t handle, rocdss_spmat_import_config config, int32_t* p_argc, const char* argv[], rocsparse_error* p_error)
try
{
    RETURN_IF_HIPDSS_ERROR(rocdss_spmat_import_config_file_set(
        handle, config, rocdss_spmat_import_config_file_name, argv[1], p_error));

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
