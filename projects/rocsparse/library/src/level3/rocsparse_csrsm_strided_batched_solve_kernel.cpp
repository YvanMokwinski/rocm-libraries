/*! \file */
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
#include "internal/level3/rocsparse_csrsm.h"
#include "rocsparse_csrsm.hpp"

#include "rocsparse_assign_async.hpp"
#include "rocsparse_common.h"
#include "rocsparse_common.hpp"
#include "rocsparse_control.hpp"
#include "rocsparse_utility.hpp"

#include "../level1/rocsparse_gthr.hpp"
#include "../level2/rocsparse_csrsv.hpp"
#include "csrsm_device.h"
#include "rocsparse_csrsm_solve_kernel.hpp"
#include <map>

namespace rocsparse
{

    template <uint32_t BLOCKSIZE, uint32_t WFSIZE, bool SLEEP, typename I, typename J, typename T>
    ROCSPARSE_KERNEL(BLOCKSIZE)
    void csrsm_strided_batched_kernel(rocsparse_operation trans_B,
                                      J                   m,
                                      J                   nrhs,
                                      ROCSPARSE_DEVICE_HOST_SCALAR_PARAMS(T, alpha),
                                      int64_t alpha_stride,
                                      const I* __restrict__ csr_row_ptr,
                                      const J* __restrict__ csr_col_ind,
                                      const T* __restrict__ csr_val,
                                      int64_t csr_val_stride,
                                      T* __restrict__ B,
                                      int64_t ldb,
                                      int64_t B_stride,
                                      int* __restrict__ done_array,
                                      int64_t done_array_stride,
                                      const J* __restrict__ map,
                                      J* __restrict__ zero_pivot,
                                      int64_t              zero_pivot_stride,
                                      rocsparse_index_base idx_base,
                                      rocsparse_fill_mode  fill_mode,
                                      rocsparse_diag_type  diag_type,
                                      bool                 is_host_mode)
    {
        auto i = hipBlockIdx_y;
        ROCSPARSE_DEVICE_HOST_SCALAR_GET(alpha);
        rocsparse::csrsm_device<BLOCKSIZE, WFSIZE, SLEEP>(trans_B,
                                                          m,
                                                          nrhs,
                                                          (is_host_mode) ? alpha
                                                                         : alpha + i * alpha_stride,
                                                          csr_row_ptr,
                                                          csr_col_ind,
                                                          csr_val + i * csr_val_stride,
                                                          B + i * B_stride,
                                                          ldb,
                                                          done_array + i * done_array_stride,
                                                          map,
                                                          zero_pivot + i * zero_pivot_stride,
                                                          idx_base,
                                                          fill_mode,
                                                          diag_type);
    }

