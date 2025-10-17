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
#include "rocsparse_enum.hpp"
#include "testing.hpp"
template <typename T>
void testing_csrsv_strided_batched_bad_arg(const Arguments& arg)
{
}

template <typename T>
void testing_csrsv_strided_batched(const Arguments& arg)
{
}

#if 0
template <typename T>
void testing_csrsv_strided_batched_bad_arg(const Arguments& arg)
{

    static const size_t safe_size = 100;

    const T h_alpha = static_cast<T>(1);

    // Create rocsparse handle
    rocsparse_local_handle local_handle;

    // Create matrix descriptor
    rocsparse_local_mat_descr local_descr;

    // Create matrix info
    rocsparse_local_mat_info local_info;

    rocsparse_handle          handle            = local_handle;
    rocsparse_operation       trans             = rocsparse_operation_none;
    rocsparse_int             batch_count       = safe_size;
    rocsparse_int             m                 = safe_size;
    rocsparse_int             nnz               = safe_size;
    const T*                  alpha_device_host = &h_alpha;
    int64_t                   alpha_stride      = 0;
    const rocsparse_mat_descr descr             = local_descr;
    const T*                  csr_val           = (const T*)0x4;
    int64_t                   csr_val_stride    = 0;
    const rocsparse_int*      csr_row_ptr       = (const rocsparse_int*)0x4;
    const rocsparse_int*      csr_col_ind       = (const rocsparse_int*)0x4;
    rocsparse_mat_info        info              = local_info;
    const T*                  x                 = (const T*)0x4;
    int64_t                   x_stride          = 0;
    T*                        y                 = (T*)0x4;
    int64_t                   y_stride          = 0;
    rocsparse_analysis_policy analysis          = rocsparse_analysis_policy_reuse;
    rocsparse_solve_policy    solve             = rocsparse_solve_policy_auto;
    rocsparse_solve_policy    policy            = rocsparse_solve_policy_auto;
    size_t*                   buffer_size       = (size_t*)0x4;
    void*                     temp_buffer       = (void*)0x4;

#define PARAMS_BUFFER_SIZE                                                                        \
    handle, trans, batch_count, m, nnz, descr, csr_val, csr_val_stride, csr_row_ptr, csr_col_ind, \
        info, buffer_size

    {
        static const int32_t nexcludes           = 1;
        static const int32_t excludes[nexcludes] = {7};
        select_bad_arg_analysis(rocsparse_csrsv_strided_batched_buffer_size<T>,
                                nexcludes,
                                excludes,
                                PARAMS_BUFFER_SIZE);
    }

#define PARAMS_ANALYSIS                                                                           \
    handle, trans, batch_count, m, nnz, descr, csr_val, csr_val_stride, csr_row_ptr, csr_col_ind, \
        info, analysis, solve, temp_buffer
    {
        static const int32_t nexcludes           = 1;
        static const int32_t excludes[nexcludes] = {7};
        select_bad_arg_analysis(
            rocsparse_csrsv_strided_batched_analysis<T>, nexcludes, excludes, PARAMS_ANALYSIS);
    }

#define PARAMS_SOLVE                                                                      \
    handle, trans, batch_count, m, nnz, alpha_device_host, alpha_stride, descr, csr_val,  \
        csr_val_stride, csr_row_ptr, csr_col_ind, info, x, x_stride, y, y_stride, policy, \
        temp_buffer

    {
        static const int32_t nexcludes           = 4;
        static const int32_t excludes[nexcludes] = {6, 9, 14, 16};
        select_bad_arg_analysis(
            rocsparse_csrsv_strided_batched_solve<T>, nexcludes, excludes, PARAMS_SOLVE);
    }

    for(auto matrix_type : rocsparse_matrix_type_t::values)
    {
        if(matrix_type != rocsparse_matrix_type_general
           && matrix_type != rocsparse_matrix_type_triangular)
        {
            CHECK_ROCSPARSE_ERROR(rocsparse_set_mat_type(descr, matrix_type));
            EXPECT_ROCSPARSE_STATUS(
                rocsparse_csrsv_strided_batched_buffer_size<T>(PARAMS_BUFFER_SIZE),
                rocsparse_status_not_implemented);
            EXPECT_ROCSPARSE_STATUS(rocsparse_csrsv_strided_batched_analysis<T>(PARAMS_ANALYSIS),
                                    rocsparse_status_not_implemented);
            EXPECT_ROCSPARSE_STATUS(rocsparse_csrsv_strided_batched_solve<T>(PARAMS_SOLVE),
                                    rocsparse_status_not_implemented);
        }
    }
    CHECK_ROCSPARSE_ERROR(rocsparse_set_mat_type(descr, rocsparse_matrix_type_general));

    CHECK_ROCSPARSE_ERROR(rocsparse_set_mat_storage_mode(descr, rocsparse_storage_mode_unsorted));
    EXPECT_ROCSPARSE_STATUS(rocsparse_csrsv_strided_batched_buffer_size<T>(PARAMS_BUFFER_SIZE),
                            rocsparse_status_requires_sorted_storage);
    EXPECT_ROCSPARSE_STATUS(rocsparse_csrsv_strided_batched_analysis<T>(PARAMS_ANALYSIS),
                            rocsparse_status_requires_sorted_storage);
    EXPECT_ROCSPARSE_STATUS(rocsparse_csrsv_strided_batched_solve<T>(PARAMS_SOLVE),
                            rocsparse_status_requires_sorted_storage);
    CHECK_ROCSPARSE_ERROR(rocsparse_set_mat_storage_mode(descr, rocsparse_storage_mode_sorted));

#undef PARAMS_BUFFER_SIZE
#undef PARAMS_ANALYSIS
#undef PARAMS_SOLVE

    // Test rocsparse_csrsv_strided_batched_zero_pivot()
    rocsparse_int position;
    EXPECT_ROCSPARSE_STATUS(
        rocsparse_csrsv_strided_batched_zero_pivot(nullptr, descr, info, batch_count, &position),
        rocsparse_status_invalid_handle);
    EXPECT_ROCSPARSE_STATUS(
        rocsparse_csrsv_strided_batched_zero_pivot(handle, descr, nullptr, batch_count, &position),
        rocsparse_status_invalid_pointer);
    EXPECT_ROCSPARSE_STATUS(
        rocsparse_csrsv_strided_batched_zero_pivot(handle, descr, info, batch_count, nullptr),
        rocsparse_status_invalid_pointer);
}

