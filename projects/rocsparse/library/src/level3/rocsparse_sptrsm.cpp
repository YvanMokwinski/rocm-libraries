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

#include "internal/generic/rocsparse_sptrsm.h"
#include "rocsparse_control.hpp"
#include "rocsparse_enum_utils.hpp"
#include "rocsparse_handle.hpp"
#include "rocsparse_utility.hpp"

#include "../conversion/rocsparse_convert_array.hpp"
#include "../conversion/rocsparse_convert_scalar.hpp"
#include "internal/level3/rocsparse_csrsm.h"
#include "rocsparse_common.h"
#include "rocsparse_coosm.hpp"
#include "rocsparse_csrsm.hpp"
#include "rocsparse_sptrsm_descr.hpp"

template <>
inline bool rocsparse::enum_utils::is_invalid(rocsparse_sptrsm_stage value)
{
    switch(value)
    {
    case rocsparse_sptrsm_stage_analysis:
    case rocsparse_sptrsm_stage_compute:
    {
        return false;
    }
    }
    return true;
};

template <>
inline bool rocsparse::enum_utils::is_invalid(rocsparse_sptrsm_alg value)
{
    switch(value)
    {
    case rocsparse_sptrsm_alg_default:
    {
        return false;
    }
    }
    return true;
};

template <>
inline bool rocsparse::enum_utils::is_invalid(rocsparse_sptrsm_input value)
{
    switch(value)
    {
    case rocsparse_sptrsm_input_alg:
    case rocsparse_sptrsm_input_operation_A:
    case rocsparse_sptrsm_input_operation_X:
    case rocsparse_sptrsm_input_compute_datatype:
    case rocsparse_sptrsm_input_scalar_datatype:
    case rocsparse_sptrsm_input_scalar_alpha:
    case rocsparse_sptrsm_input_analysis_policy:
    {
        return false;
    }
    }
    return true;
};

template <>
inline bool rocsparse::enum_utils::is_invalid(rocsparse_sptrsm_output value)
{
    switch(value)
    {
    case rocsparse_sptrsm_output_zero_pivot_position:
    {
        return false;
    }
    }
    return true;
};

extern "C" rocsparse_status rocsparse_sptrsm_set_input(rocsparse_handle       handle,
                                                       rocsparse_sptrsm_descr sptrsm_descr,
                                                       rocsparse_sptrsm_input input,
                                                       const void*            data,
                                                       size_t                 data_size_in_bytes,
                                                       rocsparse_error*       p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, sptrsm_descr);
    ROCSPARSE_CHECKARG_ENUM(2, input);
    ROCSPARSE_CHECKARG_POINTER(3, data);

    switch(input)
    {
    case rocsparse_sptrsm_input_alg:
    {
        RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
            sptrsm_descr->get_stage() != ((rocsparse_sptrsm_stage)-1)
                ? rocsparse_status_invalid_value
                : rocsparse_status_success,
            "rocsparse_sptrsm_set_input cannot modify the descriptor after any of the stages "
            "rocsparse_sptrsm_stage was executed");
        ROCSPARSE_CHECKARG(4,
                           data_size_in_bytes,
                           data_size_in_bytes != sizeof(rocsparse_sptrsm_alg),
                           rocsparse_status_invalid_size);
        const rocsparse_sptrsm_alg alg = *reinterpret_cast<const rocsparse_sptrsm_alg*>(data);
        sptrsm_descr->set_alg(alg);
        return rocsparse_status_success;
    }

    case rocsparse_sptrsm_input_scalar_alpha:
    {
        ROCSPARSE_CHECKARG(4,
                           data_size_in_bytes,
                           data_size_in_bytes != sizeof(const void*),
                           rocsparse_status_invalid_size);
        sptrsm_descr->set_scalar_alpha(data);
        return rocsparse_status_success;
    }

    case rocsparse_sptrsm_input_scalar_datatype:
    {
        ROCSPARSE_CHECKARG(4,
                           data_size_in_bytes,
                           data_size_in_bytes != sizeof(rocsparse_datatype),
                           rocsparse_status_invalid_size);
        const rocsparse_datatype datatype = *reinterpret_cast<const rocsparse_datatype*>(data);
        sptrsm_descr->set_scalar_datatype(datatype);
        return rocsparse_status_success;
    }

    case rocsparse_sptrsm_input_compute_datatype:
    {
        RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
            sptrsm_descr->get_stage() != ((rocsparse_sptrsm_stage)-1)
                ? rocsparse_status_invalid_value
                : rocsparse_status_success,
            "rocsparse_sptrsm_set_input cannot modify the descriptor after any of the stages "
            "rocsparse_sptrsm_stage was executed");
        ROCSPARSE_CHECKARG(4,
                           data_size_in_bytes,
                           data_size_in_bytes != sizeof(rocsparse_datatype),
                           rocsparse_status_invalid_size);
        const rocsparse_datatype datatype = *reinterpret_cast<const rocsparse_datatype*>(data);
        sptrsm_descr->set_compute_datatype(datatype);
        return rocsparse_status_success;
    }

    case rocsparse_sptrsm_input_analysis_policy:
    {
        RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
            sptrsm_descr->get_stage() != ((rocsparse_sptrsm_stage)-1)
                ? rocsparse_status_invalid_value
                : rocsparse_status_success,
            "rocsparse_sptrsm_set_input cannot modify the descriptor after any of the stages "
            "rocsparse_sptrsm_stage was executed");
        ROCSPARSE_CHECKARG(4,
                           data_size_in_bytes,
                           data_size_in_bytes != sizeof(rocsparse_analysis_policy),
                           rocsparse_status_invalid_size);
        const rocsparse_analysis_policy policy
            = *reinterpret_cast<const rocsparse_analysis_policy*>(data);
        sptrsm_descr->set_analysis_policy(policy);
        return rocsparse_status_success;
    }
    case rocsparse_sptrsm_input_operation_A:
    {
        RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
            sptrsm_descr->get_stage() != ((rocsparse_sptrsm_stage)-1)
                ? rocsparse_status_invalid_value
                : rocsparse_status_success,
            "rocsparse_sptrsm_set_input cannot modify the descriptor after any of the stages "
            "rocsparse_sptrsm_stage was executed");
        ROCSPARSE_CHECKARG(4,
                           data_size_in_bytes,
                           data_size_in_bytes != sizeof(rocsparse_operation),
                           rocsparse_status_invalid_size);
        const rocsparse_operation op = *reinterpret_cast<const rocsparse_operation*>(data);
        sptrsm_descr->set_operation_A(op);
        return rocsparse_status_success;
    }

    case rocsparse_sptrsm_input_operation_X:
    {
        RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
            sptrsm_descr->get_stage() != ((rocsparse_sptrsm_stage)-1)
                ? rocsparse_status_invalid_value
                : rocsparse_status_success,
            "rocsparse_sptrsm_set_input cannot modify the descriptor after any of the stages "
            "rocsparse_sptrsm_stage was executed");
        ROCSPARSE_CHECKARG(4,
                           data_size_in_bytes,
                           data_size_in_bytes != sizeof(rocsparse_operation),
                           rocsparse_status_invalid_size);
        const rocsparse_operation op = *reinterpret_cast<const rocsparse_operation*>(data);
        sptrsm_descr->set_operation_B(op);
        return rocsparse_status_success;
    }
    }
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}

