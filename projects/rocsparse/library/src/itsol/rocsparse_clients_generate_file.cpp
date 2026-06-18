#include <rocsparse/rocsparse.h>
#include <map>
#include <iostream>

#include "rocsparse_clients_generate_file.h"
#include "rocsparse_clients_generate_descr.hpp"
#include "rocsparse_clients_generate_file_descr.hpp"
#include "rocsparse_clients_generate_input_file_descr.hpp"
#include "rocsparse_clients_generate_output_file_descr.hpp"
#include "rocsparse_clients_generate_csr_pointer_descr.hpp"



/* ============================================================================================ */
/*! \brief  Read matrix from mtx file in COO format */
static inline void read_mtx_value(std::istringstream& is, int64_t& row, int64_t& col, int8_t& val)
{
    is >> row >> col >> val;
}

static inline void read_mtx_value(std::istringstream& is, int64_t& row, int64_t& col, float& val)
{
    is >> row >> col >> val;
}

static inline void read_mtx_value(std::istringstream& is, int64_t& row, int64_t& col, _Float16& val)
{
    float temp;
    read_mtx_value(is, row, col, temp);
    val = temp;
}

static inline void read_mtx_value(std::istringstream& is, int64_t& row, int64_t& col, double& val)
{
    is >> row >> col >> val;
}

static inline void
    read_mtx_value(std::istringstream& is, int64_t& row, int64_t& col, rocsparse_float_complex& val)
{
    float real{};
    float imag{};

    is >> row >> col >> real >> imag;

    val = {real, imag};
}

static inline void read_mtx_value(std::istringstream&       is,
                                  int64_t&                  row,
                                  int64_t&                  col,
                                  rocsparse_double_complex& val)
{
    double real{};
    double imag{};

    is >> row >> col >> real >> imag;

    val = {real, imag};
}

#include <vector>
template <typename T, typename I>
rocsparse_status import_sparse_csr(rocsparse_clients_generate_file_descr self,
				   void* row_ind_, void* col_ind_, void* val_)
{
  rocsparse_clients_generate_output_file_descr output = (rocsparse_clients_generate_output_file_descr)self->m_output;

  I* row_ind = (I*)row_ind_;
  I* col_ind = (I*)col_ind_;
  T* val = (T*)val_;
  char           line[1024];
  const size_t   nnz = output->m_nnz;
  
  std::vector<I> unsorted_row(nnz);
  std::vector<I> unsorted_col(nnz);
  std::vector<T> unsorted_val(nnz);
  
  // Read entries
  I idx = 0;
  while(fgets(line, 1024, self->f))
    {
      if(idx >= nnz)
        {
            throw rocsparse_status_internal_error;
        }

      int64_t irow{};
      int64_t icol{};
      T       ival;

      std::istringstream ss(line);
      
      if(!strcmp(self->m_data, "pattern"))
        {
	  ss >> irow >> icol;
	  ival = static_cast<T>(1);
        }
      else
        {
            read_mtx_value(ss, irow, icol, ival);
        }

        unsorted_row[idx] = (I)irow;
        unsorted_col[idx] = (I)icol;
        unsorted_val[idx] = ival;

        ++idx;

        if(self->m_symm && irow != icol)
        {
            if(idx >= nnz)
            {
                throw rocsparse_status_internal_error;
            }

            unsorted_row[idx] = (I)icol;
            unsorted_col[idx] = (I)irow;
            unsorted_val[idx] = ival;
            ++idx;
        }
    }
    fclose(self->f);

    // Sort by row and column index
    std::vector<I> perm(nnz);
    for(I i = 0; i < nnz; ++i)
      {
        perm[i] = i;
    }
    
    std::sort(perm.begin(), perm.end(), [&](const I& a, const I& b) {
      if(unsorted_row[a] < unsorted_row[b])
	{
	  return true;
	}
      else if(unsorted_row[a] == unsorted_row[b])
        {
	  return (unsorted_col[a] < unsorted_col[b]);
        }
      else
        {
	  return false;
        }
    });


    for(I i =0; i < output->m_m+1; ++i)
      {
        row_ind[i] = 0;
      }
    for(I i = 0; i < nnz; ++i)
      {
        row_ind[unsorted_row[perm[i]]] += 1;
      }
    for(I i = 1; i < output->m_m+1; ++i)
      {
        row_ind[i] += row_ind[i-1];
      }
    for(I i = 0; i < nnz; ++i)
      {
        col_ind[i] = unsorted_col[perm[i]]-1;
    }
    for(I i = 0; i < nnz; ++i)
    {
        val[i] = unsorted_val[perm[i]];
    }

    return rocsparse_status_success;
}

extern "C" void rocsparse_clients_generate_input_file_set(rocsparse_clients_generate_handle handle,
							  rocsparse_clients_generate_input_file input_file,
							  const void * data,
							  size_t data_size)
{
  switch(input_file)
    {
    case rocsparse_clients_generate_input_filename:
      {
	((rocsparse_clients_generate_input_file_descr)handle->m_descr->m_input)->set_filename((const char*)data);
	break;
      }
    }  
}





