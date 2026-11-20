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

#include "hipdss_spreordering_rcmk.hpp"
#include "hipdssContext.hpp"
#include "hipdssMatrix.hpp"
#include "hipdss_control.hpp"
#include "hipdss_idvec_descr.hpp"
#include "rocsparse_indextype_utils.hpp"
#include <rocsparse/rocsparse.h>
#include <stdlib.h>

// Global pointers used for qsort to sort strictly using the CSR degree arrays
const int* global_degrees = NULL;

template <typename J>
static int compareDegrees(const void* a,
                          const void* b)
{
    const J nodeA = *(const J*)a;
    const J nodeB = *(const J*)b;
    return (global_degrees[nodeA] > global_degrees[nodeB]) - (global_degrees[nodeA] < global_degrees[nodeB]);
}

template <typename J>
static void iperm(int64_t     size,
                  void*       new2old_,
                  const void* old2new_)
{
    auto       new2old = reinterpret_cast<J*>(new2old_);
    const auto old2new = reinterpret_cast<const J*>(old2new_);
    for(int64_t i = 0; i < size; ++i)
    {
        new2old[old2new[i]] = i;
    }
}

// Pure CSR Reverse Cuthill-McKee Implementation
template <typename I,
          typename J>
static void rcmk(int64_t     M,
                 const void* ia_,
                 const void* ja_,
                 void*       R_)
{
    const auto ia = reinterpret_cast<const I*>(ia_);
    const auto ja = reinterpret_cast<const J*>(ja_);
    auto       R  = reinterpret_cast<J*>(R_);

    // 1. Calculate vertex degrees directly from the CSR row offsets (ia)
    J* degrees = (J*)malloc(M * sizeof(J));
    for(J i = 0; i < M; i++)
    {
        // Degree = number of elements in row i (ignoring any explicit self-diagonal
        // entries if present)
        J deg = 0;
        for(I k = ia[i]; k < ia[i + 1]; k++)
        {
            if(ja[k] != i)
                deg++;
        }
        degrees[i] = deg;
    }

    bool* visited   = (bool*)calloc(M,
                                  sizeof(bool));
    J     r_index   = 0;
    J*    all_nodes = (J*)malloc(M * sizeof(J));
    for(J i = 0; i < M; i++)
    {
        all_nodes[i] = i;
    }

    // Set up globals for standard sorting
    global_degrees = degrees;
    qsort(all_nodes,
          M,
          sizeof(J),
          compareDegrees<J>);

    // 3. Allocate tracking arrays for the structural BFS queues
    J* Q         = (J*)malloc(M * sizeof(J));
    J* neighbors = (J*)malloc(M * sizeof(J));

    // 4. Execute standard CM ordering traversal loops
    for(J i = 0; i < M; i++)
    {
        J start_node = all_nodes[i];
        if(visited[start_node])
            continue;

        J head = 0, tail = 0;
        visited[start_node] = true;
        R[r_index++]        = start_node;
        Q[tail++]           = start_node;

        while(head < tail)
        {
            const J i = Q[head++];

            // Read bounds directly out of CSR row pointer slices
            const I row_start = ia[i];
            const I row_end   = ia[i + 1];

            J neighbor_count = 0;
            for(I k = row_start; k < row_end; k++)
            {
                const J j = ja[k];
                if(j != i && !visited[j])
                {
                    visited[j]                  = true;
                    neighbors[neighbor_count++] = j;
                }
            }

            // Sort discovered neighbor slices by their indexed degrees
            qsort(neighbors, neighbor_count, sizeof(J), compareDegrees<J>);

            // Commit sorted layouts into execution arrays
            for(J j = 0; j < neighbor_count; j++)
            {
                R[r_index++] = neighbors[j];
                Q[tail++]    = neighbors[j];
            }
        }
    }

    // 5. Reverse the standard Cuthill-McKee layout into strict RCM
    for(J j = 0; j < M / 2; j++)
    {
        J temp       = R[j];
        R[j]         = R[M - 1 - j];
        R[M - 1 - j] = temp;
    }

    // Cleanup local allocations
    free(degrees);
    free(visited);
    free(all_nodes);
    free(Q);
    free(neighbors);
    //    return R;
}