extern "C" rocsparse_status rocsparse_sptrsm_get_output(rocsparse_handle        handle,
                                                        rocsparse_sptrsm_descr  sptrsm_descr,
                                                        rocsparse_sptrsm_output output,
                                                        void*                   data,
                                                        size_t                  data_size_in_bytes,
                                                        rocsparse_error*        p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, sptrsm_descr);
    ROCSPARSE_CHECKARG_ENUM(2, output);
    ROCSPARSE_CHECKARG_POINTER(3, data);
    ROCSPARSE_CHECKARG(
        4, data_size_in_bytes, data_size_in_bytes == 0, rocsparse_status_invalid_size);

    switch(output)
    {
    case rocsparse_sptrsm_output_zero_pivot_position:
    {
        ROCSPARSE_CHECKARG(4,
                           data_size_in_bytes,
                           data_size_in_bytes != sizeof(int64_t),
                           rocsparse_status_invalid_size);

        auto csrsm_info = sptrsm_descr->get_csrsm_info();

        auto status
            = rocsparse::csrsm_zero_pivot(handle, csrsm_info, rocsparse_indextype_i64, data);
        if(status != rocsparse_status_zero_pivot)
        {
            RETURN_IF_ROCSPARSE_ERROR(status);
        }

        return status;
    }
    }
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}

namespace rocsparse
{
    typedef enum
    {
        NT_NT,
        T_NT,
        NT_T,
        T_T
    } sptrsm_case;

    static sptrsm_case sptrsm_get_case(rocsparse_operation B_operation,
                                       rocsparse_order     order_B,
                                       rocsparse_order     order_C)
    {
        const bool B_is_transposed
            = ((B_operation == rocsparse_operation_none) && (order_B == rocsparse_order_row))
              || ((B_operation != rocsparse_operation_none) && (order_B == rocsparse_order_column));

        const bool C_is_transposed = (order_C == rocsparse_order_row);

        if(B_is_transposed && C_is_transposed)
        {
            // 1) B col order + transposed and C row order
            // 2) B row order + non-transposed and C row order
            return sptrsm_case::T_T;
        }
        else if(B_is_transposed && !C_is_transposed)
        {
            // 1) B col order + transposed and C col order
            // 2) B row order + non-transposed and C col order
            return sptrsm_case::T_NT;
        }
        else if(!B_is_transposed && C_is_transposed)
        {
            // 1) B row order + transposed and C row order
            // 2) B col order + non-transposed and C row order
            return sptrsm_case::NT_T;
        }
        else
        {
            // 1) B row order + transposed and C col order
            // 2) B col order + non-transposed and C col order
            return sptrsm_case::NT_NT;
        }
    }

    rocsparse_status copy2d_strided_batched(int64_t            batch_count,
                                            int64_t            nrows,
                                            int64_t            ncols,
                                            rocsparse_datatype dst_datatype,
                                            void*              dst,
                                            int64_t            dst_ld,
                                            int64_t            dst_stride,
                                            rocsparse_datatype src_datatype,
                                            const void*        src,
                                            int64_t            src_ld,
                                            int64_t            src_stride,
                                            hipStream_t        stream)
    {
        for(int64_t i = 0; i < batch_count; ++i)
        {
            RETURN_IF_HIP_ERROR(
                hipMemcpy2DAsync(reinterpret_cast<char*>(dst)
                                     + i * dst_stride * rocsparse::datatype_sizeof(dst_datatype),
                                 rocsparse::datatype_sizeof(dst_datatype) * dst_ld,
                                 reinterpret_cast<const char*>(src)
                                     + i * src_stride * rocsparse::datatype_sizeof(src_datatype),
                                 rocsparse::datatype_sizeof(src_datatype) * src_ld,
                                 rocsparse::datatype_sizeof(src_datatype) * ncols,
                                 nrows,
                                 hipMemcpyDeviceToDevice,
                                 stream));
        }
        return rocsparse_status_success;
    }