template <typename T>
struct csrsv_strided_batched_t
{
    rocsparse_handle                   handle;
    rocsparse_int                      batch_count;
    rocsparse_analysis_policy          apol;
    rocsparse_solve_policy             spol;
    rocsparse_operation                operation;
    rocsparse_local_mat_descr          descr;
    rocsparse_mat_info                 info;
    host_dense_vector<rocsparse_int>   h_analysis_no_pivot{};
    device_dense_vector<rocsparse_int> d_analysis_no_pivot{};
    host_dense_vector<rocsparse_int>   h_analysis_pivot{};
    device_dense_vector<rocsparse_int> d_analysis_pivot{};
    host_dense_vector<rocsparse_int>   h_solve_pivot{};
    device_dense_vector<rocsparse_int> d_solve_pivot{};
    ~csrsv_strided_batched_t()
    {
        // Clear csrsv meta data
        std::ignore = rocsparse_csrsv_clear(this->handle, this->descr, this->info);
    }

    csrsv_strided_batched_t(rocsparse_handle          handle_,
                            rocsparse_int             batch_count_,
                            rocsparse_analysis_policy apol_,
                            rocsparse_solve_policy    spol_,
                            rocsparse_operation       operation_,
                            rocsparse_diag_type       diag,
                            rocsparse_fill_mode       uplo,
                            rocsparse_index_base      base,
                            rocsparse_mat_info        info_)
        : handle(handle_)
        , batch_count(batch_count_)
        , apol(apol_)
        , spol(spol_)
        , operation(operation_)
        , info(info_)
        , h_analysis_no_pivot(batch_count)
        , d_analysis_no_pivot(batch_count)
        , h_analysis_pivot(std::max(batch_count, 1))
        , d_analysis_pivot(std::max(batch_count, 1))
        , h_solve_pivot(std::max(batch_count, 1))
        , d_solve_pivot(std::max(batch_count, 1))
    {
        std::ignore = (rocsparse_set_mat_diag_type(descr, diag));
        std::ignore = (rocsparse_set_mat_fill_mode(descr, uplo));
        std::ignore = (rocsparse_set_mat_index_base(descr, base));
        for(int64_t i = 0; i < batch_count; ++i)
        {
            h_analysis_no_pivot[i] = -1;
        }
        d_analysis_no_pivot.transfer_from(h_analysis_no_pivot);
    }

protected:
    int call(bool                               serial,
             const T*                           alpha,
             int64_t                            alpha_stride,
             const device_csr_matrix<T>&        A,
             const device_dense_matrix_view<T>& x,
             device_dense_matrix_view<T>&       y,
             rocsparse_status*                  p_status)
    {
        if(serial && batch_count == 0)
        {
            p_status[0] = rocsparse_status_success;
            return 0;
        }
        device_dense_vector<char> buffer;
        call_analysis(serial, A, x, y, buffer, p_status);
        if(A.batch_count > 0)
            this->check_analysis_pivot(serial, p_status);
        if(p_status[0] == rocsparse_status_zero_pivot)
        {
            return 1;
        }

        p_status[0]
            = call_solve(this->handle, *this, serial, alpha, alpha_stride, A, x, y, buffer, true);

        if(A.batch_count > 0)
            this->check_solve_pivot(serial, p_status);
        if(p_status[0] == rocsparse_status_zero_pivot)
        {
            return 2;
        }

        return 0;
    }

public:
    int call_serial(const T*                           alpha,
                    int64_t                            alpha_stride,
                    const device_csr_matrix<T>&        A,
                    const device_dense_matrix_view<T>& x,
                    device_dense_matrix_view<T>&       y,
                    rocsparse_status*                  p_status)
    {
        return call(true, alpha, alpha_stride, A, x, y, p_status);
    }
    int call_parallel(const T*                           alpha,
                      int64_t                            alpha_stride,
                      const device_csr_matrix<T>&        A,
                      const device_dense_matrix_view<T>& x,
                      device_dense_matrix_view<T>&       y,
                      rocsparse_status*                  p_status)
    {
        return call(false, alpha, alpha_stride, A, x, y, p_status);
    }

