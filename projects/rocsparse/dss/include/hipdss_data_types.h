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
#ifndef HIPDSS_TYPES_H
#define HIPDSS_TYPES_H

#include <hip/library_types.h>
#include <stddef.h>

/// \cond DO_NOT_DOCUMENT
#define HIPDSS_KERNEL_W(MAX_THREADS_PER_BLOCK, MIN_WARPS_PER_EXECUTION_UNIT) \
    __launch_bounds__(MAX_THREADS_PER_BLOCK,                                 \
                      MIN_WARPS_PER_EXECUTION_UNIT) static __global__
#define HIPDSS_KERNEL(MAX_THREADS_PER_BLOCK) \
    __launch_bounds__(MAX_THREADS_PER_BLOCK) static __global__
#define HIPDSS_DEVICE_ILF static __device__ __forceinline__

typedef struct ihipStream_t* hipStream_t;
/// \endcond

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief hipdss library context handle. */
struct hipdssContext;
typedef struct hipdssContext* hipdssHandle_t;

/*! \brief Opaque matrix descriptor (dense, CSR, batch or distributed). */
struct hipdssMatrix;
typedef struct hipdssMatrix* hipdssMatrix_t;

/*! \brief Opaque solver data descriptor (factors, permutations, ...). */
struct hipdssData;
typedef struct hipdssData* hipdssData_t;

/*! \brief Opaque solver configuration descriptor (algorithmic knobs). */
struct hipdssConfig;
typedef struct hipdssConfig* hipdssConfig_t;

typedef enum hipdssDataType_t
{
    HIPDSS_DATA_TYPE_UNSET = 1024, /**< Type not set */
    HIPDSS_R_32F           = HIP_R_32F, /**< 32-bit real */
    HIPDSS_R_64F           = HIP_R_64F, /**< 64-bit real */
    HIPDSS_C_32F           = HIP_C_32F, /**< 32-bit complex */
    HIPDSS_C_64F           = HIP_C_64F, /**< 64-bit complex */
    HIPDSS_R_64F_64F       = 1025 + HIP_R_64F, /**< Double-double mixed precision */
    HIPDSS_R_32I           = HIP_R_32I, /**< 32-bit signed integer */
    HIPDSS_R_64I           = HIP_R_64I /**< 64-bit signed integer */
} hipdssDataType_t;

typedef enum hipdssSolveMode_t
{
    HIPDSS_SOLVE_MODE_NORMAL              = 0,
    HIPDSS_SOLVE_MODE_TRANSPOSE           = 1,
    HIPDSS_SOLVE_MODE_CONJUGATE_TRANSPOSE = 2
} hipdssSolveMode_t;

typedef enum hipdssMemoryEstimateState_t
{
    HIPDSS_MEMORY_ESTIMATE_STATE_NONE       = 0,
    HIPDSS_MEMORY_ESTIMATE_STATE_FILL_KNOWN = 1,
    HIPDSS_MEMORY_ESTIMATE_STATE_FINAL      = 2
} hipdssMemoryEstimateState_t;

typedef enum hipdssDowncastFactorsMode_t
{
    HIPDSS_DOWNCAST_FACTORS_MODE_DISABLED             = 0,
    HIPDSS_DOWNCAST_FACTORS_MODE_KEEP_BLOCK_DIAGONAL  = 1,
    HIPDSS_DOWNCAST_FACTORS_MODE_KEEP_STRICT_DIAGONAL = 2
} hipdssDowncastFactorsMode_t;

typedef enum hipdssMemoryOptimizationFlags_t
{
    HIPDSS_MEMORY_OPTIMIZATION_DEFAULT    = 0,
    HIPDSS_MEMORY_OPTIMIZATION_SOLVE_ONLY = 1 << 0
} hipdssMemoryOptimizationFlags_t;

