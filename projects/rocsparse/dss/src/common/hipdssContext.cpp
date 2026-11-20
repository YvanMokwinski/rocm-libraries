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

#include "hipdssContext.hpp"
#include "hipdss_control.hpp"
#include "hipdss_mem_handler.hpp"
#include <rocsparse/rocsparse-auxiliary.h>

HIPDSS_KERNEL(1)
void init_kernel(){};

hipError_t hipdssContext::synchronize_stream()
{
    return hipStreamSynchronize(this->m_stream);
}

const void* hipdssContext::get_host_one(hipDataType value)
{
    switch(value)
    {

    case HIP_C_32F:
    case HIP_R_32F:
    {
        return &this->m_host_one_32[0];
    }

    case HIP_R_64F:
    case HIP_C_64F:
    {
        return &this->m_host_one_64[0];
    }

    case HIP_R_32I:
    case HIP_R_64I:
    default:
    {
        THROW_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR,
                                           "unsupported datatype");
    }
    }
}

hipdssContext::hipdssContext()
    : m_device_mem_handler(hipdss::default_device_mem_handler())
    , m_pinned_mem_handler(hipdss::default_pinned_mem_handler())
{
    hipStream_t stream{};
    // Default device is active device
    THROW_IF_HIP_ERROR(hipGetDevice(&this->m_device));
    THROW_IF_HIP_ERROR(hipGetDeviceProperties(&this->m_properties, this->m_device));

    // Device wavefront size
    this->m_wavefront_size = this->m_properties.warpSize;

    // Shared memory per block opt-in
    this->m_shared_mem_per_block_optin = this->m_properties.sharedMemPerBlockOptin;

#if HIP_VERSION >= 307
    // ASIC revision
    this->m_asic_rev = this->m_properties.asicRevision;
#else
    this->m_asic_rev = 0;
#endif

    // Execute empty kernel for initialization
    THROW_WITH_MESSAGE_IF_HIP_ERROR(hipGetLastError(),
                                    "prior to hipLaunchKernelGGL");
    hipLaunchKernelGGL(init_kernel, dim3(1), dim3(1), 0, stream);
    THROW_WITH_MESSAGE_IF_HIP_ERROR(hipGetLastError(),
                                    "'empty kernel scheduling failed'");

    hipdss_rf_descr rf_descr{};
    THROW_IF_HIPDSS_ERROR(hipdss_rf_descr_create(this,
                                                 &rf_descr));

    this->set_rf_descr(rf_descr);

    THROW_IF_ROCSPARSE_ERROR(rocsparse_create_handle(&this->m_rocsparse_handle));
}

hipdssContext::~hipdssContext()
{
    const hipdssStatus_t status = hipdss_rf_descr_destroy(this, this->m_rf_descr);
    this->m_rf_descr            = nullptr;
    if(status != HIPDSS_STATUS_SUCCESS)
    {
        HIPDSS_ERROR_MESSAGE(status, "rf descriptor destruction failed");
    }
}

