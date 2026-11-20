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

#include "hipdss.h"

#include "hipdss_control.hpp"

#include "hipdssContext.hpp"
#include "hipdss_matrix_import_config.hpp"

#include <fstream>

namespace hipdss
{
    static hipdssStatus_t matrix_import(const char*  filename,
                                        int64_t*     p_m,
                                        int64_t*     p_n,
                                        int64_t*     p_nnz,
                                        hipDataType* p_ptr_indextype,
                                        size_t*      p_ptr_size_in_bytes,
                                        void**       p_ptr,
                                        hipDataType* p_ind_indextype,
                                        size_t*      p_ind_size_in_bytes,
                                        void**       p_ind,
                                        hipDataType* p_val_datatype,
                                        size_t*      p_val_size_in_bytes,
                                        void**       p_val)
    {

        std::ifstream* in{};
        std::cout << "Opening file '" << filename << "' ... " << std::endl;

        in = new std::ifstream(filename,
                               std::ios::in | std::ios::binary);
        if(!in->is_open())
        {
            std::cerr << "cannot open " << filename << std::endl;
            return HIPDSS_STATUS_INTERNAL_ERROR;
        }

        std::string header;
        std::getline(in[0],
                     header);
        if(header != "#rocALUTION binary csr file")
        {
            return HIPDSS_STATUS_INTERNAL_ERROR;
        }
        int32_t version;
        in->read((char*)&version,
                 sizeof(int32_t));
        int32_t iM;
        int32_t iN;
        int32_t innz;
        in->read((char*)&iM,
                 sizeof(int32_t));
        in->read((char*)&iN,
                 sizeof(int32_t));
        in->read((char*)&innz,
                 sizeof(int32_t));

        //  rocsparse_status status;
        p_m[0]   = iM;
        p_n[0]   = iN;
        p_nnz[0] = innz;

        p_ptr_indextype[0]     = HIP_R_32I;
        p_ptr_size_in_bytes[0] = sizeof(int32_t) * (iM + 1);

        p_ind_indextype[0]     = HIP_R_32I;
        p_ind_size_in_bytes[0] = sizeof(int32_t) * innz;

        p_val_datatype[0]      = HIP_R_64F;
        p_val_size_in_bytes[0] = sizeof(double) * innz;

        p_ptr[0] = malloc(p_ptr_size_in_bytes[0]);
        p_ind[0] = malloc(p_ind_size_in_bytes[0]);
        p_val[0] = malloc(p_val_size_in_bytes[0]);
        in->read((char*)p_ptr[0],
                 sizeof(int32_t) * (iM + 1));
        in->read((char*)p_ind[0],
                 sizeof(int32_t) * innz);
        in->read((char*)p_val[0],
                 sizeof(double) * innz);
        in->close();
        delete in;
        in = nullptr;
        std::cout << "Import done." << std::endl;
        return HIPDSS_STATUS_SUCCESS;
    }
}

extern "C" hipdssStatus_t hipdssextMatrixImportConfigCmdline(hipdssHandle_t                handle,
                                                             hipdssextMatrixImportConfig_t matrixImportConfig,
                                                             int32_t*                      p_argc,
                                                             const char*                   argv[])
try
{
    const char* filename = argv[1];
    RETURN_IF_HIPDSS_ERROR(hipdssextMatrixImportConfigFileSet(handle,
                                                              matrixImportConfig,
                                                              HIPDSSEXT_MATRIX_IMPORT_CONFIG_FILE_NAME,
                                                              filename));

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}

extern "C" hipdssStatus_t hipdssextMatrixImport(hipdssHandle_t                handle,
                                                hipdssextMatrixImportConfig_t matrixImportConfig,
                                                hipdssMatrix_t*               p_matrix)
try
{

    int64_t     M;
    int64_t     N;
    int64_t     nnz;
    hipDataType ptr_indextype{};
    void*       ptr{};
    hipDataType ind_indextype{};
    void*       ind{};
    hipDataType val_datatype{};
    void*       val{};
    size_t      ptr_size_in_bytes{};
    size_t      ind_size_in_bytes{};
    size_t      val_size_in_bytes{};

    RETURN_IF_HIPDSS_ERROR(hipdss::matrix_import(matrixImportConfig->get_filename(),
                                                 &M,
                                                 &N,
                                                 &nnz,
                                                 &ptr_indextype,
                                                 &ptr_size_in_bytes,
                                                 &ptr,
                                                 &ind_indextype,
                                                 &ind_size_in_bytes,
                                                 &ind,
                                                 &val_datatype,
                                                 &val_size_in_bytes,
                                                 &val));

    void* dptr{};
    void* dind{};
    void* dval{};

    RETURN_IF_HIP_ERROR(handle->device_malloc(&dptr,
                                              ptr_size_in_bytes));

    RETURN_IF_HIP_ERROR(hipMemcpyAsync(dptr,
                                       ptr,
                                       ptr_size_in_bytes,
                                       hipMemcpyHostToDevice,
                                       handle->get_stream()));

    RETURN_IF_HIP_ERROR(hipMallocAsync(&dind,
                                       ind_size_in_bytes,
                                       handle->get_stream()));

    RETURN_IF_HIP_ERROR(hipMemcpyAsync(dind,
                                       ind,
                                       ind_size_in_bytes,
                                       hipMemcpyHostToDevice,
                                       handle->get_stream()));

    RETURN_IF_HIP_ERROR(hipMallocAsync(&dval,
                                       val_size_in_bytes,
                                       handle->get_stream()));

    RETURN_IF_HIP_ERROR(hipMemcpyAsync(dval,
                                       val,
                                       val_size_in_bytes,
                                       hipMemcpyHostToDevice,
                                       handle->get_stream()));

    RETURN_IF_HIPDSS_ERROR(hipdssMatrixCreateCsr(p_matrix,
                                                 M,
                                                 N,
                                                 nnz,
                                                 dptr,
                                                 nullptr,
                                                 dind,
                                                 dval,
                                                 ptr_indextype,
                                                 ind_indextype,
                                                 val_datatype,
                                                 HIPDSS_MTYPE_GENERAL,
                                                 HIPDSS_MVIEW_FULL,
                                                 HIPDSS_BASE_ZERO));
    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}

#if 0

#endif