    typedef rocsparse_status (*csrsm_strided_batched_kernel_t)(
        rocsparse_handle    handle,
        rocsparse_operation transB,
        int64_t             batch_count,
        int64_t             m,
        int64_t             nrhs,
        const void*         alpha_,
        int64_t             alpha_stride,
        const void* __restrict__ csr_row_ptr_,
        const void* __restrict__ csr_col_ind_,
        const void* __restrict__ csr_val_,
        int64_t csr_val_stride,
        void* __restrict__ B_,
        int64_t ldb,
        int64_t B_stride,
        int32_t* __restrict__ done_array,
        int64_t done_array_stride,
        const void* __restrict__ map_,
        void* __restrict__ zero_pivot_,
        int64_t              zero_pivot_stride,
        rocsparse_index_base idx_base,
        rocsparse_fill_mode  fill_mode,
        rocsparse_diag_type  diag_type);
    template <uint32_t BLOCKSIZE, uint32_t WFSIZE, bool SLEEP, typename I, typename J, typename T>
    static rocsparse_status
        csrsm_strided_batched_kernel_launch(rocsparse_handle    handle,
                                            rocsparse_operation transB,
                                            int64_t             batch_count,
                                            int64_t             m,
                                            int64_t             nrhs,
                                            const void*         alpha_,
                                            int64_t             alpha_stride,
                                            const void* __restrict__ csr_row_ptr_,
                                            const void* __restrict__ csr_col_ind_,
                                            const void* __restrict__ csr_val_,
                                            int64_t csr_val_stride,
                                            void* __restrict__ B_,
                                            int64_t ldb,
                                            int64_t B_stride,
                                            int32_t* __restrict__ done_array,
                                            int64_t done_array_stride,
                                            const void* __restrict__ map_,
                                            void* __restrict__ zero_pivot_,
                                            int64_t              zero_pivot_stride,
                                            rocsparse_index_base idx_base,
                                            rocsparse_fill_mode  fill_mode,
                                            rocsparse_diag_type  diag_type)
    {
        const dim3 csrsm_blocks(((nrhs - 1) / BLOCKSIZE + 1) * m, batch_count);
        const dim3 csrsm_threads(BLOCKSIZE);
        auto       alpha = reinterpret_cast<const T*>(alpha_);
        // rocsparse_pointer_mode_device
        RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
            (rocsparse::csrsm_strided_batched_kernel<BLOCKSIZE, WFSIZE, SLEEP>),
            csrsm_blocks,
            csrsm_threads,
            0,
            handle->stream,
            transB,
            static_cast<J>(m),
            static_cast<J>(nrhs),
            ROCSPARSE_DEVICE_HOST_SCALAR_ARGS(handle, alpha),
            alpha_stride,
            reinterpret_cast<const I*>(csr_row_ptr_),
            reinterpret_cast<const J*>(csr_col_ind_),
            reinterpret_cast<const T*>(csr_val_),
            csr_val_stride,
            reinterpret_cast<T*>(B_),
            ldb,
            B_stride,
            done_array,
            done_array_stride,
            reinterpret_cast<const J*>(map_),
            reinterpret_cast<J*>(zero_pivot_),
            zero_pivot_stride,
            idx_base,
            fill_mode,
            diag_type,
            handle->pointer_mode == rocsparse_pointer_mode_host);
        return rocsparse_status_success;
    }

    template <uint32_t A, uint32_t B, bool S, typename I, typename J>
    static rocsparse::csrsm_strided_batched_kernel_t find_T(rocsparse_datatype b)
    {
        switch(b)
        {
        case rocsparse_datatype_f32_r:
        {
            return csrsm_strided_batched_kernel_launch<A, B, S, I, J, float>;
        }
        case rocsparse_datatype_f32_c:
        {
            return csrsm_strided_batched_kernel_launch<A, B, S, I, J, rocsparse_float_complex>;
        }
        case rocsparse_datatype_f64_r:
        {
            return csrsm_strided_batched_kernel_launch<A, B, S, I, J, double>;
        }
        case rocsparse_datatype_f64_c:
        {
            return csrsm_strided_batched_kernel_launch<A, B, S, I, J, rocsparse_double_complex>;
        }
        default:
        {
            return nullptr;
        }
        }
    }

    template <uint32_t A, uint32_t B, bool S, typename I, typename... P>
    static rocsparse::csrsm_strided_batched_kernel_t find_J(rocsparse_indextype b, P... p)
    {
        switch(b)
        {
        case rocsparse_indextype_i64:
        {
            return find_T<A, B, S, I, int64_t>(p...);
        }
        case rocsparse_indextype_i32:
        {
            return find_T<A, B, S, I, int32_t>(p...);
        }
        default:
        {
            return nullptr;
        }
        }
    }

    template <uint32_t A, uint32_t B, bool S, typename... P>
    static rocsparse::csrsm_strided_batched_kernel_t find_I(rocsparse_indextype b, P... p)
    {
        switch(b)
        {
        case rocsparse_indextype_i64:
        {
            return find_J<A, B, S, int64_t>(p...);
        }
        case rocsparse_indextype_i32:
        {
            return find_J<A, B, S, int32_t>(p...);
        }
        default:
        {
            return nullptr;
        }
        }
    }

    template <uint32_t A, uint32_t B, typename... P>
    static rocsparse::csrsm_strided_batched_kernel_t find_S(bool b, P... p)
    {
        if(b)
        {
            return find_I<A, B, true>(p...);
        }
        else
        {
            return find_I<A, B, false>(p...);
        }
    }

    template <uint32_t A, typename... P>
    static rocsparse::csrsm_strided_batched_kernel_t find_wf(uint32_t b, P... p)
    {
        switch(b)
        {
        case 64:
        {
            return find_S<A, 64>(p...);
        }
        default:
        {
            return nullptr;
        }
        }
    }

    template <typename... P>
    static rocsparse::csrsm_strided_batched_kernel_t find(uint32_t b, P... p)
    {
        switch(b)
        {
        case 64:
        {
            return find_wf<64>(p...);
        }
        case 128:
        {
            return find_wf<128>(p...);
        }
        case 256:
        {
            return find_wf<256>(p...);
        }
        case 512:
        {
            return find_wf<512>(p...);
        }
        case 1024:
        {
            return find_wf<1024>(p...);
        }
        default:
        {
            return nullptr;
        }
        }
    }

    rocsparse_status
        csrsm_strided_batched_kernel_launch_find(rocsparse::csrsm_strided_batched_kernel_t* f_,
                                                 uint32_t                                   A,
                                                 uint32_t                                   B,
                                                 bool                                       C,
                                                 rocsparse_indextype                        i_type_,
                                                 rocsparse_indextype                        j_type_,
                                                 rocsparse_datatype                         a_type_)
    {
        f_[0] = find(A, B, C, i_type_, i_type_, a_type_);
        if(f_[0] == nullptr)
        {
            std::stringstream sstr;
            sstr << "invalid precision configuration: "
                 << ", blocksize: " << A << ", wfsize: " << B << ", sleep: " << C
                 << ", i_type: " << rocsparse::enum_utils::to_string(i_type_)
                 << ", j_type: " << rocsparse::enum_utils::to_string(j_type_)
                 << ", a_type: " << rocsparse::enum_utils::to_string(a_type_);
            RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value,
                                                   sstr.str().c_str());
        }
        // LCOV_EXCL_STOP
        return rocsparse_status_success;
    }

    rocsparse_status
        csrsm_solve_strided_batched_kernel_launch(rocsparse_handle          handle,
                                                  rocsparse_operation       trans_B,
                                                  int64_t                   batch_count,
                                                  int64_t                   m,
                                                  int64_t                   nrhs,
                                                  rocsparse_datatype        alpha_datatype,
                                                  const void*               alpha_,
                                                  int64_t                   alpha_stride,
                                                  const rocsparse_mat_descr descr,
                                                  rocsparse_indextype       csr_row_ptr_indextype,
                                                  const void* __restrict__ csr_row_ptr_,
                                                  rocsparse_indextype csr_col_ind_indextype,
                                                  const void* __restrict__ csr_col_ind_,
                                                  rocsparse_datatype csr_val_datatype,
                                                  const void* __restrict__ csr_val_,
                                                  int64_t            csr_val_stride,
                                                  rocsparse_datatype B_datatype,
                                                  void* __restrict__ B_,
                                                  int64_t                      ldb,
                                                  int64_t                      B_stride,
                                                  rocsparse_mat_info           info,
                                                  rocsparse_fill_mode          fill_mode,
                                                  int32_t*                     done_array,
                                                  int64_t                      done_array_stride,
                                                  const rocsparse::trm_info_t* trm_info,
                                                  rocsparse::pivot_info_t*     pivot_info)

    {

        uint32_t blockdim = 512;
        while(nrhs <= blockdim && blockdim > 32)
            blockdim >>= 1;
        blockdim <<= 1;

        // Determine gcnArch and ASIC revision
        const std::string gcn_arch_name = rocsparse::handle_get_arch_name(handle);
        const int         asicRev       = handle->asic_rev;
        const bool        S = (gcn_arch_name == rocpsarse_arch_names::gfx908 && asicRev < 2);
        rocsparse::csrsm_strided_batched_kernel_t launch_kernel{};
        RETURN_IF_ROCSPARSE_ERROR(csrsm_strided_batched_kernel_launch_find(&launch_kernel,
                                                                           blockdim,
                                                                           64,
                                                                           S,
                                                                           csr_row_ptr_indextype,
                                                                           csr_col_ind_indextype,
                                                                           csr_val_datatype));
        if(launch_kernel == nullptr)
        {
            std::stringstream sstr;
            sstr << "invalid precision configuration: "
                 << ", blocksize: " << blockdim << ", wfsize: "
                 << "64"
                 << ", sleep: " << S
                 << ", i_type: " << rocsparse::enum_utils::to_string(csr_row_ptr_indextype)
                 << ", j_type: " << rocsparse::enum_utils::to_string(csr_col_ind_indextype)
                 << ", a_type: " << rocsparse::enum_utils::to_string(csr_val_datatype);
            RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value,
                                                   sstr.str().c_str());
        }

        RETURN_IF_ROCSPARSE_ERROR(launch_kernel(handle,
                                                trans_B,
                                                batch_count,
                                                m,
                                                nrhs,
                                                alpha_,
                                                alpha_stride,
                                                csr_row_ptr_,
                                                csr_col_ind_,
                                                csr_val_,
                                                csr_val_stride,
                                                B_,
                                                ldb,
                                                B_stride,
                                                done_array,
                                                done_array_stride,
                                                trm_info->get_row_map(),
                                                pivot_info->get_zero_pivot(),
                                                static_cast<int64_t>(1),
                                                descr->base,
                                                fill_mode,
                                                descr->diag_type));
        return rocsparse_status_success;
    }

