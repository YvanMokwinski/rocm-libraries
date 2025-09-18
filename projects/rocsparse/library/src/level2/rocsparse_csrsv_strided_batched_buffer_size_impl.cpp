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

#include "rocsparse_control.hpp"
#include "rocsparse_csrsv.hpp"
#include "rocsparse_csrsv_strided_batched.hpp"
#include "rocsparse_primitives.hpp"
#include "rocsparse_utility.hpp"

#if 0
rocsparse_status rocsparse::csrsv_strided_batched_analysis_buffer_size(rocsparse_handle          handle,
								       rocsparse_operation       trans,
								       int64_t                   batch_count,
								       int64_t                   m,
								       int64_t                   nnz,
								       const rocsparse_mat_descr descr,
								       int64_t 		       csr_val_stride,
								       rocsparse_indextype csr_row_ptr_indextype,
								       const void*               csr_row_ptr,
								       rocsparse_indextype csr_col_ind_indextype,
								       const void*               csr_col_ind,
								       rocsparse_mat_info        info,
								       size_t*                   buffer_size)
{
  // Check matrix type
  ROCSPARSE_CHECKARG(5,
		     descr,
		     (descr->type != rocsparse_matrix_type_general
		      && descr->type != rocsparse_matrix_type_triangular),
		     rocsparse_status_not_implemented);

  // Check matrix sorting mode

  ROCSPARSE_CHECKARG(5,
		     descr,
		     (descr->storage_mode != rocsparse_storage_mode_sorted),
		     rocsparse_status_requires_sorted_storage);

  // Quick return if possible
  if(m == 0)
    {
      *buffer_size = 0;
      return rocsparse_status_success;
    }

  const size_t sizeof_I = rocsparse::indextype_sizeof(csr_row_ptr_indextype);
  const size_t sizeof_J = rocsparse::indextype_sizeof(csr_col_ind_indextype);
  const size_t sizeof_T = rocsparse::datatype_sizeof(csr_val_datatype);

  const size_t head_size=256;
  const size_t done_array_size = ((sizeof(int32_t) * m * batch_count  - 1) / 256 + 1) * 256;

  const size_t tmp_work1_size = (transpose) ? ((sizeof_J * nnz - 1) / 256 + 1) * 256 : 0;
  const size_t tmp_work2_size = (transpose) ? ((sizeof_I * nnz - 1) / 256 + 1) * 256 : 0;
  size_t transpose_size = tmp_work1_size + tmp_work2_size;
  if(trans != rocsparse_operation_none)
    {
      size_t size;
      auto calculate_size = rocsparse::find_radix_sort_pairs_buffer_size(csr_col_ind_indextype,
									 csr_row_ptr_indextype);
      RETURN_IF_ROCSPARSE_ERROR((calculate_size(handle,
						nnz,
						startbit,
						endbit,
						&size,
						true)));
      transpose_size += size;
    }


  //
  // End of analysis.
  //
  const size_t workspace1_size = ((sizeof_J * m - 1) / 256 + 1) * 256;
  const size_t workspace2_size = ((sizeof(int32_t) * m - 1) / 256 + 1) * 256;
  size_t workspace_size = workspace1_size + workspace2_size;



  {
    uint32_t startbit = 0;
    uint32_t endbit   = rocsparse::clz(m);

    auto calculate_rocprim_size = rocsparse::find_radix_sort_pairs_buffer_size(rocsparse_indextype_i32,
									       csr_col_ind_indextype);

    RETURN_IF_ROCSPARSE_ERROR((calculate_rocprim_size(handle, m, startbit, endbit, &rocprim_size, true)));
  }

  p_buffer_size[0] = head_size + done_array_size + std::max(transpose_size, workspace_size);
  const size_t rocprim_size;


  // Buffer
  char* ptr = reinterpret_cast<char*>(temp_buffer);
  ptr += 256;

  // done array
  int32_t* done_array = reinterpret_cast<int32_t*>(ptr);
  const size_t done_array_size_in_bytes = ((sizeof(int32_t) * m * batch_count- 1) / 256 + 1) * 256;
  ptr += done_array_size_in_bytes;
  if (transpose)
    csrt_val;

    // rocsparse_int max_nnz
    *buffer_size = 256;

    // rocsparse_int done_array[m]
    *buffer_size += ((sizeof(int32_t) * m * batch_count - 1) / 256 + 1) * 256;

    // rocsparse_int workspace
    *buffer_size += ((sizeof_J * m  - 1) / 256 + 1) * 256;
    // rocsparse_int workspace2
    *buffer_size += ((sizeof(int32_t) * m - 1) / 256 + 1) * 256;

    size_t rocprim_size = 0;
    {
      uint32_t startbit = 0;
      uint32_t endbit   = rocsparse::clz(m);

      auto calculate_rocprim_size = rocsparse::find_radix_sort_pairs_buffer_size(rocsparse_indextype_i32,
										 csr_col_ind_indextype);

      RETURN_IF_ROCSPARSE_ERROR((calculate_rocprim_size(handle, m, startbit, endbit, &rocprim_size, true)));
    }
    p_buffer_size[0] += rocprim_size;
    // On transposed case, we might need more temporary storage for transposing
    size_t transpose_size = 0;
    if(trans != rocsparse_operation_none)
      {
	{
	  auto calculate_size = rocsparse::find_radix_sort_pairs_buffer_size(csr_col_ind_indextype,
									     csr_row_ptr_indextype);
	  RETURN_IF_ROCSPARSE_ERROR((calculate_size(handle, nnz, startbit, endbit, &transpose_size, true)));
	}

        // rocPRIM does not support in-place sorting, so we need an additional buffer
	transpose_size += ((sizeof_J * nnz - 1) / 256 + 1) * 256;
        transpose_size += ((sizeof_I * nnz  - 1) / 256 + 1) * 256;
    }
    *buffer_size += transpose_size;
    return rocsparse_status_success;
}


