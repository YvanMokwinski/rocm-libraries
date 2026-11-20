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

#include "hipdss_rf_host_symbolic_factor_calculation.hpp"
#include "hipdssContext.hpp"
#include "hipdssMatrix.hpp"
#include "hipdss_control.hpp"
#include "hipdss_datatype_utils.hpp"
#include "hipdss_enums.hpp"
#include "hipdss_working_row_t.hpp"
#include <memory>
#include <rocsparse/rocsparse-auxiliary.h>
#include <vector>

namespace hipdss
{
    // Upper bound on nnz(L + U) of the permuted matrix B: 2 nnz(chol(B + B^T)) - M.
    template <typename I, typename J, typename PJ>
    static void csr_lu_nnz_bound(int64_t           M,
                                 const void*       Ap_,
                                 const void*       Ap_end_,
                                 const void*       Ai_,
                                 hipdssIndexBase_t Abase,
                                 const void*       new2old_row_perm_,
                                 hipdssIndexBase_t new2old_row_perm_base,
                                 const void*       old2new_col_perm_,
                                 hipdssIndexBase_t old2new_col_perm_base,
                                 int64_t*          bound)
    {

        auto Ap               = reinterpret_cast<const I*>(Ap_);
        auto Ap_end           = (Ap_end_) ? reinterpret_cast<const I*>(Ap_end_) : (Ap + 1);
        auto Ai               = reinterpret_cast<const J*>(Ai_);
        auto new2old_row_perm = reinterpret_cast<const PJ*>(new2old_row_perm_);
        auto old2new_col_perm = reinterpret_cast<const PJ*>(old2new_col_perm_);

        const auto for_each_in_row = [&](int64_t r, auto&& f) {
            const int64_t i
                = (new2old_row_perm != nullptr) ? (new2old_row_perm[r] - new2old_row_perm_base) : r;
            for(auto k = Ap[i] - Abase; k < Ap_end[i] - Abase; ++k)
            {
                const auto j = Ai[k] - Abase;
                f((old2new_col_perm != nullptr) ? (old2new_col_perm[j] - old2new_col_perm_base)
                                                : j);
            }
        };

        // Strictly lower pattern of B + B^T, stored by rows (duplicates allowed).
        std::vector<int64_t> lower_ptr(M + 1, 0);
        for(int64_t r = 0; r < M; ++r)
        {
            for_each_in_row(r, [&](int64_t s) {
                if(s != r)
                {
                    ++lower_ptr[std::max(r, s) + 1];
                }
            });
        }

        for(int64_t r = 0; r < M; ++r)
        {
            lower_ptr[r + 1] += lower_ptr[r];
        }

        std::vector<int64_t> lower_ind(lower_ptr[M]);
        std::vector<int64_t> fill(lower_ptr.begin(), lower_ptr.end() - 1);
        for(int64_t r = 0; r < M; ++r)
        {
            for_each_in_row(r, [&](int64_t s) {
                if(s != r)
                {
                    lower_ind[fill[std::max(r, s)]++] = std::min(r, s);
                }
            });
        }

        std::vector<int64_t> parent(M, -1);
        {
            std::vector<int64_t> ancestor(M, -1);
            for(int64_t i = 0; i < M; ++i)
            {
                for(int64_t k = lower_ptr[i]; k < lower_ptr[i + 1]; ++k)
                {
                    int64_t j = lower_ind[k];
                    while(j != -1 && j < i)
                    {
                        const int64_t next = ancestor[j];
                        ancestor[j]        = i;
                        if(next == -1)
                        {
                            parent[j] = i;
                        }
                        j = next;
                    }
                }
            }
        }

        // Row counts: row i of L is the row subtree reached from the entries of row i.
        std::vector<int64_t> marker(M, -1);
        int64_t              nnz_L = M;
        for(int64_t i = 0; i < M; ++i)
        {
            marker[i] = i;
            for(int64_t k = lower_ptr[i]; k < lower_ptr[i + 1]; ++k)
            {
                for(int64_t j = lower_ind[k]; marker[j] != i; j = parent[j])
                {
                    marker[j] = i;
                    ++nnz_L;
                }
            }
        }

        bound[0] = 2 * nnz_L - M;
    }