    static rocsparse_status sptrsm_preprocess_solve_T_T(rocsparse_handle            handle,
                                                        rocsparse_const_dnmat_descr B,
                                                        const rocsparse_dnmat_descr C,
                                                        rocsparse_sptrsm_alg        alg,
                                                        rocsparse_datatype* local_C_datatype,
                                                        void**              local_C,
                                                        int64_t*            local_C_ld,
                                                        int64_t*            local_C_stride,
                                                        void*               buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;

        // 1) B col order + transposed and C row order
        // 2) B row order + non-transposed and C row order
        if(B->rows > 0 && B->cols > 0)
        {
            const int64_t M = (B->order == rocsparse_order_column) ? B->rows : B->cols;
            const int64_t N = (B->order == rocsparse_order_column) ? B->cols : B->rows;

            RETURN_IF_ROCSPARSE_ERROR(
                (rocsparse::copy2d_strided_batched(std::max(B->batch_count, C->batch_count),
                                                   N,
                                                   M,
                                                   C->data_type,
                                                   C->values,
                                                   C->ld,
                                                   C->batch_stride,
                                                   B->data_type,
                                                   B->const_values,
                                                   B->ld,
                                                   B->batch_stride,
                                                   handle->stream)));
        }
        return rocsparse_status_success;
    }

    static rocsparse_status sptrsm_postprocess_solve_T_T(rocsparse_handle            handle,
                                                         rocsparse_const_dnmat_descr B,
                                                         const rocsparse_dnmat_descr C,
                                                         rocsparse_sptrsm_alg        alg,
                                                         rocsparse_datatype local_C_datatype,
                                                         void*              local_C,
                                                         int64_t            local_C_ld,
                                                         int64_t            local_C_stride,
                                                         void*              buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;

        // 1) B col order + transposed and C row order
        // 2) B row order + non-transposed and C row order

        return rocsparse_status_success;
    }

    static rocsparse_status sptrsm_preprocess_solve_T_NT(rocsparse_handle            handle,
                                                         rocsparse_const_dnmat_descr B,
                                                         const rocsparse_dnmat_descr C,
                                                         rocsparse_sptrsm_alg        alg,
                                                         rocsparse_datatype* local_C_datatype,
                                                         void**              local_C,
                                                         int64_t*            local_C_ld,
                                                         int64_t*            local_C_stride,
                                                         void*               buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;

        if(B->rows > 0 && B->cols > 0)
        {
            // 1) B col order + transposed and C col order
            // 2) B row order + non-transposed and C col order
            const int64_t M     = (B->order == rocsparse_order_column) ? B->rows : B->cols;
            const int64_t N     = (B->order == rocsparse_order_column) ? B->cols : B->rows;
            local_C_datatype[0] = B->data_type;
            local_C[0]          = buffer;
            local_C_ld[0]       = (B->order == rocsparse_order_column) ? B->rows : B->cols;
            local_C_stride[0]   = B->cols * B->rows;

            RETURN_IF_ROCSPARSE_ERROR(
                rocsparse::copy2d_strided_batched(std::max(B->batch_count, C->batch_count),
                                                  N,
                                                  M,
                                                  local_C_datatype[0],
                                                  local_C[0],
                                                  local_C_ld[0],
                                                  local_C_stride[0],
                                                  B->data_type,
                                                  B->const_values,
                                                  B->ld,
                                                  B->batch_stride,
                                                  handle->stream));
        }

        return rocsparse_status_success;
    }

    static rocsparse_status sptrsm_postprocess_solve_T_NT(rocsparse_handle            handle,
                                                          rocsparse_const_dnmat_descr B,
                                                          const rocsparse_dnmat_descr C,
                                                          rocsparse_sptrsm_alg        alg,
                                                          rocsparse_datatype local_C_datatype,
                                                          void*              local_C,
                                                          int64_t            local_C_ld,
                                                          int64_t            local_C_stride,

                                                          void* buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;

        if(B->rows > 0 && B->cols > 0)
        {
            // 1) B col order + transposed and C col order
            // 2) B row order + non-transposed and C col order
            const int64_t M     = (B->order == rocsparse_order_column) ? B->rows : B->cols;
            const int64_t N     = (B->order == rocsparse_order_column) ? B->cols : B->rows;
            double        s_one = 1;
            if(B->data_type == rocsparse_datatype_f32_r || B->data_type == rocsparse_datatype_f32_c)
            {
                *reinterpret_cast<float*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_i32_r)
            {
                *reinterpret_cast<int32_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_u32_r)
            {
                *reinterpret_cast<uint32_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_i8_r)
            {
                *reinterpret_cast<int8_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_u8_r)
            {
                *reinterpret_cast<uint8_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_bf16_r)
            {
            }

            RETURN_IF_ROCSPARSE_ERROR(
                rocsparse::dense_transpose_strided_batched(handle,
                                                           rocsparse_pointer_mode_host,
                                                           C->batch_count,
                                                           M,
                                                           N,
                                                           B->data_type,
                                                           &s_one,
                                                           static_cast<int64_t>(0),
                                                           local_C_datatype,
                                                           local_C,
                                                           local_C_ld,
                                                           local_C_stride,

                                                           C->data_type,
                                                           C->values,
                                                           C->ld,
                                                           C->batch_stride));
        }
        return rocsparse_status_success;
    }