rocsparse_status rocsparse::csrsv_strided_batched_solve_buffer_size(rocsparse_handle          handle,
							      rocsparse_operation       trans,
							      int64_t                   batch_count,
							      int64_t                   m,
							      int64_t                   nnz,
							      const rocsparse_mat_descr descr,
							      rocsparse_datatype        csr_val_datatype,
							      const void*               csr_val,
							      int64_t 		       csr_val_stride,
							      rocsparse_indextype csr_row_ptr_indextype,
							      const void*               csr_row_ptr,
							      rocsparse_indextype csr_col_ind_indextype,
							      const void*               csr_col_ind,
							      rocsparse_mat_info        info,
							      size_t*                   buffer_size)
{
    // Check matrix type
    ROCSPARSE_CHECKARG(4,
                       descr,
                       (descr->type != rocsparse_matrix_type_general
                        && descr->type != rocsparse_matrix_type_triangular),
                       rocsparse_status_not_implemented);

    // Check matrix sorting mode

    ROCSPARSE_CHECKARG(4,
                       descr,
                       (descr->storage_mode != rocsparse_storage_mode_sorted),
                       rocsparse_status_requires_sorted_storage);

    // Quick return if possible
    if(m == 0)
    {
        *buffer_size = 0;
        return rocsparse_status_success;
    }

    const size_t sizeof_I = rocsparse::indextype_sizeof(csr_row_ptr_indextype);
    const size_t sizeof_J = rocsparse::indextype_sizeof(csr_col_ind_indextype);
    const size_t sizeof_T = rocsparse::datatype_sizeof(csr_val_datatype);

    // rocsparse_int max_nnz
    *buffer_size = 256;

    // rocsparse_int done_array[m]
    *buffer_size += ((sizeof(int32_t) * m - 1) / 256 + 1) * 256;

    // rocsparse_int workspace
    *buffer_size += ((sizeof_J * m * batch_count - 1) / 256 + 1) * 256;

    // rocsparse_int workspace2
    *buffer_size += ((sizeof(int32_t) * m - 1) / 256 + 1) * 256;

    uint32_t startbit = 0;
    uint32_t endbit   = rocsparse::clz(m);

    size_t rocprim_size = 0;

    auto calculate_rocprim_size = rocsparse::find_radix_sort_pairs_buffer_size(rocsparse_indextype_i32,
									       csr_col_ind_indextype);

    RETURN_IF_ROCSPARSE_ERROR((calculate_rocprim_size(handle, m, startbit, endbit, &rocprim_size, true)));

    // rocprim buffer
    *buffer_size += rocprim_size;

    // On transposed case, we might need more temporary storage for transposing
    if(trans == rocsparse_operation_transpose || trans == rocsparse_operation_conjugate_transpose)
    {
        size_t transpose_size;

        // Determine rocprim buffer size
	auto calculate_size = rocsparse::find_radix_sort_pairs_buffer_size(csr_col_ind_indextype,
									   csr_row_ptr_indextype);

	RETURN_IF_ROCSPARSE_ERROR((calculate_size(handle, m, startbit, endbit, &transpose_size, true)));
        // rocPRIM does not support in-place sorting, so we need an additional buffer
	// rocsparse_int max_nnz
	transpose_size += 256 + ((sizeof(int32_t) * m - 1) / 256 + 1) * 256;
        transpose_size += ((sizeof_J * nnz - 1) / 256 + 1) * 256;

	const int64_t csr_val_batch_count = (csr_val_stride == 0) ? 1 : batch_count;

        transpose_size += ((rocsparse::max(sizeof_I, sizeof_T * csr_val_batch_count) * nnz  - 1) / 256 + 1) * 256;

        *buffer_size = rocsparse::max(*buffer_size, transpose_size);
    }

    return rocsparse_status_success;
}
#endif