    typedef void (*csr_lu_nnz_bound_t)(int64_t,
                                       const void*,
                                       const void*,
                                       const void*,
                                       hipdssIndexBase_t,
                                       const void*,
                                       hipdssIndexBase_t,
                                       const void*,
                                       hipdssIndexBase_t,
                                       int64_t*);

    template <typename I, typename J>
    static hipdssStatus_t find_csr_lu_nnz_bound_P(csr_lu_nnz_bound_t* p_that,
                                                  const hipDataType   P)
    {
        switch(P)
        {

        case HIP_R_32I:
        {
            p_that[0] = hipdss::csr_lu_nnz_bound<I, J, int32_t>;
            return HIPDSS_STATUS_SUCCESS;
        }

        case HIP_R_64I:
        {
            p_that[0] = hipdss::csr_lu_nnz_bound<I, J, int64_t>;
            return HIPDSS_STATUS_SUCCESS;
        }

        default:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
        }
    }

    template <typename I>
    static hipdssStatus_t find_csr_lu_nnz_bound_J(csr_lu_nnz_bound_t* p_that,
                                                  const hipDataType   J,
                                                  const hipDataType   P)
    {
        switch(J)
        {

        case HIP_R_32I:
        {
            RETURN_IF_HIPDSS_ERROR((hipdss::find_csr_lu_nnz_bound_P<I, int32_t>)(p_that,
                                                                                 P));

            return HIPDSS_STATUS_SUCCESS;
        }

        case HIP_R_64I:
        {
            RETURN_IF_HIPDSS_ERROR((hipdss::find_csr_lu_nnz_bound_P<I, int64_t>)(p_that,
                                                                                 P));
            return HIPDSS_STATUS_SUCCESS;
        }

        default:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
        }
    }

    static hipdssStatus_t find_csr_lu_nnz_bound(csr_lu_nnz_bound_t* p_that,
                                                const hipDataType   I,
                                                const hipDataType   J,
                                                const hipDataType   P)
    {
        switch(I)
        {

        case HIP_R_32I:
        {
            RETURN_IF_HIPDSS_ERROR(hipdss::find_csr_lu_nnz_bound_J<int32_t>(p_that,
                                                                            J,
                                                                            P));

            return HIPDSS_STATUS_SUCCESS;
        }

        case HIP_R_64I:
        {
            RETURN_IF_HIPDSS_ERROR(hipdss::find_csr_lu_nnz_bound_J<int64_t>(p_that,
                                                                            J,
                                                                            P));
            return HIPDSS_STATUS_SUCCESS;
        }

        default:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
        }
    }

}

hipdssStatus_t hipdss::csr_lu_nnz_bound(hipdssHandle_t        handle,
                                        hipdssMatrix_t        inputMatrix,
                                        hipdss::idvec_descr_t P,
                                        hipdss::idvec_descr_t Qinv,
                                        int64_t*              p_nnz)
{

    csr_lu_nnz_bound_t apply{};

    RETURN_IF_HIPDSS_ERROR(find_csr_lu_nnz_bound(&apply,
                                                 inputMatrix->get_row_type(),
                                                 inputMatrix->get_col_type(),
                                                 (P)
                                                     ? P->get_indextype()
                                                     : HIP_R_32I));

    apply(inputMatrix->get_nrows(),
          inputMatrix->get_const_row_host_data(),
          inputMatrix->get_const_row_end_host_data(),
          inputMatrix->get_const_col_host_data(),
          inputMatrix->get_index_base(),
          (P) ? P->const_host_data() : nullptr,
          (P) ? P->get_base() : HIPDSS_BASE_ZERO,
          (Qinv) ? Qinv->const_host_data() : nullptr,
          (Qinv) ? Qinv->get_base() : HIPDSS_BASE_ZERO,
          p_nnz);

    return HIPDSS_STATUS_SUCCESS;
}

