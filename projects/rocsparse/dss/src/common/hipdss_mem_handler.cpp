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

#include "hipdss_mem_handler.hpp"
#include <hip/hip_runtime_api.h>

namespace
{
    int default_device_alloc(void*       ctx,
                             void**      ptr,
                             size_t      size,
                             hipStream_t stream)
    {
        if(ptr == nullptr)
        {
            return static_cast<int>(hipErrorInvalidValue);
        }
        *ptr = nullptr;
        if(size == 0)
        {
            return static_cast<int>(hipSuccess);
        }

#if HIP_VERSION >= 50300000
        return static_cast<int>(hipMallocAsync(ptr, size, stream));
#else
        return static_cast<int>(hipMalloc(ptr, size));
#endif
    }

    int default_device_free(void*       ctx,
                            void*       ptr,
                            size_t      size,
                            hipStream_t stream)
    {

        if(ptr == nullptr)
        {
            return static_cast<int>(hipSuccess);
        }

#if HIP_VERSION >= 50300000
        return static_cast<int>(hipFreeAsync(ptr, stream));
#else
        return static_cast<int>(hipFree(ptr));
#endif
    }

    int default_pinned_alloc(void*       ctx,
                             void**      ptr,
                             size_t      size,
                             hipStream_t stream)
    {
        if(ptr == nullptr)
        {
            return static_cast<int>(hipErrorInvalidValue);
        }
        *ptr = nullptr;
        if(size == 0)
        {
            return static_cast<int>(hipSuccess);
        }
        return static_cast<int>(hipHostMalloc(ptr,
                                              size));
    }

    int default_pinned_free(void*       ctx,
                            void*       ptr,
                            size_t      size,
                            hipStream_t stream)
    {
        if(ptr == nullptr)
        {
            return static_cast<int>(hipSuccess);
        }
        return static_cast<int>(hipHostFree(ptr));
    }

}

const hipdssDeviceMemHandler_t& hipdss::default_device_mem_handler()
{
    static const hipdssDeviceMemHandler_t s_handler = {nullptr,
                                                       default_device_alloc,
                                                       default_device_free,
                                                       "hipdss default device memory handler"};
    return s_handler;
}

const hipdssPinnedMemHandler_t& hipdss::default_pinned_mem_handler()
{
    static const hipdssPinnedMemHandler_t s_handler = {nullptr,
                                                       default_pinned_alloc,
                                                       default_pinned_free,
                                                       "hipdss default pinned memory handler"};
    return s_handler;
}
