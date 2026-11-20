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
#include "hipdss_enums.hpp"
#include "hipdss_rf_descr.hpp"

namespace hipdss
{
    hipdssStatus_t rf_solve_diagonal(hipdssHandle_t  handle,
                                     hipdss_rf_descr rf_descr,
                                     hipdssMatrix_t  RHS,
                                     hipdssMatrix_t  X)
    {
        const auto     factorization_type = rf_descr->get_factorization_type();
        hipdssMatrix_t F                  = rf_descr->get_factorizationMatrix();
        switch(factorization_type)
        {

        case rf_factorization_type_chol:
        case rf_factorization_type_lu:
        {
            break;
        }

        case rf_factorization_type_ldl:
        {

            auto forward_sptrsm_descr = rf_descr->get_forward_sptrsm_descr();
            if(forward_sptrsm_descr == nullptr)
            {
                RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INTERNAL_ERROR);
            }

            //
            // Switch to diagonal mode.
            //
            {
                const rocsparse_solve_mode diagonal = rocsparse_solve_mode_diagonal;
                RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_set_input(*handle,
                                                                     forward_sptrsm_descr,
                                                                     rocsparse_sptrsm_input_solve_mode,
                                                                     &diagonal,
                                                                     sizeof(diagonal),
                                                                     nullptr));
            }

            RETURN_IF_HIPDSS_ERROR(hipdss::sptrsm_compute(handle,
                                                          forward_sptrsm_descr,
                                                          *F,
                                                          *RHS,
                                                          *X));

            //
            // Switch to triangular mode.
            //
            {
                const rocsparse_solve_mode diagonal = rocsparse_solve_mode_triangular;
                RETURN_IF_ROCSPARSE_ERROR(rocsparse_sptrsm_set_input(*handle,
                                                                     forward_sptrsm_descr,
                                                                     rocsparse_sptrsm_input_solve_mode,
                                                                     &diagonal,
                                                                     sizeof(diagonal),
                                                                     nullptr));
            }

            break;
        }
        }

        return HIPDSS_STATUS_SUCCESS;
    }

} // namespace hipdss