    void check_analysis_pivot(bool serial, rocsparse_status* p_status)
    {
        rocsparse_pointer_mode mode;
        CHECK_ROCSPARSE_ERROR(rocsparse_get_pointer_mode(handle, &mode));
        rocsparse_status status = rocsparse_status_success;
        if(serial == false)
        {
            status = rocsparse_csrsv_strided_batched_zero_pivot(
                this->handle,
                this->descr,
                this->info,
                this->batch_count,
                (rocsparse_pointer_mode_host == mode) ? this->h_analysis_pivot
                                                      : this->d_analysis_pivot);
            p_status[0] = status;
        }
        else
        {
            status      = rocsparse_csrsv_zero_pivot(this->handle,
                                                this->descr,
                                                this->info,
                                                (rocsparse_pointer_mode_host == mode)
                                                         ? this->h_analysis_pivot
                                                         : this->d_analysis_pivot);
            p_status[0] = status;
        }
        hipStream_t stream;
        CHECK_ROCSPARSE_ERROR(rocsparse_get_stream(handle, &stream));
        CHECK_HIP_ERROR(hipStreamSynchronize(stream));
        int64_t count = 0;
        if(serial == false)
        {

            if(rocsparse_pointer_mode_host == mode)
            {
                for(int64_t i = 0; i < this->batch_count; ++i)
                    if(this->h_analysis_pivot[i] != -1)
                        ++count;
            }
            else
            {
                host_dense_vector<rocsparse_int> tmp(this->d_analysis_pivot);
                for(int64_t i = 0; i < batch_count; ++i)
                    if(tmp[i] != -1)
                        ++count;
            }
        }
        else
        {
            if(rocsparse_pointer_mode_host == mode)
            {
                if(this->h_analysis_pivot[0] != -1)
                    ++count;
            }
            else
            {
                host_dense_vector<rocsparse_int> tmp(this->d_analysis_pivot);
                if(tmp[0] != -1)
                    ++count;
            }
        }

        EXPECT_ROCSPARSE_STATUS(
            status, (count > 0) ? rocsparse_status_zero_pivot : rocsparse_status_success);

        p_status[0] = (count > 0) ? rocsparse_status_zero_pivot : rocsparse_status_success;
    }

