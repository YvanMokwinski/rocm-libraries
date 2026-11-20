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

#include "hipdss.hpp"
#include "hipdss_control.hpp"

extern "C" hipdssStatus_t hipdssGetProperty(libraryPropertyType propertyType,
                                            int*                value)
try
{

    HIPDSS_CHECKARG_POINTER(1,
                            value);

    switch(propertyType)
    {

    case MAJOR_VERSION:
    {
        value[0] = HIPDSS_VERSION_MAJOR;
        return HIPDSS_STATUS_SUCCESS;
    }

    case MINOR_VERSION:
    {
        value[0] = HIPDSS_VERSION_PATCH;
        return HIPDSS_STATUS_SUCCESS;
    }

    case PATCH_LEVEL:
    {
        value[0] = HIPDSS_VERSION_PATCH;
        return HIPDSS_STATUS_SUCCESS;
    }
    }

    RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
}

catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