typedef enum hipdssConfigParam_t
{
    HIPDSS_CONFIG_REORDERING_ALG, /**< Reordering algorithm (hipdssReorderingAlg_t) */
    HIPDSS_CONFIG_FACTORIZATION_ALG, /**< Factorization algorithm (hipdssFactorizationAlg_t) */
    HIPDSS_CONFIG_SOLVE_ALG, /**< Solve algorithm (hipdssSolveAlg_t) */
    HIPDSS_CONFIG_MATCHING_ALG, /**< Matching algorithm (hipdssMatchingAlg_t) */
    HIPDSS_CONFIG_SOLVE_MODE, /**< Solve mode: 0=A, 1=A^T, 2=A^H */
    HIPDSS_CONFIG_IR_N_STEPS, /**< Iterative refinement steps */
    HIPDSS_CONFIG_IR_TOL, /**< IR tolerance (not supported) */
    HIPDSS_CONFIG_PIVOT_TYPE, /**< Pivoting type (hipdssPivotType_t) */
    HIPDSS_CONFIG_PIVOT_THRESHOLD, /**< Pivoting threshold (double) */
    HIPDSS_CONFIG_PIVOT_EPSILON, /**< Pivoting epsilon (double) */
    HIPDSS_CONFIG_MAX_LU_NNZ, /**< Max factor NNZ (int64_t) -- BTF_COLAMD/COLAMD */
    HIPDSS_CONFIG_HYBRID_MEMORY_MODE, /**< Hybrid memory mode (int: 0=off) */
    HIPDSS_CONFIG_HYBRID_DEVICE_MEMORY_LIMIT, /**< GPU memory limit in hybrid mode */
    HIPDSS_CONFIG_USE_HIP_REGISTER_MEMORY, /**< Use HIP registered memory (int: 1=on) */
    HIPDSS_CONFIG_HOST_NTHREADS, /**< Number of CPU threads (int) */
    HIPDSS_CONFIG_HYBRID_EXECUTE_MODE, /**< Hybrid execute mode (int: 0=off) */
    HIPDSS_CONFIG_PIVOT_EPSILON_ALG, /**< Pivot epsilon algorithm (hipdssPivotEpsilonAlg_t) */
    HIPDSS_CONFIG_ND_NLEVELS, /**< Nested dissection levels (int) */
    HIPDSS_CONFIG_UBATCH_SIZE, /**< Uniform batch size (int) */
    HIPDSS_CONFIG_UBATCH_INDEX, /**< Uniform batch index (int) */
    HIPDSS_CONFIG_USE_SUPERPANELS, /**< Superpanel optimisation (int: 0/1) */
    HIPDSS_CONFIG_DEVICE_COUNT, /**< Number of GPUs for MG mode (int) */
    HIPDSS_CONFIG_DEVICE_INDICES, /**< Array of GPU device indices (int*) */
    HIPDSS_CONFIG_SCHUR_MODE, /**< Schur complement mode (int) */
    HIPDSS_CONFIG_DETERMINISTIC_MODE, /**< Deterministic execution (int: 0/1) */
    HIPDSS_CONFIG_ND_UBFACTOR, /**< Nested dissection unbalance factor (percent, int) */
    HIPDSS_CONFIG_EMULATION_MANTISSA_BIT_COUNT,
    HIPDSS_CONFIG_FORCE_NONSYMMETRIC_MATCHING,
    HIPDSS_CONFIG_DOWNCAST_FACTORS,
    HIPDSS_CONFIG_MEMORY_OPTIMIZATION_SETTING

} hipdssConfigParam_t;