    void check_solve_pivot(bool serial, rocsparse_status* p_status)
    {
        rocsparse_pointer_mode mode;
        CHECK_ROCSPARSE_ERROR(rocsparse_get_pointer_mode(handle, &mode));
        int64_t count = 0;
        if(rocsparse_pointer_mode_host == mode)
        {
            for(int64_t i = 0; i < this->batch_count; ++i)
                if(this->h_solve_pivot[i] != -1)
                    ++count;
        }
        else
        {
            host_dense_vector<rocsparse_int> tmp(this->d_solve_pivot);
            for(int64_t i = 0; i < batch_count; ++i)
                if(tmp[i] != -1)
                    ++count;
        }
        EXPECT_ROCSPARSE_STATUS(
            p_status[0], (count > 0) ? rocsparse_status_zero_pivot : rocsparse_status_success);
    }

    static rocsparse_status call_solve(rocsparse_handle,
                                       csrsv_strided_batched_t&           self_,
                                       bool                               serial,
                                       const T*                           alpha,
                                       int64_t                            alpha_stride,
                                       const device_csr_matrix<T>&        A,
                                       const device_dense_matrix_view<T>& x,
                                       device_dense_matrix_view<T>&       y,
                                       device_dense_vector<char>&         buffer,
                                       bool                               get_zero_pivot = true)
    {
        rocsparse_status       status = rocsparse_status_success;
        rocsparse_pointer_mode mode;
        std::ignore = rocsparse_get_pointer_mode(self_.handle, &mode);
        if(serial)
        {
            status = rocsparse_status_success;
            for(int64_t i = 0; i < self_.batch_count; ++i)
            {
                std::ignore = rocsparse_csrsv_solve<T>(self_.handle,
                                                       self_.operation,
                                                       A.m,
                                                       A.nnz,
                                                       alpha + i * alpha_stride,
                                                       self_.descr,
                                                       A.val + i * A.val_stride,
                                                       A.ptr,
                                                       A.ind,
                                                       self_.info,
                                                       x + i * x.ld,
                                                       y + i * y.ld,
                                                       self_.spol,
                                                       buffer);

                if(get_zero_pivot)
                {
                    auto status2 = rocsparse_csrsv_zero_pivot(self_.handle,
                                                              self_.descr,
                                                              self_.info,
                                                              (rocsparse_pointer_mode_host == mode)
                                                                  ? self_.h_solve_pivot + i
                                                                  : self_.d_solve_pivot + i);

                    hipStream_t stream;
                    rocsparse_get_stream(self_.handle, &stream);
                    std::ignore = hipStreamSynchronize(stream);

                    if(status2 == rocsparse_status_zero_pivot)
                    {
                        status = status2;
                    }
                }
            }
        }
        else
        {
            std::ignore = rocsparse_csrsv_strided_batched_solve<T>(self_.handle,
                                                                   self_.operation,
                                                                   A.batch_count,
                                                                   A.m,
                                                                   A.nnz,
                                                                   alpha,
                                                                   alpha_stride,
                                                                   self_.descr,
                                                                   A.val,
                                                                   A.val_stride,
                                                                   A.ptr,
                                                                   A.ind,
                                                                   self_.info,
                                                                   x,
                                                                   x.ld,
                                                                   y,
                                                                   y.ld,
                                                                   self_.spol,
                                                                   buffer);
            if(get_zero_pivot)
            {
                status = rocsparse_csrsv_strided_batched_zero_pivot(
                    self_.handle,
                    self_.descr,
                    self_.info,
                    A.batch_count,
                    (rocsparse_pointer_mode_host == mode) ? self_.h_solve_pivot
                                                          : self_.d_solve_pivot);

                hipStream_t stream;
                rocsparse_get_stream(self_.handle, &stream);
                std::ignore = hipStreamSynchronize(stream);
            }
        }
        return status;
    }

    static rocsparse_status call_serial_solve(rocsparse_handle                   handle,
                                              csrsv_strided_batched_t&           self_,
                                              const T*                           alpha,
                                              int64_t                            alpha_stride,
                                              const device_csr_matrix<T>&        A,
                                              const device_dense_matrix_view<T>& x,
                                              device_dense_matrix_view<T>&       y,
                                              device_dense_vector<char>&         buffer)
    {
        return call_solve(handle, self_, true, alpha, alpha_stride, A, x, y, buffer, false);
    }

