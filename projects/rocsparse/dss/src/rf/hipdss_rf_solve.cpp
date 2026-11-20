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
#include "hipdss_rf_descr.hpp"

hipdssStatus_t hipdss::rf_solve(hipdssHandle_t  handle,
                                hipdss_rf_descr rf_descr,
                                hipdssMatrix_t  rhs_matrix,
                                hipdssMatrix_t  solution_matrix)
{

    hipdssMatrix_t intermediate_solve_matrix = rf_descr->get_intermediate_solve_matrix();
    if(intermediate_solve_matrix == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(hipdssMatrix::create_dense(handle,
                                                          &intermediate_solve_matrix,
                                                          solution_matrix->get_layout(),
                                                          solution_matrix->get_val_type(),
                                                          solution_matrix->get_batch_count(),
                                                          solution_matrix->get_nrows(),
                                                          solution_matrix->get_ncols()));

        rf_descr->set_intermediate_solve_matrix(intermediate_solve_matrix);
    }

    //
    // Create, if needed,
    // and apply row permutation on the right-hand-sides.
    //

    auto rhs = rhs_matrix;
    auto P   = rf_descr->get_P();
    if(P)
    {
        RETURN_IF_HIPDSS_ERROR(P->to_device(handle));

        hipdssMatrix_t permuted_rhs_matrix = rf_descr->get_permuted_rhs_matrix();
        if(permuted_rhs_matrix == nullptr)
        {
            RETURN_IF_HIPDSS_ERROR(hipdssMatrix::create_dense(handle,
                                                              &permuted_rhs_matrix,
                                                              rhs_matrix->get_layout(),
                                                              rhs_matrix->get_val_type(),
                                                              rhs_matrix->get_batch_count(),
                                                              rhs_matrix->get_nrows(),
                                                              rhs_matrix->get_ncols()));
            rf_descr->set_permuted_rhs_matrix(permuted_rhs_matrix);
        }

        //
        // Apply the permutation.
        //
        RETURN_IF_HIPDSS_ERROR(hipdssMatrixRowPermutation(handle,
                                                          rhs_matrix,
                                                          permuted_rhs_matrix,
                                                          P));
        rhs = permuted_rhs_matrix;
    }

    //
    // Create, if needed, the buffer on X
    // for column permutation on the solution matrix.
    //
    hipdss::idvec_descr_t Q        = rf_descr->get_Q();
    auto                  solution = solution_matrix;
    if(Q)
    {
        hipdssMatrix_t permuted_solution_matrix = rf_descr->get_permuted_solution_matrix();
        if(permuted_solution_matrix == nullptr)
        {
            RETURN_IF_HIPDSS_ERROR(hipdssMatrix::create_dense(handle,
                                                              &permuted_solution_matrix,
                                                              solution_matrix->get_layout(),
                                                              solution_matrix->get_val_type(),
                                                              solution_matrix->get_batch_count(),
                                                              solution_matrix->get_nrows(),
                                                              solution_matrix->get_ncols()));
            rf_descr->set_permuted_solution_matrix(permuted_solution_matrix);
        }

        //
        // No need to permute, for now...
        // if there is a reuse of the solution matrix, we will.
        //
        solution = permuted_solution_matrix;
    }

    //
    // Solve forward.
    //
    RETURN_IF_HIPDSS_ERROR(hipdss::rf_solve_forward(handle,
                                                    rf_descr,
                                                    rhs,
                                                    intermediate_solve_matrix));

    //
    // Solve diagonal.
    //
    RETURN_IF_HIPDSS_ERROR(hipdss::rf_solve_diagonal(handle,
                                                     rf_descr,
                                                     intermediate_solve_matrix,
                                                     intermediate_solve_matrix));

    //
    // Solve backward.
    //
    RETURN_IF_HIPDSS_ERROR(hipdss::rf_solve_backward(handle,
                                                     rf_descr,
                                                     intermediate_solve_matrix,
                                                     solution));

    //
    // if column permutation, permute back the solution.
    //
    if(Q)
    {
        RETURN_IF_HIPDSS_ERROR(hipdssMatrixRowPermutation(handle,
                                                          solution,
                                                          solution_matrix,
                                                          Q));
    }

    return HIPDSS_STATUS_SUCCESS;
}
