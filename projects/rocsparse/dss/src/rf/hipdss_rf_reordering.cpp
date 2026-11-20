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

#include "hipdssConfig.hpp"
#include "hipdssMatrix.hpp"
#include "hipdss_rf_descr.hpp"

namespace hipdss
{

    static hipdssStatus_t hipdssReorderingAlg_t2spreordering_alg(hipdssReorderingAlg_t source,
                                                                 spreordering_alg*     target)
    {
        switch(source)
        {

        case HIPDSS_REORDERING_ALG_COLAMD:
        case HIPDSS_REORDERING_ALG_BTF_COLAMD:
        case HIPDSS_REORDERING_ALG_AMD:
        {
            target[0] = hipdss::spreordering_alg_none;
            return HIPDSS_STATUS_NOT_SUPPORTED;
        }

        case HIPDSS_REORDERING_ALG_NESTED_DISSECTION:
        {
            target[0] = hipdss::spreordering_alg_nd_metis;
            return HIPDSS_STATUS_SUCCESS;
        }

        case HIPDSSEXT_REORDERING_ALG_RCMK:
        {
            target[0] = hipdss::spreordering_alg_rcmk;
            return HIPDSS_STATUS_SUCCESS;
        }

        case HIPDSS_REORDERING_ALG_DEFAULT:
        {
            target[0] = hipdss::spreordering_alg_none;
            return HIPDSS_STATUS_SUCCESS;
        }

        case HIPDSS_REORDERING_ALG_NONE:
        {
            target[0] = hipdss::spreordering_alg_none;
            return HIPDSS_STATUS_SUCCESS;
        }
        }

        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

    static hipdssStatus_t rf_reordering_prepare(hipdssHandle_t  handle,
                                                hipdss_rf_descr rf_descr,
                                                hipdssConfig_t  solverConfig,
                                                hipdssData_t    solverData,
                                                hipdssMatrix_t  A)
    {
        auto       spreordering_descr = rf_descr->get_spreordering_descr();
        const auto reordering_alg     = solverConfig->get_reordering_alg();

        {
            hipdss::spreordering_alg alg;
            RETURN_IF_HIPDSS_ERROR(hipdss::hipdssReorderingAlg_t2spreordering_alg(reordering_alg,
                                                                                  &alg));
            spreordering_descr->set_alg(alg);
        }

        const auto nrows = A->get_nrows();

        switch(reordering_alg)
        {
        case HIPDSS_REORDERING_ALG_COLAMD:
        case HIPDSS_REORDERING_ALG_BTF_COLAMD:
        case HIPDSS_REORDERING_ALG_AMD:
        {
            RETURN_WITH_MESSAGE_IF_HIPDSS_ERROR(HIPDSS_STATUS_NOT_SUPPORTED,
                                                "reordering algorithm is not implemented.");
        }

        case HIPDSS_REORDERING_ALG_DEFAULT:
        {
            return HIPDSS_STATUS_SUCCESS;
        }

        case HIPDSSEXT_REORDERING_ALG_RCMK:
        case HIPDSS_REORDERING_ALG_NESTED_DISSECTION:
        {
            auto At = rf_descr->get_At();
            if(At == nullptr)
            {
                RETURN_IF_HIPDSS_ERROR(hipdss::MatrixTransposeSparsityPattern(handle,
                                                                              A,
                                                                              &At));
                rf_descr->set_At(At);
            }

            hipdss::idvec_descr_t P         = rf_descr->get_P();
            hipdss::idvec_descr_t P_inverse = rf_descr->get_P_inverse();
            if(P == nullptr || P_inverse == nullptr)
            {
                P = new hipdss::idvec_descr(handle,
                                            A->get_col_type(),
                                            A->get_index_base(),
                                            nrows,
                                            false);
                rf_descr->set_P(P);
                rf_descr->set_Q(P);

                P_inverse = new hipdss::idvec_descr(handle,
                                                    A->get_col_type(),
                                                    A->get_index_base(),
                                                    nrows,
                                                    false);
                rf_descr->set_P_inverse(P_inverse);
                rf_descr->set_Q_inverse(P_inverse);
            }

            return HIPDSS_STATUS_SUCCESS;
        }

        case HIPDSS_REORDERING_ALG_NONE:
        {
            return HIPDSS_STATUS_SUCCESS;
        }
        }

        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }

} // namespace hipdss

hipdssStatus_t hipdss::rf_reordering(hipdssHandle_t  handle,
                                     hipdss_rf_descr rf_descr,
                                     hipdssConfig_t  solverConfig,
                                     hipdssData_t    solverData,
                                     hipdssMatrix_t  A)
{

    auto spreordering_descr = rf_descr->get_spreordering_descr();

    if(spreordering_descr == nullptr)
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::spreordering_descr_create(handle,
                                                                 &spreordering_descr));
        rf_descr->set_spreordering_descr(spreordering_descr);
    }

    //
    // Prepare the needed data structures.
    //
    RETURN_IF_HIPDSS_ERROR(hipdss::rf_reordering_prepare(handle,
                                                         rf_descr,
                                                         solverConfig,
                                                         solverData,
                                                         A));
    //
    // Apply reordering.
    //
    RETURN_IF_HIPDSS_ERROR(hipdss::spreordering(handle,
                                                rf_descr->get_spreordering_descr(),
                                                hipdss::spreordering_stage_compute,
                                                A,
                                                rf_descr->get_At(),
                                                rf_descr->get_P(),
                                                rf_descr->get_P_inverse(),
                                                rf_descr->get_Q(),
                                                rf_descr->get_Q_inverse()));

    return HIPDSS_STATUS_SUCCESS;
}