typedef enum hipdssDataParam_t
{
    HIPDSS_DATA_INFO                     = 0, /**< (out) General information struct */
    HIPDSS_DATA_LU_NNZ                   = 1, /**< (out) Effective NNZ of L+U factor */
    HIPDSS_DATA_NPIVOTS                  = 2, /**< (out) Number of pivots performed */
    HIPDSS_DATA_INERTIA                  = 3, /**< (out) Inertia */
    HIPDSS_DATA_PERM_REORDER_ROW         = 4, /**< (out) Reordering permutation (rows) */
    HIPDSS_DATA_PERM_REORDER_COL         = 5, /**< (out) Reordering permutation (cols) */
    HIPDSS_DATA_PERM_ROW                 = 6, /**< (out) Full row perm -- BTF_COLAMD/COLAMD only */
    HIPDSS_DATA_PERM_COL                 = 7, /**< (out) Full col perm -- BTF_COLAMD/COLAMD only */
    HIPDSS_DATA_DIAG                     = 8, /**< (out) Diagonal elements of the factor */
    HIPDSS_DATA_USER_PERM                = 9, /**< (in)  User-provided permutation */
    HIPDSS_DATA_HYBRID_DEVICE_MEMORY_MIN = 10, /**< (out) Minimum GPU memory in hybrid mode */
    HIPDSS_DATA_COMM_DEVICE              = 11, /**< (in)  Communicator for device buffers (void*) */
    HIPDSS_DATA_COMM_HOST                = 12, /**< (in)  Communicator for host buffers (void*) */
    HIPDSS_DATA_MEMORY_ESTIMATES         = 13, /**< (out) Memory usage estimates */
    HIPDSS_DATA_PERM_MATCHING            = 14, /**< (out) Matching permutation */
    HIPDSS_DATA_SCALE_ROW                = 15, /**< (out) Row scaling */
    HIPDSS_DATA_SCALE_COL                = 16, /**< (out) Column scaling */
    HIPDSS_DATA_NSUPERPANELS             = 17, /**< (out) Number of superpanels */
    HIPDSS_DATA_USER_SCHUR_INDICES       = 18, /**< (in)  Row indices for Schur complement */
    HIPDSS_DATA_SCHUR_SHAPE              = 19, /**< (out) Shape of Schur complement block */
    HIPDSS_DATA_SCHUR_MATRIX             = 20, /**< (out) Schur complement matrix descriptor */
    HIPDSS_DATA_USER_ND_PARTITION_TREE   = 21, /**< (in,out) User-provided ND partition tree */
    HIPDSS_DATA_ND_PARTITION_TREE        = 22, /**< (out) Nested dissection partition tree */
    HIPDSS_DATA_USER_HOST_INTERRUPT      = 23, /**< (in)  Host interrupt flag (volatile int*) */
    HIPDSS_DATA_IR_N_STEPS               = 24, /**< (out) Number of IR steps performed */
    HIPDSS_DATA_UBATCH_MASK              = 25, /**< (in,out) 0/1 mask to select uniform-batch matrices */
    HIPDSS_DATA_FLOPS                    = 26 /**< (out) Floating-point operation count */
} hipdssDataParam_t;

typedef enum hipdssPhase_t
{
    HIPDSS_PHASE_REORDERING             = 1 << 0, /**< Fill-reducing reordering */
    HIPDSS_PHASE_SYMBOLIC_FACTORIZATION = 1 << 1, /**< Symbolic factorization */
    HIPDSS_PHASE_ANALYSIS               = HIPDSS_PHASE_REORDERING | HIPDSS_PHASE_SYMBOLIC_FACTORIZATION,
    HIPDSS_PHASE_FACTORIZATION          = 1 << 2, /**< Numerical LU/LDL^T/LL^T */
    HIPDSS_PHASE_REFACTORIZATION        = 1 << 3, /**< Refactorization (same sparsity) */
    HIPDSS_PHASE_SOLVE_FWD_PERM         = 1 << 4, /**< Apply row perm + fwd sub */
    HIPDSS_PHASE_SOLVE_FWD              = 1 << 5, /**< Forward substitution (L) */
    HIPDSS_PHASE_SOLVE_DIAG             = 1 << 6, /**< Diagonal solve (D) */
    HIPDSS_PHASE_SOLVE_BWD              = 1 << 7, /**< Backward substitution */
    HIPDSS_PHASE_SOLVE_BWD_PERM         = 1 << 8, /**< Bwd sub + apply perm */
    HIPDSS_PHASE_SOLVE_REFINEMENT       = 1 << 9, /**< Iterative refinement */
    HIPDSS_PHASE_SOLVE                  = HIPDSS_PHASE_SOLVE_FWD_PERM | HIPDSS_PHASE_SOLVE_FWD | HIPDSS_PHASE_SOLVE_DIAG | HIPDSS_PHASE_SOLVE_BWD | HIPDSS_PHASE_SOLVE_BWD_PERM | HIPDSS_PHASE_SOLVE_REFINEMENT
} hipdssPhase_t;

