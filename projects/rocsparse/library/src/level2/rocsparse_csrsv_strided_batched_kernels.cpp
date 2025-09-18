/*! \file */
/* ************************************************************************
 * Copyright (C) 2018-2025 Advanced Micro Devices, Inc. All rights Reserved.
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

#include <map>
#include <sstream>

#include "rocsparse.h"
#include "rocsparse_control.hpp"
#include "rocsparse_enum_utils.hpp"
#include "rocsparse_handle.hpp"
#include "rocsparse_utility.hpp"

#include "internal/level2/rocsparse_csrsv.h"
#include "rocsparse_csrsv.hpp"

#include "../level1/rocsparse_gthr.hpp"
#include "csrsv_device.h"
#include "rocsparse_assign_async.hpp"
#include "rocsparse_common.h"
#include "rocsparse_control.hpp"
#include "rocsparse_csrsv_solve_kernel.hpp"
#include "rocsparse_utility.hpp"
#include <map>

#include "rocsparse_csrsv_solve_kernel.hpp"
#include "rocsparse_determine_indextype.hpp"
#include <map>

namespace rocsparse
{
    template <uint32_t BLOCKSIZE, uint32_t WF_SIZE, bool SLEEP, typename I, typename J, typename T>
    ROCSPARSE_KERNEL(BLOCKSIZE)
    void csrsv_kernel(J m,
                      ROCSPARSE_DEVICE_HOST_SCALAR_PARAMS(T, alpha),
                      int64_t alpha_stride,
                      const I* __restrict__ csr_row_ptr,
                      const J* __restrict__ csr_col_ind,
                      const T* __restrict__ csr_val,
                      int64_t csr_val_stride,
                      const T* __restrict__ x,
                      int64_t x_inc,
                      int64_t x_stride,
                      T* __restrict__ y,
                      int64_t y_stride,
                      int* __restrict__ done_array,
                      const J* __restrict__ map,
                      int offset,
                      J* __restrict__ zero_pivot,
                      rocsparse_index_base idx_base,
                      rocsparse_fill_mode  fill_mode,
                      rocsparse_diag_type  diag_type,
                      bool                 is_host_mode)
    {
        const uint32_t batch_index = blockIdx.y;
        ROCSPARSE_DEVICE_HOST_SCALAR_GET(alpha);
        rocsparse::csrsv_device<BLOCKSIZE, WF_SIZE, SLEEP>(m,
                                                           alpha,
                                                           csr_row_ptr,
                                                           csr_col_ind,
                                                           csr_val + batch_index * csr_val_stride,
                                                           x + batch_index * x_stride,
                                                           x_inc,
                                                           y + batch_index * y_stride,
                                                           done_array + batch_index * m,
                                                           map,
                                                           offset,
                                                           zero_pivot + batch_index,
                                                           idx_base,
                                                           fill_mode,
                                                           diag_type);
    }

    template <uint32_t BLOCKSIZE, uint32_t WF_SIZE, bool SLEEP, typename I, typename J, typename T>
    static rocsparse_status launch_csrsv_kernel(rocsparse_handle handle,
                                                int64_t          batch_count,
                                                int64_t          m,
                                                const void*      alpha_,
                                                int64_t          alpha_stride,
                                                const void* __restrict__ csr_row_ptr,
                                                const void* __restrict__ csr_col_ind,
                                                const void* __restrict__ csr_val,
                                                int64_t csr_val_stride,
                                                const void* __restrict__ x,
                                                int64_t x_inc,
                                                int64_t x_stride,
                                                void* __restrict__ y,
                                                int64_t y_stride,
                                                int32_t* __restrict__ done_array,
                                                const void* __restrict__ map,
                                                int64_t offset,
                                                void* __restrict__ zero_pivot,
                                                rocsparse_index_base idx_base,
                                                rocsparse_fill_mode  fill_mode,
                                                rocsparse_diag_type  diag_type,
                                                bool                 is_host_mode)
    {
        auto alpha = reinterpret_cast<const T*>(alpha_);
        dim3 csrsv_blocks((m * handle->wavefront_size - 1) / BLOCKSIZE + 1, batch_count);
        dim3 csrsv_threads(BLOCKSIZE);
        RETURN_IF_HIPLAUNCHKERNELGGL_ERROR(
            (rocsparse::csrsv_kernel<BLOCKSIZE, WF_SIZE, SLEEP, I, J, T>),
            csrsv_blocks,
            csrsv_threads,
            0,
            handle->stream,
            static_cast<J>(m),
            ROCSPARSE_DEVICE_HOST_SCALAR_ARGS(handle, alpha),
            alpha_stride,
            reinterpret_cast<const I* __restrict__>(csr_row_ptr),
            reinterpret_cast<const J* __restrict__>(csr_col_ind),
            reinterpret_cast<const T* __restrict__>(csr_val),
            csr_val_stride,
            reinterpret_cast<const T* __restrict__>(x),
            x_inc,
            x_stride,
            reinterpret_cast<T* __restrict__>(y),
            y_stride,
            done_array,
            reinterpret_cast<const J* __restrict__>(map),
            0,
            reinterpret_cast<J* __restrict__>(zero_pivot),
            idx_base,
            fill_mode,
            diag_type,
            handle->pointer_mode == rocsparse_pointer_mode_host);

        return rocsparse_status_success;
    }

    using tpl_t = std::tuple<uint32_t,
                             uint32_t,
                             bool,
                             rocsparse_indextype,
                             rocsparse_indextype,
                             rocsparse_datatype>;
    // clang-format off
#define CONFIG(A_,B_,C_,I_, J_, T_)				\
  {									\
    tpl_t(A_,B_,C_,I_, J_, T_),				\
      launch_csrsv_kernel<A_,B_,C_,typename rocsparse::indextype_traits<I_>::type_t, \
                          typename rocsparse::indextype_traits<J_>::type_t, \
                          typename rocsparse::datatype_traits<T_>::type_t> \
      }
    // clang-format on
#define BLOCKSIZE 512
    static const std::map<tpl_t, rocsparse::csrsv_launch_kernel_t> s_spmm_template_dispatch{{

        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_r),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_r)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_r),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_r),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_r)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_c),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_c),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_c)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_r),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_r),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_r)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_r),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_r),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_r)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_c),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_c),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_c)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_r),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_r),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_r)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_r),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_r),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_r)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_c),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_c),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i32,
               rocsparse_indextype_i64,
               rocsparse_datatype_f64_c)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_r),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_r),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_r)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f32_c),
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_r),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_r),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_r)

            ,
        CONFIG(BLOCKSIZE,
               64,
               true,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_c),
        CONFIG(BLOCKSIZE,
               64,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_c),
        CONFIG(BLOCKSIZE,
               32,
               false,
               rocsparse_indextype_i64,
               rocsparse_indextype_i32,
               rocsparse_datatype_f64_c)}};

    rocsparse_status csrsv_launch_kernel_find(rocsparse::csrsv_launch_kernel_t* spmm_function_,
                                              uint32_t                          A,
                                              uint32_t                          B,
                                              bool                              C,
                                              rocsparse_indextype               i_type_,
                                              rocsparse_indextype               j_type_,
                                              rocsparse_datatype                a_type_)
    {
        const auto& it = rocsparse::s_spmm_template_dispatch.find(
            rocsparse::tpl_t(A, B, C, i_type_, j_type_, a_type_));

        if(it != rocsparse::s_spmm_template_dispatch.end())
        {
            spmm_function_[0] = it->second;
        }
        // LCOV_EXCL_START
        else
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
}

#if 0
namespace rocsparse
{
      template <uint32_t BLOCKSIZE, uint32_t WF_SIZE, uint32_t SLEEP>
      struct AA_t
      {
    template <typename I,
	      typename J,
	      typename T>
    static rocsparse_status launch_csrsv_kernel(rocsparse_handle handle,
							     int64_t m,
							     const void* alpha_,
							     const void* __restrict__ csr_row_ptr,
							     const void* __restrict__ csr_col_ind,
							     const void* __restrict__ csr_val,
							     const void* __restrict__ x,
							     int64_t x_inc,
							     void* __restrict__ y,
							     int32_t* __restrict__ done_array,
							     const void* __restrict__ map,
							     int64_t offset,
							     void* __restrict__ zero_pivot,
							     rocsparse_index_base idx_base,
							     rocsparse_fill_mode  fill_mode,
							     rocsparse_diag_type  diag_type,
							     bool                 is_host_mode)
    {
      return rocsparse_status_success;
    }


	using tpl_t = std::tuple<rocsparse_indextype,
				 rocsparse_indextype,
				 rocsparse_datatype>;

	// clang-format off
#define CONFIG(I,J,T)	\
	{tpl_t(I,J,T),						\
	    AA_t<BLOCKSIZE, WF_SIZE, SLEEP>::launch_csrsv_kernel<typename rocsparse::indextype_traits<I>::type_t, \
				typename rocsparse::indextype_traits<J>::type_t, \
      typename rocsparse::datatype_traits<T>::type_t>}
  // clang-format on
  	static const std::map<tpl_t, rocsparse::csrsv_launch_kernel_t> s_map;
      };


  template <uint32_t BLOCKSIZE, uint32_t WF_SIZE, uint32_t SLEEP>
  const std::map<typename AA_t<BLOCKSIZE, WF_SIZE, SLEEP>::tpl_t, rocsparse::csrsv_launch_kernel_t> AA_t<BLOCKSIZE, WF_SIZE, SLEEP>::s_map
      {
	{ AA_t<BLOCKSIZE, WF_SIZE, SLEEP>::tpl_t(rocsparse_indextype_i32,
						 rocsparse_indextype_i32,
						 rocsparse_datatype_f32_r),
	    AA_t<BLOCKSIZE, WF_SIZE, SLEEP>::launch_csrsv_kernel<typename rocsparse::indextype_traits<rocsparse_indextype_i32>::type_t, \
				typename rocsparse::indextype_traits<rocsparse_indextype_i32>::type_t, \
      typename rocsparse::datatype_traits<rocsparse_data_type_f32_r>::type_t>
	    }
#if 0
	CONFIG(rocsparse_indextype_i32,
			  rocsparse_indextype_i32,
			  rocsparse_datatype_f32_r)
#endif
#if 0
#endif
      };

#if 0
  static const std::map<tpl_t, rocsparse::rocsparse::csrsv_launch_kernel_t> s_map{}
    { CONFIG(rocsparse_indextype_i32,
			  rocsparse_indextype_i32,
			  rocsparse_datatype_f32_r)  };

#endif
#undef CONFIG

#if 0

#endif


}


    rocsparse_status csrsv_launch_kernel_find(rocsparse::csrsv_launch_kernel_t*    f_,
				       uint32_t blocksize_,
				       uint32_t wfsize_,
					      uint32_t sleep_,
				       rocsparse_indextype i_type_,
				       rocsparse_indextype j_type_,
				       rocsparse_datatype  a_type_)
    {
      f_[0] = nullptr;
      // csrsv_kernel_template_t;
      const auto& it = rocsparse::AA_t<BLOCKSIZE,64,true>::s_map.find(rocsparse::AA_t<BLOCKSIZE,64,true>::tpl_t(i_type_,
							      j_type_,
							      a_type_));

      if(it != rocsparse::AA_t<BLOCKSIZE,64,false>::s_map.end())
        {
	  f_[0] = it->second;
        }
        // LCOV_EXCL_START
        else
        {
            std::stringstream sstr;
            sstr << "invalid precision configuration: "
                 << ", blocksize: " << rocsparse::enum_utils::to_string(blocksize_)
                 << ", wfsize: " << rocsparse::enum_utils::to_string(wfsize_)
                 << ", sleep: " << sleep_
                 << ", i_type: " << rocsparse::enum_utils::to_string(i_type_)
                 << ", j_type: " << rocsparse::enum_utils::to_string(j_type_)
                 << ", a_type: " << rocsparse::enum_utils::to_string(a_type_);

            RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value,
                                                   sstr.str().c_str());
        }
        // LCOV_EXCL_STOP
      return rocsparse_status_success;
    }
#endif
