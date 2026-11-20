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

#include "hipdss_data_types.h"
#include "hipdss_idvec_descr.hpp"

namespace hipdss
{
    //
    // Reordering algorithms.
    //
    typedef enum _spreordering_alg
    {
        spreordering_alg_none,
        spreordering_alg_rcmk,
        spreordering_alg_nd_metis,
    } spreordering_alg;

    //
    // Descriptor of the reordering.
    //
    typedef struct _spreordering_descr* spreordering_descr;

    //
    // Create descriptor.
    //
    hipdssStatus_t spreordering_descr_create(hipdssHandle_t      handle,
                                             spreordering_descr* p_descr);

    //
    // Destroy descriptor.
    //
    hipdssStatus_t spreordering_descr_destroy(hipdssHandle_t     handle,
                                              spreordering_descr descr);

    //
    // Stage.
    //
    typedef enum _spreordering_stage
    {
        spreordering_stage_compute,
    } spreordering_stage;

    //
    // Explicit definition of the descriptor.
    //
    struct _spreordering_descr
    {

    protected:
        hipdss::spreordering_alg m_alg{hipdss::spreordering_alg_none};

    public:
        hipdss::spreordering_alg get_alg() const;
        void                     set_alg(hipdss::spreordering_alg);
    };

    hipdssStatus_t spreordering(hipdssHandle_t        handle,
                                spreordering_descr    descr,
                                spreordering_stage    stage,
                                hipdssMatrix_t        A,
                                hipdssMatrix_t        At,
                                hipdss::idvec_descr_t P_old2new,
                                hipdss::idvec_descr_t P_new2old,
                                hipdss::idvec_descr_t Q_old2new,
                                hipdss::idvec_descr_t Q_new2old);

    hipdssStatus_t spmat_adjgraph_create(hipdssHandle_t       handle,
                                         hipdssMatrix_t       A,
                                         hipdssMatrix_t       At,
                                         int64_t*             p_M,
                                         void**               p_G_p,
                                         void**               p_G_i,
                                         rocsparse_index_base G_base);

} // namespace hipdss
