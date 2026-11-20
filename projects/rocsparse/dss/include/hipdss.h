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

#ifndef HIPDSS_H
#define HIPDSS_H

#include <hip/hip_runtime.h>
#include <stddef.h>
#include <stdint.h>

/* Version macros */
#define HIPDSS_VERSION_MAJOR 0
#define HIPDSS_VERSION_MINOR 8
#define HIPDSS_VERSION_PATCH 0
#define HIPDSS_VERSION \
    (HIPDSS_VERSION_MAJOR * 10000 + HIPDSS_VERSION_MINOR * 100 + HIPDSS_VERSION_PATCH)

/* Visibility macros and calling convention */
#include "hipdss-export.h"

/* Shared interface descriptors (same types as hipdss public layer) */
#include "hipdss_data_types.h"
#include "hipdss_distributed_interface.h"
#include "hipdss_threading_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssConfigSet(hipdssConfig_t      config,
                                         hipdssConfigParam_t param,
                                         const void*         value,
                                         size_t              sizeInBytes);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssConfigGet(const hipdssConfig_t config,
                                         hipdssConfigParam_t  param,
                                         void*                value,
                                         size_t               sizeInBytes,
                                         size_t*              sizeWritten);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssDataSet(const hipdssHandle_t handle,
                                       hipdssData_t         data,
                                       hipdssDataParam_t    param,
                                       const void*          value,
                                       size_t               sizeInBytes);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssDataGet(const hipdssHandle_t handle,
                                       const hipdssData_t   data,
                                       hipdssDataParam_t    param,
                                       void*                value,
                                       size_t               sizeInBytes,
                                       size_t*              sizeWritten);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssExecute(hipdssHandle_t       handle,
                                       int                  phase,
                                       const hipdssConfig_t config,
                                       hipdssData_t         data,
                                       const hipdssMatrix_t inputMatrix,
                                       hipdssMatrix_t       solution,
                                       const hipdssMatrix_t rhs);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssSetStream(hipdssHandle_t handle,
                                         hipStream_t    stream);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssSetMgStreams(hipdssHandle_t     handle,
                                            const hipStream_t* streams,
                                            int                stream_count);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssSetCommLayer(hipdssHandle_t handle,
                                            const char*    commLibFileName);

// Setting the threading layer library name (in the library handle)
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssSetThreadingLayer(hipdssHandle_t handle,
                                                 const char*    thrLibFileName);

// Setting the on-disk cache directory for runtime-compiled kernels (in the library handle)
// NULL: read HIPDSS_JIT_CACHE_DIR (default); "": no on-disk cache; otherwise the directory to use
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssSetJitCacheDir(hipdssHandle_t handle,
                                              const char*    cacheDir);

// Create/Destroy APIs (allocating structures + set defaults)
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssConfigCreate(hipdssConfig_t* config);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssConfigDestroy(hipdssConfig_t config);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssDataCreate(const hipdssHandle_t handle,
                                          hipdssData_t*        data);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssDataDestroy(hipdssHandle_t handle,
                                           hipdssData_t   data);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssCreate(hipdssHandle_t* handle);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssCreateMg(hipdssHandle_t* handle,
                                        int             device_count,
                                        const int*      device_indices);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssDestroy(hipdssHandle_t handle);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssGetProperty(libraryPropertyType propertyType,
                                           int*                value);

// Create/Destroy API helpers for matrix wrappers
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixCreateDn(hipdssMatrix_t* matrix,
                                              int64_t         nrows,
                                              int64_t         ncols,
                                              int64_t         ld,
                                              const void*     values,
                                              hipDataType     valueType,
                                              hipdssLayout_t  layout);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixCreateCsr(hipdssMatrix_t*        matrix,
                                               int64_t                nrows,
                                               int64_t                ncols,
                                               int64_t                nnz,
                                               const void*            rowStart,
                                               const void*            rowEnd,
                                               const void*            colIndices,
                                               const void*            values,
                                               hipDataType            offsetType,
                                               hipDataType            indexType,
                                               hipDataType            valueType,
                                               hipdssMatrixType_t     mtype,
                                               hipdssMatrixViewType_t mview,
                                               hipdssIndexBase_t      indexBase);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixCreateBatchDn(hipdssMatrix_t*    matrix,
                                                   int64_t            batchCount,
                                                   const void*        nrows,
                                                   const void*        ncols,
                                                   const void*        ld,
                                                   const void* const* values,
                                                   hipDataType        integerType,
                                                   hipDataType        valueType,
                                                   hipdssLayout_t     layout);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixCreateBatchCsr(hipdssMatrix_t*        matrix,
                                                    int64_t                batchCount,
                                                    const void*            nrows,
                                                    const void*            ncols,
                                                    const void*            nnz,
                                                    const void* const*     rowStart,
                                                    const void* const*     rowEnd,
                                                    const void* const*     colIndices,
                                                    const void* const*     values,
                                                    hipDataType            offsetType,
                                                    hipDataType            indexType,
                                                    hipDataType            valueType,
                                                    hipdssMatrixType_t     mtype,
                                                    hipdssMatrixViewType_t mview,
                                                    hipdssIndexBase_t      indexBase);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixDestroy(hipdssMatrix_t matrix);

