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

#include "rocsparse_clients_objects.hpp"
#include <stdio.h>
#include <stdlib.h>

/**
 * Expands a triangular CSR matrix (Lower or Upper) into a full symmetric CSR.
 * n: Number of rows/cols
 * Ap, Ai: Input Triangular CSR
 * n_Ap, n_Ai: Output Full Symmetric CSR
 */
template<typename I,typename J>
void symmetrize_csr(J n, I *Ap, J *Ai, I **n_Ap, J **n_Ai) {
    J *counts = (J *)calloc(n, sizeof(J));
    
    // Pass 1: Count non-zeros for each row in the full symmetric matrix
    for (J i = 0; i < n; i++) {
        for (I p = Ap[i]; p < Ap[i+1]; p++) {
            J j = Ai[p];
            counts[i]++;          // Original entry (i, j)
            if (i != j) counts[j]++; // Symmetric entry (j, i)
        }
    }

    // Allocate and build new row poIers
    *n_Ap = (I *)malloc((n + 1) * sizeof(I));
    (*n_Ap)[0] = 0;
    for (J i = 0; i < n; i++) {
        (*n_Ap)[i+1] = (*n_Ap)[i] + counts[i];
    }

    // Pass 2: Populate the new column indices
    *n_Ai = (J *)malloc((*n_Ap)[n] * sizeof(J));
    I *next = (I *)malloc(n * sizeof(I));
    for (int i = 0; i < n; i++) next[i] = (*n_Ap)[i];

    for (J i = 0; i < n; i++) {
        for (int p = Ap[i]; p < Ap[i+1]; p++) {
            J j = Ai[p];
            
            // Add original (i, j)
            (*n_Ai)[next[i]++] = j;
            
            // Add symmetric counterpart (j, i) if not on diagonal
            if (i != j) {
                (*n_Ai)[next[j]++] = i;
            }
        }
    }

    free(counts);
    free(next);
}

template <typename T, typename I, typename J>
rocsparse_status rocsparse_csrequilibrate_ruiz(J nrows,
					       J ncols,
					       rocsparse_index_base base,
					       const I* ptr,
					       const J* ind,
					       T* val,
					       floating_data_t<T>* DL,
					       floating_data_t<T>* DR,
					       int64_t nmaxiter,					       
					       floating_data_t<T>  tolerance,
					       int64_t * p_iter,					       
					       floating_data_t<T> * p_residual,							      
					       size_t buffer_size,
					       void * buffer)
  
