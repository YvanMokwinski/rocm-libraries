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
#include "hipdss_rf_descr.hpp"
#include <hip/hip_runtime_api.h>
#include <rocsparse/rocsparse-types.h>

namespace hipdss
{
    struct arch_names
    {
        static constexpr const char* gfx908 = "gfx908";
    };

    //
    // Get architecture name.
    //
    std::string get_arch_name(hipdssHandle_t handle);

    //
    // Get xnack mode.
    //
    std::string get_xnack_mode(hipdssHandle_t handle);
}

/********************************************************************************
 * \brief hipdssContext is a structure holding the library context.
 *******************************************************************************/
struct hipdssContext
{
protected:
    int32_t         m_device;
    hipDeviceProp_t m_properties;
    int32_t         m_wavefront_size;
    int32_t         m_shared_mem_per_block_optin;
    int32_t         m_asic_rev;
    hipStream_t     m_stream = 0;

    std::string m_comm_layer_filename;
    std::string m_threading_layer_filename;

    hipdss_rf_descr          m_rf_descr{};
    hipdssDeviceMemHandler_t m_device_mem_handler{};
    hipdssPinnedMemHandler_t m_pinned_mem_handler{};
    rocsparse_handle         m_rocsparse_handle{};

    double m_host_one_64[2]{1, 0};
    float  m_host_one_32[2]{1, 0};

public:
    bool reordering_phase_completed             = {};
    bool symbolic_factorization_phase_completed = {};
    bool factorization_phase_completed          = {};
    bool refactorization_phase_completed        = {};

    hipdssContext();
    ~hipdssContext();

    hipdssStatus_t set_streams(int32_t            num_streams,
                               const hipStream_t* streams);

    hipdssStatus_t set_stream(hipStream_t user_stream);
    hipStream_t    get_stream();
    hipdssStatus_t get_stream(hipStream_t* user_stream) const;

    const void* get_host_one(hipDataType datatype);

    static std::string get_arch_name(hipdssHandle_t handle);
    static std::string get_xnack_mode(hipdssHandle_t handle);

    operator rocsparse_handle();

    hipdss_rf_descr get_rf_descr();
    void            set_rf_descr(hipdss_rf_descr);

    std::string& get_comm_layer_filename();
    void         set_comm_layer_filename(const char*);

    const hipdssDeviceMemHandler_t& get_device_mem_handler() const;
    void                            set_device_mem_handler(const hipdssDeviceMemHandler_t&);

    const hipdssPinnedMemHandler_t& get_pinned_mem_handler() const;
    void                            set_pinned_mem_handler(const hipdssPinnedMemHandler_t&);

    hipError_t device_malloc(void** ptr,
                             size_t nbytes);

    hipError_t device_free(void* ptr);

    hipError_t device_memcpy(void*         dst,
                             const void*   src,
                             size_t        nbytes,
                             hipMemcpyKind kind);

    hipError_t synchronize_stream();

    template <typename T>
    hipError_t device_malloc(T** ptr, size_t nbytes)
    {
        return this->device_malloc(reinterpret_cast<void**>(ptr), nbytes);
    }
};