#if 0
    rocsparse_status csrsm_solve_kernel_launch(rocsparse_handle          handle,
                                               rocsparse_operation       trans_B,
                                               int64_t                   m,
                                               int64_t                   nrhs,
                                               rocsparse_datatype        alpha_datatype,
                                               const void*               alpha_,
                                               const rocsparse_mat_descr descr,
                                               rocsparse_indextype       csr_row_ptr_indextype,
                                               const void* __restrict__ csr_row_ptr_,
                                               rocsparse_indextype csr_col_ind_indextype,
                                               const void* __restrict__ csr_col_ind_,
                                               rocsparse_datatype csr_val_datatype,
                                               const void* __restrict__ csr_val_,
                                               rocsparse_datatype B_datatype,
                                               void* __restrict__ B_,
                                               int64_t                      ldb,
                                               rocsparse_mat_info           info,
                                               rocsparse_fill_mode          fill_mode,
                                               int32_t*                     done_array,
                                               const rocsparse::trm_info_t* trm_info,
					       rocsparse::pivot_info_t*pivot_info)

    {
        RETURN_IF_ROCSPARSE_ERROR(csrsm_solve_strided_batched_kernel_launch(handle,
                                                                            trans_B,
                                                                            static_cast<int64_t>(1),
                                                                            m,
                                                                            nrhs,
                                                                            alpha_datatype,
                                                                            alpha_,
                                                                            static_cast<int64_t>(0),

                                                                            descr,
                                                                            csr_row_ptr_indextype,
                                                                            csr_row_ptr_,
                                                                            csr_col_ind_indextype,
                                                                            csr_col_ind_,
                                                                            csr_val_datatype,
                                                                            csr_val_,
                                                                            static_cast<int64_t>(0),

                                                                            B_datatype,
                                                                            B_,
                                                                            ldb,
                                                                            static_cast<int64_t>(0),
                                                                            info,
                                                                            fill_mode,
                                                                            done_array,
                                                                            static_cast<int64_t>(0),
                                                                            trm_info,
									    pivot_info));
        return rocsparse_status_success;
    }
#endif
}
