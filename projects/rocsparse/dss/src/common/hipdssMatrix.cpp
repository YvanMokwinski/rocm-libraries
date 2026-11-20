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

#include "hipdssMatrix.hpp"
#include "hipdssContext.hpp"
#include "hipdss_control.hpp"
#include "hipdss_datatype_utils.hpp"
#include "hipdss_enums.hpp"

#include <rocsparse/rocsparse-auxiliary.h>

bool hipdssMatrix::is_dense() const
{
    return this->m_is_dense;
}
bool hipdssMatrix::is_sparse() const
{
    return (this->m_is_dense == false);
}

int64_t hipdssMatrix::get_ncols() const
{
    return this->m_ncols;
}
int64_t hipdssMatrix::get_nrows() const
{
    return this->m_nrows;
}
int64_t hipdssMatrix::get_nnz() const
{
    return this->m_nnz;
}
int64_t hipdssMatrix::get_ld() const
{
    return this->m_dense_ld;
}

int64_t hipdssMatrix::get_batch_count() const
{
    return this->m_batch_count;
}

int64_t hipdssMatrix::get_batch_stride() const
{
    return this->m_batch_stride;
}

hipDataType hipdssMatrix::get_row_type() const
{
    return this->m_row_type;
}
hipDataType hipdssMatrix::get_col_type() const
{
    return this->m_col_type;
}
hipDataType hipdssMatrix::get_val_type() const
{
    return this->m_val_type;
}
hipdssIndexBase_t hipdssMatrix::get_index_base() const
{
    return this->m_index_base;
}
hipdssLayout_t hipdssMatrix::get_layout() const
{
    return this->m_layout;
}
hipdssMatrixType_t hipdssMatrix::get_mtype() const
{
    return this->m_mtype;
}
hipdssMatrixViewType_t hipdssMatrix::get_mview() const
{
    return this->m_mview;
}

const void* hipdssMatrix::get_const_row_data() const
{
    return this->m_const_row_data;
}

const void* hipdssMatrix::get_const_row_end_data() const
{
    return this->m_const_row_end_data;
}

const void* hipdssMatrix::get_const_col_data() const
{
    return this->m_const_col_data;
}

const void* hipdssMatrix::get_const_row_host_data() const
{
    return this->m_const_row_host_data;
}
const void* hipdssMatrix::get_const_row_end_host_data() const
{
    return this->m_const_row_end_host_data;
}
const void* hipdssMatrix::get_const_col_host_data() const
{
    return this->m_const_col_host_data;
}

const void* hipdssMatrix::get_const_val_data() const
{
    return this->m_const_val_data;
}

const void* hipdssMatrix::get_const_val_host_data() const
{
    return this->m_const_val_host_data;
}

void* hipdssMatrix::get_row_data()
{
    return this->m_row_data;
}
void* hipdssMatrix::get_row_end_data()
{
    return this->m_row_end_data;
}
void* hipdssMatrix::get_col_data()
{
    return this->m_col_data;
}

void* hipdssMatrix::get_val_data()
{
    return this->m_val_data;
}

void* hipdssMatrix::get_val_host_data()
{
    return this->m_val_host_data;
}

void* hipdssMatrix::get_row_host_data()
{
    return this->m_row_host_data;
}

void* hipdssMatrix::get_row_end_host_data()
{
    return this->m_row_end_host_data;
}

void* hipdssMatrix::get_col_host_data()
{
    return this->m_col_host_data;
}

int32_t hipdssMatrix::get_format() const
{
    return this->m_format;
}

hipdssMatrix::hipdssMatrix(const int64_t nrows,
                           const int64_t ncols)
{
    this->m_nrows = nrows;
    this->m_ncols = ncols;
}

hipdssMatrix::~hipdssMatrix()
{
    WARNING_IF_HIPDSS_ERROR(this->destroy(nullptr));
}