typedef enum hipdssStatus_t
{
    HIPDSS_STATUS_SUCCESS          = 0, /**< Operation successful */
    HIPDSS_STATUS_NOT_INITIALIZED  = 1, /**< Handle not created */
    HIPDSS_STATUS_ALLOC_FAILED     = 2, /**< Memory allocation failure */
    HIPDSS_STATUS_INVALID_VALUE    = 3, /**< Invalid argument */
    HIPDSS_STATUS_NOT_SUPPORTED    = 4, /**< Operation not supported */
    HIPDSS_STATUS_EXECUTION_FAILED = 5, /**< GPU execution failure */
    HIPDSS_STATUS_INTERNAL_ERROR   = 6, /**< Internal error */
    HIPDSS_STATUS_IR_FAILED        = 7 /**< Iterative refinement failed to converge */
} hipdssStatus_t;

typedef enum hipdssMatrixType_t
{
    HIPDSS_MTYPE_GENERAL   = 0, /**< General (non-symmetric) matrix */
    HIPDSS_MTYPE_SYMMETRIC = 1, /**< Symmetric (A = A^T) */
    HIPDSS_MTYPE_HERMITIAN = 2, /**< Hermitian (A = A^H) */
    HIPDSS_MTYPE_SPD       = 3, /**< Symmetric positive definite */
    HIPDSS_MTYPE_HPD       = 4 /**< Hermitian positive definite */
} hipdssMatrixType_t;

typedef enum hipdssMatrixViewType_t
{
    HIPDSS_MVIEW_FULL  = 0, /**< Full matrix */
    HIPDSS_MVIEW_LOWER = 1, /**< Lower triangle */
    HIPDSS_MVIEW_UPPER = 2 /**< Upper triangle */
} hipdssMatrixViewType_t;

typedef enum hipdssIndexBase_t
{
    HIPDSS_BASE_ZERO = 0, /**< 0-based indexing (C) */
    HIPDSS_BASE_ONE  = 1 /**< 1-based indexing (Fortran) */
} hipdssIndexBase_t;

typedef enum hipdssLayout_t
{
    HIPDSS_LAYOUT_COL_MAJOR = 0, /**< Column-major (Fortran) */
    HIPDSS_LAYOUT_ROW_MAJOR = 1 /**< Row-major (C) */
} hipdssLayout_t;

typedef enum hipdssReorderingAlg_t
{
    HIPDSS_REORDERING_ALG_DEFAULT           = 0, /**< Automatic choice */
    HIPDSS_REORDERING_ALG_BTF_COLAMD        = 1, /**< Block triangular form + COLAMD */
    HIPDSS_REORDERING_ALG_COLAMD            = 2, /**< COLAMD */
    HIPDSS_REORDERING_ALG_AMD               = 3, /**< AMD */
    HIPDSS_REORDERING_ALG_NESTED_DISSECTION = 4, /**< Nested dissection */
    HIPDSS_REORDERING_ALG_NONE              = 5, /**< Natural (identity) order */
    HIPDSSEXT_REORDERING_ALG_RCMK           = 128 /**< Reverse Cuthill McKee */
} hipdssReorderingAlg_t;

typedef enum hipdssFactorizationAlg_t
{
    HIPDSS_FACTORIZATION_ALG_DEFAULT    = 0, /**< Automatic choice */
    HIPDSS_FACTORIZATION_ALG_MULTIBLOCK = 1, /**< Multiblock factorization */
    HIPDSS_FACTORIZATION_ALG_GENERAL    = 2 /**< General factorization */
} hipdssFactorizationAlg_t;

typedef enum hipdssPivotEpsilonAlg_t
{
    HIPDSS_PIVOT_EPSILON_ALG_DEFAULT = 0, /**< Automatic choice */
    HIPDSS_PIVOT_EPSILON_ALG_SCALED  = 1, /**< Scaled epsilon */
    HIPDSS_PIVOT_EPSILON_ALG_STATIC  = 2 /**< Static epsilon */
} hipdssPivotEpsilonAlg_t;

