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

#include "hipdss_sptranspose.hpp"
#include "hipdssContext.hpp"
#include "hipdss_control.hpp"
#include "hipdss_enums.hpp"
#include "hipdss_idvec_descr.hpp"
#include "rocsparse/rocsparse-auxiliary.h"
#include "rocsparse_datatype_utils.hpp"
#include "rocsparse_indextype_utils.hpp"

namespace hipdss
{
    typedef void (*csr2csc_host_t)(const int64_t M,
                                   const int64_t N,
                                   const void* __restrict__ sptr_begin_,
                                   const void* __restrict__ sptr_end_,
                                   const void* __restrict__ sind_,
                                   hipdssIndexBase_t sbase,
                                   void* __restrict__ tptr_,
                                   void* __restrict__ tind_,
                                   hipdssIndexBase_t tbase);

    template <typename SI,
              typename SJ,
              typename TI,
              typename TJ>
    void csr2csc_host(const int64_t M,
                      const int64_t N,
                      const void* __restrict__ sptr_begin_,
                      const void* __restrict__ sptr_end_,
                      const void* __restrict__ sind_,
                      hipdssIndexBase_t sbase,
                      void* __restrict__ tptr_,
                      void* __restrict__ tind_,
                      hipdssIndexBase_t tbase)
    {
        const SI* __restrict__ sptr_begin = reinterpret_cast<const SI* __restrict__>(sptr_begin_);

        const SI* __restrict__ sptr_end = (sptr_end_)
                                              ? reinterpret_cast<const SI* __restrict__>(sptr_end_)
                                              : (sptr_begin + 1);

        const SJ* __restrict__ sind = reinterpret_cast<const SJ* __restrict__>(sind_);

        TI* __restrict__ tptr = reinterpret_cast<TI* __restrict__>(tptr_);
        TJ* __restrict__ tind = reinterpret_cast<TJ* __restrict__>(tind_);

        for(int64_t j = 0; j <= N; ++j)
        {
            tptr[j] = 0;
        }

        for(int64_t i = 0; i < M; ++i)
        {
            const auto start = sptr_begin[i] - sbase;
            const auto bound = sptr_end[i] - sbase;
            for(auto k = start; k < bound; ++k)
            {
                const auto j = sind[k] - sbase;
                ++tptr[j + 1];
            }
        }

        for(int64_t i = 2; i <= N; ++i)
        {
            tptr[i] += tptr[i - 1];
        }

        for(int64_t i = 0; i < M; ++i)
        {
            const auto start = sptr_begin[i] - sbase;
            const auto bound = sptr_end[i] - sbase;
            for(auto k = start; k < bound; ++k)
            {
                const auto j  = sind[k] - sbase;
                const auto at = tptr[j]++;
                tind[at]      = static_cast<TJ>(i + tbase);
            }
        }

        for(int64_t i = N; i > 0; --i)
        {
            tptr[i] = tptr[i - 1] + tbase;
        }

        tptr[0] = tbase;
    }

    template <typename SI,
              typename SJ,
              typename TI>
    static hipdssStatus_t find_csr2csc_host_TJ(hipdss::csr2csc_host_t* p_that,
                                               hipDataType             TJ)
    {
        switch(TJ)
        {

        case HIP_R_32I:
        {
            p_that[0] = hipdss::csr2csc_host<SI, SJ, TI, int32_t>;
            return HIPDSS_STATUS_SUCCESS;
        }

        case HIP_R_64I:
        {
            p_that[0] = hipdss::csr2csc_host<SI, SJ, TI, int64_t>;
            return HIPDSS_STATUS_SUCCESS;
        }

        default:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
        }
    }

    template <typename SI,
              typename SJ>
    static hipdssStatus_t find_csr2csc_host_TI(hipdss::csr2csc_host_t* p_that,
                                               hipDataType             TI,
                                               hipDataType             TJ)
    {
        switch(TI)
        {

        case HIP_R_32I:
        {
            RETURN_IF_HIPDSS_ERROR((hipdss::find_csr2csc_host_TJ<SI, SJ, int32_t>)(p_that,
                                                                                   TJ));
            return HIPDSS_STATUS_SUCCESS;
        }

        case HIP_R_64I:
        {
            RETURN_IF_HIPDSS_ERROR((hipdss::find_csr2csc_host_TJ<SI, SJ, int64_t>)(p_that,
                                                                                   TJ));
            return HIPDSS_STATUS_SUCCESS;
        }

        default:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
        }
    }

    template <typename SI>
    static hipdssStatus_t find_csr2csc_host_SJ(hipdss::csr2csc_host_t* p_that,
                                               hipDataType             SJ,
                                               hipDataType             TI,
                                               hipDataType             TJ)
    {
        switch(SJ)
        {

        case HIP_R_32I:
        {
            RETURN_IF_HIPDSS_ERROR((hipdss::find_csr2csc_host_TI<SI, int32_t>)(p_that,
                                                                               TI,
                                                                               TJ));
            return HIPDSS_STATUS_SUCCESS;
        }

        case HIP_R_64I:
        {
            RETURN_IF_HIPDSS_ERROR((hipdss::find_csr2csc_host_TI<SI, int64_t>)(p_that,
                                                                               TI,
                                                                               TJ));
            return HIPDSS_STATUS_SUCCESS;
        }

        default:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
        }
    }