namespace hipdss
{
    template <typename I, typename J, typename PJ>
    void csr_calculate_bands(int64_t nrows,
                             const I* __restrict__ ptr_begin_,
                             const I* __restrict__ ptr_end_,
                             const J* __restrict__ ind,
                             hipdssIndexBase_t base,
                             const PJ*         new2old_row_perm,
                             hipdssIndexBase_t new2old_row_perm_base,
                             const PJ*         old2new_col_perm,
                             hipdssIndexBase_t old2new_col_perm_base,
                             int64_t* __restrict__ bandl,
                             int64_t* __restrict__ bandu)
    {
        bandl[0] = 0;
        bandu[0] = 0;
        for(int64_t row = 0; row < nrows; ++row)
        {
            const int64_t i = (new2old_row_perm != nullptr)
                                  ? (new2old_row_perm[row] - new2old_row_perm_base)
                                  : row;

            for(auto k = ptr_begin_[i] - base; k < ptr_end_[i] - base; ++k)
            {
                const auto    j   = ind[k] - base;
                const int64_t col = (old2new_col_perm != nullptr)
                                        ? (old2new_col_perm[j] - old2new_col_perm_base)
                                        : j;
                bandu[0]          = std::max(bandu[0], (col > row) ? int64_t(col - row) : 0);
                bandl[0]          = std::max(bandl[0], (col < row) ? int64_t(row - col) : 0);
            }
        }
    }

    template <typename AI,
              typename AJ,
              typename FI,
              typename FJ>
    hipdssStatus_t host_symbolic_factor_calculation_general(int64_t M,
                                                            const void* __restrict__ Ap_,
                                                            const void* __restrict__ Ap_end_,
                                                            const void* __restrict__ Ai_,
                                                            hipdssIndexBase_t Abase,
                                                            void* __restrict__ Fp_,
                                                            void* __restrict__ Fi_,
                                                            hipdssIndexBase_t Fbase,
                                                            void* __restrict__ A2F_,
                                                            hipdssIndexBase_t A2Fbase,
                                                            const void* __restrict__ new2old_row_perm_,
                                                            hipdssIndexBase_t new2old_row_perm_base,
                                                            const void* __restrict__ old2new_col_perm_,
                                                            hipdssIndexBase_t old2new_col_perm_base,
                                                            int64_t*          F_nnz)
    {
        const AI* __restrict__ Ap     = reinterpret_cast<const AI* __restrict__>(Ap_);
        const AI* __restrict__ Ap_end = (Ap_end_) ? reinterpret_cast<const AI* __restrict__>(Ap_end_) : (Ap + 1);
        const AJ* __restrict__ Ai     = reinterpret_cast<const AJ* __restrict__>(Ai_);
        FI* __restrict__ Fp           = reinterpret_cast<FI* __restrict__>(Fp_);
        FJ* __restrict__ Fi           = reinterpret_cast<FJ* __restrict__>(Fi_);
        FI* __restrict__ A2F          = reinterpret_cast<FI* __restrict__>(A2F_);
        const FJ* new2old_row_perm    = reinterpret_cast<const FJ* __restrict__>(new2old_row_perm_);
        const FJ* old2new_col_perm    = reinterpret_cast<const FJ* __restrict__>(old2new_col_perm_);

        int64_t bandl = 0;
        int64_t bandu = 0;

        Fp[0] = 0;

#if 0


    std::cout << "  M: "
	      << M
	      << std::endl;

    std::cout << "  original bandl: "
	      << bandl
	      << std::endl;

    std::cout << "  original bandu: "
	      << bandu
	      << std::endl;

    std::cout << "  original band density: "
	      << (double(M + (((bandl + 1) * bandl) / 2 - bandl) + (M - bandl) * bandl + (((bandu + 1) * bandu) / 2 - bandu) + (M - bandu) * bandu) / double(M)) / double(M)
	      << std::endl;

#endif

        hipdss::csr_calculate_bands<AI, AJ>(M,
                                            Ap,
                                            Ap_end,
                                            Ai,
                                            Abase,
                                            new2old_row_perm,
                                            new2old_row_perm_base,
                                            old2new_col_perm,
                                            old2new_col_perm_base,
                                            &bandl,
                                            &bandu);

        std::cout << "  permuted bandl: "
                  << bandl
                  << std::endl;

        std::cout << "  permuted bandu: "
                  << bandu
                  << std::endl;

        std::cout << "  permuted band density: "
                  << (double(M + (((bandl + 1) * bandl) / 2 - bandl) + (M - bandl) * bandl + (((bandu + 1) * bandu) / 2 - bandu) + (M - bandu) * bandu) / double(M)) / double(M)
                  << std::endl;

        const int64_t max_size_per_row = std::min(M,
                                                  int64_t(bandu) + bandl + 1);

        hipdss::working_row_t<FJ> working_row(M,
                                              max_size_per_row);

        for(int64_t row = 0; row < M; ++row)
        {
            const AJ row_A = (new2old_row_perm)
                                 ? (new2old_row_perm[row] - new2old_row_perm_base)
                                 : row;

            //
            // Load
            //
            for(auto k = Ap[row_A] - Abase; k < Ap_end[row_A] - Abase; ++k)
            {
                const auto j   = Ai[k] - Abase;
                const auto col = (old2new_col_perm)
                                     ? (old2new_col_perm[j] - old2new_col_perm_base)
                                     : j;

                working_row.add(col);
            }

            //
            // (i,j)->(r = perm[i],s = perm[j])
            // (r,s)->(i = iperm[r],j = iperm[s])
            //

            //
            // Sparse accumulation
            //
            for(int64_t i = 0; i < working_row.size(); ++i)
            {
                const auto col = working_row.get(i);
                if(col < row)
                {
                    for(auto k = Fp[col]; k < Fp[col + 1]; ++k)
                    {
                        const auto col2 = Fi[k] - Fbase;
                        if(col2 >= col)
                        {
                            working_row.add(col2);
                        }
                    }
                }
            }

            if(working_row.has(row) == false)
            {
                std::cout << "missing diagonal " << row << std::endl;
            }

#if 0
	if(Fp[row] + working_row.size() > nnz)
	  {
	    nnz += 10 * M;
	    std::cout << "enlarge nnz " << int64_t(M) * M << std::endl;
	    std::cout << "enlarge nnz " << nnz << std::endl;
	    p_Fi[0] = (FJ*)realloc(p_Fi[0], nnz * sizeof(FI));
	    Fi      = p_Fi[0];
	  }
#endif
            //
            // Sort selection.
            //
            working_row.sort();

            //
            // Offload select.
            //
            const auto n = working_row.size();
            for(FJ i = 0; i < n; ++i)
            {
                const auto s  = working_row.get(i);
                const auto at = Fp[row] + i;

                Fi[at] = s + Fbase;

                for(AI k = Ap[row_A] - Abase; k < Ap_end[row_A] - Abase; ++k)
                {
                    const AJ j   = Ai[k] - Abase;
                    const AJ col = (old2new_col_perm)
                                       ? (old2new_col_perm[j] - old2new_col_perm_base)
                                       : j;

                    if(col == s)
                    {
                        A2F[k] = at + A2Fbase;
                        break;
                    }
                }
            }
            Fp[row + 1] = Fp[row] + n;

            //
            // Reset table.
            //
            working_row.reset();
        }

        if(Fbase == HIPDSS_BASE_ONE)
        {
            for(FJ row = 0; row <= M; ++row)
            {
                Fp[row] += Fbase;
            }
        }

#if 0
    if((Fp[M] - Fp[0]) < nnz)
      {
	//
	// Adjust.
	//
	p_Fi[0] = (FJ*)realloc(p_Fi[0], (Fp[M] - Fp[0]) * sizeof(FI));
      }

    std::cout << "nnz_F: " << (Fp[M] - Fp[0]) << std::endl;
    std::cout << "density factorization "
	      << (double(Fp[M] - Fp[0]) / double(M)) / double(M) << std::endl;
#endif

        F_nnz[0] = Fp[M] - Fp[0];
        return HIPDSS_STATUS_SUCCESS;
    }

