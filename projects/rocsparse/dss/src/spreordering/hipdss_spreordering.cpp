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

#include "hipdss_spreordering.hpp"
#include "hipdss_control.hpp"
#include "hipdss_enum_utils.hpp"
#include "nd_metis/hipdss_spreordering_nd_metis.hpp"
#include "rcmk/hipdss_spreordering_rcmk.hpp"

void hipdss::_spreordering_descr::set_alg(hipdss::spreordering_alg value)
{
    this->m_alg = value;
}

hipdss::spreordering_alg hipdss::_spreordering_descr::get_alg() const
{
    return this->m_alg;
}

template <>
bool hipdss::enum_utils::is_invalid<hipdss::spreordering_alg>(hipdss::spreordering_alg value)
{
    switch(value)
    {
    case hipdss::spreordering_alg_none:
    case hipdss::spreordering_alg_rcmk:
    case hipdss::spreordering_alg_nd_metis:
    {
        return false;
    }
    }
    return true;
}

template <>
bool hipdss::enum_utils::is_invalid<hipdss::spreordering_stage>(hipdss::spreordering_stage value)
{
    switch(value)
    {
    case hipdss::spreordering_stage_compute:
    {
        return false;
    }
    }
    return true;
}

hipdssStatus_t hipdss::spreordering_descr_create(hipdssHandle_t              handle,
                                                 hipdss::spreordering_descr* p_descr)
{
    p_descr[0] = new hipdss::_spreordering_descr();
    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdss::spreordering_descr_destroy(hipdssHandle_t             handle,
                                                  hipdss::spreordering_descr descr)
{
    if(descr)
    {
        delete descr;
    }
    return HIPDSS_STATUS_SUCCESS;
}

hipdssStatus_t hipdss::spreordering(hipdssHandle_t             handle,
                                    hipdss::spreordering_descr descr,
                                    hipdss::spreordering_stage stage,
                                    hipdssMatrix_t             A,
                                    hipdssMatrix_t             At,
                                    hipdss::idvec_descr_t      P_old2new,
                                    hipdss::idvec_descr_t      P_new2old,
                                    hipdss::idvec_descr_t      Q_old2new,
                                    hipdss::idvec_descr_t      Q_new2old)
{
    switch(descr->get_alg())
    {
    case hipdss::spreordering_alg_none:
    {
        return HIPDSS_STATUS_SUCCESS;
    }

    case hipdss::spreordering_alg_rcmk:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::spreordering_rcmk(handle,
                                                         descr,
                                                         stage,
                                                         A,
                                                         At,
                                                         P_old2new,
                                                         P_new2old));
        return HIPDSS_STATUS_SUCCESS;
    }

    case hipdss::spreordering_alg_nd_metis:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::spreordering_nd_metis(handle,
                                                             descr,
                                                             stage,
                                                             A,
                                                             At,
                                                             P_old2new,
                                                             P_new2old));
        return HIPDSS_STATUS_SUCCESS;
    }
    }

    RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
}
