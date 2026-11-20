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

#include "hipdssDataParam_t.hpp"
#include "hipdss_control.hpp"

template <>
bool hipdss::enum_utils::is_invalid(const hipdssDataParam_t value)
{
    switch(value)
    {
    case HIPDSS_DATA_INFO:
    case HIPDSS_DATA_LU_NNZ:
    case HIPDSS_DATA_NPIVOTS:
    case HIPDSS_DATA_INERTIA:
    case HIPDSS_DATA_PERM_REORDER_ROW:
    case HIPDSS_DATA_PERM_REORDER_COL:
    case HIPDSS_DATA_PERM_ROW:
    case HIPDSS_DATA_PERM_COL:
    case HIPDSS_DATA_DIAG:
    case HIPDSS_DATA_USER_PERM:
    case HIPDSS_DATA_HYBRID_DEVICE_MEMORY_MIN:
    case HIPDSS_DATA_COMM_DEVICE:
    case HIPDSS_DATA_COMM_HOST:
    case HIPDSS_DATA_MEMORY_ESTIMATES:
    case HIPDSS_DATA_PERM_MATCHING:
    case HIPDSS_DATA_SCALE_ROW:
    case HIPDSS_DATA_SCALE_COL:
    case HIPDSS_DATA_NSUPERPANELS:
    case HIPDSS_DATA_USER_SCHUR_INDICES:
    case HIPDSS_DATA_SCHUR_SHAPE:
    case HIPDSS_DATA_SCHUR_MATRIX:
    case HIPDSS_DATA_USER_ND_PARTITION_TREE:
    case HIPDSS_DATA_ND_PARTITION_TREE:
    case HIPDSS_DATA_USER_HOST_INTERRUPT:
    case HIPDSS_DATA_IR_N_STEPS:
    case HIPDSS_DATA_UBATCH_MASK:
    case HIPDSS_DATA_FLOPS:
    {
        return false;
    }
    }
    return true;
}

template <>
const char* hipdss::enum_utils::to_string(const hipdssDataParam_t value)
{
    switch(value)
    {
    case HIPDSS_DATA_INFO:
        return "HIPDSS_DATA_INFO";
    case HIPDSS_DATA_LU_NNZ:
        return "HIPDSS_DATA_LU_NNZ";
    case HIPDSS_DATA_NPIVOTS:
        return "HIPDSS_DATA_NPIVOTS";
    case HIPDSS_DATA_INERTIA:
        return "HIPDSS_DATA_INERTIA";
    case HIPDSS_DATA_PERM_REORDER_ROW:
        return "HIPDSS_DATA_PERM_REORDER_ROW";
    case HIPDSS_DATA_PERM_REORDER_COL:
        return "HIPDSS_DATA_PERM_REORDER_COL";
    case HIPDSS_DATA_PERM_ROW:
        return "HIPDSS_DATA_PERM_ROW";
    case HIPDSS_DATA_PERM_COL:
        return "HIPDSS_DATA_PERM_COL";
    case HIPDSS_DATA_DIAG:
        return "HIPDSS_DATA_DIAG";
    case HIPDSS_DATA_USER_PERM:
        return "HIPDSS_DATA_USER_PERM";
    case HIPDSS_DATA_HYBRID_DEVICE_MEMORY_MIN:
        return "HIPDSS_DATA_HYBRID_DEVICE_MEMORY_MIN";
    case HIPDSS_DATA_COMM_DEVICE:
        return "HIPDSS_DATA_COMM_DEVICE";
    case HIPDSS_DATA_COMM_HOST:
        return "HIPDSS_DATA_COMM_HOST";
    case HIPDSS_DATA_MEMORY_ESTIMATES:
        return "HIPDSS_DATA_MEMORY_ESTIMATES";
    case HIPDSS_DATA_PERM_MATCHING:
        return "HIPDSS_DATA_PERM_MATCHING";
    case HIPDSS_DATA_SCALE_ROW:
        return "HIPDSS_DATA_SCALE_ROW";
    case HIPDSS_DATA_SCALE_COL:
        return "HIPDSS_DATA_SCALE_COL";
    case HIPDSS_DATA_NSUPERPANELS:
        return "HIPDSS_DATA_NSUPERPANELS";
    case HIPDSS_DATA_USER_SCHUR_INDICES:
        return "HIPDSS_DATA_USER_SCHUR_INDICES";
    case HIPDSS_DATA_SCHUR_SHAPE:
        return "HIPDSS_DATA_SCHUR_SHAPE";
    case HIPDSS_DATA_SCHUR_MATRIX:
        return "HIPDSS_DATA_SCHUR_MATRIX";
    case HIPDSS_DATA_USER_ND_PARTITION_TREE:
        return "HIPDSS_DATA_USER_ND_PARTITION_TREE";
    case HIPDSS_DATA_ND_PARTITION_TREE:
        return "HIPDSS_DATA_ND_PARTITION_TREE";
    case HIPDSS_DATA_USER_HOST_INTERRUPT:
        return "HIPDSS_DATA_USER_HOST_INTERRUPT";
    case HIPDSS_DATA_IR_N_STEPS:
        return "HIPDSS_DATA_IR_N_STEPS";
    case HIPDSS_DATA_UBATCH_MASK:
        return "HIPDSS_DATA_UBATCH_MASK";
    case HIPDSS_DATA_FLOPS:
        return "HIPDSS_DATA_FLOPS";
    }
    // LCOV_EXCL_START
    THROW_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    // LCOV_EXCL_STOP
}