    template <typename AI,
              typename AJ,
              typename FI,
              typename FJ>
    hipdssStatus_t host_symbolic_factor_calculation_symmetric(int64_t M,
                                                              const void* __restrict__ Ap_,
                                                              const void* __restrict__ Ap_end_,
                                                              const void* __restrict__ Ai_,
                                                              hipdssIndexBase_t Abase,
                                                              void* __restrict__ Fp_,
                                                              void* __restrict__ Fi_,
                                                              hipdssIndexBase_t Fbase,
                                                              void* __restrict__ A2F_,
                                                              hipdssIndexBase_t A2Fbase,
                                                              const void* __restrict__ new2old_row_perm_,
                                                              hipdssIndexBase_t new2old_row_perm_base,
                                                              const void* __restrict__ old2new_col_perm_,
                                                              hipdssIndexBase_t old2new_col_perm_base,
                                                              int64_t*          F_nnz)
    {
        auto Ap     = reinterpret_cast<const AI* __restrict__>(Ap_);
        auto Ap_end = (Ap_end_) ? reinterpret_cast<const AI* __restrict__>(Ap_end_) : (Ap + 1);
        auto Ai     = reinterpret_cast<const AJ* __restrict__>(Ai_);
        auto Fp     = reinterpret_cast<FI* __restrict__>(Fp_);
        auto Fi     = reinterpret_cast<FJ* __restrict__>(Fi_);
        auto A2F    = reinterpret_cast<FI* __restrict__>(A2F_);

        //
        // Strictly lower pattern C of P A P^T, stored by rows.
        //
        std::vector<FI> Cp(M + 1, 0);
        for(int64_t i = 0; i < M; ++i)
        {
            for(auto k = Ap[i] - Abase; k < Ap_end[i] - Abase; ++k)
            {
                const auto c = Ai[k] - Abase;
                if(i != c)
                {
                    ++Cp[std::max(i, int64_t(c)) + 1];
                }
            }
        }

        for(int64_t i = 0; i < M; ++i)
        {
            Cp[i + 1] += Cp[i];
        }

        std::vector<FJ> Ci(Cp[M]);
        {
            std::vector<FI> pos(Cp.begin(), Cp.end() - 1);
            for(int64_t i = 0; i < M; ++i)
            {
                for(auto k = Ap[i] - Abase; k < Ap_end[i] - Abase; ++k)
                {
                    const auto c = Ai[k] - Abase;
                    if(i != c)
                    {
                        Ci[pos[std::max(i, int64_t(c))]++] = std::min(i, int64_t(c));
                    }
                }
            }
        }

        //
        // Elimination tree (Liu's algorithm with path compression).
        //
        std::vector<FJ> parent(M, FJ(-1));
        {
            std::vector<FJ> ancestor(M, FJ(-1));
            for(int64_t i = 0; i < M; ++i)
            {
                for(auto k = Cp[i]; k < Cp[i + 1]; ++k)
                {

                    for(FJ j = Ci[k], next; j != -1 && j < i; j = next)
                    {
                        next        = ancestor[j];
                        ancestor[j] = i;
                        if(next == -1)
                        {
                            parent[j] = i;
                        }
                    }
                }
            }
        }

        //
        // Row i of L is the row subtree: union of etree paths from each C(i,j) up to i.
        //
        std::vector<FJ> vFi;
        vFi.reserve(Cp[M] + M);
        std::vector<FJ> mark(M, FJ(-1));
        Fp[0] = 0;
        for(int64_t i = 0; i < M; ++i)
        {
            const size_t start = vFi.size();
            mark[i]            = i;
            for(auto k = Cp[i]; k < Cp[i + 1]; ++k)
            {
                for(auto j = Ci[k]; mark[j] != i; j = parent[j])
                {
                    vFi.push_back(j);
                    mark[j] = i;
                }
            }
            vFi.push_back(i);
            std::sort(vFi.begin() + start, vFi.end());
            Fp[i + 1] = static_cast<FI>(vFi.size());
        }

        //
        // A2F: entry (r, c) of P A P^T is stored at L(max(r,c), min(r,c)).
        //
        for(int64_t i = 0; i < M; ++i)
        {
            for(auto k = Ap[i] - Abase; k < Ap_end[i] - Abase; ++k)
            {
                const auto c     = Ai[k] - Abase;
                const auto lrow  = std::max(i, int64_t(c));
                const auto lcol  = std::min(i, int64_t(c));
                const auto first = vFi.begin() + Fp[lrow];
                const auto it    = std::lower_bound(first, vFi.begin() + Fp[lrow + 1], lcol);
                A2F[k]           = static_cast<FI>(it - vFi.begin()) + A2Fbase;
            }
        }

        const FI nnz_F = Fp[M];
        for(FI k = 0; k < nnz_F; ++k)
        {
            Fi[k] = vFi[k] + Fbase;
        }

        if(Fbase == HIPDSS_BASE_ONE)
        {
            for(int64_t row = 0; row <= M; ++row)
            {
                Fp[row] += Fbase;
            }
        }
        F_nnz[0] = nnz_F;
        return HIPDSS_STATUS_SUCCESS;
    }
}