hipdssStatus_t hipdssContext::set_streams(int32_t            num_streams,
                                          const hipStream_t* streams)
{
    this->m_stream = streams[0];
    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssContext::set_stream(hipStream_t user_stream)
{
    this->m_stream = user_stream;
    return HIPDSS_STATUS_SUCCESS;
}

hipStream_t hipdssContext::get_stream()
{
    return this->m_stream;
}

hipdssStatus_t hipdssContext::get_stream(hipStream_t* user_stream) const
{

    *user_stream = this->m_stream;
    return HIPDSS_STATUS_SUCCESS;
}

hipdssContext::operator rocsparse_handle()
{
    return this->m_rocsparse_handle;
}

hipdss_rf_descr hipdssContext::get_rf_descr()
{
    return this->m_rf_descr;
}

void hipdssContext::set_rf_descr(hipdss_rf_descr pointer)
{
    this->m_rf_descr = pointer;
}

std::string& hipdssContext::get_comm_layer_filename()
{
    return this->m_comm_layer_filename;
}

void hipdssContext::set_comm_layer_filename(const char* pointer)
{
    this->m_comm_layer_filename = std::string(pointer);
}

hipError_t hipdssContext::device_malloc(void** ptr,
                                        size_t nbytes)
{

    const hipdssDeviceMemHandler_t& h = this->m_device_mem_handler;
    return (h.device_alloc(h.ctx, ptr, nbytes, this->m_stream) == 0)
               ? hipSuccess
               : hipErrorOutOfMemory;
}

hipError_t hipdssContext::device_free(void* ptr)
{
    if(ptr == nullptr)
    {
        return hipSuccess;
    }

    const hipdssDeviceMemHandler_t& h = this->m_device_mem_handler;
    return (h.device_free(h.ctx, ptr, 0, this->m_stream) == 0)
               ? hipSuccess
               : hipErrorInvalidValue;
}

hipError_t hipdssContext::device_memcpy(void*         dst,
                                        const void*   src,
                                        size_t        nbytes,
                                        hipMemcpyKind kind)
{
    return hipMemcpyAsync(dst, src, nbytes, kind, this->m_stream);
}

const hipdssDeviceMemHandler_t& hipdssContext::get_device_mem_handler() const
{
    return this->m_device_mem_handler;
}

void hipdssContext::set_device_mem_handler(const hipdssDeviceMemHandler_t& handler)
{
    this->m_device_mem_handler = handler;
}

const hipdssPinnedMemHandler_t& hipdssContext::get_pinned_mem_handler() const
{
    return this->m_pinned_mem_handler;
}

void hipdssContext::set_pinned_mem_handler(const hipdssPinnedMemHandler_t& handler)
{
    this->m_pinned_mem_handler = handler;
}

// Emulate C++17 std::void_t
template <typename...>
using void_t = void;

// By default, use gcnArch converted to a string prepended by gfx
template <typename PROP, typename = void>
struct ArchName
{
    std::string operator()(const PROP& prop) const;
};

// If gcnArchName exists as a member, use it instead
template <typename PROP>
struct ArchName<PROP, void_t<decltype(PROP::gcnArchName)>>
{
    std::string operator()(const PROP& prop) const
    {
        // strip out xnack/ecc from name
        std::string gcnArchName(prop.gcnArchName);
        std::string gcnArch = gcnArchName.substr(0, gcnArchName.find(":"));
        return gcnArch;
    }
};

// If gcnArchName not present, no xnack mode
template <typename PROP, typename = void>
struct XnackMode
{
    std::string operator()(const PROP& prop) const
    {
        return "";
    }
};

// If gcnArchName exists as a member, use it
template <typename PROP>
struct XnackMode<PROP, void_t<decltype(PROP::gcnArchName)>>
{
    std::string operator()(const PROP& prop) const
    {
        // strip out xnack/ecc from name
        std::string gcnArchName(prop.gcnArchName);
        auto        loc = gcnArchName.find("xnack");
        std::string xnackMode;
        if(loc != std::string::npos)
        {
            xnackMode = gcnArchName.substr(loc, 6);
            // guard against missing +/- at end of xnack mode
            if(xnackMode.size() < 6)
                xnackMode = "";
        }
        return xnackMode;
    }
};

std::string hipdssContext::get_arch_name(hipdssHandle_t handle)
{
    return ArchName<hipDeviceProp_t>{}(handle->m_properties);
}

std::string hipdssContext::get_xnack_mode(hipdssHandle_t handle)
{
    return XnackMode<hipDeviceProp_t>{}(handle->m_properties);
}

hipdssStatus_t hipdss::exception_to_hipdss_status(std::exception_ptr e)
try
{
    if(e)
        std::rethrow_exception(e);
    return HIPDSS_STATUS_SUCCESS;
}
catch(const hipdssStatus_t& status)
{
    return status;
}
catch(const std::bad_alloc&)
{
    return HIPDSS_STATUS_ALLOC_FAILED;
}
catch(...)
{
    return HIPDSS_STATUS_INTERNAL_ERROR;
}

/*******************************************************************************
 * \brief convert hipError_t to hipdssStatus_t
 * TODO - enumerate library calls to hip runtime, enumerate possible errors from
 * those calls
 ******************************************************************************/
hipdssStatus_t hipdss::get_hipdss_status_for_hip_status(hipError_t status)
{
    switch(status)
    {
    // success
    case hipSuccess:
        return HIPDSS_STATUS_SUCCESS;

    // internal hip memory allocation
    case hipErrorMemoryAllocation:
    case hipErrorLaunchOutOfResources:
        return HIPDSS_STATUS_ALLOC_FAILED;

    // user-allocated hip memory
    case hipErrorInvalidDevicePointer: // hip memory
        return HIPDSS_STATUS_INVALID_VALUE;

    // user-allocated device, stream, event
    case hipErrorInvalidDevice:
    case hipErrorInvalidResourceHandle:
        return HIPDSS_STATUS_INVALID_VALUE;

    // library using hip incorrectly
    case hipErrorInvalidValue:
        return HIPDSS_STATUS_INTERNAL_ERROR;

    // hip runtime failing
    case hipErrorNoDevice: // no hip devices
    case hipErrorUnknown:
    default:
        return HIPDSS_STATUS_EXECUTION_FAILED;
    }
}