rocsparse_status
    rocsparse::csrsv_strided_batched_buffer_size(rocsparse_handle          handle,
                                                 rocsparse_operation       trans,
                                                 int64_t                   batch_count,
                                                 int64_t                   m,
                                                 int64_t                   nnz,
                                                 const rocsparse_mat_descr descr,
                                                 rocsparse_datatype        csr_val_datatype,
                                                 const void*               csr_val,
                                                 int64_t                   csr_val_stride,
                                                 rocsparse_indextype       csr_row_ptr_indextype,
                                                 const void*               csr_row_ptr,
                                                 rocsparse_indextype       csr_col_ind_indextype,
                                                 const void*               csr_col_ind,
                                                 rocsparse_mat_info        info,
                                                 size_t*                   buffer_size)
{
    // Check matrix type
    ROCSPARSE_CHECKARG(4,
                       descr,
                       (descr->type != rocsparse_matrix_type_general
                        && descr->type != rocsparse_matrix_type_triangular),
                       rocsparse_status_not_implemented);

    // Check matrix sorting mode

    ROCSPARSE_CHECKARG(4,
                       descr,
                       (descr->storage_mode != rocsparse_storage_mode_sorted),
                       rocsparse_status_requires_sorted_storage);

    // Quick return if possible
    if(m == 0)
    {
        *buffer_size = 0;
        return rocsparse_status_success;
    }

    const size_t sizeof_I = rocsparse::indextype_sizeof(csr_row_ptr_indextype);
    const size_t sizeof_J = rocsparse::indextype_sizeof(csr_col_ind_indextype);
    const size_t sizeof_T = rocsparse::datatype_sizeof(csr_val_datatype);

    // rocsparse_int max_nnz
    *buffer_size = 256;

    // rocsparse_int done_array[m]
    *buffer_size += ((sizeof(int32_t) * m * batch_count - 1) / 256 + 1) * 256;

    // rocsparse_int workspace
    *buffer_size += ((sizeof_J * m - 1) / 256 + 1) * 256;

    // rocsparse_int workspace2
    *buffer_size += ((sizeof(int32_t) * m - 1) / 256 + 1) * 256;

    uint32_t startbit = 0;
    uint32_t endbit   = rocsparse::clz(m);

    size_t rocprim_size = 0;

    auto calculate_rocprim_size = rocsparse::find_radix_sort_pairs_buffer_size(
        rocsparse_indextype_i32, csr_col_ind_indextype);

    RETURN_IF_ROCSPARSE_ERROR(
        (calculate_rocprim_size(handle, m, startbit, endbit, &rocprim_size, true)));

    // rocprim buffer
    *buffer_size += rocprim_size;

    // On transposed case, we might need more temporary storage for transposing
    if(trans == rocsparse_operation_transpose || trans == rocsparse_operation_conjugate_transpose)
    {
        size_t transpose_size;

        // Determine rocprim buffer size
        auto calculate_size = rocsparse::find_radix_sort_pairs_buffer_size(csr_col_ind_indextype,
                                                                           csr_row_ptr_indextype);

        RETURN_IF_ROCSPARSE_ERROR(
            (calculate_size(handle, nnz, startbit, endbit, &transpose_size, true)));
        // rocPRIM does not support in-place sorting, so we need an additional buffer
        // rocsparse_int max_nnz
        transpose_size += 256 + ((sizeof(int32_t) * m - 1) / 256 + 1) * 256;

        transpose_size += ((sizeof_J * nnz - 1) / 256 + 1) * 256;

        const int64_t csr_val_batch_count = (csr_val_stride == 0) ? 1 : batch_count;

        transpose_size
            += ((rocsparse::max(sizeof_I, sizeof_T * csr_val_batch_count) * nnz - 1) / 256 + 1)
               * 256;

        *buffer_size = rocsparse::max(*buffer_size, transpose_size);
    }

    return rocsparse_status_success;
}