{
  
  if ( buffer_size < sizeof(floating_data_t<T>) * (nrows + ncols) )
    {
      return rocsparse_status_invalid_size;
    }

  p_iter[0] = nmaxiter;
  p_residual[0] = 0;
  
  floating_data_t<T>*      tmpDL   = reinterpret_cast<floating_data_t<T>*>(buffer);
  floating_data_t<T>*      tmpDR   = tmpDL + nrows;
  
  for(J row = 0; row < nrows; ++row)
    {
      DL[row] = static_cast<floating_data_t<T>>(1);
    }
  
  for(J col = 0; col < ncols; ++col)
    {
      DR[col] = static_cast<floating_data_t<T>>(1);
    }
  
  for(int64_t iter = 0; iter < nmaxiter; ++iter)
    {
      for(J col = 0; col < ncols; ++col)
	{
	  tmpDR[col] = static_cast<floating_data_t<T>>(0);
	}
      for(J row = 0; row < nrows; ++row)
	{
	  tmpDL[row] = static_cast<floating_data_t<T>>(0);
	}
      
      for(J row = 0; row < nrows; ++row)
        {
	  auto mxv = static_cast<floating_data_t<T>>(0);
	  
	  for(J k = ptr[row] - base; k < ptr[row + 1] - base; ++k)
            {
	      auto v = std::abs(val[k]);	      
	      mxv = std::max(v, mxv);
	      
	      const auto col = ind[k] - base;
	      v = std::max(v, tmpDR[col]);
	      tmpDR[col] = v;
            }
	  
	  tmpDL[row] = std::max(tmpDL[row], mxv);
        }
      
      auto res = static_cast<floating_data_t<T>>(0);
      for(J row = 0; row < nrows; ++row)
	{
	  res = std::max(res, std::abs((floating_data_t<T>(1) - tmpDL[row])));
	}
      
      for(J col = 0; col < ncols; ++col)
	{
	  res = std::max(res, std::abs((floating_data_t<T>(1) - tmpDR[col])));
	}
      std::cout << "host residual " << res << std::endl;
      if(res <= tolerance)
	{
	  p_iter[0] = iter;
	  p_residual[0] = res;
	  break;
	}
      
      for(J row = 0; row < nrows; ++row)
	{
	  if (std::abs(tmpDL[row]) > 0)
	    {
	      tmpDL[row] = static_cast<floating_data_t<T>>(1) / sqrt(tmpDL[row]);
	    }
	  else
	    tmpDL[row] = static_cast<floating_data_t<T>>(1);
	}
      
      for(J col = 0; col < ncols; ++col)
	{
	  if (std::abs(tmpDR[col]) > 0)
	    {
	      tmpDR[col] = static_cast<floating_data_t<T>>(1) / sqrt(tmpDR[col]);
	    }
	  else
	    {
	      tmpDR[col] = static_cast<floating_data_t<T>>(1);
	    }
	  
	}
      
      for(J row = 0; row < nrows; ++row)
        {
	  for(J k = ptr[row] - base; k < ptr[row + 1] - base; ++k)
            {
	      const J col = ind[k] - base;
	      val[k] *= tmpDR[col] * tmpDL[row];
            }
        }
      
      for(J row = 0; row < nrows; ++row)
	{
	  DL[row] *= tmpDL[row];
	}
      
      for(J col = 0; col < ncols; ++col)
	{
	  DR[col] *= tmpDR[col];
	}
    }
    
  return rocsparse_status_success;
}


template <typename I, typename J, typename T>
void testing_spequilibrate_bad_arg(const Arguments& arg)
{

  //
  // bad arguments for rocsparse_create_spequilibrate_descr
  // 
  do
    {
      rocsparse_handle handle = (rocsparse_handle)0x4;
      rocsparse_spequilibrate_descr * p_descr = (rocsparse_spequilibrate_descr *)0x4;
      rocsparse_error * p_error = nullptr;      
      bad_arg_analysis(rocsparse_spequilibrate_descr_create,
		       handle,
		       p_descr,
		       p_error);
      
    } while(false);

  
  //
  // bad arguments for rocsparse_spequilibrate_set_input
  // 
  do
    {
      rocsparse_handle handle = (rocsparse_handle)0x4;
      rocsparse_spequilibrate_descr descr = (rocsparse_spequilibrate_descr)0x4;
      rocsparse_spequilibrate_input value = rocsparse_spequilibrate_input_alg;
      void * data = (void*)0x4;
      rocsparse_error * p_error = nullptr;
      bad_arg_analysis(rocsparse_spequilibrate_set_input,
		       handle,
		       descr,
		       value,
		       data,
		       sizeof(data),
		       p_error);      
    } while(false);

  
  //
  // bad arguments for rocsparse_spequilibrate_get_output
  // 
  do
    {
      rocsparse_handle handle = (rocsparse_handle)0x4;
      rocsparse_spequilibrate_descr descr = (rocsparse_spequilibrate_descr)0x4;
      rocsparse_spequilibrate_output value = rocsparse_spequilibrate_output_ruiz_iter;
      int64_t data[1];
      rocsparse_error * p_error = nullptr;
      bad_arg_analysis(rocsparse_spequilibrate_get_output,
		       handle,
		       descr,
		       value,
		       data,
		       sizeof(data[0]),
		       p_error);      
    } while(false);


  
  //
  // bad arguments for rocsparse_spequilibrate_buffer_size
  // 
  do
    {
      rocsparse_handle handle = (rocsparse_handle)0x4;
      rocsparse_spequilibrate_descr descr = (rocsparse_spequilibrate_descr)0x4;
      rocsparse_spmat_descr A  = (rocsparse_spmat_descr)0x4;
      rocsparse_dnvec_descr DL = (rocsparse_dnvec_descr)0x4;
      rocsparse_dnvec_descr DR = (rocsparse_dnvec_descr)0x4;
      rocsparse_spequilibrate_stage stage = rocsparse_spequilibrate_stage_analysis;
      size_t * p_buffer_size_in_bytes = (size_t*)0x4;
      rocsparse_error * p_error = nullptr;
      bad_arg_analysis(rocsparse_spequilibrate_buffer_size,
		       handle,
		       descr,
		       A,
		       DL,
		       DR,
		       stage,
		       p_buffer_size_in_bytes,
		       p_error);      
    } while(false);

  
  //
  // bad arguments for rocsparse_spequilibrate
  // 
  do
    {
      rocsparse_handle handle = (rocsparse_handle)0x4;
      rocsparse_spequilibrate_descr descr = (rocsparse_spequilibrate_descr)0x4;
      rocsparse_spmat_descr A  = (rocsparse_spmat_descr)0x4;
      rocsparse_dnvec_descr DL = (rocsparse_dnvec_descr)0x4;
      rocsparse_dnvec_descr DR = (rocsparse_dnvec_descr)0x4;
      rocsparse_spequilibrate_stage stage = rocsparse_spequilibrate_stage_analysis;
      size_t buffer_size_in_bytes = 1;
      void * buffer = (void*)0x4;      
      rocsparse_error * p_error = nullptr;
      bad_arg_analysis(rocsparse_spequilibrate,
		       handle,
		       descr,
		       A,
		       DL,
		       DR,
		       stage,
		       buffer_size_in_bytes,
		       buffer,
		       p_error);      
    } while(false);

}