hipdssStatus_t hipdssMatrix::create_dense(hipdssHandle_t       handle,
                                          hipdssMatrix_t*      p_matrix,
                                          const hipdssLayout_t layout,
                                          const hipDataType    data_type,
                                          const int64_t        batch_count,
                                          const int64_t        nrows,
                                          const int64_t        ncols,
                                          const int64_t        ld,
                                          void*                memory)
{
    p_matrix[0] = new hipdssMatrix(nrows,
                                   ncols);

    auto self = p_matrix[0];

    self->m_is_dense = true;
    self->m_layout   = layout;
    //

    self->m_format = (batch_count == 1) ? HIPDSS_MFORMAT_DENSE : (HIPDSS_MFORMAT_DENSE | HIPDSS_MFORMAT_BATCH);

    //
    self->m_nnz = nrows * ncols;

    //
    self->m_const_row_data = nullptr;
    self->m_row_data       = nullptr;

    //
    self->m_const_row_end_data = nullptr;
    self->m_row_end_data       = nullptr;

    //
    self->m_const_col_data = nullptr;
    self->m_col_data       = nullptr;

    //
    self->m_dense_ld = ld;

    //
    self->m_row_type   = static_cast<hipDataType>(-1);
    self->m_col_type   = static_cast<hipDataType>(-1);
    self->m_index_base = static_cast<hipdssIndexBase_t>(-1);
    self->m_val_type   = data_type;

    //
    self->m_mtype = static_cast<hipdssMatrixType_t>(-1);
    self->m_mview = static_cast<hipdssMatrixViewType_t>(-1);

    //
    const size_t memory_size_in_bytes = nrows * ncols * batch_count * hipdss::datatype_sizeof(data_type);
    self->m_const_val_data            = memory;
    self->m_val_data                  = memory;

    //
    if(memory == nullptr)
    {
        if(handle == nullptr)
        {
            THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
        }

        RETURN_IF_HIP_ERROR(handle->device_malloc(&self->m_val_data, memory_size_in_bytes));
        self->m_const_val_data = self->m_val_data;

        self->m_own      = true;
        self->m_dense_ld = (layout == HIPDSS_LAYOUT_COL_MAJOR)
                               ? nrows
                               : ncols;
    }

    self->m_batch_count  = batch_count;
    self->m_batch_stride = self->m_dense_ld * ((layout == HIPDSS_LAYOUT_COL_MAJOR) ? ncols : nrows);

    RETURN_IF_ROCSPARSE_ERROR(rocsparse_create_dnmat_descr(&self->m_dn,
                                                           self->m_nrows,
                                                           self->m_ncols,
                                                           self->m_dense_ld,
                                                           self->m_val_data,
                                                           hipDataType2rocsparse_datatype(self->m_val_type),
                                                           hipdssLayout_t2rocsparse_order(self->m_layout)));

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssMatrix::create_dense(hipdssHandle_t       handle,
                                          hipdssMatrix_t*      p_matrix,
                                          const hipdssLayout_t layout,
                                          const hipDataType    data_type,
                                          const int64_t        batch_count,
                                          const int64_t        nrows,
                                          const int64_t        ncols)
{
    RETURN_IF_HIPDSS_ERROR(hipdssMatrix::create_dense(handle,
                                                      p_matrix,
                                                      layout,
                                                      data_type,
                                                      batch_count,
                                                      nrows,
                                                      ncols,
                                                      0,
                                                      nullptr));
    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssMatrix::destroy(hipdssHandle_t handle)
{

    if(this->m_dn)
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_destroy_dnmat_descr(this->m_dn));
        this->m_dn = nullptr;
    }

    if(this->m_sp)
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_destroy_spmat_descr(this->m_sp));
        this->m_sp = nullptr;
    }

    return HIPDSS_STATUS_SUCCESS;
}

rocsparse_const_spmat_descr hipdssMatrix::get_sparse() const
{
    return this->m_sp;
}

rocsparse_spmat_descr hipdssMatrix::get_sparse()
{
    return this->m_sp;
}

rocsparse_const_dnmat_descr hipdssMatrix::get_dense() const
{
    return this->m_dn;
}

rocsparse_dnmat_descr hipdssMatrix::get_dense()
{
    return this->m_dn;
}

void hipdssMatrix::set_sparse(rocsparse_spmat_descr pointer)
{
    this->m_sp = pointer;
}

void hipdssMatrix::set_dense(rocsparse_dnmat_descr pointer)
{
    this->m_dn = pointer;
}

void hipdssMatrix::set_mtype(const hipdssMatrixType_t value)
{
    this->m_mtype = value;
}

void hipdssMatrix::set_mview(const hipdssMatrixViewType_t value)
{
    this->m_mview = value;
}