    static rocsparse_status sptrsm_preprocess_solve_NT_T(rocsparse_handle            handle,
                                                         rocsparse_const_dnmat_descr B,
                                                         const rocsparse_dnmat_descr C,
                                                         rocsparse_sptrsm_alg        alg,
                                                         rocsparse_datatype* local_C_datatype,
                                                         void**              local_C,
                                                         int64_t*            local_C_ld,
                                                         int64_t*            local_C_stride,
                                                         void*               buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;

        // 1) B row order + transposed and C row order
        // 2) B col order + non-transposed and C row order
        local_C_datatype[0] = C->data_type;
        local_C[0]          = buffer;
        local_C_ld[0]       = C->ld;
        local_C_stride[0]   = C->batch_stride;
        if(B->rows > 0 && B->cols > 0)
        {
            const int64_t M = (B->order == rocsparse_order_column) ? B->rows : B->cols;
            const int64_t N = (B->order == rocsparse_order_column) ? B->cols : B->rows;

            double s_one = 1;
            if(B->data_type == rocsparse_datatype_f32_r || B->data_type == rocsparse_datatype_f32_c)
            {
                *reinterpret_cast<float*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_i32_r)
            {
                *reinterpret_cast<int32_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_u32_r)
            {
                *reinterpret_cast<uint32_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_i8_r)
            {
                *reinterpret_cast<int8_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_u8_r)
            {
                *reinterpret_cast<uint8_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_bf16_r)
            {
            }

            RETURN_IF_ROCSPARSE_ERROR(
                rocsparse::dense_transpose_strided_batched(handle,
                                                           rocsparse_pointer_mode_host,
                                                           B->batch_count,
                                                           M,
                                                           N,
                                                           B->data_type,
                                                           &s_one,
                                                           static_cast<int64_t>(0),
                                                           B->data_type,
                                                           B->values,
                                                           B->ld,
                                                           B->batch_stride,

                                                           local_C_datatype[0],
                                                           local_C,
                                                           local_C_ld[0],
                                                           local_C_stride[0]));
        }
        return rocsparse_status_success;
    }

    static rocsparse_status sptrsm_postprocess_solve_NT_T(rocsparse_handle            handle,
                                                          rocsparse_const_dnmat_descr B,
                                                          const rocsparse_dnmat_descr C,
                                                          rocsparse_sptrsm_alg        alg,
                                                          rocsparse_datatype local_C_datatype,
                                                          void*              local_C,
                                                          int64_t            local_C_ld,
                                                          int64_t            local_C_stride,
                                                          void*              buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;

        // 1) B row order + transposed and C row order
        // 2) B col order + non-transposed and C row order

        return rocsparse_status_success;
    }

    static rocsparse_status sptrsm_preprocess_solve_NT_NT(rocsparse_handle            handle,
                                                          rocsparse_const_dnmat_descr B,
                                                          const rocsparse_dnmat_descr C,
                                                          rocsparse_sptrsm_alg        alg,
                                                          rocsparse_datatype* local_C_datatype,
                                                          void**              local_C,
                                                          int64_t*            local_C_ld,
                                                          int64_t*            local_C_stride,
                                                          void*               buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;

        // 1) B row order + transposed and C col order
        // 2) B col order + non-transposed and C col order
        local_C_datatype[0] = B->data_type;
        local_C[0]          = buffer;
        local_C_ld[0]       = (B->order == rocsparse_order_column) ? B->cols : B->rows;
        local_C_stride[0]   = B->batch_stride;
        if(B->rows > 0 && B->cols > 0)
        {
            const int64_t M     = (B->order == rocsparse_order_column) ? B->rows : B->cols;
            const int64_t N     = (B->order == rocsparse_order_column) ? B->cols : B->rows;
            double        s_one = 1;
            if(B->data_type == rocsparse_datatype_f32_r || B->data_type == rocsparse_datatype_f32_c)
            {
                *reinterpret_cast<float*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_i32_r)
            {
                *reinterpret_cast<int32_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_u32_r)
            {
                *reinterpret_cast<uint32_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_i8_r)
            {
                *reinterpret_cast<int8_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_u8_r)
            {
                *reinterpret_cast<uint8_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_bf16_r)
            {
            }

            RETURN_IF_ROCSPARSE_ERROR(
                rocsparse::dense_transpose_strided_batched(handle,
                                                           rocsparse_pointer_mode_host,
                                                           B->batch_count,
                                                           M,
                                                           N,
                                                           B->data_type,
                                                           &s_one,
                                                           static_cast<int64_t>(0),
                                                           B->data_type,
                                                           B->values,
                                                           B->ld,
                                                           B->batch_stride,

                                                           local_C_datatype[0],
                                                           local_C,
                                                           local_C_ld[0],
                                                           local_C_stride[0]));
        }

        return rocsparse_status_success;
    }

