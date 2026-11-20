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

#pragma once

#include "hipdss_data_types.h"
#include <rocsparse/rocsparse-types.h>

#include <cstdio>

namespace hipdss
{
    struct idvec_descr;
    using idvec_descr_t = idvec_descr*;
}

struct hipdss::idvec_descr
{
protected:
    hipDataType       m_indextype{};
    hipdssIndexBase_t m_index_base{};
    int64_t           m_size{};
    bool              m_own_device_data{};
    bool              m_own_host_data{};

    const void* m_const_device_data{};
    void*       m_device_data{};

    const void* m_const_host_data{};
    void*       m_host_data{};

public:
    hipdssStatus_t print(hipdssHandle_t);
    hipdssStatus_t to_device(hipdssHandle_t);
    hipdssStatus_t to_host(hipdssHandle_t);
    hipdssStatus_t destroy(hipdssHandle_t handle);

    hipDataType get_indextype() const;
    void        set_indextype(hipDataType value);

    hipdssStatus_t set_host_identity(hipdssHandle_t handle);
    //
    //
    //
    hipdssIndexBase_t get_base() const;
    void              set_base(hipdssIndexBase_t value);

    //
    //
    //
    int64_t get_size() const;
    void    set_size(int64_t value);

    size_t get_size_in_bytes() const;

    const void* const_host_data() const;
    const void* const_host_data();
    const void* host_data() const;
    void*       host_data();

    const void* const_device_data() const;
    const void* const_device_data();
    const void* device_data() const;
    void*       device_data();

    idvec_descr()  = default;
    ~idvec_descr() = default;

    idvec_descr(hipDataType       indextype,
                hipdssIndexBase_t base,
                int64_t           size,
                const void*       const_data,
                void*             data,
                bool              on_device);

    idvec_descr(hipdssHandle_t    handle,
                hipDataType       indextype,
                hipdssIndexBase_t base,
                int64_t           size,
                bool              on_device);
};