rocsparse_status import_sparse_csr(rocsparse_clients_generate_file_descr self,
				   int64_t*                    m,
				   int64_t*                    n,
				   int64_t*              nnz,
				   rocsparse_index_base* base)
{
  rocsparse_clients_generate_input_file_descr input = (rocsparse_clients_generate_input_file_descr)self->m_input;
  rocsparse_clients_generate_output_file_descr output = (rocsparse_clients_generate_output_file_descr)self->m_output;
  char line[1024];
  std::cout << "filename " <<input->get_filename()<< std::endl;
  self->f = fopen(input->get_filename(), "r");
  if(!self->f)
    {
      std::cerr << "cannot read '" << input->get_filename()<< "'" << std::endl;
      return rocsparse_status_internal_error;
    }
  // Check for banner
  if(!fgets(line, 1024, self->f))
    {
      throw rocsparse_status_internal_error;
    }
  
  char banner[16];
  char array[16];
  char coord[16];
  char type[16];
  
  // Extract banner
  if(sscanf(line, "%15s %15s %15s %15s %15s", banner, array, coord, self->m_data, type) != 5)
    {
      throw rocsparse_status_internal_error;
    }
  
  // Convert to lower case
  for(char* p = array; *p != '\0'; *p = tolower(*p), p++)
    ;
  for(char* p = coord; *p != '\0'; *p = tolower(*p), p++)
    ;
  for(char* p = self->m_data; *p != '\0'; *p = tolower(*p), p++)
    ;
  for(char* p = type; *p != '\0'; *p = tolower(*p), p++)
    ;
  
  // Check banner
  if(strncmp(line, "%%MatrixMarket", 14) != 0)
    {
      throw rocsparse_status_internal_error;
    }
  
  // Check array type
  if(strcmp(array, "matrix") != 0)
    {
      throw rocsparse_status_internal_error;
    }
  
  // Check coord
  if(strcmp(coord, "coordinate") != 0)
    {
      throw rocsparse_status_internal_error;
    }
  
  // Check self->m_data
  if(strcmp(self->m_data, "real") != 0 && strcmp(self->m_data, "integer") != 0
     && strcmp(self->m_data, "pattern") != 0 && strcmp(self->m_data, "complex") != 0)
    {
      throw rocsparse_status_internal_error;
    }
  
  // Check type
  if(strcmp(type, "general") != 0 && strcmp(type, "symmetric") != 0)
    {
      throw rocsparse_status_internal_error;
    }
  
  // Symmetric flag
  self->m_symm = !strcmp(type, "symmetric");
  
  // Skip comments
  while(fgets(line, 1024, self->f))
    {
      if(line[0] != '%')
        {
	  break;
        }
    }
  
  // Read dimensions
  int64_t snnz;
  
  int64_t inrow;
  int64_t incol;
  int64_t innz;
  
  sscanf(line, "%ld %ld %ld", &inrow, &incol, &innz);
  
  rocsparse_status status;
  m[0] = inrow;
  n[0] = incol;
  snnz = innz;
  
  if(self->m_symm)
    {
        //
        //
        // We need to count how many diagonal elements are in the file.
        //
        //

        //
        // Record position.
        //
        fpos_t pos;
        if(0 != fgetpos(self->f, &pos))
        {
            throw rocsparse_status_internal_error;
        }

        //
        // Count diagonal coefficients.
        //
        int64_t num_diagonal_coefficients = 0;
        while(fgets(line, 1024, self->f))
        {
	  int64_t irow{};
            int64_t icol{};
            sscanf(line, "%ld %ld", &irow, &icol);
            if(irow == icol)
            {
                ++num_diagonal_coefficients;
            }
        }

        //
        // Set position.
        //
        if(0 != fsetpos(self->f, &pos))
        {
            throw rocsparse_status_internal_error;
        }

        //
        // Now calculate the right number of coefficients.
        //
        snnz = (snnz - num_diagonal_coefficients) * 2 + num_diagonal_coefficients;
    }
  
  nnz[0] = snnz;
  base[0]     = rocsparse_index_base_one;
  output->m_nnz=snnz;
  return rocsparse_status_success;
}






void rocsparse_clients_generate_file_descr_::generate(rocsparse_clients_generate_stage stage,
						      size_t buffer_size,
						      void * buffer)
{
  switch(stage)
      {
      case rocsparse_clients_generate_stage_analysis:
	{	  
	  int64_t m,n,nnz;
	  rocsparse_index_base base = rocsparse_index_base_zero;
	  import_sparse_csr(this,
			    &m,
			    &n,
			    &nnz,
			    &base);
	  
	  this->m_output->m_m = m;	  
	  this->m_output->m_n = n;	    
	  this->m_output->m_nnz = nnz;
	  if (m <= std::numeric_limits<int32_t>::max() )
	    {
	      this->m_output->m_ind_indextype = rocsparse_indextype_i32;
	    }
	  else
	    {
	      this->m_output->m_ind_indextype = rocsparse_indextype_i64;
	    }
	  if (nnz <= std::numeric_limits<int32_t>::max() )
	    {
	      this->m_output->m_ptr_indextype = rocsparse_indextype_i32;
	    }
	  else
	    {
	      this->m_output->m_ptr_indextype = rocsparse_indextype_i64;
	    }
	  
	  this->m_output->m_val_datatype = rocsparse_datatype_f64_r;	  
	  break;
	}
	
      case rocsparse_clients_generate_stage_compute:
	{
	  import_sparse_csr<double,int32_t>(this,
	    reinterpret_cast<rocsparse_clients_generate_csr_pointer_descr>(this->m_pointer)->m_ptr,
	    reinterpret_cast<rocsparse_clients_generate_csr_pointer_descr>(this->m_pointer)->m_ind,
	    reinterpret_cast<rocsparse_clients_generate_csr_pointer_descr>(this->m_pointer)->m_val);	  
	  break;
	}
      }
  }