    static rocsparse_status sptrsm_postprocess_solve_NT_NT(rocsparse_handle            handle,
                                                           rocsparse_const_dnmat_descr B,
                                                           const rocsparse_dnmat_descr C,
                                                           rocsparse_sptrsm_alg        alg,
                                                           rocsparse_datatype local_C_datatype,
                                                           void*              local_C,
                                                           int64_t            local_C_ld,
                                                           int64_t            local_C_stride,
                                                           void*              buffer)
    {
        ROCSPARSE_ROUTINE_TRACE;

        // 1) B row order + transposed and C col order
        // 2) B col order + non-transposed and C col order
        if(B->rows > 0 && B->cols > 0)
        {
            const int64_t M     = (B->order == rocsparse_order_column) ? B->cols : B->rows;
            const int64_t N     = (B->order == rocsparse_order_column) ? B->rows : B->cols;
            double        s_one = 1;
            if(B->data_type == rocsparse_datatype_f32_r || B->data_type == rocsparse_datatype_f32_c)
            {
                *reinterpret_cast<float*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_i32_r)
            {
                *reinterpret_cast<int32_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_u32_r)
            {
                *reinterpret_cast<uint32_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_i8_r)
            {
                *reinterpret_cast<int8_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_u8_r)
            {
                *reinterpret_cast<uint8_t*>(&s_one) = 1;
            }
            else if(B->data_type == rocsparse_datatype_bf16_r)
            {
            }

            RETURN_IF_ROCSPARSE_ERROR(
                rocsparse::dense_transpose_strided_batched(handle,
                                                           rocsparse_pointer_mode_host,
                                                           C->batch_count,
                                                           M,
                                                           N,
                                                           B->data_type,
                                                           &s_one,
                                                           static_cast<int64_t>(0),

                                                           local_C_datatype,
                                                           local_C,
                                                           local_C_ld,
                                                           local_C_stride,

                                                           C->data_type,
                                                           C->values,
                                                           C->ld,
                                                           C->batch_stride));
        }

        return rocsparse_status_success;
    }