namespace hipdss
{

    template <typename AI,
              typename AJ,
              typename FI>
    static hipdssStatus_t find_host_symbolic_factor_calculation_FJ(hipdss::host_symbolic_factor_calculation_t* p_that,
                                                                   bool                                        symmetric,
                                                                   hipDataType                                 FJ)
    {
        switch(FJ)
        {

        case HIP_R_32I:
        {

            p_that[0] = (symmetric)
                            ? hipdss::host_symbolic_factor_calculation_symmetric<AI, AJ, FI, int32_t>
                            : hipdss::host_symbolic_factor_calculation_general<AI, AJ, FI, int32_t>;

            return HIPDSS_STATUS_SUCCESS;
        }

        case HIP_R_64I:
        {

            p_that[0] = (symmetric)
                            ? hipdss::host_symbolic_factor_calculation_symmetric<AI, AJ, FI, int64_t>
                            : hipdss::host_symbolic_factor_calculation_general<AI, AJ, FI, int64_t>;

            return HIPDSS_STATUS_SUCCESS;
        }

        default:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
        }
    }

    template <typename AI,
              typename AJ>
    static hipdssStatus_t find_host_symbolic_factor_calculation_FI(hipdss::host_symbolic_factor_calculation_t* p_that,
                                                                   bool                                        symmetric,
                                                                   hipDataType                                 FI,
                                                                   hipDataType                                 FJ)
    {
        switch(FI)
        {

        case HIP_R_32I:
        {
            RETURN_IF_HIPDSS_ERROR((hipdss::find_host_symbolic_factor_calculation_FJ<AI, AJ, int32_t>)(p_that,
                                                                                                       symmetric,
                                                                                                       FJ));
            return HIPDSS_STATUS_SUCCESS;
        }

        case HIP_R_64I:
        {
            RETURN_IF_HIPDSS_ERROR((hipdss::find_host_symbolic_factor_calculation_FJ<AI, AJ, int64_t>)(p_that,
                                                                                                       symmetric,
                                                                                                       FJ));
            return HIPDSS_STATUS_SUCCESS;
        }

        default:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
        }
    }