// DSU Find with Path-Halving
template<typename J>
J find(J i, J *parent) {
    while (parent[i] != i) {
        parent[i] = parent[parent[i]]; 
        i = parent[i];
    }
    return i;
}

template<typename I, typename J>
void symbolic_csr(J n, I *Ap, J *Ai, I **Lp, J **Li, I **Up, J **Ui) {
    // Use Gilbert-Peierls algorithm on A + A^T pattern
    
    // First, symmetrize the pattern to get A + A^T
    I *sym_Ap;
    J *sym_Ai;
    symmetrize_csr<I,J>(n, Ap, Ai, &sym_Ap, &sym_Ai);
    
    J *parent = (J *)malloc(n * sizeof(J));
    J *w = (J *)malloc(n * sizeof(J));
    J *l_counts = (J *)calloc(n, sizeof(J));
    J *u_counts = (J *)calloc(n, sizeof(J));

    // Initialize parent array (elimination tree)
    for (J i = 0; i < n; i++) {
        parent[i] = -1;
        w[i] = -1;
    }

    // Verify diagonal entries exist
    for (J k = 0; k < n; k++) {
        bool found = false;
        for (I p = Ap[k]; p < Ap[k + 1]; p++) {
            J col = Ai[p];
            if (col == k) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::cerr << "missing diagonal " << k << std::endl;
        }
    }

    // Pass 1: Build elimination tree and count non-zeros
    for (J k = 0; k < n; k++) {
        // Reset w for this column
        for (J i = 0; i < n; i++) {
            w[i] = -1;
        }
        
        // Force diagonal in U
        w[k] = k;
        u_counts[k]++; 

        // Process symmetric pattern at column k
        for (I p = sym_Ap[k]; p < sym_Ap[k + 1]; p++) {
            J i = sym_Ai[p];
            if (i == k) continue;  // Skip diagonal
            
            // Follow path from i to root in elimination tree
            J len = 0;
            for (J j = i; j != -1 && j < k && w[j] != k; j = parent[j]) {
                w[j] = k;
                len++;
            }
            
            // Count fill-in
            if (i < k) {
                l_counts[k] += len;
            } else {
                u_counts[k] += len;
            }
        }
        
        // Update elimination tree: parent of each child of k is k
        for (I p = sym_Ap[k]; p < sym_Ap[k + 1]; p++) {
            J i = sym_Ai[p];
            if (i < k && parent[i] == -1) {
                parent[i] = k;
            }
        }
    }

    // Allocate and prefix-sum for pointers
    *Lp = (I *)malloc((n + 1) * sizeof(I));
    *Up = (I *)malloc((n + 1) * sizeof(I));
    (*Lp)[0] = 0;
    (*Up)[0] = 0;
    
    for (J i = 0; i < n; i++) {
        (*Lp)[i + 1] = (*Lp)[i] + l_counts[i];
        (*Up)[i + 1] = (*Up)[i] + u_counts[i];
    }

    // Pass 2: Populate Indices
    *Li = (J *)malloc((*Lp)[n] * sizeof(J));
    *Ui = (J *)malloc((*Up)[n] * sizeof(J));
    I *l_ptr = (I *)malloc(n * sizeof(I));
    I *u_ptr = (I *)malloc(n * sizeof(I));
    
    for (J i = 0; i < n; i++) {
        l_ptr[i] = (*Lp)[i];
        u_ptr[i] = (*Up)[i];
        parent[i] = -1;
        w[i] = -1;
    }

    // Second pass: populate indices
    for (J k = 0; k < n; k++) {
        // Reset w for this column
        for (J i = 0; i < n; i++) {
            w[i] = -1;
        }
        
        // Force diagonal in U
        (*Ui)[u_ptr[k]++] = k;
        w[k] = k;

        // Process symmetric pattern at column k
        for (I p = sym_Ap[k]; p < sym_Ap[k + 1]; p++) {
            J i = sym_Ai[p];
            if (i == k) continue;  // Skip diagonal
            
            // Follow path from i to root in elimination tree
            for (J j = i; j != -1 && j < k && w[j] != k; j = parent[j]) {
                if (w[j] != k) {
                    w[j] = k;
                    if (j < k) {
                        (*Li)[l_ptr[k]++] = j;
                    } else {
                        (*Ui)[u_ptr[k]++] = j;
                    }
                }
            }
            
            // Add the structural entry itself
            if (w[i] != k) {
                w[i] = k;
                if (i < k) {
                    (*Li)[l_ptr[k]++] = i;
                } else {
                    (*Ui)[u_ptr[k]++] = i;
                }
            }
        }
        
        // Update elimination tree
        for (I p = sym_Ap[k]; p < sym_Ap[k + 1]; p++) {
            J i = sym_Ai[p];
            if (i < k && parent[i] == -1) {
                parent[i] = k;
            }
        }
    }

    free(parent);
    free(w);
    free(l_counts);
    free(u_counts);
    free(l_ptr);
    free(u_ptr);
    free(sym_Ap);
    free(sym_Ai);
}