    static hipdssStatus_t find_csr2csc_host(hipdss::csr2csc_host_t* p_that,
                                            hipDataType             SI,
                                            hipDataType             SJ,
                                            hipDataType             TI,
                                            hipDataType             TJ)
    {
        switch(SI)
        {

        case HIP_R_32I:
        {
            RETURN_IF_HIPDSS_ERROR(hipdss::find_csr2csc_host_SJ<int32_t>(p_that,
                                                                         SJ,
                                                                         TI,
                                                                         TJ));

            return HIPDSS_STATUS_SUCCESS;
        }

        case HIP_R_64I:
        {
            RETURN_IF_HIPDSS_ERROR(hipdss::find_csr2csc_host_SJ<int64_t>(p_that,
                                                                         SJ,
                                                                         TI,
                                                                         TJ));
            return HIPDSS_STATUS_SUCCESS;
        }

        default:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
        }
    }

}

hipdssStatus_t hipdss::MatrixTransposeSparsityPattern(hipdssHandle_t  handle,
                                                      hipdssMatrix_t  source,
                                                      hipdssMatrix_t* p_target)
{
    const auto view = source->get_mview();

    const auto mview = (view == HIPDSS_MVIEW_FULL)
                           ? HIPDSS_MVIEW_FULL
                       : (view == HIPDSS_MVIEW_LOWER)
                           ? HIPDSS_MVIEW_UPPER
                           : HIPDSS_MVIEW_LOWER;

    RETURN_IF_HIPDSS_ERROR(hipdssMatrix::create_host_csr_sparsity_pattern(handle,
                                                                          p_target,
                                                                          source->get_ncols(),
                                                                          source->get_nrows(),
                                                                          source->get_nnz(),
                                                                          HIPDSS_MTYPE_GENERAL,
                                                                          mview));

    hipdss::csr2csc_host_t convert{};
    RETURN_IF_HIPDSS_ERROR(hipdss::find_csr2csc_host(&convert,
                                                     source->get_row_type(),
                                                     source->get_col_type(),
                                                     p_target[0]->get_row_type(),
                                                     p_target[0]->get_col_type()));

    convert(source->get_nrows(),
            source->get_ncols(),
            source->get_const_row_host_data(),
            source->get_const_row_end_host_data(),
            source->get_const_col_host_data(),
            source->get_index_base(),
            p_target[0]->get_row_host_data(),
            p_target[0]->get_col_host_data(),
            p_target[0]->get_index_base());

    return HIPDSS_STATUS_SUCCESS;

#if 0


  int64_t              source_rows{};
  int64_t              source_cols{};
  int64_t              source_nnz{};
  const void*          source_csr_row_ptr{};
  const void*          source_csr_col_ind{};
  const void*          source_csr_val{};
  rocsparse_indextype  source_row_ptr_type{};
  rocsparse_indextype  source_col_ind_type{};
  rocsparse_index_base source_idx_base{};
  rocsparse_datatype   source_data_type{};

  RETURN_IF_ROCSPARSE_ERROR(rocsparse_const_csr_get(*source,
						    &source_rows,
						    &source_cols,
						    &source_nnz,
						    &source_csr_row_ptr,
						    &source_csr_col_ind,
						    &source_csr_val,
						    &source_row_ptr_type,
						    &source_col_ind_type,
						    &source_idx_base,
						    &source_data_type));

  const int64_t M = source_rows;
  const int64_t N = source_cols;

  int64_t              target_rows{};
  int64_t              target_cols{};
  int64_t              target_nnz{};
  void*                target_csr_row_ptr{};
  void*                target_csr_col_ind{};
  void*                target_csr_val{};
  rocsparse_indextype  target_row_ptr_type{};
  rocsparse_indextype  target_col_ind_type{};
  rocsparse_index_base target_idx_base{};
  rocsparse_datatype   target_data_type{};

  RETURN_IF_ROCSPARSE_ERROR(rocsparse_csr_get(*target,
					      &target_rows,
					      &target_cols,
					      &target_nnz,
					      &target_csr_row_ptr,
					      &target_csr_col_ind,
					      &target_csr_val,
					      &target_row_ptr_type,
					      &target_col_ind_type,
					      &target_idx_base,
					      &target_data_type));


  void* tptr = calloc(N + 1,
		      rocsparse::indextype_sizeof(target_row_ptr_type));
  void* tind = malloc(target_nnz * rocsparse::indextype_sizeof(target_col_ind_type));
  void* ptr  = malloc((M + 1) * rocsparse::indextype_sizeof(source_row_ptr_type));
  void* ind  = malloc(source_nnz * rocsparse::indextype_sizeof(source_col_ind_type));

  RETURN_IF_HIP_ERROR(handle->device_memcpy(ptr,
					    source_csr_row_ptr,
					    (M + 1) * rocsparse::indextype_sizeof(source_row_ptr_type),
					    hipMemcpyDeviceToHost));

  RETURN_IF_HIP_ERROR(handle->device_memcpy(ind,
					    source_csr_col_ind,
					    source_nnz * rocsparse::indextype_sizeof(source_col_ind_type),
					    hipMemcpyDeviceToHost));

  RETURN_IF_HIP_ERROR(handle->synchronize_stream());


  auto host = sptranspose_host<int32_t,
			       int32_t>;



  calculate(M,
	    N,
	    ptr,
	    reinterpret_cast<char*>(ptr) + rocsparse::indextype_sizeof(source_row_ptr_type),
	    ind,
	    source_idx_base,
	    tptr,
	    tind,
	    target_idx_base,
	    to_t,
	    to_t_base,
	    filter);


  RETURN_IF_HIP_ERROR(handle->device_memcpy(target_csr_row_ptr,
					    tptr,
					    (N + 1) * rocsparse::indextype_sizeof(target_row_ptr_type),
					    hipMemcpyHostToDevice));

  RETURN_IF_HIP_ERROR(handle->device_memcpy(target_csr_col_ind,
					    tind,
					    target_nnz * rocsparse::indextype_sizeof(target_col_ind_type),
					    hipMemcpyHostToDevice));

  free(tind);
  free(tptr);
#endif

    return HIPDSS_STATUS_SUCCESS;
}
