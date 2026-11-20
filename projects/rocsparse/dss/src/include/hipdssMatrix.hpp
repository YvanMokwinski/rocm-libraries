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

#include "hipdss.h"
#include "hipdss_idvec_descr.hpp"
#include <rocsparse/rocsparse-types.h>

struct hipdssMatrix
{
protected:
    bool m_own{};
    bool m_is_dense{};

    int64_t m_nrows{};
    int64_t m_ncols{};
    int64_t m_nnz{};
    int64_t m_dense_ld{};
    int64_t m_batch_count{};
    int64_t m_batch_stride{};

    hipdssLayout_t m_layout{};

    //
    //
    //
    const void* m_const_row_data{};
    void*       m_row_data{};
    const void* m_const_row_host_data{};
    void*       m_row_host_data{};
    hipDataType m_row_type{};
    size_t      m_row_size_in_bytes{};

    //
    //
    //
    const void* m_const_row_end_data{};
    void*       m_row_end_data{};
    const void* m_const_row_end_host_data{};
    void*       m_row_end_host_data{};
    hipDataType m_row_end_type{};
    size_t      m_row_end_size_in_bytes{};

    //
    //
    //
    const void* m_const_col_data{};
    void*       m_col_data{};
    const void* m_const_col_host_data{};
    void*       m_col_host_data{};
    hipDataType m_col_type{};
    size_t      m_col_size_in_bytes{};

    //
    //
    //
    const void* m_const_val_data{};
    void*       m_val_data{};
    const void* m_const_val_host_data{};
    hipDataType m_val_type{};
    void*       m_val_host_data{};
    size_t      m_val_size_in_bytes{};

    hipdssIndexBase_t      m_index_base{};
    hipdssMatrixType_t     m_mtype{};
    hipdssMatrixViewType_t m_mview{};

    int32_t m_format{};

    void set_ncols(int64_t);
    void set_nrows(int64_t);

    rocsparse_spmat_descr m_sp{};
    rocsparse_dnmat_descr m_dn{};

public:
    hipdssMatrix(int64_t nrows,
                 int64_t ncols);

    ~hipdssMatrix();

    hipdssStatus_t to_device(hipdssHandle_t handle);

    hipdssStatus_t create_rocsparse_spmat_descr();
    hipdssStatus_t device_numeric_allocation(hipdssHandle_t handle,
                                             hipDataType    datatype);

    hipdssStatus_t to_host(hipdssHandle_t handle);
    hipdssStatus_t free_host(hipdssHandle_t handle);

    int32_t get_format() const;

    hipdssStatus_t set_const_val_data(const void*);

    void set_sparse(rocsparse_spmat_descr);
    void set_dense(rocsparse_dnmat_descr);

    void set_mtype(hipdssMatrixType_t);
    void set_mview(hipdssMatrixViewType_t);

    bool is_dense() const;
    bool is_sparse() const;

    int64_t get_ncols() const;
    int64_t get_nrows() const;
    int64_t get_nnz() const;
    int64_t get_ld() const;
    int64_t get_batch_count() const;
    int64_t get_batch_stride() const;

    size_t get_row_size_in_bytes() const;
    size_t get_row_end_size_in_bytes() const;
    size_t get_col_size_in_bytes() const;
    size_t get_val_size_in_bytes() const;

    hipDataType            get_row_type() const;
    hipDataType            get_col_type() const;
    hipDataType            get_val_type() const;
    hipdssIndexBase_t      get_index_base() const;
    hipdssLayout_t         get_layout() const;
    hipdssMatrixType_t     get_mtype() const;
    hipdssMatrixViewType_t get_mview() const;

    const void* get_const_row_data() const;
    const void* get_const_row_end_data() const;
    const void* get_const_col_data() const;
    const void* get_const_val_data() const;

    void* get_row_data();
    void* get_row_end_data();
    void* get_col_data();
    void* get_val_data();

    const void* get_const_row_host_data() const;
    const void* get_const_row_end_host_data() const;
    const void* get_const_col_host_data() const;
    const void* get_const_val_host_data() const;

    void* get_row_host_data();
    void* get_row_end_host_data();
    void* get_col_host_data();
    void* get_val_host_data();

    static hipdssStatus_t create_dense(hipdssHandle_t  handle,
                                       hipdssMatrix_t* p_matrix,
                                       hipdssLayout_t  layout,
                                       hipDataType     data_type,
                                       int64_t         batch_count,
                                       int64_t         nrows,
                                       int64_t         ncols);

    static hipdssStatus_t create_dense(hipdssHandle_t  handle,
                                       hipdssMatrix_t* p_matrix,
                                       hipdssLayout_t  layout,
                                       hipDataType     data_type,
                                       int64_t         batch_count,
                                       int64_t         nrows,
                                       int64_t         ncols,
                                       int64_t         ld,
                                       void*           memory);

    static hipdssStatus_t create_sparse_csr(hipdssHandle_t         handle,
                                            hipdssMatrix_t*        matrix,
                                            int64_t                nrows,
                                            int64_t                ncols,
                                            int64_t                nnz,
                                            const void*            const_rowStart,
                                            void*                  rowStart,
                                            const void*            const_rowEnd,
                                            void*                  rowEnd,
                                            const void*            const_colIndices,
                                            void*                  colIndices,
                                            const void*            const_values,
                                            void*                  values,
                                            hipDataType            offsetType,
                                            hipDataType            indexType,
                                            hipDataType            valueType,
                                            hipdssMatrixType_t     mtype,
                                            hipdssMatrixViewType_t mview,
                                            hipdssIndexBase_t      indexBase);

    static hipdssStatus_t create_host_csr_sparsity_pattern(hipdssHandle_t         handle,
                                                           hipdssMatrix_t*        matrix,
                                                           int64_t                nrows,
                                                           int64_t                ncols,
                                                           int64_t                nnz,
                                                           hipdssMatrixType_t     mtype,
                                                           hipdssMatrixViewType_t mview);

    hipdssStatus_t realloc_host_sparsity_pattern(hipdssHandle_t handle,
                                                 int64_t        nnz);

    hipdssStatus_t destroy(hipdssHandle_t);

    rocsparse_const_spmat_descr get_sparse() const;
    rocsparse_spmat_descr       get_sparse();

    rocsparse_const_dnmat_descr get_dense() const;
    rocsparse_dnmat_descr       get_dense();

    operator rocsparse_spmat_descr();
    operator rocsparse_const_spmat_descr() const;

    operator rocsparse_dnmat_descr();
    operator rocsparse_const_dnmat_descr() const;
};

hipdssStatus_t hipdssMatrixInfo(hipdssMatrix_t A);
hipdssStatus_t hipdssMatrixPrint(hipdssMatrix_t A);

hipdssStatus_t hipdssMatrixSpyTerminal(hipdssHandle_t handle,
                                       hipdssMatrix_t source);

hipdssStatus_t hipdssMatrixRowPermutation(hipdssHandle_t        handle,
                                          hipdssMatrix_t        source,
                                          hipdssMatrix_t        target,
                                          hipdss::idvec_descr_t permutation);
