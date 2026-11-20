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

struct hipdssData
{
    //
    //    HIPDSS_DATA_INFO                     = 0, /**< (out) General information struct */
    //    HIPDSS_DATA_LU_NNZ                   = 1, /**< (out) Effective NNZ of L+U factor */
    //    HIPDSS_DATA_NPIVOTS                  = 2, /**< (out) Number of pivots performed */
    //    HIPDSS_DATA_INERTIA                  = 3, /**< (out) Inertia */
    //    HIPDSS_DATA_PERM_REORDER_ROW         = 4, /**< (out) Reordering permutation (rows) */
    //    HIPDSS_DATA_PERM_REORDER_COL         = 5, /**< (out) Reordering permutation (cols) */
    //    HIPDSS_DATA_PERM_ROW                 = 6, /**< (out) Full row perm -- BTF_COLAMD/COLAMD only */
    //    HIPDSS_DATA_PERM_COL                 = 7, /**< (out) Full col perm -- BTF_COLAMD/COLAMD only */
    //    HIPDSS_DATA_DIAG                     = 8, /**< (out) Diagonal elements of the factor */
    //    HIPDSS_DATA_USER_PERM                = 9, /**< (in)  User-provided permutation */
    //    HIPDSS_DATA_HYBRID_DEVICE_MEMORY_MIN = 10, /**< (out) Minimum GPU memory in hybrid mode */
    //    HIPDSS_DATA_COMM_DEVICE              = 11, /**< (in)  Communicator for device buffers (void*) */
    //    HIPDSS_DATA_COMM_HOST                = 12, /**< (in)  Communicator for host buffers (void*) */
    //    HIPDSS_DATA_MEMORY_ESTIMATES         = 13, /**< (out) Memory usage estimates */
    //    HIPDSS_DATA_PERM_MATCHING            = 14, /**< (out) Matching permutation */
    //    HIPDSS_DATA_SCALE_ROW                = 15, /**< (out) Row scaling */
    //    HIPDSS_DATA_SCALE_COL                = 16, /**< (out) Column scaling */
    //    HIPDSS_DATA_NSUPERPANELS             = 17, /**< (out) Number of superpanels */
    //    HIPDSS_DATA_USER_SCHUR_INDICES       = 18, /**< (in)  Row indices for Schur complement */
    //    HIPDSS_DATA_SCHUR_SHAPE              = 19, /**< (out) Shape of Schur complement block */
    //    HIPDSS_DATA_SCHUR_MATRIX             = 20, /**< (out) Schur complement matrix descriptor */
    //    HIPDSS_DATA_USER_ND_PARTITION_TREE   = 21, /**< (in,out) User-provided ND partition tree */
    //    HIPDSS_DATA_ND_PARTITION_TREE        = 22, /**< (out) Nested dissection partition tree */
    //    HIPDSS_DATA_USER_HOST_INTERRUPT      = 23, /**< (in)  Host interrupt flag (volatile int*) */
    //    HIPDSS_DATA_IR_N_STEPS               = 24, /**< (out) Number of IR steps performed */
    //    HIPDSS_DATA_UBATCH_MASK              = 25, /**< (in,out) 0/1 mask to select uniform-batch matrices */
    //    HIPDSS_DATA_FLOPS                    = 26 /**< (out) Floating-point operation count */
    //
};