template <typename I, typename J, typename T>
void testing_spequilibrate(const Arguments& arg)
{
    if(arg.M != arg.N)
      {
        return;
      }
    
    //
    // Create handle.
    //
    rocsparse_local_handle handle(arg);    
    
    //
    // Create host matrix.
    //
    host_csr_matrix<T, I, J> hA;    
    do
    {
      rocsparse_matrix_factory<T, I, J> matrix_factory(arg);
      matrix_factory.init_csr(hA);

      std::cout << "base " << hA.base << std::endl;



    // Example CSR Matrix (4x4)
    // Non-zeros at (0,0), (0,2), (1,0), (1,1), (2,2), (2,3), (3,1), (3,3)
    //    int Ap[] = {0, 2, 4, 6, 8};
    //    int Ai[] = {0, 2, 0, 1, 2, 3, 1, 3};

      I * sym_Ap;
      J * sym_Ai;
      symmetrize_csr<I,J>(hA.m, hA.ptr,hA.ind, &sym_Ap, &sym_Ai);
      
    I *Lp, *Up;
    
    J *Li, *Ui;
    symbolic_csr<I,J>(hA.m, hA.ptr, hA.ind, &Lp, &Li, &Up, &Ui);
    
    std::cout << "nnz " << hA.nnz <<std::endl;
    std::cout << "m " << hA.m <<std::endl;
    std::cout << "sym nnz " << (sym_Ap[hA.m] - sym_Ap[0]) <<std::endl;
    std::cout << "total nnz " << (Lp[hA.m] - Lp[0] + Up[hA.m] - Up[0] )<<std::endl;
    printf("L Factor (Strictly Lower):\n");
    std::cout << "nnz " << (Lp[hA.m] - Lp[0])<<std::endl;
    if (0)
      for (int i = 0; i < hA.m; i++) {
        printf("Row %d: ", i);
        for (int j = Lp[i]; j < Lp[i+1]; j++) printf("%d ", Li[j]);
        printf("\n");
    }

    printf("\nU Factor (Upper Triangle):\n");
    std::cout << "nnz " << (Up[hA.m] - Up[0])<<std::endl;
    if (0)
    for (int i = 0; i < hA.m; i++) {
        printf("Row %d: ", i);
        for (int j = Up[i]; j < Up[i+1]; j++) printf("%d ", Ui[j]);
        printf("\n");
    }

    free(Lp); free(Li); free(Up); free(Ui);

    exit(1);
    

    }
    while(false);
    
    const J M = hA.m;
    const J N = hA.n;    
    
    device_csr_matrix<T, I, J> dA(hA);
    device_dense_vector<floating_data_t<T>> dl(M);
    device_dense_vector<floating_data_t<T>> dr(N);
    
    rocsparse_local_spmat A(dA);
    rocsparse_local_dnvec DL(dl);
    rocsparse_local_dnvec DR(dr);

    rocsparse_error*p_error = nullptr;
    rocsparse_spequilibrate_descr spequilibrate_descr;
    CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate_descr_create(handle,
							       &spequilibrate_descr,
							       p_error));
    
    const rocsparse_spequilibrate_alg alg = rocsparse_spequilibrate_alg_ruiz;    
    CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate_set_input(handle,
							    spequilibrate_descr,
							    rocsparse_spequilibrate_input_alg,
							    &alg,
							    sizeof(alg),
							    p_error));
    
    const double tolerance
      = (std::is_same<floating_data_t<T>, double>())
      ? 4.0e-15
      : (std::is_same<floating_data_t<T>, float>())
      ? 4.0e-7
      : (std::is_same<floating_data_t<T>, _Float16>())
      ? 4.0e-3
      : 0;
    
    //const int64_t nmaxiter = std::max(M,N);
    const int64_t nmaxiter = 40;
    CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate_set_input(handle,
							    spequilibrate_descr,
							    rocsparse_spequilibrate_input_ruiz_nmaxiter,
							    &nmaxiter,
							    sizeof(nmaxiter),
							    p_error));

    
    CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate_set_input(handle,
							    spequilibrate_descr,
							    rocsparse_spequilibrate_input_ruiz_tol,
							    &tolerance,
							    sizeof(tolerance),
							    p_error));


    
    device_dense_vector<char> buffer;
    size_t buffer_size_in_bytes = std::numeric_limits<size_t>::max();    
    CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate_buffer_size(handle,
							      spequilibrate_descr,
							      A,
							      DL,
							      DR,
							      rocsparse_spequilibrate_stage_analysis,
							      &buffer_size_in_bytes,
							      p_error));    
    buffer.resize(buffer_size_in_bytes);
    
    CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate(handle,
						  spequilibrate_descr,
						  A,
						  DL,
						  DR,
						  rocsparse_spequilibrate_stage_analysis,
						  buffer.size(),
						  buffer,
						  p_error));    
    
    buffer_size_in_bytes = std::numeric_limits<size_t>::max();    
    CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate_buffer_size(handle,
							      spequilibrate_descr,
							      A,
							      DL,
							      DR,
							      rocsparse_spequilibrate_stage_compute,
							      &buffer_size_in_bytes,
							      p_error));    
    buffer.resize(buffer_size_in_bytes);    
    if(arg.unit_check)
      {
	int64_t iter{};      
	floating_data_t<T> residual{};	
	host_dense_vector<floating_data_t<T>> hDL(M);
	host_dense_vector<floating_data_t<T>> hDR(N);	
	host_dense_vector<char> hbuffer( sizeof(T)*(M+N) );
    std::cout << "> CPU " << __LINE__<< std::endl;
	rocsparse_status status = rocsparse_csrequilibrate_ruiz<T,I,J>(M,
								       N,
								       hA.base,
								       hA.ptr,
								       hA.ind,
								       hA.val,
								       hDL,
								       hDR,
								       nmaxiter,
								       tolerance,
								       &iter,
								       &residual,
								       sizeof(T) * hbuffer.size(),
								       hbuffer);
	std::cout << "cpu status   " << status << std::endl;
	std::cout << "cpu iter     " << iter << std::endl;
	std::cout << "cpu residual " << residual << std::endl;      
	
    std::cout << "> GPU " << __LINE__<< std::endl;
	CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate(handle,
						      spequilibrate_descr,
						      A,
						      DL,
						      DR,
						      rocsparse_spequilibrate_stage_compute,
						      buffer.size(),
						      buffer,
						      p_error));    


	int64_t gpu_niter;
	CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate_get_output(handle,
								 spequilibrate_descr,
								 rocsparse_spequilibrate_output_ruiz_iter,
								 &gpu_niter,
								 sizeof(gpu_niter),
								 p_error));
	hipStream_t stream;
	CHECK_ROCSPARSE_ERROR(rocsparse_get_stream(handle, &stream));
	CHECK_HIP_ERROR(hipStreamSynchronize(stream));
	std::cout << "gpu niter " << gpu_niter << std::endl;      
	double gpu_residual;
	CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate_get_output(handle,
								 spequilibrate_descr,
								 rocsparse_spequilibrate_output_ruiz_nrm,
								 &gpu_residual,
								 sizeof(gpu_residual),
								 p_error));    

	std::cout << "gpu residual " << gpu_residual << std::endl;      
	//
	// Check the convergence is the same.
	//

	//
	// Left diagonal.
	//
	hDL.near_check(dl);

	//
	// Right diagonal.
	//
	hDR.near_check(dr);

	//
	// Check scaled values are the same.
	//
	hA.val.near_check(dA.val);	
    }

    if(arg.timing)
    {
    }

    CHECK_ROCSPARSE_ERROR(rocsparse_spequilibrate_descr_destroy(handle,
								spequilibrate_descr,
								p_error));
}