typedef enum hipdssSolveAlg_t
{
    HIPDSS_SOLVE_ALG_DEFAULT = 0, /**< Automatic choice */
    HIPDSS_SOLVE_ALG_GENERAL = 1 /**< General solve */
} hipdssSolveAlg_t;

typedef enum hipdssMatchingAlg_t
{
    HIPDSS_MATCHING_ALG_NONE             = 0, /**< Matching disabled */
    HIPDSS_MATCHING_ALG_MAX_DIAG_COUNT   = 1, /**< Maximize count on diagonal */
    HIPDSS_MATCHING_ALG_MAX_MIN_DIAG     = 2, /**< Maximize minimum diagonal */
    HIPDSS_MATCHING_ALG_MAX_MIN_DIAG_ALT = 3, /**< Maximize minimum diagonal (alt) */
    HIPDSS_MATCHING_ALG_MAX_DIAG_SUM     = 4, /**< Maximize diagonal sum */
    HIPDSS_MATCHING_ALG_MAX_DIAG_PRODUCT = 5, /**< Maximize diagonal product */
    HIPDSS_MATCHING_ALG_AUTO             = 6 /**< Library selects the algorithm */
} hipdssMatchingAlg_t;

typedef enum hipdssPivotType_t
{
    HIPDSS_PIVOT_AUTO          = 0, /**< Automatic pivoting strategy [default] */
    HIPDSS_PIVOT_NONE          = 1, /**< Disable pivoting search */
    HIPDSS_PIVOT_GLOBAL_COL    = 2, /**< Global column pivoting (BTF_COLAMD/COLAMD) */
    HIPDSS_PIVOT_GLOBAL_ROW    = 3, /**< Global row pivoting (BTF_COLAMD/COLAMD) */
    HIPDSS_PIVOT_DIAGONAL      = 4, /**< Diagonal pivoting (symmetric indefinite, AMD/ND) */
    HIPDSS_PIVOT_LOCAL_BLOCK   = 5, /**< Local block pivoting (general, AMD/ND) */
    HIPDSS_PIVOT_BUNCH_KAUFMAN = 6 /**< Bunch-Kaufman (reserved, not supported yet) */
} hipdssPivotType_t;

typedef enum hipdssMatrixFormat_t
{
    HIPDSS_MFORMAT_DENSE       = 1, /**< Dense matrix */
    HIPDSS_MFORMAT_CSR         = 2, /**< Sparse CSR matrix */
    HIPDSS_MFORMAT_BATCH       = 4, /**< Batch of matrices */
    HIPDSS_MFORMAT_DISTRIBUTED = 8 /**< Distributed matrix (MPI) */
} hipdssMatrixFormat_t;

#define HIPDSS_ALLOCATOR_NAME_LEN 64

typedef struct hipdssDeviceMemHandler_t
{
    void* ctx;
    int (*device_alloc)(void*       ctx,
                        void**      ptr,
                        size_t      size,
                        hipStream_t stream);

    int (*device_free)(void*       ctx,
                       void*       ptr,
                       size_t      size,
                       hipStream_t stream);

    char name[HIPDSS_ALLOCATOR_NAME_LEN];

} hipdssDeviceMemHandler_t;

typedef struct hipdssPinnedMemHandler_t
{
    void* ctx;

    int (*pinned_alloc)(void*       ctx,
                        void**      ptr,
                        size_t      size,
                        hipStream_t stream);

    int (*pinned_free)(void*       ctx,
                       void*       ptr,
                       size_t      size,
                       hipStream_t stream);

    char name[HIPDSS_ALLOCATOR_NAME_LEN];

} hipdssPinnedMemHandler_t;

//
// Versioning
//
typedef enum libraryPropertyType
{
    MAJOR_VERSION,
    MINOR_VERSION,
    PATCH_LEVEL
} libraryPropertyType;

#ifdef __cplusplus
}
#endif

#endif /* HIPDSS_TYPES_H */
