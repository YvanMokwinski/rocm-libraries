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

#if 0

#include "rocdss_spmat_import.hpp"


HIPDSS_EXPORT
extern "C" hipdssStatus_t rocdss_spmat_info(hipdssHandle_t            handle,
                                            rocsparse_const_spmat_descr A,
                                            FILE*                       out,
                                            rocsparse_error*            p_error)
{
    int64_t              rows{};
    int64_t              cols{};
    int64_t              nnz{};
    const void*          csr_row_ptr{};
    const void*          csr_col_ind{};
    const void*          csr_val{};
    rocsparse_indextype  row_ptr_type{};
    rocsparse_indextype  col_ind_type{};
    rocsparse_index_base idx_base{};
    rocsparse_datatype   data_type{};
    RETURN_IF_ROCSPARSE_ERROR(rocsparse_const_csr_get(
        A, &rows, &cols, &nnz, &csr_row_ptr, &csr_col_ind, &csr_val, &row_ptr_type, &col_ind_type, &idx_base, &data_type));

    fprintf(out, "m        : %ld\n", rows);
    fprintf(out, "n        : %ld\n", cols);
    fprintf(out, "nnz      : %ld\n", nnz);
    fprintf(out, "base     : %d\n", idx_base);
    fprintf(out, "ptr_type : %d\n", row_ptr_type);
    fprintf(out, "ind_type : %d\n", col_ind_type);
    fprintf(out, "val_type : %d\n", data_type);

    {
        size_t val_size_in_bytes = sizeof(double) * nnz;
        size_t ind_size_in_bytes = sizeof(int32_t) * nnz;
        size_t ptr_size_in_bytes = sizeof(int32_t) * (rows + 1);
        void * ptr, *ind, *val;
        ptr = malloc(ptr_size_in_bytes);
        RETURN_IF_HIP_ERROR(
            hipMemcpy(ptr, csr_row_ptr, ptr_size_in_bytes, hipMemcpyDeviceToHost));
        ind = malloc(ind_size_in_bytes);
        RETURN_IF_HIP_ERROR(
            hipMemcpy(ind, csr_col_ind, ind_size_in_bytes, hipMemcpyDeviceToHost));
        val = malloc(val_size_in_bytes);
        RETURN_IF_HIP_ERROR(
            hipMemcpy(val, csr_val, val_size_in_bytes, hipMemcpyDeviceToHost));

        int32_t* p             = reinterpret_cast<int32_t*>(ptr);
        int32_t* c             = reinterpret_cast<int32_t*>(ind);
        double*  v             = reinterpret_cast<double*>(val);
        int32_t  mx_per_row    = 0;
        int32_t  min_per_row   = cols;
        int32_t* median_sample = (int32_t*)malloc(rows * sizeof(int32_t));
        for(int32_t i = 0; i < rows; ++i)
        {
            const int32_t n_per_row = p[i + 1] - p[i];
            mx_per_row              = std::max(mx_per_row, n_per_row);
            min_per_row             = std::min(min_per_row, n_per_row);
            median_sample[i]        = n_per_row;
        }
        std::sort(median_sample, median_sample + rows);
        int32_t L_nnz  = 0;
        int32_t U_nnz  = 0;
        int32_t D_nnz  = 0;
        int32_t L_band = 0;
        int32_t U_band = 0;
        for(int32_t i = 0; i < rows; ++i)
        {
            for(int32_t k = p[i] - idx_base; k < p[i + 1] - idx_base; ++k)
            {
                int32_t j = c[k];
                if(i > j)
                {
                    ++L_nnz;
                    L_band = std::max(i - j, L_band);
                }
                else if(i < j)
                {
                    ++U_nnz;
                    U_band = std::max(j - i, U_band);
                }
                else
                {
                    ++D_nnz;
                }
            }
        }

        std::cout << "nnz_L    : " << L_nnz << std::endl;
        std::cout << "nnz_D    : " << D_nnz << std::endl;
        std::cout << "nnz_U    : " << U_nnz << std::endl;
        std::cout << "band_L   : " << L_band << std::endl;
        std::cout << "band_U   : " << U_band << std::endl;

        bool is_sym     = true;
        bool is_sym_val = true;
        for(int32_t i = 0; i < rows; ++i)
        {
            for(auto k = p[i] - idx_base; k < p[i + 1] - idx_base; ++k)
            {
                auto j = c[k] = idx_base;
                bool found    = false;
                bool same     = true;
                //	    std::cout << v[k] << std::endl;
                for(auto k2 = p[j] - idx_base; k2 < p[j + 1] - idx_base; ++k2)
                {
                    const auto s = c[k2] - idx_base;
                    if(s == i)
                    {
                        found = true;
                        if(v[k2] != v[k])
                        {
                            same = false;
                        }
                        break;
                    }
                }
                if(!found)
                {
                    is_sym     = false;
                    is_sym_val = false;
                    break;
                }
                else if(!same)
                    is_sym_val = false;
            }
        }

        std::cout << "min_nnz_per_row     " << min_per_row << std::endl;
        std::cout << "max_nnz_per_row     " << mx_per_row << std::endl;
        std::cout << "mean_nnz_per_row    " << nnz / rows << std::endl;
        if(rows % 2 != 0)
        {
            std::cout << "median_nnz_per_row  " << median_sample[rows / 2]
                      << std::endl;
        }
        else
        {
            std::cout << "median_nnz_per_row  "
                      << (median_sample[rows / 2] + median_sample[rows / 2 + 1]) / 2
                      << std::endl;
        }
        std::cout << "symmetric ?             " << ((is_sym) ? "yes" : "no")
                  << std::endl;
        std::cout << "symmetric values ?      " << ((is_sym_val) ? "yes" : "no")
                  << std::endl;
        free(val);
        free(ind);
        free(ptr);
    }

    return HIPDSS_STATUS_SUCCESS;
}

#endif