#define INSTANTIATE(I, J, T)                                                 \
    template void testing_spequilibrate_bad_arg<I, J, T>(const Arguments& arg); \
    template void testing_spequilibrate<I, J, T>(const Arguments& arg)

INSTANTIATE(int32_t, int32_t, float);
INSTANTIATE(int32_t, int32_t, double);
INSTANTIATE(int32_t, int32_t, rocsparse_float_complex);
INSTANTIATE(int32_t, int32_t, rocsparse_double_complex);

INSTANTIATE(int64_t, int32_t, float);
INSTANTIATE(int64_t, int32_t, double);
INSTANTIATE(int64_t, int32_t, rocsparse_float_complex);
INSTANTIATE(int64_t, int32_t, rocsparse_double_complex);

// not instantiated which does not make any sense!
// INSTANTIATE(int32_t, int64_t, float);
// INSTANTIATE(int32_t, int64_t, double);
// INSTANTIATE(int32_t, int64_t, rocsparse_float_complex);
// INSTANTIATE(int32_t, int64_t, rocsparse_double_complex);

INSTANTIATE(int64_t, int64_t, float);
INSTANTIATE(int64_t, int64_t, double);
INSTANTIATE(int64_t, int64_t, rocsparse_float_complex);
INSTANTIATE(int64_t, int64_t, rocsparse_double_complex);

void testing_spequilibrate_extra(const Arguments& arg) {}