void hipdssMatrix::set_ncols(const int64_t value)
{
    this->m_ncols = value;
}

void hipdssMatrix::set_nrows(const int64_t value)
{
    this->m_nrows = value;
}

hipdssMatrix::operator rocsparse_spmat_descr()
{
    return this->get_sparse();
}

hipdssMatrix::operator rocsparse_const_spmat_descr() const
{

    return this->get_sparse();
}

hipdssMatrix::operator rocsparse_dnmat_descr()
{

    return this->get_dense();
}

hipdssMatrix::operator rocsparse_const_dnmat_descr() const
{

    return this->get_dense();
}

//
// To rewrite
//
hipdssStatus_t hipdssMatrixInfo(hipdssMatrix_t A)
{
    std::cout << "ORDER: " << A->get_layout() << std::endl;
    std::cout << "M    : " << A->get_nrows() << std::endl;
    std::cout << "N    : " << A->get_ncols() << std::endl;
    std::cout << "LD   : " << A->get_ld() << std::endl;
    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssMatrix::set_const_val_data(const void* const_val_data)
{
    this->m_const_val_data = const_val_data;

    if(this->is_dense())
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_dnmat_set_values(*this,
                                                             const_cast<void*>(const_val_data)));
        return HIPDSS_STATUS_SUCCESS;
    }

    if(this->is_sparse())
    {
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_values(*this,
                                                             const_cast<void*>(const_val_data)));
        return HIPDSS_STATUS_SUCCESS;
    }

    RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
}

