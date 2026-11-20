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
#include "hipdssContext.hpp"
#include "hipdss_control.hpp"
#include "hipdss_datatype_utils.hpp"

hipdssStatus_t hipdss::idvec_descr::to_device(hipdssHandle_t handle)
{
    const auto nbytes = this->get_size_in_bytes();

    //
    // if host
    //
    if(this->m_const_host_data == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
    }

    if(this->m_device_data == nullptr && this->m_const_device_data != nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
    }

    if(this->m_device_data == nullptr)
    {
        RETURN_IF_HIP_ERROR(handle->device_malloc(&this->m_device_data,
                                                  nbytes));
        this->m_const_device_data = this->m_device_data;
        this->m_own_device_data   = true;
    }

    RETURN_IF_HIP_ERROR(handle->device_memcpy(this->m_device_data,
                                              this->m_const_host_data,
                                              nbytes,
                                              hipMemcpyHostToDevice));

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdss::idvec_descr::to_host(hipdssHandle_t handle)
{
    const auto nbytes = this->get_size_in_bytes();

    if(this->m_const_device_data == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
    }

    if(this->m_host_data == nullptr && this->m_const_host_data != nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
    }

    if(this->m_host_data == nullptr)
    {
        this->m_host_data       = malloc(nbytes);
        this->m_const_host_data = this->m_host_data;
        this->m_own_host_data   = true;
    }

    RETURN_IF_HIP_ERROR(handle->device_memcpy(this->m_host_data,
                                              this->m_const_device_data,
                                              nbytes,
                                              hipMemcpyDeviceToHost));

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdss::idvec_descr::set_host_identity(hipdssHandle_t handle)
{

    if(this->m_indextype == HIP_R_32I)
    {
        auto* v = reinterpret_cast<int32_t*>(this->m_host_data);
        for(int64_t i = 0; i < this->m_size; ++i)
        {
            v[i] = i;
        }
        return HIPDSS_STATUS_SUCCESS;
    }
    else if(this->m_indextype == HIP_R_64I)
    {
        auto* v = reinterpret_cast<int64_t*>(this->m_host_data);
        for(int64_t i = 0; i < this->m_size; ++i)
        {
            v[i] = i;
        }
        return HIPDSS_STATUS_SUCCESS;
    }
    else
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
    }
}

hipdss::idvec_descr::idvec_descr(hipdssHandle_t    handle,
                                 hipDataType       indextype,
                                 hipdssIndexBase_t base,
                                 int64_t           size,
                                 bool              on_device)
    : m_indextype(indextype)
    , m_index_base(base)
    , m_size(size)
{
    const auto nbytes = this->get_size_in_bytes();
    if(on_device)
    {
        std::ignore = handle->device_malloc(&this->m_device_data,
                                            nbytes);

        this->m_const_device_data = this->m_device_data;
        this->m_own_device_data   = true;
    }
    else
    {
        this->m_host_data       = malloc(nbytes);
        this->m_const_host_data = this->m_host_data;
        this->m_own_host_data   = true;
    }
}

size_t hipdss::idvec_descr::get_size_in_bytes() const
{
    return this->get_size() * hipdss::datatype_sizeof(this->m_indextype);
}

hipDataType hipdss::idvec_descr::get_indextype() const
{
    return this->m_indextype;
}

void hipdss::idvec_descr::set_indextype(hipDataType value)
{
    this->m_indextype = value;
}

hipdssIndexBase_t hipdss::idvec_descr::get_base() const
{
    return this->m_index_base;
}

void hipdss::idvec_descr::set_base(hipdssIndexBase_t value)
{
    this->m_index_base = value;
}

int64_t hipdss::idvec_descr::get_size() const
{
    return this->m_size;
}

void hipdss::idvec_descr::set_size(int64_t value)
{
    this->m_size = value;
}

const void* hipdss::idvec_descr::const_host_data() const
{
    return this->m_const_host_data;
}

const void* hipdss::idvec_descr::const_host_data()
{
    return this->m_const_host_data;
}

const void* hipdss::idvec_descr::const_device_data() const
{
    return this->m_const_device_data;
}

const void* hipdss::idvec_descr::const_device_data()
{
    return this->m_const_device_data;
}

const void* hipdss::idvec_descr::host_data() const
{
    return this->m_host_data;
}

void* hipdss::idvec_descr::host_data()
{
    return this->m_host_data;
}

const void* hipdss::idvec_descr::device_data() const
{
    return this->m_device_data;
}

void* hipdss::idvec_descr::device_data()
{
    return this->m_device_data;
}

hipdssStatus_t hipdss::idvec_descr::destroy(hipdssHandle_t handle)
{

    if(this->m_own_device_data && this->m_device_data)
    {
        if(this->m_device_data)
        {
            RETURN_IF_HIP_ERROR(handle->device_free(this->m_device_data));
        }
    }

    if(this->m_own_host_data && this->m_host_data)
    {
        if(this->m_host_data)
        {
            free(this->m_host_data);
        }
    }

    this->m_device_data       = nullptr;
    this->m_const_device_data = nullptr;

    this->m_host_data       = nullptr;
    this->m_const_host_data = nullptr;

    return HIPDSS_STATUS_SUCCESS;
}

hipdss::idvec_descr::idvec_descr(hipDataType       indextype,
                                 hipdssIndexBase_t base,
                                 int64_t           size,
                                 const void*       const_data,
                                 void*             data,
                                 bool              on_device)
    : m_indextype(indextype)
    , m_index_base(base)
    , m_size(size)
{
    if(on_device)
    {
        this->m_const_device_data = const_data;
        this->m_device_data       = data;
    }
    else
    {
        this->m_const_host_data = const_data;
        this->m_host_data       = data;
    }
}

namespace hipdss
{

    template <typename T>
    static void idvec_print(int64_t     size,
                            const void* values)
    {
        const T* v = reinterpret_cast<const T*>(values);
        for(int64_t i = 0; i < size; ++i)
        {
            std::cout << "v[" << i << "] = " << v[i] << std::endl;
        }
    }

}

hipdssStatus_t hipdss::idvec_descr::print(hipdssHandle_t handle)
{

    RETURN_IF_HIPDSS_ERROR(this->to_host(handle));

    const auto datatype = this->get_indextype();
    auto       apply    = hipdss::idvec_print<int32_t>;
    switch(datatype)
    {
    case HIP_R_32I:
    {
        apply = hipdss::idvec_print<int32_t>;
        break;
    }

    case HIP_R_64I:
    {
        apply = hipdss::idvec_print<int64_t>;
        break;
    }

    default:
    {
        RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                            "unsupported datatype");
    }
    }

    apply(this->get_size(),
          this->const_host_data());

    return HIPDSS_STATUS_SUCCESS;
}