    static rocsparse_status sptrsm_buffer_size(rocsparse_handle            handle,
                                               rocsparse_sptrsm_descr      sptrsm_descr,
                                               rocsparse_const_spmat_descr A,
                                               rocsparse_const_dnmat_descr B,
                                               rocsparse_const_dnmat_descr C,
                                               rocsparse_sptrsm_stage      sptrsm_stage,
                                               size_t*                     buffer_size_in_bytes)
    {

        ROCSPARSE_ROUTINE_TRACE;
        const rocsparse_operation    operation_B = sptrsm_descr->get_operation_B();
        const rocsparse::sptrsm_case sptrsm_case = sptrsm_get_case(
            operation_B, sptrsm_descr->get_B_order(), sptrsm_descr->get_C_order());

        const rocsparse_operation operation      = sptrsm_descr->get_operation_A();
        const rocsparse_datatype  alpha_datatype = sptrsm_descr->get_compute_datatype();
        const rocsparse_format    format         = A->format;
        const int64_t             nrhs           = sptrsm_descr->get_nrhs();
        const int64_t             n              = A->rows;
        const int64_t             alpha_stride   = static_cast<int64_t>(0);
        switch(format)
        {
        case rocsparse_format_csr:
        {
            RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_buffer_size(handle,
                                                                   operation,
                                                                   operation_B,
                                                                   alpha_datatype,
                                                                   alpha_stride,
                                                                   A,
                                                                   C,
                                                                   rocsparse_solve_policy_auto,
                                                                   buffer_size_in_bytes));

            switch(sptrsm_case)
            {
            case rocsparse::sptrsm_case::NT_NT:
            case rocsparse::sptrsm_case::T_NT:
            {

                *buffer_size_in_bytes
                    += ((rocsparse::datatype_sizeof(sptrsm_descr->get_B_datatype()) * nrhs * n - 1)
                            / 256
                        + 1)
                       * 256;

                break;
            }
            case rocsparse::sptrsm_case::T_T:
            case rocsparse::sptrsm_case::NT_T:
            {
                break;
            }
            }
            return rocsparse_status_success;
        }

        case rocsparse_format_coo:
        {
            RETURN_IF_ROCSPARSE_ERROR(rocsparse::coosm_buffer_size(handle,
                                                                   operation,
                                                                   operation_B,
                                                                   alpha_datatype,
                                                                   alpha_stride,
                                                                   A,
                                                                   C,
                                                                   rocsparse_solve_policy_auto,
                                                                   buffer_size_in_bytes));

            switch(sptrsm_case)
            {
            case rocsparse::sptrsm_case::NT_NT:
            case rocsparse::sptrsm_case::T_NT:
            {
                *buffer_size_in_bytes
                    += ((rocsparse::datatype_sizeof(sptrsm_descr->get_B_datatype()) * nrhs * n - 1)
                            / 256
                        + 1)
                       * 256;
                break;
            }
            case rocsparse::sptrsm_case::T_T:
            case rocsparse::sptrsm_case::NT_T:
            {
                break;
            }
            }
            return rocsparse_status_success;
        }

        case rocsparse_format_csc:
        case rocsparse_format_bsr:
        case rocsparse_format_ell:
        case rocsparse_format_bell:
        case rocsparse_format_coo_aos:
        {
            // LCOV_EXCL_START
            RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_not_implemented);
            // LCOV_EXCL_STOP
        }
        }

        // LCOV_EXCL_START
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value);
        // LCOV_EXCL_STOP
    }

    static rocsparse_status convert_scalars(rocsparse_handle             handle,
                                            const rocsparse_sptrsm_descr descr,
                                            const void*                  alpha,
                                            const void**                 local_alpha)
    {
        ROCSPARSE_ROUTINE_TRACE;
        const rocsparse_datatype scalar_datatype  = descr->get_scalar_datatype();
        const rocsparse_datatype compute_datatype = descr->get_compute_datatype();

        *local_alpha = alpha;
        if(scalar_datatype != compute_datatype)
        {
            // Convert scalars from scalar_datatype to compute_datatype
            switch(handle->pointer_mode)
            {
            case rocsparse_pointer_mode_host:
            {
                RETURN_IF_ROCSPARSE_ERROR(rocsparse::convert_host_scalars(
                    scalar_datatype, compute_datatype, alpha, descr->get_local_host_alpha()));

                *local_alpha = descr->get_local_host_alpha();
                break;
            }
            case rocsparse_pointer_mode_device:
            {
                RETURN_IF_ROCSPARSE_ERROR(rocsparse::convert_device_scalars(
                    handle->stream, scalar_datatype, compute_datatype, alpha, handle->alpha));

                *local_alpha = handle->alpha;
                break;
            }
            }
        }

        return rocsparse_status_success;
    }

    static rocsparse_status sptrsm(rocsparse_handle            handle,
                                   rocsparse_sptrsm_descr      sptrsm_descr,
                                   rocsparse_const_spmat_descr A,
                                   rocsparse_const_dnmat_descr B,
                                   const rocsparse_dnmat_descr C,
                                   rocsparse_sptrsm_stage      sptrsm_stage,
                                   size_t                      buffer_size_in_bytes,
                                   void*                       buffer)
    {

        static decltype(rocsparse::sptrsm_preprocess_solve_T_T)* preprocess_solve[4]
            = {rocsparse::sptrsm_preprocess_solve_NT_NT,
               rocsparse::sptrsm_preprocess_solve_T_NT,
               rocsparse::sptrsm_preprocess_solve_NT_T,
               rocsparse::sptrsm_preprocess_solve_T_T};

        static decltype(rocsparse::sptrsm_postprocess_solve_T_T)* postprocess_solve[4]
            = {rocsparse::sptrsm_postprocess_solve_NT_NT,
               rocsparse::sptrsm_postprocess_solve_T_NT,
               rocsparse::sptrsm_postprocess_solve_NT_T,
               rocsparse::sptrsm_postprocess_solve_T_T};

        ROCSPARSE_ROUTINE_TRACE;

        const rocsparse_operation operation      = sptrsm_descr->get_operation_A();
        const rocsparse_operation B_operation    = sptrsm_descr->get_operation_B();
        const rocsparse_datatype  alpha_datatype = sptrsm_descr->get_compute_datatype();

        const void*   alpha        = sptrsm_descr->get_scalar_alpha();
        const int64_t alpha_stride = static_cast<int64_t>(0);

        const rocsparse::sptrsm_case sptrsm_case = sptrsm_get_case(B_operation, B->order, C->order);
        const rocsparse_sptrsm_stage previous_stage = sptrsm_descr->get_stage();

        void*           csrsm_buffer = buffer;
        rocsparse_order C_order      = C->order;
        switch(sptrsm_case)
        {
        case rocsparse::sptrsm_case::NT_NT:
        case rocsparse::sptrsm_case::T_NT:
        {
            C_order = rocsparse_order_row;
            csrsm_buffer
                = reinterpret_cast<char*>(buffer)
                  + ((rocsparse::datatype_sizeof(B->data_type) * B->rows * B->cols - 1) / 256 + 1)
                        * 256;
            break;
        }
        case rocsparse::sptrsm_case::T_T:
        case rocsparse::sptrsm_case::NT_T:
        {
            break;
        }
        }

        switch(sptrsm_stage)
        {
        case rocsparse_sptrsm_stage_analysis:
        {
            switch(previous_stage)
            {
            case rocsparse_sptrsm_stage_analysis:
            {
                RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
                    rocsparse_status_invalid_value,
                    "invalid stage, the stage rocsparse_sptrsm_stage_analysis has already "
                    "been "
                    "executed");
            }

            case rocsparse_sptrsm_stage_compute:
            {
                RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
                    rocsparse_status_invalid_value,
                    "invalid stage, the stage rocsparse_sptrsm_stage_analysis cannot be "
                    "called "
                    "after "
                    "the stage rocsparse_sptrsm_stage_compute");
            }
            }

            const rocsparse_analysis_policy analysis_policy = sptrsm_descr->get_analysis_policy();
            if(rocsparse::enum_utils::is_invalid(analysis_policy))
            {
                RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(rocsparse_status_invalid_value,
                                                       "invalid analysis_policy");
            }

            _rocsparse_dnmat_descr ordered_C(C->batch_count,
                                             C->rows,
                                             C->cols,
                                             C->ld,
                                             C_order,
                                             C->data_type,
                                             C->const_values,
                                             C->values,
                                             C->batch_stride);

            switch(A->format)
            {
            case rocsparse_format_csr:
            {
                rocsparse_csrsm_info csrsm_info{};
                switch(analysis_policy)
                {
                case rocsparse_analysis_policy_reuse:
                {
                    sptrsm_descr->set_shared_csrsm_info(A->info->get_shared_csrsm_info());
                    csrsm_info = sptrsm_descr->get_csrsm_info();
                    break;
                }
                case rocsparse_analysis_policy_force:
                {
                    csrsm_info = nullptr;
                    break;
                }
                }

                RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_analysis(handle,
                                                                    operation,
                                                                    B_operation,
                                                                    alpha_datatype,
                                                                    alpha_stride,
                                                                    A,
                                                                    &ordered_C,
                                                                    analysis_policy,
                                                                    rocsparse_solve_policy_auto,
                                                                    &csrsm_info,
                                                                    csrsm_buffer));
                switch(analysis_policy)
                {
                case rocsparse_analysis_policy_reuse:
                {
                    break;
                }
                case rocsparse_analysis_policy_force:
                {
                    sptrsm_descr->set_csrsm_info(csrsm_info);
                    break;
                }
                }
                sptrsm_descr->set_stage(rocsparse_sptrsm_stage_analysis);
                return rocsparse_status_success;
            }

            case rocsparse_format_coo:
            {
                rocsparse_csrsm_info csrsm_info{};
                switch(analysis_policy)
                {
                case rocsparse_analysis_policy_reuse:
                {
                    sptrsm_descr->set_shared_csrsm_info(A->info->get_shared_csrsm_info());
                    csrsm_info = sptrsm_descr->get_csrsm_info();
                    break;
                }
                case rocsparse_analysis_policy_force:
                {
                    csrsm_info = nullptr;
                    break;
                }
                }

                RETURN_IF_ROCSPARSE_ERROR(rocsparse::coosm_analysis(handle,
                                                                    operation,
                                                                    B_operation,
                                                                    alpha_datatype,
                                                                    alpha_stride,
                                                                    A,
                                                                    &ordered_C,
                                                                    analysis_policy,
                                                                    rocsparse_solve_policy_auto,
                                                                    &csrsm_info,
                                                                    csrsm_buffer));
                switch(analysis_policy)
                {
                case rocsparse_analysis_policy_reuse:
                {
                    break;
                }
                case rocsparse_analysis_policy_force:
                {
                    sptrsm_descr->set_csrsm_info(csrsm_info);
                    break;
                }
                }
                sptrsm_descr->set_stage(rocsparse_sptrsm_stage_analysis);
                return rocsparse_status_success;
            }

            case rocsparse_format_coo_aos:
            case rocsparse_format_csc:
            case rocsparse_format_bsr:
            case rocsparse_format_ell:
            case rocsparse_format_bell:
            {
                RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_not_implemented);
            }
            }
        }

        case rocsparse_sptrsm_stage_compute:
        {

            if(previous_stage == ((rocsparse_sptrsm_stage)-1))
            {
                RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
                    rocsparse_status_invalid_value,
                    "invalid stage, the stage rocsparse_sptrsm_stage_analysis must be executed "
                    "before "
                    "the stage rocsparse_sptrsm_stage_compute");
            }

            RETURN_WITH_MESSAGE_IF_ROCSPARSE_ERROR(
                (alpha == nullptr) ? rocsparse_status_invalid_pointer : rocsparse_status_success,
                "rocsparse_sptrsm_input_scalar_alpha must be set up.");

            RETURN_IF_ROCSPARSE_ERROR(
                convert_scalars(handle, sptrsm_descr, sptrsm_descr->get_scalar_alpha(), &alpha));

            rocsparse_datatype local_C_datatype = (rocsparse_datatype)-1;
            void*              local_C_val      = nullptr;
            int64_t            local_C_ld       = 0;
            int64_t            local_C_stride   = 0;

            RETURN_IF_ROCSPARSE_ERROR(preprocess_solve[sptrsm_case](handle,
                                                                    B,
                                                                    C,
                                                                    sptrsm_descr->get_alg(),
                                                                    &local_C_datatype,
                                                                    &local_C_val,
                                                                    &local_C_ld,
                                                                    &local_C_stride,
                                                                    buffer));
            _rocsparse_dnmat_descr local_C(C->batch_count,
                                           C->rows,
                                           C->cols,
                                           C->ld,
                                           C_order,
                                           local_C_datatype,
                                           local_C_val,
                                           local_C_val,
                                           local_C_stride);

            switch(A->format)
            {
            case rocsparse_format_csr:
            {
                RETURN_IF_ROCSPARSE_ERROR(rocsparse::csrsm_solve(handle,
                                                                 operation,
                                                                 B_operation,
                                                                 alpha_datatype,
                                                                 alpha,
                                                                 alpha_stride,
                                                                 A,
                                                                 &local_C,
                                                                 rocsparse_solve_policy_auto,
                                                                 sptrsm_descr->get_csrsm_info(),
                                                                 csrsm_buffer));

                break;
            }

            case rocsparse_format_coo:
            {
                RETURN_IF_ROCSPARSE_ERROR(rocsparse::coosm_solve(handle,
                                                                 operation,
                                                                 B_operation,
                                                                 alpha_datatype,
                                                                 alpha,
                                                                 alpha_stride,
                                                                 A,
                                                                 &local_C,
                                                                 rocsparse_solve_policy_auto,
                                                                 sptrsm_descr->get_csrsm_info(),
                                                                 csrsm_buffer));
                break;
            }

            case rocsparse_format_coo_aos:
            case rocsparse_format_csc:
            case rocsparse_format_bsr:
            case rocsparse_format_ell:
            case rocsparse_format_bell:
            {
                RETURN_IF_ROCSPARSE_ERROR(rocsparse_status_not_implemented);
            }
            }

            // postprocess case.
            RETURN_IF_ROCSPARSE_ERROR(postprocess_solve[sptrsm_case](handle,
                                                                     B,
                                                                     C,
                                                                     sptrsm_descr->get_alg(),
                                                                     local_C_datatype,
                                                                     local_C_val,
                                                                     local_C_ld,
                                                                     local_C_stride,
                                                                     buffer));

            sptrsm_descr->set_stage(rocsparse_sptrsm_stage_compute);
            return rocsparse_status_success;
        }
        }
    }

}

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */
extern "C" rocsparse_status rocsparse_sptrsm_buffer_size(rocsparse_handle       handle, // 0
                                                         rocsparse_sptrsm_descr sptrsm_descr, // 1
                                                         rocsparse_const_spmat_descr A, // 2
                                                         rocsparse_const_dnmat_descr B, // 3
                                                         rocsparse_const_dnmat_descr C, // 4
                                                         rocsparse_sptrsm_stage sptrsm_stage, // 5
                                                         size_t*          buffer_size_in_bytes, // 6
                                                         rocsparse_error* p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;
    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, sptrsm_descr);
    ROCSPARSE_CHECKARG_POINTER(2, A);
    ROCSPARSE_CHECKARG_POINTER(3, B);
    ROCSPARSE_CHECKARG_POINTER(4, C);
    ROCSPARSE_CHECKARG_ENUM(5, sptrsm_stage);
    ROCSPARSE_CHECKARG_POINTER(6, buffer_size_in_bytes);

    switch(sptrsm_stage)
    {
    case rocsparse_sptrsm_stage_analysis:
    {
        //
        // Let's record B order and B datatype.
        //
        sptrsm_descr->set_B_datatype(B->data_type);
        sptrsm_descr->set_B_order(B->order);
        sptrsm_descr->set_C_datatype(C->data_type);
        sptrsm_descr->set_C_order(C->order);
        sptrsm_descr->set_nrhs(C->cols);
        break;
    }

    case rocsparse_sptrsm_stage_compute:
    {
        break;
    }
    }

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::sptrsm_buffer_size(
        handle, sptrsm_descr, A, B, C, sptrsm_stage, buffer_size_in_bytes));

    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP

extern "C" rocsparse_status rocsparse_sptrsm(rocsparse_handle            handle, // 0
                                             rocsparse_sptrsm_descr      sptrsm_descr, // 1
                                             rocsparse_const_spmat_descr A, // 2
                                             rocsparse_const_dnmat_descr B, // 3
                                             rocsparse_dnmat_descr       C, // 4
                                             rocsparse_sptrsm_stage      sptrsm_stage, // 5
                                             size_t                      buffer_size_in_bytes, // 6
                                             void*                       buffer, // 7
                                             rocsparse_error*            p_error)
try
{
    ROCSPARSE_ROUTINE_TRACE;

    ROCSPARSE_CHECKARG_HANDLE(0, handle);
    ROCSPARSE_CHECKARG_POINTER(1, sptrsm_descr);
    ROCSPARSE_CHECKARG_POINTER(2, A);
    ROCSPARSE_CHECKARG_POINTER(3, B);
    ROCSPARSE_CHECKARG_POINTER(4, C);

    ROCSPARSE_CHECKARG_ENUM(5, sptrsm_stage);

    ROCSPARSE_CHECKARG(6,
                       buffer_size_in_bytes,
                       (buffer_size_in_bytes == 0) && (buffer != nullptr),
                       rocsparse_status_invalid_size);

    ROCSPARSE_CHECKARG(7,
                       buffer,
                       (buffer == nullptr) && (buffer_size_in_bytes != 0),
                       rocsparse_status_invalid_pointer);

    // Check if descriptors are initialized
    // Basically this never happens, but I let it here.
    // LCOV_EXCL_START
    ROCSPARSE_CHECKARG(2, A, (A->init == false), rocsparse_status_not_initialized);
    ROCSPARSE_CHECKARG(3, B, (B->init == false), rocsparse_status_not_initialized);
    ROCSPARSE_CHECKARG(4, C, (C->init == false), rocsparse_status_not_initialized);
    // LCOV_EXCL_STOP

    const rocsparse_datatype compute_type = sptrsm_descr->get_compute_datatype();

    // Check for matching types while we do not support mixed precision computation
    ROCSPARSE_CHECKARG(2, A, (A->data_type != compute_type), rocsparse_status_not_implemented);
    ROCSPARSE_CHECKARG(3, B, (B->data_type != compute_type), rocsparse_status_not_implemented);
    ROCSPARSE_CHECKARG(4, C, (C->data_type != compute_type), rocsparse_status_not_implemented);

    RETURN_IF_ROCSPARSE_ERROR(rocsparse::sptrsm(
        handle, sptrsm_descr, A, B, C, sptrsm_stage, buffer_size_in_bytes, buffer));

    return rocsparse_status_success;
    // LCOV_EXCL_START
}
catch(...)
{
    RETURN_ROCSPARSE_EXCEPTION();
}
// LCOV_EXCL_STOP