namespace hipdss
{
    template <typename I, typename J>
    hipdssStatus_t spmat_adjgraph_compute(int64_t           M,
                                          const void*       A_p_,
                                          const void*       A_i_,
                                          hipdssIndexBase_t A_base,
                                          const void*       At_p_,
                                          const void*       At_i_,
                                          hipdssIndexBase_t At_base,
                                          void**            p_G_p,
                                          void**            p_G_i,
                                          hipdssIndexBase_t G_base)
    {
        p_G_p[0]  = malloc(sizeof(I) * (M + 1));
        auto* G_p = reinterpret_cast<I*>(p_G_p[0]);

        const I* A_p = reinterpret_cast<const I*>(A_p_);
        const J* A_i = reinterpret_cast<const J*>(A_i_);

        {
            const I nnz = A_p[M];
            std::cout << "nnz " << nnz << std::endl;
            for(int i = 0; i < nnz; ++i)
            {
                if(A_i[i] < 0 || A_i[i] >= M)
                {
                    std::cout << "----- out of bounds A_i" << std::endl;
                    exit(1);
                }
            }
        }

        const I* At_p = reinterpret_cast<const I*>(At_p_);
        const J* At_i = reinterpret_cast<const J*>(At_i_);

        {

            const I nnz = A_p[M];
            if(At_p[M] != A_p[M])
            {
                std::cout << "A_p[M] " << A_p[M] << std::endl;
                std::cout << "At_p[M] " << At_p[M] << std::endl;
                std::cout << "----- out of bounds At_p" << std::endl;
                exit(1);
            }

            for(int i = 0; i < nnz; ++i)
            {
                if(At_i[i] < 0 || At_i[i] >= M)
                {
                    std::cout << "----- out of bounds At_i" << std::endl;
                    exit(1);
                }
            }
        }

        J*      marker = (J*)calloc(M, sizeof(J));
        int64_t mx     = 0;
        for(int64_t i = 0; i < M; ++i)
        {
            mx = std::max(mx, int64_t((A_p[i + 1] - A_p[i]) + (At_p[i + 1] - At_p[i])));
        }
        mx = std::min(mx, static_cast<int64_t>(M));

        auto* select = (J*)malloc(sizeof(J) * mx);

        G_p[0] = G_base;
        for(J i = 0; i < M; ++i)
        {
            J select_n = 0;
            for(I k = A_p[i] - A_base; k < A_p[i + 1] - A_base; ++k)
            {
                const J j = A_i[k] - A_base;
                if(i != j)
                {
                    if(marker[j] == 0)
                    {
                        select[select_n] = j;
                        marker[j]        = ++select_n;
                    }
                }
            }

            for(I k = At_p[i] - At_base; k < At_p[i + 1] - At_base; ++k)
            {
                const J j = At_i[k] - At_base;
                if(i != j)
                {
                    if(marker[j] == 0)
                    {
                        select[select_n] = j;
                        marker[j]        = ++select_n;
                    }
                }
            }

            G_p[i + 1] = G_p[i] + select_n;
            for(J k = 0; k < select_n; ++k)
            {
                marker[select[k]] = 0;
            }
        }
        free(select);
        free(marker);

        marker   = (J*)calloc(M, sizeof(J));
        select   = (J*)malloc(sizeof(J) * mx);
        p_G_i[0] = malloc(sizeof(J) * (G_p[M] - G_base));

        auto* G_i = reinterpret_cast<J*>(p_G_i[0]);
        for(J i = 0; i < M; ++i)
        {
            J select_n = 0;
            for(I k = A_p[i] - A_base; k < A_p[i + 1] - A_base; ++k)
            {
                const J j = A_i[k] - A_base;
                if(i != j)
                {
                    if(marker[j] == 0)
                    {
                        select[select_n] = j;
                        marker[j]        = ++select_n;
                    }
                }
            }

            for(I k = At_p[i] - At_base; k < At_p[i + 1] - At_base; ++k)
            {
                const J j = At_i[k] - At_base;
                if(i != j)
                {
                    if(marker[j] == 0)
                    {
                        select[select_n] = j;
                        marker[j]        = ++select_n;
                    }
                }
            }

            std::sort(select, select + select_n);
            for(J k = 0; k < select_n; ++k)
                G_i[G_p[i] - G_base + k] = select[k] + G_base;
            for(J k = 0; k < select_n; ++k)
                marker[select[k]] = 0;
        }
        free(select);
        free(marker);
        return HIPDSS_STATUS_SUCCESS;
    }

    hipdssStatus_t spmat_adjgraph_create(hipdssHandle_t    handle,
                                         hipdssMatrix_t    A,
                                         hipdssMatrix_t    At,
                                         int64_t*          p_M,
                                         void**            p_G_p,
                                         void**            p_G_i,
                                         hipdssIndexBase_t G_base)
    {
        auto apply = spmat_adjgraph_compute<int32_t, int32_t>;

        RETURN_IF_HIPDSS_ERROR(apply(A->get_nrows(),
                                     A->get_const_row_host_data(),
                                     A->get_const_col_host_data(),
                                     A->get_index_base(),
                                     At->get_const_row_host_data(),
                                     At->get_const_col_host_data(),
                                     At->get_index_base(),
                                     p_G_p,
                                     p_G_i,
                                     G_base));

        p_M[0] = A->get_nrows();
        return HIPDSS_STATUS_SUCCESS;
    }

} // namespace hipdss

hipdssStatus_t hipdss::spreordering_rcmk(hipdssHandle_t             handle,
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
    std::cout << "-- spmat_adjgraph_create " << std::endl;
    RETURN_IF_HIPDSS_ERROR(hipdss::spmat_adjgraph_create(handle,
                                                         A,
                                                         At,
                                                         &M,
                                                         &gptr,
                                                         &gind,
                                                         HIPDSS_BASE_ZERO));
    std::cout << "-- spmat_adjgraph_create done" << std::endl;
    //
    // Select the right template.
    //
    auto apply_rcmk  = rcmk<int32_t, int32_t>;
    auto apply_iperm = iperm<int32_t>;
    if(new2old->get_indextype() == HIP_R_64I)
    {
        apply_iperm = iperm<int64_t>;
    }

    apply_rcmk(M,
               gptr,
               gind,
               old2new->host_data());

    apply_iperm(M,
                new2old->host_data(),
                old2new->host_data());

    return HIPDSS_STATUS_SUCCESS;
}