    static rocsparse_status call_parallel_solve(rocsparse_handle                   handle,
                                                csrsv_strided_batched_t&           self_,
                                                const T*                           alpha,
                                                int64_t                            alpha_stride,
                                                const device_csr_matrix<T>&        A,
                                                const device_dense_matrix_view<T>& x,
                                                device_dense_matrix_view<T>&       y,
                                                device_dense_vector<char>&         buffer)
    {
        return call_solve(handle, self_, false, alpha, alpha_stride, A, x, y, buffer, false);
    }

    void call_analysis(bool                               serial,
                       const device_csr_matrix<T>&        A,
                       const device_dense_matrix_view<T>& x,
                       device_dense_matrix_view<T>&       y,
                       device_dense_vector<char>&         buffer,
                       rocsparse_status*                  p_status)
    {
        rocsparse_status       status = rocsparse_status_success;
        rocsparse_pointer_mode mode;
        CHECK_ROCSPARSE_ERROR(rocsparse_get_pointer_mode(handle, &mode));
        size_t buffer_size;
        if(serial)
        {
            CHECK_ROCSPARSE_ERROR(rocsparse_csrsv_buffer_size<T>(
                handle, operation, A.m, A.nnz, descr, A.val, A.ptr, A.ind, info, &buffer_size));
        }
        else
        {
            CHECK_ROCSPARSE_ERROR(rocsparse_csrsv_strided_batched_buffer_size<T>(handle,
                                                                                 operation,
                                                                                 A.batch_count,
                                                                                 A.m,
                                                                                 A.nnz,
                                                                                 descr,
                                                                                 A.val,
                                                                                 A.val_stride,
                                                                                 A.ptr,
                                                                                 A.ind,
                                                                                 info,
                                                                                 &buffer_size));
        }
        buffer.resize(buffer_size);
        if(serial)
        {

            CHECK_ROCSPARSE_ERROR(rocsparse_csrsv_analysis<T>(handle,
                                                              operation,
                                                              A.m,
                                                              A.nnz,
                                                              descr,
                                                              A.val,
                                                              A.ptr,
                                                              A.ind,
                                                              info,
                                                              apol,
                                                              spol,
                                                              buffer));
        }
        else
        {
            CHECK_ROCSPARSE_ERROR(rocsparse_csrsv_strided_batched_analysis<T>(handle,
                                                                              operation,
                                                                              A.batch_count,
                                                                              A.m,
                                                                              A.nnz,
                                                                              descr,
                                                                              A.val,
                                                                              A.nnz,
                                                                              A.ptr,
                                                                              A.ind,
                                                                              info,
                                                                              apol,
                                                                              spol,
                                                                              buffer));
        }
        p_status[0] = status;
    }

    void call_serial_analysis(const device_csr_matrix<T>&        A,
                              const device_dense_matrix_view<T>& x,
                              device_dense_matrix_view<T>&       y,
                              device_dense_vector<char>&         buffer,
                              rocsparse_status*                  p_status)
    {
        call_analysis(true, A, x, y, buffer, p_status);
    }

    void call_parallel_analysis(const device_csr_matrix<T>&        A,
                                const device_dense_matrix_view<T>& x,
                                device_dense_matrix_view<T>&       y,
                                device_dense_vector<char>&         buffer,
                                rocsparse_status*                  p_status)
    {
        call_analysis(false, A, x, y, buffer, p_status);
    }
};