hipdssStatus_t hipdssMatrix::to_host(hipdssHandle_t handle)
{

    if(this->is_sparse())
    {

        //
        //
        //

        if((this->m_const_row_data) && (!this->m_row_host_data))
        {
            this->m_row_host_data       = malloc(this->m_row_size_in_bytes);
            this->m_const_row_host_data = this->m_row_host_data;
        }

        //
        //
        //
        if((this->m_const_row_end_data) && (!this->m_row_end_host_data))
        {
            if(this->m_row_end_size_in_bytes > 0)
            {
                this->m_row_end_host_data = malloc(this->m_row_end_size_in_bytes);
            }
            this->m_const_row_end_host_data = this->m_row_end_host_data;
        }

        //
        //
        //
        if((this->m_const_col_data) && (!this->m_col_host_data))
        {
            this->m_col_host_data       = malloc(this->m_col_size_in_bytes);
            this->m_const_col_host_data = this->m_col_host_data;
        }

        //
        //
        //
        RETURN_IF_HIP_ERROR(handle->device_memcpy(this->m_row_host_data,
                                                  this->m_const_row_data,
                                                  this->m_row_size_in_bytes,
                                                  hipMemcpyDeviceToHost));

        //
        //
        //
        if(this->m_const_row_end_host_data)
        {
            RETURN_IF_HIP_ERROR(handle->device_memcpy(this->m_row_end_host_data,
                                                      this->m_const_row_end_data,
                                                      this->m_row_end_size_in_bytes,
                                                      hipMemcpyDeviceToHost));
        }

        //
        //
        //
        RETURN_IF_HIP_ERROR(handle->device_memcpy(this->m_col_host_data,
                                                  this->m_const_col_data,
                                                  this->m_col_size_in_bytes,
                                                  hipMemcpyDeviceToHost));

        RETURN_IF_HIP_ERROR(handle->synchronize_stream());
    }

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssMatrix::device_numeric_allocation(hipdssHandle_t    handle,
                                                       const hipDataType datatype)
{
    this->m_val_type = datatype;

    this->m_val_size_in_bytes = hipdss::datatype_sizeof(this->m_val_type) * this->m_nnz;

    RETURN_IF_HIP_ERROR(handle->device_malloc(&this->m_val_data,
                                              this->m_val_size_in_bytes));

    this->m_const_val_data = this->m_val_data;
    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssMatrix::to_device(hipdssHandle_t handle)
{

    if(this->is_sparse())
    {

        if((this->m_const_row_host_data) && (this->m_row_data == nullptr))
        {
            RETURN_IF_HIP_ERROR(handle->device_malloc(&this->m_row_data,
                                                      this->m_row_size_in_bytes));
            this->m_const_row_data = this->m_row_data;
        }

        if((this->m_const_row_end_host_data) && (this->m_row_end_data == nullptr))
        {
            RETURN_IF_HIP_ERROR(handle->device_malloc(&this->m_row_end_data,
                                                      this->m_row_end_size_in_bytes));
            this->m_const_row_end_data = this->m_row_end_data;
        }

        if((this->m_const_col_host_data) && (this->m_col_data == nullptr))
        {
            RETURN_IF_HIP_ERROR(handle->device_malloc(&this->m_col_data,
                                                      this->m_col_size_in_bytes));
            this->m_const_col_data = this->m_col_data;
        }

        if((this->m_const_val_host_data) && (this->m_val_data == nullptr))
        {
            RETURN_IF_HIP_ERROR(handle->device_malloc(&this->m_val_data,
                                                      this->m_val_size_in_bytes));
            this->m_const_val_data = this->m_val_data;
        }

        if(this->m_const_row_host_data)
        {
            RETURN_IF_HIP_ERROR(handle->device_memcpy(this->m_row_data,
                                                      this->m_const_row_host_data,
                                                      this->m_row_size_in_bytes,
                                                      hipMemcpyHostToDevice));
        }

        if(this->m_const_row_end_host_data)
        {
            RETURN_IF_HIP_ERROR(handle->device_memcpy(this->m_row_end_data,
                                                      this->m_const_row_end_host_data,
                                                      this->m_row_end_size_in_bytes,
                                                      hipMemcpyHostToDevice));
        }

        if(this->m_const_col_host_data)
        {
            RETURN_IF_HIP_ERROR(handle->device_memcpy(this->m_col_data,
                                                      this->m_const_col_host_data,
                                                      this->m_col_size_in_bytes,
                                                      hipMemcpyHostToDevice));
        }

        if(this->m_const_val_host_data)
        {
            RETURN_IF_HIP_ERROR(handle->device_memcpy(this->m_val_data,
                                                      this->m_const_val_host_data,
                                                      this->m_val_size_in_bytes,
                                                      hipMemcpyHostToDevice));
        }
    }
    else
    {

        if((this->m_const_val_host_data) && (this->m_val_data == nullptr))
        {
            RETURN_IF_HIP_ERROR(handle->device_malloc(&this->m_val_data,
                                                      this->m_val_size_in_bytes));
            this->m_const_val_data = this->m_val_data;
        }

        if(this->m_const_val_host_data)
        {
            RETURN_IF_HIP_ERROR(handle->device_memcpy(this->m_val_data,
                                                      this->m_const_val_host_data,
                                                      this->m_val_size_in_bytes,
                                                      hipMemcpyHostToDevice));
        }
    }

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssMatrix::free_host(hipdssHandle_t handle)
{
    if(this->is_sparse())
    {
        //
        if(this->m_row_host_data)
        {
            free(this->m_row_host_data);
            this->m_row_host_data = nullptr;
        }
        this->m_const_row_host_data = nullptr;

        //
        if(this->m_row_end_host_data)
        {
            free(this->m_row_end_host_data);
            this->m_row_end_host_data = nullptr;
        }
        this->m_const_row_end_host_data = nullptr;

        //
        if(this->m_col_host_data)
        {
            free(this->m_col_host_data);
            this->m_col_host_data = nullptr;
        }
        this->m_const_col_host_data = nullptr;

        //
        if(this->m_val_host_data)
        {
            free(this->m_val_host_data);
            this->m_val_host_data = nullptr;
        }
        this->m_const_val_host_data = nullptr;
    }
    else
    {

        //
        if(this->m_val_host_data)
        {
            free(this->m_val_host_data);
            this->m_val_host_data = nullptr;
        }
        this->m_const_val_host_data = nullptr;
    }

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssMatrix::create_sparse_csr(hipdssHandle_t               handle,
                                               hipdssMatrix_t*              matrix,
                                               const int64_t                nrows,
                                               const int64_t                ncols,
                                               const int64_t                nnz,
                                               const void*                  const_rowStart,
                                               void*                        rowStart,
                                               const void*                  const_rowEnd,
                                               void*                        rowEnd,
                                               const void*                  const_colIndices,
                                               void*                        colIndices,
                                               const void*                  const_values,
                                               void*                        values,
                                               const hipDataType            offsetType,
                                               const hipDataType            indexType,
                                               const hipDataType            valueType,
                                               const hipdssMatrixType_t     mtype,
                                               const hipdssMatrixViewType_t mview,
                                               const hipdssIndexBase_t      indexBase)
{

    matrix[0] = new hipdssMatrix(nrows,
                                 ncols);
    auto self = matrix[0];

    self->m_format = HIPDSS_MFORMAT_CSR;

    self->m_mview        = mview;
    self->m_mtype        = mtype;
    self->m_row_type     = offsetType;
    self->m_row_end_type = offsetType;
    self->m_col_type     = indexType;
    self->m_val_type     = valueType;
    self->m_index_base   = indexBase;
    self->m_nnz          = nnz;

    //
    self->m_const_row_data    = const_rowStart;
    self->m_row_data          = rowStart;
    self->m_row_size_in_bytes = hipdss::datatype_sizeof(offsetType) * ((const_rowEnd) ? nrows : (nrows + 1));

    self->m_const_row_end_data    = const_rowEnd;
    self->m_row_end_data          = rowEnd;
    self->m_row_end_size_in_bytes = (const_rowEnd) ? (hipdss::datatype_sizeof(offsetType) * nrows) : 0;

    //
    self->m_const_col_data    = const_colIndices;
    self->m_col_data          = colIndices;
    self->m_col_size_in_bytes = hipdss::datatype_sizeof(indexType) * nnz;

    //
    self->m_const_val_data    = const_values;
    self->m_val_data          = values;
    self->m_val_size_in_bytes = (const_values) ? (hipdss::datatype_sizeof(valueType) * nnz) : 0;

    //

    const auto            offset_index_type  = hipDataType2rocsparse_indextype(offsetType);
    const auto            indices_index_type = hipDataType2rocsparse_indextype(indexType);
    const auto            data_type          = hipDataType2rocsparse_datatype(valueType);
    const auto            index_base         = hipdssIndexBase_t2rocsparse_index_base(indexBase);
    rocsparse_spmat_descr sparse{};
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_create_csr_descr(&sparse,
                                                         nrows,
                                                         ncols,
                                                         nnz,
                                                         std::remove_const_t<void*>(const_rowStart),
                                                         std::remove_const_t<void*>(const_colIndices),
                                                         std::remove_const_t<void*>(const_values),
                                                         offset_index_type,
                                                         indices_index_type,
                                                         index_base,
                                                         data_type));

    matrix[0]->set_sparse(sparse);

    //
    // We need to overcome the issue of HPD and SPD not existing.
    //
    switch(mtype)
    {
    case HIPDSS_MTYPE_HPD:
    {
        const auto value = rocsparse_matrix_type_hermitian;
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(sparse,
                                                                rocsparse_spmat_matrix_type,
                                                                &value,
                                                                sizeof(value)));
        break;
    }

    case HIPDSS_MTYPE_SPD:
    {
        const auto value = rocsparse_matrix_type_symmetric;
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(sparse,
                                                                rocsparse_spmat_matrix_type,
                                                                &value,
                                                                sizeof(value)));
        break;
    }

    case HIPDSS_MTYPE_GENERAL:
    case HIPDSS_MTYPE_SYMMETRIC:
    case HIPDSS_MTYPE_HERMITIAN:
    {
        const auto value = hipdssMatrixType_t2rocsparse(mtype);
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(sparse,
                                                                rocsparse_spmat_matrix_type,
                                                                &value,
                                                                sizeof(value)));
        break;
    }
    }

    //
    // We need to overcome the issue of FULL not existing in rocsparse.
    //
    switch(mview)
    {
    case HIPDSS_MVIEW_LOWER:
    case HIPDSS_MVIEW_UPPER:
    {
        const rocsparse_fill_mode fill_mode = hipdssMatrixViewType_t2rocsparse(mview);
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(sparse,
                                                                rocsparse_spmat_fill_mode,
                                                                &fill_mode,
                                                                sizeof(fill_mode)));
        break;
    }

    case HIPDSS_MVIEW_FULL:
    {
        //
        // Do nothing.
        //
        break;
    }
    }

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssMatrix::create_rocsparse_spmat_descr()
{

    if(this->get_row_end_data())
    {
        RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_NOT_SUPPORTED,
                                            "rocsparse does not support CSR4 format.");
    }

    rocsparse_spmat_descr spmat_descr{};
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_create_csr_descr(&spmat_descr,
                                                         this->get_nrows(),
                                                         this->get_ncols(),
                                                         this->get_nnz(),
                                                         this->get_row_data(),
                                                         this->get_col_data(),
                                                         this->get_val_data(),
                                                         hipDataType2rocsparse_indextype(this->get_row_type()),
                                                         hipDataType2rocsparse_indextype(this->get_col_type()),
                                                         hipdssIndexBase_t2rocsparse_index_base(this->get_index_base()),
                                                         hipDataType2rocsparse_datatype(this->get_val_type())));

    this->set_sparse(spmat_descr);

    const auto mtype = this->get_mtype();
    switch(mtype)
    {
    case HIPDSS_MTYPE_HPD:
    {
        const auto value = rocsparse_matrix_type_hermitian;
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(spmat_descr,
                                                                rocsparse_spmat_matrix_type,
                                                                &value,
                                                                sizeof(value)));
        break;
    }

    case HIPDSS_MTYPE_SPD:
    {
        const auto value = rocsparse_matrix_type_symmetric;
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(spmat_descr,
                                                                rocsparse_spmat_matrix_type,
                                                                &value,
                                                                sizeof(value)));
        break;
    }

    case HIPDSS_MTYPE_GENERAL:
    case HIPDSS_MTYPE_SYMMETRIC:
    case HIPDSS_MTYPE_HERMITIAN:
    {
        const auto value = hipdssMatrixType_t2rocsparse(mtype);
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(spmat_descr,
                                                                rocsparse_spmat_matrix_type,
                                                                &value,
                                                                sizeof(value)));
        break;
    }
    }

    const auto mview = this->get_mview();
    switch(mview)
    {
    case HIPDSS_MVIEW_LOWER:
    case HIPDSS_MVIEW_UPPER:
    {
        const rocsparse_fill_mode fill_mode = hipdssMatrixViewType_t2rocsparse(mview);
        RETURN_IF_ROCSPARSE_ERROR(rocsparse_spmat_set_attribute(spmat_descr,
                                                                rocsparse_spmat_fill_mode,
                                                                &fill_mode,
                                                                sizeof(fill_mode)));
        break;
    }

    case HIPDSS_MVIEW_FULL:
    {
        break;
    }
    }

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssMatrix::create_host_csr_sparsity_pattern(hipdssHandle_t         handle,
                                                              hipdssMatrix_t*        matrix,
                                                              int64_t                nrows,
                                                              int64_t                ncols,
                                                              int64_t                nnz,
                                                              hipdssMatrixType_t     mtype,
                                                              hipdssMatrixViewType_t mview)
{
    matrix[0] = new hipdssMatrix(nrows, ncols);
    auto self = matrix[0];

    self->m_format = HIPDSS_MFORMAT_CSR;
    self->m_mview  = mview;
    self->m_mtype  = mtype;

    self->m_row_type          = hipdss::datatype_minimum(nnz);
    self->m_row_end_type      = self->m_row_type;
    self->m_col_type          = hipdss::datatype_minimum(nrows);
    self->m_index_base        = HIPDSS_BASE_ZERO;
    self->m_nnz               = nnz;
    self->m_row_size_in_bytes = hipdss::datatype_sizeof(self->m_row_type) * (nrows + 1);
    self->m_col_size_in_bytes = hipdss::datatype_sizeof(self->m_row_type) * nnz;

    self->m_row_host_data       = malloc(self->m_row_size_in_bytes);
    self->m_const_row_host_data = self->m_row_host_data;

    self->m_col_host_data       = malloc(self->m_col_size_in_bytes);
    self->m_const_col_host_data = self->m_col_host_data;

    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdssMatrix::realloc_host_sparsity_pattern(hipdssHandle_t handle,
                                                           int64_t        nnz)
{
    if(this->m_nnz != nnz)
    {
        this->m_col_size_in_bytes = hipdss::datatype_sizeof(this->m_row_type) * nnz;
        void* mem                 = realloc(this->m_col_host_data,
                            this->m_col_size_in_bytes);
        if(mem == nullptr)
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_ALLOC_FAILED);
        }
        this->m_col_host_data       = mem;
        this->m_const_col_host_data = mem;
    }
    return HIPDSS_STATUS_SUCCESS;
}
