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
#include "hipdssDataParam_t.hpp"
#include "hipdss_control.hpp"

extern "C" hipdssStatus_t hipdssDataSet(hipdssHandle_t    handle,
                                        hipdssData_t      data,
                                        hipdssDataParam_t param,
                                        const void*       value,
                                        size_t            sizeInBytes)
try
{

    HIPDSS_CHECKARG_HANDLE(0, handle);
    HIPDSS_CHECKARG_POINTER(1, data);
    HIPDSS_CHECKARG_ENUM(2, param);
    HIPDSS_CHECKARG_POINTER(3, value);

    switch(param)
    {

    case HIPDSS_DATA_INFO: /**< (out) General information struct */
    case HIPDSS_DATA_LU_NNZ: /**< (out) Effective NNZ of L+U factor */
    case HIPDSS_DATA_NPIVOTS: /**< (out) Number of pivots performed */
    case HIPDSS_DATA_INERTIA: /**< (out) Inertia */
    case HIPDSS_DATA_PERM_REORDER_ROW: /**< (out) Reordering permutation (rows) */
    case HIPDSS_DATA_PERM_REORDER_COL: /**< (out) Reordering permutation (cols) */
    case HIPDSS_DATA_PERM_ROW: /**< (out) Full row perm -- BTF_COLAMD/COLAMD only */
    case HIPDSS_DATA_PERM_COL: /**< (out) Full col perm -- BTF_COLAMD/COLAMD only */
    case HIPDSS_DATA_DIAG: /**< (out) Diagonal elements of the factor */
    case HIPDSS_DATA_HYBRID_DEVICE_MEMORY_MIN: /**< (out) Minimum GPU memory in hybrid mode */
    case HIPDSS_DATA_MEMORY_ESTIMATES: /**< (out) Memory usage estimates */
    case HIPDSS_DATA_PERM_MATCHING: /**< (out) Matching permutation */
    case HIPDSS_DATA_SCALE_ROW: /**< (out) Row scaling */
    case HIPDSS_DATA_SCALE_COL: /**< (out) Column scaling */
    case HIPDSS_DATA_NSUPERPANELS: /**< (out) Number of superpanels */
    case HIPDSS_DATA_SCHUR_SHAPE: /**< (out) Shape of Schur complement block */
    case HIPDSS_DATA_SCHUR_MATRIX: /**< (out) Schur complement matrix descriptor */
    case HIPDSS_DATA_ND_PARTITION_TREE: /**< (out) Nested dissection partition tree */
    case HIPDSS_DATA_IR_N_STEPS: /**< (out) Number of IR steps performed */
    case HIPDSS_DATA_FLOPS: /**< (out) Floating-point operation count */
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_NOT_SUPPORTED);
    }

    case HIPDSS_DATA_USER_PERM: /**< (in)  User-provided permutation */
    case HIPDSS_DATA_COMM_DEVICE: /**< (in)  Communicator for device buffers (void*) */
    case HIPDSS_DATA_COMM_HOST: /**< (in)  Communicator for host buffers (void*) */
    case HIPDSS_DATA_USER_SCHUR_INDICES: /**< (in)  Row indices for Schur complement */
    case HIPDSS_DATA_USER_HOST_INTERRUPT: /**< (in)  Host interrupt flag (volatile int*) */
    case HIPDSS_DATA_USER_ND_PARTITION_TREE: /**< (in,out) User-provided ND partition tree */
    case HIPDSS_DATA_UBATCH_MASK: /**< (in,out) 0/1 mask to select uniform-batch matrices */
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_NOT_SUPPORTED);
    }
    }

    return HIPDSS_STATUS_SUCCESS;
}
catch(...)
{
    RETURN_HIPDSS_EXCEPTION();
}