template <typename T>
void testing_csrsv_strided_batched(const Arguments& arg)
{

    const rocsparse_operation       trans       = arg.transA;
    const rocsparse_diag_type       diag        = arg.diag;
    const rocsparse_fill_mode       uplo        = arg.uplo;
    const rocsparse_analysis_policy apol        = arg.apol;
    const rocsparse_solve_policy    spol        = arg.spol;
    const rocsparse_index_base      base        = arg.baseA;
    const auto                      tol         = get_near_check_tol<T>(arg);
    const rocsparse_int             batch_count = (arg.batch_count_A == -1) ? 7 : arg.batch_count_A;

    device_csr_matrix<T>   dA;
    device_dense_vector<T> d_alpha_mem, dx_mem, dy_mem;
    host_dense_vector<T>   h_alpha_mem;

    const int64_t h_alpha_stride = 0;
    const int64_t d_alpha_stride = 1;

    rocsparse_int M;
    int64_t       nnz;
    {
        host_csr_matrix<T>          hA;
        static constexpr bool       to_int    = false;
        static constexpr bool       full_rank = true;
        rocsparse_matrix_factory<T> matrix_factory(arg, to_int, full_rank);
        matrix_factory.init_csr(hA);
        hA.set_strided_batched(batch_count, hA.nnz);

        M   = hA.m;
        nnz = hA.nnz;
        srandom(0);
        T* v0 = hA.val;
        for(int64_t b = 0; b < batch_count; ++b)
        {
            T* v = hA.val + b * hA.val_stride;
            for(int64_t i = 0; i < hA.nnz; ++i)
            {
                v[i] = v0[i] * (1.0 + 0.01 * double(random()) / double(RAND_MAX));
            }
        }

        dA.define(M, M, nnz, base);
        dA.set_strided_batched(batch_count, dA.nnz);
        dA.transfer_from(hA);
    }

    const int64_t alpha_stride = 1;
    const int64_t x_stride     = M;
    const int64_t y_stride     = M;
    h_alpha_mem.resize(alpha_stride * batch_count);
    d_alpha_mem.resize(alpha_stride * batch_count);
    dx_mem.resize(x_stride * batch_count);
    dy_mem.resize(y_stride * batch_count);

    using host_view_t   = host_dense_matrix_view<T>;
    using device_view_t = device_dense_matrix_view<T>;
    host_view_t h_alpha(1, batch_count, h_alpha_mem, alpha_stride, rocsparse_order_column);

    device_view_t d_alpha(1, batch_count, d_alpha_mem, alpha_stride, rocsparse_order_column);

    device_view_t dx(M, batch_count, dx_mem, x_stride, rocsparse_order_column);

    device_view_t dy(M, batch_count, dy_mem, y_stride, rocsparse_order_column);

    //
    // Initialize.
    //
    for(int64_t i = 0; i < batch_count; ++i)
        h_alpha_mem[i] = arg.get_alpha<T>() + T(i + 1) / T(batch_count);
    {
        host_dense_vector<T> hx_mem;
        rocsparse_init<T>(hx_mem, 1, hx_mem.size(), 1, false);
        hx_mem.resize(x_stride * batch_count);
        dx_mem.transfer_from(hx_mem);
    }
    d_alpha_mem.transfer_from(h_alpha_mem);
    CHECK_HIP_ERROR(hipMemset(dy_mem, 255 - 1, sizeof(T) * dy_mem.size()));

    // Create matrix info
    rocsparse_local_handle handle(arg);
    if(arg.unit_check)
    {
        device_dense_vector<T> dy2_mem(y_stride * batch_count);
        CHECK_HIP_ERROR(hipMemset(dy2_mem, 255 - 1, sizeof(T) * dy2_mem.size()));
        device_dense_matrix_view<T> dy2(M, batch_count, dy2_mem, y_stride, rocsparse_order_column);

        for(const auto mode : {rocsparse_pointer_mode_host, rocsparse_pointer_mode_device})
        {
            CHECK_ROCSPARSE_ERROR(rocsparse_set_pointer_mode(handle, mode));

            int64_t  alpha_stride = (rocsparse_pointer_mode_host == mode) ? 0 : d_alpha_stride;
            const T* alpha        = (rocsparse_pointer_mode_host == mode) ? h_alpha : d_alpha;

            rocsparse_status serial_status;
            int              error_phase_serial;
            {
                rocsparse_local_mat_info   info;
                csrsv_strided_batched_t<T> f(
                    handle, batch_count, apol, spol, trans, diag, uplo, base, info);

                error_phase_serial = f.call_serial(alpha, alpha_stride, dA, dx, dy, &serial_status);
            }

            rocsparse_status status;
            int              error_phase;
            {
                rocsparse_local_mat_info   info;
                csrsv_strided_batched_t<T> f(
                    handle, batch_count, apol, spol, trans, diag, uplo, base, info);

                error_phase = f.call_parallel(alpha, alpha_stride, dA, dx, dy2, &status);
            }

            EXPECT_ROCSPARSE_STATUS((error_phase_serial != error_phase)
                                        ? rocsparse_status_invalid_value
                                        : rocsparse_status_success,
                                    rocsparse_status_success);
            EXPECT_ROCSPARSE_STATUS((serial_status != status) ? rocsparse_status_invalid_value
                                                              : rocsparse_status_success,
                                    rocsparse_status_success);

            if(status != rocsparse_status_zero_pivot)
            {
                dy.near_check(dy2, tol);
            }
            else
            {
                CHECK_ROCSPARSE_ERROR(status);
            }
        }
    }

    if(arg.timing)
    {
        CHECK_ROCSPARSE_ERROR(rocsparse_set_pointer_mode(handle, rocsparse_pointer_mode_host));

        rocsparse_status status;

        double gpu_solve_time_used;
        {
            rocsparse_local_mat_info   local_info;
            rocsparse_mat_info         info = local_info;
            csrsv_strided_batched_t<T> f(
                handle, batch_count, apol, spol, trans, diag, uplo, base, info);
            device_dense_vector<char> buffer;
            f.call_parallel_analysis(dA, dx, dy, buffer, &status);

            gpu_solve_time_used
                = rocsparse_clients::run_benchmark(arg,
                                                   csrsv_strided_batched_t<T>::call_parallel_solve,
                                                   handle,
                                                   f,
                                                   h_alpha,
                                                   h_alpha_stride,
                                                   dA,
                                                   dx,
                                                   dy,
                                                   buffer);
        }

        double serial_gpu_solve_time_used;

        {
            rocsparse_local_mat_info   local_info;
            rocsparse_mat_info         info = local_info;
            csrsv_strided_batched_t<T> f(
                handle, batch_count, apol, spol, trans, diag, uplo, base, info);

            device_dense_vector<char> buffer;
            f.call_serial_analysis(dA, dx, dy, buffer, &status);
            serial_gpu_solve_time_used
                = rocsparse_clients::run_benchmark(arg,
                                                   csrsv_strided_batched_t<T>::call_serial_solve,
                                                   handle,
                                                   f,
                                                   h_alpha,
                                                   h_alpha_stride,
                                                   dA,
                                                   dx,
                                                   dy,
                                                   buffer);
        }

        double gflop_count = csrsv_gflop_count(M, dA.nnz, diag);
        double gbyte_count = csrsv_gbyte_count<T>(M, dA.nnz);

        double gpu_gflops = get_gpu_gflops(gpu_solve_time_used, gflop_count);
        double gpu_gbyte  = get_gpu_gbyte(gpu_solve_time_used, gbyte_count);

        std::cout << "serial ms  : " << get_gpu_time_msec(serial_gpu_solve_time_used) << std::endl;
        std::cout << "batched ms : " << get_gpu_time_msec(gpu_solve_time_used) << std::endl;
        std::cout << "speed up   : " << serial_gpu_solve_time_used / gpu_solve_time_used
                  << std::endl;
        display_timing_info(display_key_t::M,
                            M,
                            display_key_t::batch_count,
                            dA.batch_count,
                            display_key_t::nnz,
                            dA.nnz,
                            display_key_t::alpha,
                            *h_alpha,
                            display_key_t::trans,
                            rocsparse_operation2string(trans),
                            display_key_t::diag_type,
                            rocsparse_diagtype2string(diag),
                            display_key_t::fill_mode,
                            rocsparse_fillmode2string(uplo),
                            display_key_t::analysis_policy,
                            rocsparse_analysis2string(apol),
                            display_key_t::solve_policy,
                            rocsparse_solve2string(spol),
                            display_key_t::gflops,
                            gpu_gflops,
                            display_key_t::bandwidth,
                            gpu_gbyte,
                            display_key_t::time_ms,
                            get_gpu_time_msec(gpu_solve_time_used));
    }
}
#endif

#define INSTANTIATE(TYPE)                                                            \
    template void testing_csrsv_strided_batched_bad_arg<TYPE>(const Arguments& arg); \
    template void testing_csrsv_strided_batched<TYPE>(const Arguments& arg)
INSTANTIATE(float);
INSTANTIATE(double);
INSTANTIATE(rocsparse_float_complex);
INSTANTIATE(rocsparse_double_complex);
void testing_csrsv_strided_batched_extra(const Arguments& arg) {}