    template <typename AI>
    static hipdssStatus_t find_host_symbolic_factor_calculation_AJ(hipdss::host_symbolic_factor_calculation_t* p_that,
                                                                   bool                                        symmetric,
                                                                   hipDataType                                 AJ,
                                                                   hipDataType                                 FI,
                                                                   hipDataType                                 FJ)
    {
        switch(AJ)
        {

        case HIP_R_32I:
        {
            RETURN_IF_HIPDSS_ERROR((hipdss::find_host_symbolic_factor_calculation_FI<AI, int32_t>)(p_that,
                                                                                                   symmetric,
                                                                                                   FI,
                                                                                                   FJ));
            return HIPDSS_STATUS_SUCCESS;
        }
        case HIP_R_64I:
        {
            RETURN_IF_HIPDSS_ERROR((hipdss::find_host_symbolic_factor_calculation_FI<AI, int64_t>)(p_that,
                                                                                                   symmetric,
                                                                                                   FI,
                                                                                                   FJ));
            return HIPDSS_STATUS_SUCCESS;
        }
        default:
        {
            RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
        }
        }
    }
}

hipdssStatus_t hipdss::find_host_symbolic_factor_calculation(hipdss::host_symbolic_factor_calculation_t* p_that,
                                                             bool                                        symmetric,
                                                             hipDataType                                 AI,
                                                             hipDataType                                 AJ,
                                                             hipDataType                                 FI,
                                                             hipDataType                                 FJ)
{
    switch(AI)
    {

    case HIP_R_32I:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::find_host_symbolic_factor_calculation_AJ<int32_t>(p_that,
                                                                                         symmetric,
                                                                                         AJ,
                                                                                         FI,
                                                                                         FJ));

        return HIPDSS_STATUS_SUCCESS;
    }

    case HIP_R_64I:
    {
        RETURN_IF_HIPDSS_ERROR(hipdss::find_host_symbolic_factor_calculation_AJ<int64_t>(p_that,
                                                                                         symmetric,
                                                                                         AJ,
                                                                                         FI,
                                                                                         FJ));
        return HIPDSS_STATUS_SUCCESS;
    }

    default:
    {
        RETURN_IF_HIPDSS_ERROR(HIPDSS_STATUS_INVALID_VALUE);
    }
    }
}