// Setters/Getters API helpers for matrix wrappers
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixGetDn(const hipdssMatrix_t matrix,
                                           int64_t*             nrows,
                                           int64_t*             ncols,
                                           int64_t*             ld,
                                           void**               values,
                                           hipDataType*         valueType,
                                           hipdssLayout_t*      layout);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixGetCsr(const hipdssMatrix_t    matrix,
                                            int64_t*                nrows,
                                            int64_t*                ncols,
                                            int64_t*                nnz,
                                            void**                  rowStart,
                                            void**                  rowEnd,
                                            void**                  colIndices,
                                            void**                  values,
                                            hipDataType*            offsetType,
                                            hipDataType*            indexType,
                                            hipDataType*            valueType,
                                            hipdssMatrixType_t*     mtype,
                                            hipdssMatrixViewType_t* mview,
                                            hipdssIndexBase_t*      indexBase);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixSetValues(hipdssMatrix_t matrix,
                                               const void*    values);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixSetCsrPointers(hipdssMatrix_t matrix,
                                                    const void*    rowStart,
                                                    const void*    rowEnd,
                                                    const void*    colIndices,
                                                    const void*    values);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixGetBatchDn(const hipdssMatrix_t matrix,
                                                int64_t*             batchCount,
                                                void**               nrows,
                                                void**               ncols,
                                                void**               ld,
                                                void***              values,
                                                hipDataType*         integerType,
                                                hipDataType*         valueType,
                                                hipdssLayout_t*      layout);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixGetBatchCsr(const hipdssMatrix_t    matrix,
                                                 int64_t*                batchCount,
                                                 void**                  nrows,
                                                 void**                  ncols,
                                                 void**                  nnz,
                                                 void***                 rowStart,
                                                 void***                 rowEnd,
                                                 void***                 colIndices,
                                                 void***                 values,
                                                 hipDataType*            offsetType,
                                                 hipDataType*            indexType,
                                                 hipDataType*            valueType,
                                                 hipdssMatrixType_t*     mtype,
                                                 hipdssMatrixViewType_t* mview,
                                                 hipdssIndexBase_t*      indexBase);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixSetBatchValues(hipdssMatrix_t     matrix,
                                                    const void* const* values);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixSetBatchCsrPointers(hipdssMatrix_t     matrix,
                                                         const void* const* rowStart,
                                                         const void* const* rowEnd,
                                                         const void* const* colIndices,
                                                         const void* const* values);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixGetFormat(const hipdssMatrix_t matrix,
                                               int*                 format);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixSetDistributionRow1d(hipdssMatrix_t matrix,
                                                          int64_t        first_row,
                                                          int64_t        last_row);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssMatrixGetDistributionRow1d(const hipdssMatrix_t matrix,
                                                          int64_t*             first_row,
                                                          int64_t*             last_row);

// Memory allocator API
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssGetDeviceMemHandler(const hipdssHandle_t      handle,
                                                   hipdssDeviceMemHandler_t* handler);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssSetDeviceMemHandler(hipdssHandle_t                  handle,
                                                   const hipdssDeviceMemHandler_t* handler);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssGetPinnedMemHandler(const hipdssHandle_t      handle,
                                                   hipdssPinnedMemHandler_t* handler);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssSetPinnedMemHandler(hipdssHandle_t                  handle,
                                                   const hipdssPinnedMemHandler_t* handler);

typedef void (*hipdssLoggerCallback_t)(int         logLevel,
                                       const char* functionName,
                                       const char* message);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssLoggerSetCallback(hipdssLoggerCallback_t callback);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssLoggerSetFile(FILE* file);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssLoggerOpenFile(const char* logFile);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssLoggerSetLevel(int level);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssLoggerSetMask(int mask);
HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssLoggerForceDisable(void);

typedef struct hipdssextMatrixImportConfig* hipdssextMatrixImportConfig_t;

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssextMatrixImportConfigDestroy(hipdssHandle_t,
                                                            hipdssextMatrixImportConfig_t);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssextMatrixImportConfigCreate(hipdssHandle_t,
                                                           hipdssextMatrixImportConfig_t*);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssextMatrixImportConfigCmdline(hipdssHandle_t,
                                                            hipdssextMatrixImportConfig_t,
                                                            int32_t*,
                                                            const char**);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssextMatrixImport(hipdssHandle_t,
                                               hipdssextMatrixImportConfig_t,
                                               hipdssMatrix_t*);

typedef enum
{
    HIPDSSEXT_MATRIX_IMPORT_CONFIG_FILE_NAME = 0
} hipdssextMatrixImportConfigFile_t;

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssextMatrixImportConfigFileSet(hipdssHandle_t,
                                                            hipdssextMatrixImportConfig_t,
                                                            hipdssextMatrixImportConfigFile_t,
                                                            const void* data);

HIPDSS_EXPORT
hipdssStatus_t HIPDSSAPI hipdssextInfo(hipdssHandle_t handle,
                                       FILE*          out);

#ifdef __cplusplus
}
#endif

#endif /* HIPDSS_H */
