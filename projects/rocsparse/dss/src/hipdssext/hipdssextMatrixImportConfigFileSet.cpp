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

#include "hipdss.h"
#include "hipdss_control.hpp"
#include "hipdss_enum_utils.hpp"

#include "hipdss_matrix_import_config.hpp"

template <>
bool hipdss::enum_utils::is_invalid<hipdssextMatrixImportConfigFile_t>(hipdssextMatrixImportConfigFile_t value)
{
    switch(value)
    {
    case HIPDSSEXT_MATRIX_IMPORT_CONFIG_FILE_NAME:
    {
        return false;
    }
    }
    return true;
}

extern "C" hipdssStatus_t hipdssextMatrixImportConfigFileSet(hipdssHandle_t                    handle,
                                                             hipdssextMatrixImportConfig_t     config,
                                                             hipdssextMatrixImportConfigFile_t field,
                                                             const void*                       data)
try
{
    HIPDSS_CHECKARG_HANDLE(0, handle);
    HIPDSS_CHECKARG_POINTER(1, config);
    HIPDSS_CHECKARG_ENUM(2, field);
    HIPDSS_CHECKARG_POINTER(3, data);
    switch(field)
    {
    case HIPDSSEXT_MATRIX_IMPORT_CONFIG_FILE_NAME:
    {
        config->set_filename(reinterpret_cast<const char*>(data));
        return HIPDSS_STATUS_SUCCESS;
    }
    }
    RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
