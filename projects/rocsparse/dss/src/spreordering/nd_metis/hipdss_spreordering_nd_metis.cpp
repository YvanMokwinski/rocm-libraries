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

#include "hipdss_spreordering_nd_metis.hpp"
#include "hipdss_control.hpp"
#include "hipdss_idvec_descr.hpp"

#include "/home/minkowski/ORNL/SuiteSparse/CHOLMOD/SuiteSparse_metis/include/original/metis.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "hipdssContext.hpp"

hipdssStatus_t hipdss::spreordering_nd_metis(hipdssHandle_t             handle,
                                             hipdss::spreordering_descr descr,
                                             hipdss::spreordering_stage stage,
                                             hipdssMatrix_t             A,
                                             hipdssMatrix_t             At,
                                             hipdss::idvec_descr_t      old2new,
                                             hipdss::idvec_descr_t      new2old)
{

    //
    // Create the adjacent graph.
    //
    int64_t M{};
    void*   gptr{};
    void*   gind{};
    RETURN_IF_HIPDSS_ERROR(hipdss::spmat_adjgraph_create(handle,
                                                         A,
                                                         At,
                                                         &M,
                                                         &gptr,
                                                         &gind,
                                                         rocsparse_index_base_zero));

    int32_t options[METIS_NOPTIONS];
    METIS_SetDefaultOptions(options);

    RETURN_IF_HIPDSS_ERROR(old2new->set_host_identity(handle));
    RETURN_IF_HIPDSS_ERROR(new2old->set_host_identity(handle));

    int32_t METIS_M = M;
    auto    result  = METIS_NodeND(&METIS_M,
                               (int32_t*)(gptr),
                               (int32_t*)(gind),
                               NULL,
                               options,
                               reinterpret_cast<int32_t*>(old2new->host_data()),
                               reinterpret_cast<int32_t*>(new2old->host_data()));

    RETURN_IF_HIPDSS_ERROR((result != METIS_OK) ? HIPDSS_STATUS_INTERNAL_ERROR
                                                : HIPDSS_STATUS_SUCCESS);
    free(gptr);
    free(gind);

    return HIPDSS_STATUS_SUCCESS;
}
