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

#include "hipdssContext.hpp"
#include "hipdssMatrix.hpp"
#include "hipdss_control.hpp"
#include "hipdss_datatype_utils.hpp"
#include "hipdss_enums.hpp"
#include "hipdss_rf_host_symbolic_factor_calculation.hpp"
#include "hipdss_working_row_t.hpp"
#include <memory>
#include <rocsparse/rocsparse-auxiliary.h>
#include <vector>

namespace hipdss
{

    static hipdssStatus_t symbolic_factorization_calculate_nnz_bound(hipdssHandle_t  handle,
                                                                     hipdss_rf_descr rf_descr,
                                                                     hipdssMatrix_t  A,
                                                                     int64_t*        p_nnz)
    {

        const auto factorization_type = rf_descr->get_factorization_type();
        switch(factorization_type)
        {

        case rf_factorization_type_lu:
        {
            RETURN_IF_HIPDSS_ERROR(hipdss::csr_lu_nnz_bound(handle,
                                                            A,
                                                            rf_descr->get_P(),
                                                            rf_descr->get_Q_inverse(),
                                                            p_nnz));
            break;
        }

        case rf_factorization_type_chol:
        case rf_factorization_type_ldl:
        {
            RETURN_IF_HIPDSS_ERROR(hipdss::csr_lu_nnz_bound(handle,
                                                            A,
                                                            rf_descr->get_P(),
                                                            rf_descr->get_Q_inverse(),
                                                            p_nnz));
            break;
        }
        }
        return HIPDSS_STATUS_SUCCESS;
    }

    static hipdssStatus_t symbolic_factorization(hipdssHandle_t  handle,
                                                 hipdss_rf_descr rf_descr,
                                                 hipdssMatrix_t  A)
    {
        //
        // Compute bound.
        //
        std::cout << __FUNCTION__
                  << ": calculate nnz bound ..."
                  << std::endl;

	hipdssMatrixSpyTerminal(handle, A);
	
        int64_t F_nnz_bound = 0;
        RETURN_IF_HIPDSS_ERROR(hipdss::symbolic_factorization_calculate_nnz_bound(handle,
                                                                                  rf_descr,
                                                                                  A,
                                                                                  &F_nnz_bound));

        std::cout << __FUNCTION__
                  << ": calculate nnz bound DONE."
                  << std::endl;

        std::cout << __FUNCTION__
                  << ": nnz bound = "
                  << F_nnz_bound
                  << std::endl;

        //
        // Create factorization sparsity pattern.
        //
        const auto mview = (A->get_mview() == HIPDSS_MVIEW_FULL)
                               ? HIPDSS_MVIEW_FULL
                               : HIPDSS_MVIEW_LOWER;

        hipdssMatrix_t F{};
        RETURN_IF_HIPDSS_ERROR(hipdssMatrix::create_host_csr_sparsity_pattern(handle,
                                                                              &F,
                                                                              A->get_nrows(),
                                                                              A->get_ncols(),
                                                                              F_nnz_bound,
                                                                              A->get_mtype(),
                                                                              mview));
        rf_descr->set_factorizationMatrix(F);

        //
        // Create A2F indexing.
        //
        auto A2F = new hipdss::idvec_descr(handle,
                                           F->get_row_type(),
                                           HIPDSS_BASE_ZERO,
                                           A->get_nnz(),
                                           false);
        rf_descr->set_A2F(A2F);

        const auto factorization_type = rf_descr->get_factorization_type();
        auto       P                  = rf_descr->get_P();
        auto       Qinv               = rf_descr->get_Q_inverse();

        hipdss::host_symbolic_factor_calculation_t symbolic_calculation{};
        RETURN_IF_HIPDSS_ERROR(hipdss::find_host_symbolic_factor_calculation(&symbolic_calculation,
                                                                             (factorization_type != rf_factorization_type_lu),
                                                                             A->get_row_type(),
                                                                             A->get_col_type(),
                                                                             F->get_row_type(),
                                                                             F->get_col_type()));

        int64_t F_nnz = 0;
        RETURN_IF_HIPDSS_ERROR(symbolic_calculation(A->get_nrows(),
                                                    A->get_const_row_host_data(),
                                                    A->get_const_row_end_host_data(),
                                                    A->get_const_col_host_data(),
                                                    A->get_index_base(),
                                                    F->get_row_host_data(),
                                                    F->get_col_host_data(),
                                                    F->get_index_base(),
                                                    A2F->host_data(),
                                                    A2F->get_base(),
                                                    (P) ? P->const_host_data() : nullptr,
                                                    (P) ? P->get_base() : HIPDSS_BASE_ZERO,
                                                    (Qinv) ? Qinv->const_host_data() : nullptr,
                                                    (Qinv) ? Qinv->get_base() : HIPDSS_BASE_ZERO,
                                                    &F_nnz));

        std::cout << __FUNCTION__
                  << ": nnz = "
                  << F_nnz
                  << std::endl;

        std::cout << __FUNCTION__
                  << ": nnz bound = "
                  << F_nnz_bound
                  << std::endl;

        RETURN_IF_HIPDSS_ERROR(F->realloc_host_sparsity_pattern(handle,
                                                                F_nnz));
		hipdssMatrixSpyTerminal(handle, F);

        RETURN_IF_HIPDSS_ERROR(F->to_device(handle));

        return HIPDSS_STATUS_SUCCESS;
    }

} // namespace hipdss

hipdssStatus_t hipdss::rf_symbolic_factorization(hipdssHandle_t  handle,
                                                 hipdss_rf_descr rf_descr,
                                                 hipdssConfig_t  solverConfig,
                                                 hipdssData_t    solverData,
                                                 hipdssMatrix_t  input_matrix)
{
    //
    // Grab the type of the matrix.
    //
    const auto mtype = input_matrix->get_mtype();

    //
    // Determine which factorization type we will be using.
    //
    auto factorization_type = static_cast<rf_factorization_type>(-1);
    switch(mtype)
    {

    case HIPDSS_MTYPE_GENERAL:
    {
        factorization_type = rf_factorization_type_lu;
        break;
    }

    case HIPDSS_MTYPE_HERMITIAN:
    case HIPDSS_MTYPE_SYMMETRIC:
    {
        factorization_type = rf_factorization_type_ldl;
        break;
    }

    case HIPDSS_MTYPE_SPD:
    case HIPDSS_MTYPE_HPD:
    {
        factorization_type = rf_factorization_type_chol;
        break;
    }
    }

    rf_descr->set_factorization_type(factorization_type);

    RETURN_IF_HIPDSS_ERROR(hipdss::symbolic_factorization(handle,
                                                          rf_descr,
                                                          input_matrix));
    return HIPDSS_STATUS_SUCCESS;
}
