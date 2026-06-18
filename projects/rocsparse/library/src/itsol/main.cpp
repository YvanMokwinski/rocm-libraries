#include <iostream>
#include <cmath>
#include <hip/hip_runtime.h>
#include <rocsparse/rocsparse.h>
#include <rocblas/rocblas.h>
#define RAW_SV

#include "rocsparse_itsol.h"
#include "rocsparse_itsol_cg.h"
#include "rocsparse_itsol_bicgstab.h"

#include <fstream>
#include <iomanip>

#include "rocsparse_clients_generate_tridiagonal.h"
#include "rocsparse_clients_generate_file.h"

void add(int64_t m,
	 int64_t nnz_src,
	 const int32_t * ptr_src,
	 const int32_t * ind_src,
	 int64_t nnz_dst,
	 const int32_t * ptr_dst,
	 const int32_t * ind_dst,
	 int32_t ** ptr_,
	 int32_t ** ind_)
{
  int32_t * ptr = new int32_t[m+1];
  ptr_[0] = ptr;
  int32_t mx_src;
  for (int64_t i=0;i<m;++i)
    mx_src = std::max(mx_src,ptr_src[i+1]-ptr_src[i]);
  int32_t mx_dst;
  for (int64_t i=0;i<m;++i)
    mx_dst = std::max(mx_dst,ptr_dst[i+1]-ptr_dst[i]);

  int32_t select_n = 0;
  int32_t * select = new int32_t[std::min((mx_src + mx_dst)*2,int32_t(m))];
  int32_t * blank = new int32_t[m];
  for (int64_t i=0;i<m;++i)
    blank[i] = 0;
  ptr[0]=0;
  for (int64_t i=0;i<m;++i)
    {
      select_n = 0;
      for (int32_t k=ptr_src[i];k<ptr_src[i+1];++k)
	{
	  const int32_t j = ind_src[k];
	  if (blank[j]==0)
	    {
	      select[select_n] = j;
	      blank[j]=++select_n;
	    }
	}
      for (int32_t k=ptr_dst[i];k<ptr_dst[i+1];++k)
	{
	  const int32_t j = ind_dst[k];
	  if (blank[j]==0)
	    {
	      select[select_n] = j;
	      blank[j]=++select_n;
	    }
	}
      ptr[i+1] = select_n;
      for (int32_t k=0;k<select_n;++k) blank[select[k]] = 0;
    }
  for (int32_t k=1;k<=m;++k) ptr[k] += ptr[k-1];
  
  int64_t nnz = ptr[m];
  int32_t * ind = new int32_t[nnz];
  ind_[0] = ind;
  for (int64_t i=0;i<m;++i)
    {
      select_n = 0;
      for (int32_t k=ptr_src[i];k<ptr_src[i+1];++k)
	{
	  const int32_t j = ind_src[k];
	  if (blank[j]==0)
	    {
	      select[select_n] = j;
	      blank[j]=++select_n;
	    }
	}
      for (int32_t k=ptr_dst[i];k<ptr_dst[i+1];++k)
	{
	  const int32_t j = ind_dst[k];
	  if (blank[j]==0)
	    {
	      select[select_n] = j;
	      blank[j]=++select_n;
	    }
	}

      for (int32_t k=0;k<select_n;++k) ind[ptr[i] + k] = select[k];
      for (int32_t k=0;k<select_n;++k) blank[select[k]] = 0;
    }

}

void gthr(int64_t len,
	  const int32_t * y,	  
	  int32_t * x,
	  const int32_t * y_ind,
	  rocsparse_index_base base)
{
  for (int64_t i=0;i<len;++i)
    {
      x[i] = y[y_ind[i]-base];
    }
}

void sctr(int64_t len,
	  const int32_t * y,	  
	  int32_t * x,
	  const int32_t * x_ind,
	  rocsparse_index_base base)
{
  for (int64_t i=0;i<len;++i)
    {
      x[x_ind[i]-base] = y[i];
    }
}

void sort(int64_t m,
	  int64_t nnz,
	  const int32_t * ptr,
	  const int32_t * ind,
	  int32_t * perm)
{
  for (int64_t i=0;i<nnz;++i)
    perm[i] = i;
  for (int64_t i=0;i<m;++i)
    {
      std::sort(perm + ptr[i], perm+ptr[i+1],[&ind,&ptr,&i](const int32_t& j,const int32_t & k) { return (ind[ptr[i]+j]<ind[ptr[i]+k]);  } );
    }
    
}

void apply_sort(int64_t m,
		int64_t nnz,
		const int32_t * ptr,
		int32_t * ind,
		const int32_t * perm,
		size_t buffer_size,
		void * buffer)
{
  gthr(nnz,
       ind,  
       (int32_t*)buffer,
       perm,
       rocsparse_index_base_zero);
  memcpy(buffer, ind,  sizeof(int32_t) * nnz);  
}


void transpose(int64_t m,
	       int64_t nnz_src,
	       const int32_t * ptr_src,
	       const int32_t * ind_src,
	       int32_t ** ptr_,
	       int32_t ** ind_)
{
  int32_t * ptr = new int32_t[m+1];
  int32_t * ind = new int32_t[nnz_src];
  ptr_[0] = ptr;
  ptr[0]=0;
  for (int64_t i=0;i<m;++i)ptr[i]=0;
  for (int64_t i=0;i<m;++i)
    {
      for (int32_t k=ptr_src[i];k<ptr_src[i+1];++k)
	{
	  const int32_t j = ind_src[k];
	  ptr[j+1] +=1;
	}
    }
  for (int32_t k=1;k<=m;++k) ptr[k] += ptr[k-1];
  
  ind_[0] = ind;
  for (int64_t i=0;i<m;++i)
    {
      for (int32_t k=ptr_src[i];k<ptr_src[i+1];++k)
	{
	  const int32_t j = ind_src[k];
	  ind[ptr[j]] = i;
	  ++ptr[j];
	}      
    }
  for (int32_t k=m;k>0;--k) ptr[k] = ptr[k-1];
  ptr[0] = 0;  
}


void spy_terminal(int64_t m,
		  int64_t n,
		  int64_t nnz,
		  const int32_t * ptr,
		  const int32_t * ind,
		  int64_t block_size=1)
{
  int32_t mat_m=(m + block_size - 1)/block_size;
  int32_t mat_n=(n + block_size - 1) /block_size;
  int32_t * mat = new int32_t[mat_m*mat_n];
  //  int32_t * r = new int32_t[mat_m*mat_n];
  for (int64_t i=0;i<mat_m*mat_n;++i) mat[i] = 0;
  //  for (int64_t i=0;i<mat_m*mat_n;++i) r[i] = 0;
  for (int64_t i=0;i<m;++i)
    {
      for (int32_t k=ptr[i];k<ptr[i+1];++k)
	{
	  const int32_t j = ind[k];
	  // std::cout << j << " " << n << std::endl;
	  mat[(j/block_size)*mat_m+(i/block_size)] += 1;
	}
    }

  //
  //  Rmat - stlower(mat) * stupper(mat)
  //

  int count = 0;
  int count_full = 0;
  for (int64_t i=0;i<mat_m;++i)
    {
      for (int64_t j=0;j<mat_n;++j)
	{if (mat[j*mat_m+i] >0)
	    count += 1;
	  if ( mat[j*mat_m+i] == block_size * block_size)
	    count_full +=1;
	  //	  std::cout << std::setw(5) << mat[j*mat_m+i]; // Output: 00123
	  //    std::setfill('0') 
	  //	  std::cout << " " << ;
	}
      //      std::cout << std::endl;
    }
  size_t s = sizeof(int32_t) * (m+1) + sizeof(int32_t)*nnz + sizeof(double)*nnz;
  size_t s1 = sizeof(int32_t) * (m+1) + sizeof(int32_t)*count + sizeof(double)*count*block_size*block_size;
  std::cout << block_size<<  " " << s << " " << s1 << " " << (nnz - count)<< std::endl;
  delete[]mat;
}

void sym(int64_t m,
	 int64_t nnz,
	 const int32_t * ptr,
	 const int32_t * ind,
	 int32_t ** ptr_sym_,
	 int32_t ** ind_sym_,
	 bool sorted)
{
  int32_t * ptr_transpose{};
  int32_t * ind_transpose{};
  
  transpose(m,
	    nnz,
	    ptr,
	    ind,
	    &ptr_transpose,
	    &ind_transpose);
  
  int32_t * ptr_sym{};
  int32_t * ind_sym{};

  add(m,
      nnz,
      ptr,
      ind,
      nnz,
      ptr_transpose,
      ind_transpose,
      &ptr_sym,
      &ind_sym);

  ptr_sym_[0] = ptr_sym;
  ind_sym_[0] = ind_sym;

  if (sorted)
    {
      int32_t * perm = new int32_t[nnz];
      sort(m,ptr_sym[m],ptr_sym,ind_sym,perm);
      void * buffer;
      buffer= malloc(sizeof(int32_t)*nnz);
      apply_sort(m,ptr_sym[m],ptr_sym,ind_sym,perm,sizeof(int32_t)*nnz,buffer);
      delete[]perm;
      free(buffer);
    }
  delete[]ptr_transpose;
  delete[]ind_transpose;
  
}




#if 0

rocsparse_status import_vector(const char * filename,
			       int64_t*m,
			       rocsparse_datatype datatype,
			       void * data)
{
  char line[1024];
  FILE*f = fopen(filename, "r");
  if(!f)
    {
      std::cerr << "cannot read '" << filename << "'" << std::endl;
      return rocsparse_status_internal_error;
    }
  // Check for banner
  if(!fgets(line, 1024, f))
    {
      throw rocsparse_status_internal_error;
    }
  
  char banner[16];
  char array[16];
  char coord[16];
  char type[16];
  char data[16];
  // Extract banner
  if(sscanf(line, "%15s %15s %15s %15s %15s", banner, array, coord, data, type) != 5)
    {
      throw rocsparse_status_internal_error;
    }
  
  // Convert to lower case
  for(char* p = array; *p != '\0'; *p = tolower(*p), p++)
    ;
  for(char* p = coord; *p != '\0'; *p = tolower(*p), p++)
    ;
  for(char* p = data; *p != '\0'; *p = tolower(*p), p++)
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
  
  // Check data
  if(strcmp(data, "real") != 0 && strcmp(data, "complex") != 0)
    {
      throw rocsparse_status_internal_error;
    }
  
  // Check type
  if(strcmp(type, "general") != 0)
    {
      throw rocsparse_status_internal_error;
    }
  
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
  
  sscanf(line, "%ld %ld", &inrow, &incol);  
  rocsparse_status status;
  m[0] = inrow;
  n[0] = incol;
   rocsparse_clients_generate_output_file_descr output = (rocsparse_clients_generate_output_file_descr)self->m_output;
  T* val = (T*)val_;
  char           line[1024];  
  // Read entries
  I idx = 0;
  while(fgets(line, 1024, self->f))
    {
      if(idx >= m)
        {
	  throw rocsparse_status_internal_error;
        }
      
      T       ival;      
      std::istringstream ss(line);
      ss >> ival;
      val[idx++] = ival;      
    }
  fclose(f);
  return rocsparse_status_success;
}

#endif
      
constexpr size_t get_sizelm_in_bytes(rocsparse_datatype datatype)
{
  switch(datatype)
    {
    case rocsparse_datatype_f32_r:
      {
	return sizeof(float);
      }
    case rocsparse_datatype_f64_r:
      {
	return sizeof(double);
      }
    default:
      {
	return 0;
      }
    }
}


constexpr size_t get_sizelm_in_bytes(rocsparse_indextype indextype)
{
  switch(indextype)
    {
    case rocsparse_indextype_i32:
      {
	return sizeof(int32_t);
      }
    case rocsparse_indextype_i64:
      {
	return sizeof(int64_t);
      }
    case rocsparse_indextype_u16:
      {
	return sizeof(uint16_t);
      }
    }
}

//#include "rocsparse_clients_generate_tridiagonal_csr.hpp"
#define PRINT_VAR(MSG,NAME) std::cout << MSG << " " <<  #NAME << " " << NAME << std::endl;
#undef PRINT_VAR
#define PRINT_VAR(MSG,NAME) (void)0

  template<typename J>
  static inline bool find(const J * ind_begin_, const J * ind_end_, const J i_)
  {
    for (const J * p = ind_begin_;p!=ind_end_;++p)
      if (i_ == *p)
	return true;
    return false;
  }

template<typename I,typename J>
  static bool is_sym_impl(int64_t m__,
		     const void * ptr__,
		     const void * ind__)
  {
    const I * ptr_ = (const I*)ptr__;
    const J * ind_ = (const J*)ind__;
    const J m_ = m__;
    for (J i=0;i<m_;++i)
      {
	for (I k=ptr_[i];k<ptr_[i+1];++k)
	  {
	    const J j = ind_[k];
	    if (i!=j)
	      {
		if (false == find(ind_+ptr_[j],
				  ind_+ptr_[j+1],
				  i))
		  return false;
	      }
	  }
      }
    return true;
  }

static const std::map<std::tuple<rocsparse_indextype,rocsparse_indextype>,bool (*)(int64_t,
										   const void *,
										   const void *)>
s_is_sym = { { {rocsparse_indextype_i32,rocsparse_indextype_i32}, is_sym_impl<int32_t,int32_t> },
	     { {rocsparse_indextype_i64,rocsparse_indextype_i32}, is_sym_impl<int64_t,int32_t> },
	     { {rocsparse_indextype_i32,rocsparse_indextype_i64}, is_sym_impl<int32_t,int64_t> },
	     { {rocsparse_indextype_i64,rocsparse_indextype_i64}, is_sym_impl<int64_t,int64_t> }};

template<typename I,typename J>
static bool is_sym(int64_t m_,
		   const void * ptr_,
		   rocsparse_indextype ptr_indextype_,
		   const void * ind_,
		   rocsparse_indextype ind_indextype_)
{
  const auto func = s_is_sym.find({ptr_indextype_,ind_indextype_})->second;
  return func(m_, ptr_, ind_);
}



// Minimal HIP error-checking macro
#define HIP_CHECK(call)						\
  do {								\
    hipError_t status = call;					\
    if (status != hipSuccess) {					\
      std::cerr << "HIP error: " << hipGetErrorString(status)	\
		<< " at line " << __LINE__ << std::endl;	\
      std::exit(1);						\
    }								\
  } while(0)

#define ROCSPARSE_CHECK(call)					\
  do {								\
    rocsparse_status status = call;				\
    if (status != rocsparse_status_success) {			\
      std::cerr << "rocsparse error: " << status		\
		<< " at line " << __LINE__ << std::endl;	\
      std::exit(1);						\
    }								\
  } while(0)

typedef  struct rocsparse_clients_index_array_t  *rocsparse_clients_index_array;
typedef  struct rocsparse_clients_data_array_t  *rocsparse_clients_data_array;

 struct rocsparse_clients_index_array_t
{
private:
  rocsparse_indextype indextype{};
  const void * const_p{};
  void * p{};
  const void * const_gpu_p{};
  void * gpu_p{};
  int64_t nelm{};
  size_t sizelm_in_bytes{};
  rocsparse_pointer_mode pointer_mode{};
public:
  void info() const
  {
    std::cout << "- index array " << std::endl;
    std::cout << "   - mode " << pointer_mode << std::endl;
    std::cout << "   - type " << indextype << std::endl;
    std::cout << "   - nelm " << nelm << std::endl;
  }
  
  int64_t size() const {return this->nelm;}
  int64_t size_in_bytes() const {return this->nelm*this->sizelm_in_bytes;}
  void print(const char * msg = nullptr, int64_t bound = 0)
  {
    if (msg)
      std::cout << "print '" << msg << "'" << std::endl;
    const bool t = pointer_mode == rocsparse_pointer_mode_device;
    if (t) this->to_host(true);
    if (rocsparse_indextype_i32 == indextype)
      {
	int64_t n = bound > 0 ? std::min(bound,nelm) : nelm;
	for (int64_t i=0;i<n;++i)
	  {
	    std::cout << "["<<i<<"] " << *(((const int32_t*)const_p)+i) << std::endl;	    
	  }
	if (bound < nelm)
	  {
	    std::cout << "... truncated" << std::endl;
	  }

      }
    if (t) this->to_device(true);
  }
  template<typename T>
   operator const T * () const { const void * tmp = (pointer_mode == rocsparse_pointer_mode_host) ? const_p : const_gpu_p;
    return (const T*)tmp;
  }
  template<typename T>
   operator T*()
  {
    void * tmp = (pointer_mode == rocsparse_pointer_mode_host) ? p : gpu_p;
    return (T*)tmp;
  }

  void resize(int64_t nelm_)
  {
    if (nelm_ != nelm)
      {
	this->nelm = nelm_;
	void * tmp = (pointer_mode == rocsparse_pointer_mode_host) ? p : gpu_p;
	if (tmp)
	  {
	    HIP_CHECK(hipFree(tmp));	   	    
	  }
	if (pointer_mode == rocsparse_pointer_mode_host)
	  {
	    HIP_CHECK(hipHostMalloc(&tmp,sizelm_in_bytes * nelm));
	    this->p = tmp;
	    this->const_p = this->p;
	  }
	else
	  {
	    HIP_CHECK(hipMalloc(&tmp,sizelm_in_bytes * nelm));
	    this->gpu_p = tmp;
	    this->const_gpu_p = this->gpu_p;
	  }
      }
  }

  void free_host()
  {
    if (pointer_mode == rocsparse_pointer_mode_device)
      {
    this->const_p = nullptr;
    if (this->p != nullptr)
      {
	HIP_CHECK(hipFree(this->p));
	this->p = nullptr;
      }
      }
  }
  
  void free_device()
  {
    if (pointer_mode == rocsparse_pointer_mode_host)
      {
    this->const_gpu_p = nullptr;
    if (this->gpu_p != nullptr)
      {
	HIP_CHECK(hipFree(this->gpu_p));
	this->gpu_p = nullptr;
      }
      }
  }
  
  void to_host(bool keep = false)
  {
    if (pointer_mode == rocsparse_pointer_mode_host) return;
    
    if (this->p == nullptr)
      {
	HIP_CHECK(hipHostMalloc(&this->p,sizelm_in_bytes * nelm));
	this->const_p = this->p;
      }
	
    HIP_CHECK(hipMemcpy(this->p, this->const_gpu_p, sizelm_in_bytes * nelm, hipMemcpyDefault));
    if (false == keep)
      {
	this->const_gpu_p = nullptr;
	if (this->gpu_p != nullptr)
	  {
	    HIP_CHECK(hipFree(this->gpu_p));
	    this->gpu_p = nullptr;
	  }
      }
    pointer_mode = rocsparse_pointer_mode_host;
  }
  
  void to_device(bool keep = false)
  {
    if (pointer_mode == rocsparse_pointer_mode_device) return;

    if (this->gpu_p == nullptr)
      {	
	HIP_CHECK(hipMalloc(&this->gpu_p,sizelm_in_bytes * nelm));
	this->const_gpu_p = this->gpu_p;
      }
	
    HIP_CHECK(hipMemcpy(this->gpu_p, this->const_p, sizelm_in_bytes * this->nelm, hipMemcpyDefault));
    if (false == keep)
      {
    this->const_p = nullptr;
    if (this->p != nullptr)
      {
	HIP_CHECK(hipFree(this->p));
	this->p = nullptr;
      }
      }
    
    pointer_mode = rocsparse_pointer_mode_device;
  }
  
  rocsparse_indextype get_indextype() const {return indextype;} 
  void set_indextype(rocsparse_indextype  value) {sizelm_in_bytes=get_sizelm_in_bytes(value);indextype = value;} 
  
  const void  * data() const { return (pointer_mode == rocsparse_pointer_mode_device) ? this->const_gpu_p : this->const_p;; }
  void  * data() { return (pointer_mode == rocsparse_pointer_mode_device) ? this->gpu_p : this->p; }
  rocsparse_clients_index_array_t(){};
  
  rocsparse_clients_index_array_t(int64_t n,
				  rocsparse_indextype indextype_,
				  void * p_,
				  rocsparse_pointer_mode mode = rocsparse_pointer_mode_host)
    : indextype(indextype_),
      const_p((mode == rocsparse_pointer_mode_device) ? nullptr : p_),
      p((mode == rocsparse_pointer_mode_device) ? nullptr : p_),
      const_gpu_p((mode == rocsparse_pointer_mode_device) ? p_ : nullptr),
      gpu_p((mode == rocsparse_pointer_mode_device) ? p_ : nullptr),
      nelm(n),
      sizelm_in_bytes(get_sizelm_in_bytes(indextype_)),
      pointer_mode(mode)
  {
  }

    rocsparse_clients_index_array_t(int64_t n,
				    rocsparse_indextype indextype_,
				    rocsparse_pointer_mode mode = rocsparse_pointer_mode_host)
    : indextype(indextype_),
      const_p(nullptr),
      p(nullptr),
      const_gpu_p(nullptr),
      gpu_p(nullptr),
      nelm(n),
      sizelm_in_bytes(get_sizelm_in_bytes(indextype_)),
      pointer_mode(mode)
  {
    if (pointer_mode == rocsparse_pointer_mode_host)
      {
	void * tmp;
	HIP_CHECK(hipHostMalloc(&tmp,sizelm_in_bytes * this->nelm));
	this->p = tmp;
	this->const_p = this->p;
      }
    else
      {
	void * tmp;
	HIP_CHECK(hipMalloc(&tmp,sizelm_in_bytes * this->nelm));
	this->gpu_p = tmp;
	this->const_gpu_p = this->gpu_p;
      }
  }

  rocsparse_clients_index_array_t(int64_t n,
				  rocsparse_indextype indextype_,
				  const void * const_p_,
				  rocsparse_pointer_mode mode = rocsparse_pointer_mode_host)
    : indextype(indextype_),
      const_p((mode == rocsparse_pointer_mode_device) ? nullptr : const_p_),
      p(nullptr),
      const_gpu_p((mode == rocsparse_pointer_mode_device) ? const_p_ : nullptr),
      gpu_p(nullptr),
      nelm(n),
      sizelm_in_bytes(get_sizelm_in_bytes(indextype_)),
      pointer_mode(mode)
  {
  }
  
};



struct rocsparse_clients_data_array_t
{
private:
  rocsparse_datatype datatype{};
  const void * const_p{};
  void * p{};
  const void * const_gpu_p{};
  void * gpu_p{};
  int64_t nelm{};
  size_t sizelm_in_bytes{};
  rocsparse_pointer_mode pointer_mode{};
public:
  rocsparse_clients_data_array_t(){};
  int64_t size() const {return this->nelm;}
  int64_t size_in_bytes() const {return this->nelm*this->sizelm_in_bytes;}
  void info() const
  {
    std::cout << "- data array " << std::endl;
    std::cout << "   - mode " << pointer_mode << std::endl;
    std::cout << "   - type " << datatype << std::endl;
    std::cout << "   - nelm " << nelm << std::endl;
  }

  void print(const char * msg = nullptr,int64_t bound = 0)
  {
    if (msg)
      std::cout << "print '" << msg << "'" << std::endl;
    const bool t = (pointer_mode == rocsparse_pointer_mode_device);
    if (t) this->to_host(true);
    const int64_t n = bound > 0 ? std::min(bound,nelm) : nelm;
    
    if (rocsparse_datatype_f64_r == datatype)
      {
	for (int64_t i=0;i<n;++i)
	  std::cout << "["<<i<<"] " << *(((const double*)const_p)+i) << std::endl;
      }
    else if (rocsparse_datatype_f32_r == datatype)
      {
	for (int64_t i=0;i<n;++i)
	  std::cout << "["<<i<<"] " << *(((const float*)const_p)+i) << std::endl;
      }
    else
      {
	std::cout << "not supported " <<  __LINE__ <<  std::endl;
      }
    if (bound < nelm)
      {
	std::cout << "... truncated" << std::endl;
      }
    if (t)
      {
	this->to_device(true);
	this->free_host();
      }
  }

  void print(std::ostream&out)
  {
    const bool t = (pointer_mode == rocsparse_pointer_mode_device);
    if (t) this->to_host(true);
    if (rocsparse_datatype_f64_r == datatype)
      {
	for (int64_t i=0;i<nelm;++i)
	  {
	    out <<i<<" " << *(((const double*)const_p)+i) << std::endl;
	  }
      }
    else if (rocsparse_datatype_f32_r == datatype)
      {
	for (int64_t i=0;i<nelm;++i)
	  {
	    out <<i<<" " << *(((const float*)const_p)+i) << std::endl;
	  }
      }
    else
      {
	std::cout << "not supported " <<  __LINE__ <<  std::endl;
      }

    if (t)
      {
	this->to_device(true);
	this->free_host();
      }
  }

    template<typename T>
     operator const T*() const { const void * tmp = (pointer_mode == rocsparse_pointer_mode_host) ? const_p : const_gpu_p;
    return (const T*)tmp;
  }
  template<typename T>
  operator T*()
  {
    void * tmp = (pointer_mode == rocsparse_pointer_mode_host) ? p : gpu_p;
    return (T*)tmp;
  }

  void resize(int64_t nelm_)
  {
    if (nelm_ != nelm)
      {
	this->nelm = nelm_;
	void * tmp = (pointer_mode == rocsparse_pointer_mode_host) ? p : gpu_p;
	if (tmp)
	  {
	    HIP_CHECK(hipFree(tmp));	   	    
	  }
	if (pointer_mode == rocsparse_pointer_mode_host)
	  {
	    HIP_CHECK(hipHostMalloc(&tmp,sizelm_in_bytes * nelm));
	    this->p = tmp;
	    this->const_p = this->p;
	  }
	else
	  {
	    HIP_CHECK(hipMalloc(&tmp,sizelm_in_bytes * nelm));
	    this->gpu_p = tmp;
	    this->const_gpu_p = this->gpu_p;
	  }
      }
  }
  
  void to_host(bool keep = false)
  {
    if (pointer_mode == rocsparse_pointer_mode_host) return;    
    if (this->p == nullptr)
      {
	HIP_CHECK(hipHostMalloc(&this->p, sizelm_in_bytes * nelm));
	this->const_p = this->p;
      }
    HIP_CHECK(hipMemcpy(this->p, this->const_gpu_p, sizelm_in_bytes * nelm, hipMemcpyDefault));
    if (false == keep)
      {
	free_device();
      }
    pointer_mode = rocsparse_pointer_mode_host;
  }
  
  void to_device(bool keep = false)
  {
    if (pointer_mode == rocsparse_pointer_mode_device) return;

    if (this->gpu_p == nullptr)
      {
	HIP_CHECK(hipMalloc(&this->gpu_p,sizelm_in_bytes * nelm));
	this->const_gpu_p = this->gpu_p;
      }
	
    HIP_CHECK(hipMemcpy(this->gpu_p, this->const_p, sizelm_in_bytes * nelm, hipMemcpyDefault));
    if (false == keep)
      {
	free_host();
      }
    pointer_mode = rocsparse_pointer_mode_device;
  }
  rocsparse_datatype get_datatype() const {return datatype;} 
  void set_datatype(rocsparse_datatype  value) {sizelm_in_bytes=get_sizelm_in_bytes(value);datatype = value;} 
  


  const void  * data() const { return (pointer_mode == rocsparse_pointer_mode_device) ? this->const_gpu_p : this->const_p;; }
  void  * data() { return (pointer_mode == rocsparse_pointer_mode_device) ? this->gpu_p : this->p; }



  void free_host()
  {
    this->const_p = nullptr;
    if (this->p != nullptr)
      {
	HIP_CHECK(hipFree(this->p));
	this->p = nullptr;
      }
  }
  
  void free_device()
  {
    this->const_gpu_p = nullptr;
    if (this->gpu_p != nullptr)
      {
	HIP_CHECK(hipFree(this->gpu_p));
	this->gpu_p = nullptr;
      }
  }
  
  rocsparse_clients_data_array_t(int64_t n,
				  rocsparse_datatype datatype_,
				  rocsparse_pointer_mode mode = rocsparse_pointer_mode_host)
    : datatype(datatype_),
      const_p(nullptr),
      p(nullptr),
      const_gpu_p(nullptr),
      gpu_p(nullptr),
      nelm(n),
      sizelm_in_bytes(get_sizelm_in_bytes(datatype_)),
      pointer_mode(mode)
  {
    if (pointer_mode == rocsparse_pointer_mode_host)
      {
	void * tmp;
	HIP_CHECK(hipHostMalloc(&tmp,sizelm_in_bytes * this->nelm));
	this->p = tmp;
	this->const_p = this->p;
      }
    else
      {
	void * tmp;
	HIP_CHECK(hipMalloc(&tmp,sizelm_in_bytes * this->nelm));
	this->gpu_p = tmp;
	this->const_gpu_p = this->gpu_p;
      }
  }
  
  void zero()
  {
    const bool t = (pointer_mode == rocsparse_pointer_mode_device);
    if (t) 
      HIP_CHECK(hipMemset(this->gpu_p, 0, this->size_in_bytes()));
    else
      memset(this->p,0, this->size_in_bytes());
  }
  
  void one()
  {
    const bool t = (pointer_mode == rocsparse_pointer_mode_device);
    if (t) this->to_host(true);
    
    if (this->datatype == rocsparse_datatype_f64_r)
      for (int64_t i=0;i<this->nelm;++i) *(((double*)this->p)+i) = 1;
    else if (this->datatype == rocsparse_datatype_f32_r)
      for (int64_t i=0;i<this->nelm;++i) *(((float*)this->p)+i) = 1;
    else
      {
	std::cerr << "icite" << std::endl;
      }
    if (t)
      {
	this->to_device(true);
	this->free_host();
      }
  }

  rocsparse_clients_data_array_t(int64_t n,
				 rocsparse_datatype datatype_,
				 void * p_,
				 rocsparse_pointer_mode mode = rocsparse_pointer_mode_host)
    : datatype(datatype_),
      p((mode == rocsparse_pointer_mode_device) ? nullptr : p_),
      const_p((mode == rocsparse_pointer_mode_device) ? nullptr : p_),
      const_gpu_p((mode == rocsparse_pointer_mode_device) ? p_ : nullptr),
      gpu_p((mode == rocsparse_pointer_mode_device) ? p_ : nullptr),
      nelm(n),
      sizelm_in_bytes(get_sizelm_in_bytes(datatype_)),
      pointer_mode(mode)
  {
  }
  
  rocsparse_clients_data_array_t(int64_t n,
				 rocsparse_datatype datatype_,
				 const void * const_p_,				 
				 rocsparse_pointer_mode mode = rocsparse_pointer_mode_host)
    : datatype(datatype_),
      const_p((mode == rocsparse_pointer_mode_device) ? nullptr : const_p_),
      p(nullptr),
      const_gpu_p((mode == rocsparse_pointer_mode_device) ? const_p_ : nullptr),
      gpu_p(nullptr),
      nelm(n),
      sizelm_in_bytes(get_sizelm_in_bytes(datatype_)),
      pointer_mode(mode)
  {
  }
};





struct rocsparse_clients_generate_file_seed_
{
  static constexpr rocsparse_clients_generate_alg alg { rocsparse_clients_generate_alg_file};
  const char * filename;
};

struct rocsparse_clients_generate_tridiagonal_seed_
{
  static constexpr rocsparse_clients_generate_alg alg{ rocsparse_clients_generate_alg_tridiagonal};
  int64_t nrows;
};



void initial_tridiagonal(const rocsparse_clients_generate_tridiagonal_seed_& seed,
			 rocsparse_clients_index_array_t& ptr,
			 rocsparse_clients_index_array_t& ind,
			 rocsparse_clients_data_array_t& val)
{  
  rocsparse_clients_generate_handle handle;
  rocsparse_clients_create_generate_handle(&handle);

  auto m = seed.nrows;
  const auto alg = seed.alg;
  
  rocsparse_clients_generate_input_set(handle,
				       rocsparse_clients_generate_input_alg,
				       &alg,
				       sizeof(alg));

  { rocsparse_format format = rocsparse_format_csr;
    rocsparse_clients_generate_input_set(handle,
					 rocsparse_clients_generate_input_format,
					 &format,
					 sizeof(format)); }


  
  //
  // Input tridiagonal
  //
  rocsparse_clients_generate_input_set(handle,
				       rocsparse_clients_generate_input_m,
				       &m,
				       sizeof(m));
      

  //
  //
  //
  {
    size_t buffer_size{};
    void * buffer{};
    rocsparse_clients_generate_buffer_size(handle,
					   rocsparse_clients_generate_stage_analysis,
					   &buffer_size);
    HIP_CHECK(hipHostMalloc(&buffer, buffer_size));
    
    rocsparse_clients_generate(handle,
			       rocsparse_clients_generate_stage_analysis,
			       buffer_size,
			       buffer);
    
    HIP_CHECK(hipFree(buffer));
  }
  //
  //
  //
  int64_t nnz;
  rocsparse_clients_generate_output_get(handle,
					rocsparse_clients_generate_output_nnz,
					&nnz,
					sizeof(nnz));
  
  rocsparse_clients_generate_output_get(handle,
					rocsparse_clients_generate_output_m,
					&m,
					sizeof(m));
  //
  // Set file.
  //
  
  { rocsparse_indextype indextype;
    rocsparse_clients_generate_output_get(handle,
					  rocsparse_clients_generate_output_ptr_indextype,
					  &indextype,
					  sizeof(indextype));

    ptr.set_indextype(indextype);
  }
  { rocsparse_indextype indextype;
    rocsparse_clients_generate_output_get(handle,
					  rocsparse_clients_generate_output_ind_indextype,
					  &indextype,
					  sizeof(indextype));
    ind.set_indextype(indextype);
  }
  
  { rocsparse_datatype datatype;
    rocsparse_clients_generate_output_get(handle,
					  rocsparse_clients_generate_output_val_datatype,
					  &datatype,
					  sizeof(datatype)); 
    val.set_datatype(datatype);
  }
#if 0
  { rocsparse_indextype indextype = ptr.get_indextype();
    rocsparse_clients_generate_input_set(handle,
					 rocsparse_clients_generate_input_ptr_indextype,
					 &indextype,
					 sizeof(indextype)); }
  
  { rocsparse_indextype indextype = ind.get_indextype();
    rocsparse_clients_generate_input_set(handle,
					 rocsparse_clients_generate_input_ind_indextype,
					 &indextype,
					 sizeof(indextype)); }
  
  { rocsparse_datatype datatype = val.get_datatype();
    rocsparse_clients_generate_input_set(handle,
					 rocsparse_clients_generate_input_val_datatype,
					 &datatype,
					 sizeof(datatype)); }
#endif


  ptr.resize(m+1);

  const size_t sizeof_ptr = get_sizelm_in_bytes(ptr.get_indextype());
  rocsparse_clients_generate_csr_pointer_set(handle,
					     rocsparse_clients_generate_csr_pointer_ptr,
					     ptr.data(),
					     sizeof_ptr*(m+1));
  
  ind.resize(nnz);
  val.resize(nnz);
  const size_t sizeof_ind = get_sizelm_in_bytes(ind.get_indextype());
  const size_t sizeof_val = get_sizelm_in_bytes(val.get_datatype());
  
  rocsparse_clients_generate_csr_pointer_set(handle,
					     rocsparse_clients_generate_csr_pointer_ind,
					     ind,
					     sizeof_ind*nnz);
  

  rocsparse_clients_generate_csr_pointer_set(handle,
					     rocsparse_clients_generate_csr_pointer_val,
					     val,
					     sizeof_val*nnz);
  

  {
    size_t buffer_size{};
    void * buffer{};
    rocsparse_clients_generate_buffer_size(handle,
					   rocsparse_clients_generate_stage_compute,
					   &buffer_size);
    HIP_CHECK(hipHostMalloc(&buffer, buffer_size));
    
    rocsparse_clients_generate(handle,
			       rocsparse_clients_generate_stage_compute,
			       buffer_size,
			       buffer);
    
    HIP_CHECK(hipFree(buffer));
  }

  
  rocsparse_clients_destroy_generate_handle(handle);
}

void initial_file(const rocsparse_clients_generate_file_seed_& seed,
		  int64_t& m,
		  int64_t& n,
		  int64_t& nnz,
		  rocsparse_clients_index_array_t& ptr,
		  rocsparse_clients_index_array_t& ind,
		  rocsparse_clients_data_array_t& val)
{
  const auto filename = seed.filename;
  const auto alg = seed.alg;

  rocsparse_clients_generate_handle handle;
  rocsparse_clients_create_generate_handle(&handle);
  

  { 
    rocsparse_clients_generate_input_set(handle,
					 rocsparse_clients_generate_input_alg,
					 &alg,
					 sizeof(alg));
  }
  
  { rocsparse_format format = rocsparse_format_csr;
    rocsparse_clients_generate_input_set(handle,
					 rocsparse_clients_generate_input_format,
					 &format,
					 sizeof(format)); }

  rocsparse_clients_generate_input_file_set(handle,
					    rocsparse_clients_generate_input_filename,
					    filename,
					    sizeof(filename)); 

  //
  //
  //
  {
    size_t buffer_size{};
    void * buffer{};
    rocsparse_clients_generate_buffer_size(handle,
					   rocsparse_clients_generate_stage_analysis,
					   &buffer_size);
    HIP_CHECK(hipHostMalloc(&buffer, buffer_size));
    
    rocsparse_clients_generate(handle,
			       rocsparse_clients_generate_stage_analysis,
			       buffer_size,
			       buffer);
    
    HIP_CHECK(hipFree(buffer));
  }
  //
  //
  //

  rocsparse_clients_generate_output_get(handle,
					rocsparse_clients_generate_output_nnz,
					&nnz,
					sizeof(nnz));
  
  rocsparse_clients_generate_output_get(handle,
					rocsparse_clients_generate_output_m,
					&m,
					sizeof(m));

  rocsparse_clients_generate_output_get(handle,
					rocsparse_clients_generate_output_n,
					&n,
					sizeof(n));
  //
  // Set file.
  //
  
  { rocsparse_indextype indextype;
    rocsparse_clients_generate_output_get(handle,
					  rocsparse_clients_generate_output_ptr_indextype,
					  &indextype,
					  sizeof(indextype));

    ptr.set_indextype(indextype);
  }
  { rocsparse_indextype indextype;
    rocsparse_clients_generate_output_get(handle,
					  rocsparse_clients_generate_output_ind_indextype,
					  &indextype,
					  sizeof(indextype));
    ind.set_indextype(indextype);
  }
  
  { rocsparse_datatype datatype;
    rocsparse_clients_generate_output_get(handle,
					  rocsparse_clients_generate_output_val_datatype,
					  &datatype,
					  sizeof(datatype)); 
    val.set_datatype(datatype);
  }

  ptr.resize(m+1);

  rocsparse_clients_generate_csr_pointer_set(handle,
					     rocsparse_clients_generate_csr_pointer_ptr,
					     ptr,
					     ptr.size_in_bytes());
  
  ind.resize(nnz);
  val.resize(nnz);
  const size_t sizeof_ind = get_sizelm_in_bytes(ind.get_indextype());

  const size_t sizeof_val = get_sizelm_in_bytes(val.get_datatype());
  
  rocsparse_clients_generate_csr_pointer_set(handle,
					     rocsparse_clients_generate_csr_pointer_ind,
					     ind,
					     ind.size_in_bytes());
  

  rocsparse_clients_generate_csr_pointer_set(handle,
					     rocsparse_clients_generate_csr_pointer_val,
					     val,
					     val.size_in_bytes());
  

  {
    size_t buffer_size{};
    void * buffer{};
    rocsparse_clients_generate_buffer_size(handle,
					   rocsparse_clients_generate_stage_compute,
					   &buffer_size);
    HIP_CHECK(hipHostMalloc(&buffer, buffer_size));
    
    rocsparse_clients_generate(handle,
			       rocsparse_clients_generate_stage_compute,
			       buffer_size,
			       buffer);
    
    HIP_CHECK(hipFree(buffer));
  }

  
  rocsparse_clients_destroy_generate_handle(handle);
}

typedef struct rocsparse_clients_csr_matrix_t
{
  void info() const
  {
    std::cout << "- csr matrix " << std::endl;
    std::cout << "   - m " << m << std::endl;
    std::cout << "   - nnz " << nnz << std::endl;
    std::cout << "   - ptr " << std::endl;
    std::cout << "   - ind " << std::endl;
    std::cout << "   - val " << std::endl;
  }

  bool is_sym()const
  {
    s_is_sym.find({ptr.get_indextype(),ind.get_indextype()})->second(m,ptr,ind);

    return true;
  }
  
  int64_t m {-1};
  int64_t n {-1};
  int64_t nnz {-1};

  rocsparse_clients_index_array_t ptr{};
  rocsparse_clients_index_array_t ind{};
  rocsparse_clients_data_array_t val{};

  void free_host()
  {
    ptr.free_host();
    ind.free_host();
    val.free_host();
  }
  
  void free_device()
  {
    ptr.free_device();
    ind.free_device();
    val.free_device();
  }
  
  void to_device(bool keep = false)
  {
    ptr.to_device(keep);
    ind.to_device(keep);
    val.to_device(keep);
  }
  
  void to_host(bool keep = false)
  {
    ptr.to_host(keep);
    ind.to_host(keep);
    val.to_host(keep);
  }
  
  rocsparse_clients_csr_matrix_t()
  {
  }
}*rocsparse_clients_csr_matrix;

  
  
void init(rocsparse_clients_csr_matrix_t* self,
	  const rocsparse_clients_generate_file_seed_& seed)
{
  initial_file(seed,
	       self->m,
	       self->n,
	       self->nnz,
	       self->ptr,
	       self->ind,
	       self->val);
}
  
void init(rocsparse_clients_csr_matrix_t* self,
	  const rocsparse_clients_generate_tridiagonal_seed_& seed)
{
  initial_tridiagonal(seed,
			self->ptr,
			self->ind,
			self->val);
    
    self->nnz = self->ind.size();
    self->m = self->ptr.size()-1;
    self->n = self->m;
  }






void operator >> (rocsparse_clients_csr_matrix_t&a,
		  const rocsparse_clients_generate_file_seed_& seed)
{
  init(&a, seed);
}

void operator >> (rocsparse_clients_csr_matrix_t&a,
		  const rocsparse_clients_generate_tridiagonal_seed_& seed)
{
  init(&a, seed);
}

void operator << (rocsparse_clients_data_array_t&a,const char *n)
{
  std::ofstream out(n);
  a.print(out);
  out.close();
}

struct rocsparse_clients_preconditioner
{
  typedef enum kind_
    {
      NONE,
      JACOBI,
      ILU0,
      ILUK
    } kind;
  static constexpr kind all[4]{NONE,
      JACOBI,
      ILU0,
      ILUK};
  static constexpr const char * names[4]{"none",
      "jacobi",
      "ilu0",
      "iluk"};
private:
  kind m_value;
public:
  operator kind() const {return m_value;}
  bool is_invalid() const
  {
    for(auto v : all)
      {
	if (m_value == v)
	  {
	    return false;
	  }
      }
    return true;
  }
  rocsparse_clients_preconditioner(const char * value)
  {
    m_value = (kind)-1;
    for(auto v : all)
      {
	if (!strcmp(names[v],value))
	  {
	    m_value = v;
	    break;
	  }
      } 
  }

  rocsparse_clients_preconditioner()
  {
    m_value = (kind)-1;
  }
  
  rocsparse_clients_preconditioner(int32_t value)
  {
    m_value = (kind)-1;
    for(auto v : all)
      {
	if (value == v)
	  {
	    m_value = v;
	    break;
	  }
      }
  }
};
const rocsparse_clients_preconditioner::kind rocsparse_clients_preconditioner::all[4];
  const char * const rocsparse_clients_preconditioner::names[4];




struct rocsparse_clients_iterative_method
{
  using kind = rocsparse_itsol_alg;
  static constexpr kind all[2]{rocsparse_itsol_alg_cg,rocsparse_itsol_alg_bicgstab};
  static constexpr const char * names[2]{"cg",
      "bicgstab"};
private:
  kind m_value;
public:
  operator kind() const {return m_value;}
  bool is_invalid() const
  {
    for(auto v : all)
      {
	if (m_value == v)
	  {
	    return false;
	  }
      }
    return true;
  }
  rocsparse_clients_iterative_method(const char * value)
  {
    m_value = (kind)-1;
    for(auto v : all)
      {
	if (!strcmp(names[v-1],value))
	  {
	    m_value = v;
	    break;
	  }
      } 
  }

  rocsparse_clients_iterative_method()
  {
    m_value = (kind)-1;
  }
  
  rocsparse_clients_iterative_method(int32_t value)
  {
    m_value = (kind)-1;
    for(auto v : all)
      {
	if (value == v)
	  {
	    m_value = v;
	    break;
	  }
      }
  }
};
const rocsparse_clients_iterative_method::kind rocsparse_clients_iterative_method::all[2];
  const char * const rocsparse_clients_iterative_method::names[2];

struct rocsparse_sparse_operators_
{
  
  rocsparse_clients_csr_matrix m_A{};
  rocsparse_clients_csr_matrix m_P{};
  rocsparse_clients_data_array m_tmp{};
  
  rocsparse_handle m_sparse_handle{};
  rocsparse_const_spmat_descr m_spmat_A{};  
  rocsparse_spmat_descr m_spmat_L{};
  rocsparse_spmat_descr m_spmat_U{};
  rocsparse_dnvec_descr m_dnvec_x{};
  rocsparse_dnvec_descr m_dnvec_tmp{};
  rocsparse_dnvec_descr m_dnvec_y{};
  size_t m_buffer_size{};
  void * m_buffer{};

  
  double m_one[1];
  double m_zero[1]{};
  rocsparse_datatype m_datatype{};

  int64_t m_m{};
  int64_t m_nnz{};

  rocsparse_mat_descr m_P_descr_L{};
  rocsparse_mat_descr m_P_descr_U{};
  rocsparse_mat_info m_P_info{};
  rocsparse_clients_preconditioner m_preconditioner{};
  ~rocsparse_sparse_operators_()
  {
    (void)hipFree(this->m_buffer);
    //    (void)hipFree(this->m_dnvec_tmp_values);
    (void)rocsparse_destroy_dnvec_descr(this->m_dnvec_x);
    (void)rocsparse_destroy_dnvec_descr(this->m_dnvec_y);
    (void)rocsparse_destroy_dnvec_descr(this->m_dnvec_tmp);
    (void)rocsparse_destroy_spmat_descr(this->m_spmat_L);
    (void)rocsparse_destroy_spmat_descr(this->m_spmat_U);

    (void)rocsparse_destroy_mat_descr(m_P_descr_L);
    (void)rocsparse_destroy_mat_descr(m_P_descr_U);
    (void)rocsparse_destroy_mat_info(m_P_info);
    (void)rocsparse_destroy_spmat_descr(this->m_spmat_A);
    (void)rocsparse_destroy_handle(this->m_sparse_handle);
  }

	      template<uint32_t BLOCKDIM,typename T>
	      static __global__ __launch_bounds__(BLOCKDIM)void  extract_diag(int32_t m,
			     const int32_t * __restrict__ ptr_,
			     const int32_t * __restrict__ ind_,
			     const T * __restrict__ val_,
									      T * __restrict__  diag_,
			     T zero_)
		{
		  int32_t gid = hipBlockIdx_x * BLOCKDIM + hipThreadIdx_x;
		  if (gid < m)
		    {
		      T d = zero_;
		      for (int32_t k  = ptr_[gid];k<ptr_[gid+1];++k)
			{
			  int32_t j = ind_[k];
			  if (j==gid)
			    {
			      d = val_[k];
			      break;
			    }
			}
		      diag_[gid] = d;
		    }
		}


  void exclude(const double * A,double * R)
  {
    for (int32_t i=0;i<this->m_m;++i)
      {
	for (int32_t j=0;j<this->m_m;++j)
	  {
	    if (R[i+j*this->m_m]!=0 && A[i+j*this->m_m]!=0)
	      {
		R[i+j*this->m_m] = 0;
	      }
	    //	    R[i+j*this->m_m] = A[i+j*this->m_m]==0 && R[i+j*this->m_m]!=0;
	    //	    false false false;
	    //	    true true true;
	  }
      }
  }
  
  void mm(double * A,double * P,double * R,int level)
  {
    for (int32_t i=0;i<this->m_m;++i)
      {
	for (int32_t j=0;j<this->m_m;++j)
	  {
	    bool sum=false;
	    for (int32_t k=0;k<=std::min(i,j);++k)
	      {
		sum|=(P[i+k*this->m_m]!=0) && (P[k+j*this->m_m]!=0);		 
	      }
	    if (sum)
	      {
		bool c = (P[i+j*this->m_m]==0);
		if (c)
		  {
		    R[i+j*this->m_m] = level + 2;
		  }
		else
		  {
		    if (R[i+j*this->m_m]==0) R[i+j*this->m_m] = 1;
		  }
	      }
	    //	    B[i+j*this->m_m] = A[i+j*this->m_m] + sum;
	  }
      }


  }
  void iluk()
  {
    const int32_t * dptr = (const int32_t*)this->m_A->ptr;
    const int32_t * dind = (const int32_t*)this->m_A->ind;
    int32_t *ptr = new int32_t[this->m_m+1];
    int32_t *ind = new int32_t[this->m_nnz];
    HIP_CHECK(hipMemcpy(ptr,dptr,sizeof(int32_t)*(this->m_m+1), hipMemcpyDefault));
    HIP_CHECK(hipMemcpy(ind,dind,sizeof(int32_t)*(this->m_nnz), hipMemcpyDefault));

    double * host_dense_A = new double[this->m_m*this->m_m];
    double * host_dense_initial = new double[this->m_m*this->m_m];
    double * host_dense_A_next = new double[this->m_m*this->m_m];
    HIP_CHECK(hipHostMalloc(&host_dense_A,sizeof(double)*this->m_m*this->m_m));
    HIP_CHECK(hipHostMalloc(&host_dense_A_next,sizeof(double)*this->m_m*this->m_m));
    HIP_CHECK(hipHostMalloc(&host_dense_initial,sizeof(double)*this->m_m*this->m_m));
    memset(host_dense_A,0,sizeof(double)*this->m_m*this->m_m);
    memset(host_dense_initial,0,sizeof(double)*this->m_m*this->m_m);
    for (int32_t i=0;i<this->m_m;++i)
      {
	for (int32_t k=ptr[i];k<ptr[i+1];++k)
	  {
	    int32_t j = ind[k];
	    
	    if (i==j)
	    host_dense_A[i+j*this->m_m] = 1.0;
	    else
	    host_dense_A[i+j*this->m_m] = 1.0;
	  }
      }
    
    for (int32_t i=0;i<this->m_m;++i)
      {
	for (int32_t j=0;j<this->m_m;++j)
	  {
	    host_dense_initial[i+j*this->m_m] = host_dense_A[i+j*this->m_m];
	  }
      }
    
    std::cout << "graph0" << std::endl;
    for (int32_t i=0;i<this->m_m;++i)
      {
	for (int32_t j=0;j<this->m_m;++j)
	  {
	    std::cout << " " << host_dense_A[i+j*this->m_m];
	  }
	std::cout << std::endl;
      }
    mm(host_dense_initial,host_dense_A,host_dense_A_next,0);
    std::cout << "graph1" << std::endl;
    for (int32_t i=0;i<this->m_m;++i)
      {
	for (int32_t j=0;j<this->m_m;++j)
	  {
	    std::cout << " " << host_dense_A_next[i+j*this->m_m];
	  }
	std::cout << std::endl;
      }
    
    for (int32_t i=0;i<this->m_m;++i)
      {
	for (int32_t j=0;j<this->m_m;++j)
	  {
	    host_dense_A[i+j*this->m_m] = host_dense_A_next[i+j*this->m_m]!=0 || host_dense_initial[i+j*this->m_m]!=0;
	  }
      }
    mm(host_dense_initial,host_dense_A,host_dense_A_next,1);


    std::cout << "graph2" << std::endl;
    for (int32_t i=0;i<this->m_m;++i)
      {
	for (int32_t j=0;j<this->m_m;++j)
	  {
	    std::cout << " " << host_dense_A_next[i+j*this->m_m];
	  }
	std::cout << std::endl;
      }
    
    for (int32_t i=0;i<this->m_m;++i)
      {
	for (int32_t j=0;j<this->m_m;++j)
	  {
	    host_dense_A[i+j*this->m_m] = host_dense_A_next[i+j*this->m_m]!=0 || host_dense_initial[i+j*this->m_m]!=0;
	    //	    host_dense_A[i+j*this->m_m] += host_dense_A_next[i+j*this->m_m];
	  }
      }

    mm(host_dense_initial,host_dense_A,host_dense_A_next,2);

    //    exclude(host_dense_initial,host_dense_A_next);

    std::cout << "graph3" << std::endl;
    for (int32_t i=0;i<this->m_m;++i)
      {
	for (int32_t j=0;j<this->m_m;++j)
	  {
	    std::cout << " " << host_dense_A_next[i+j*this->m_m];
	  }
	std::cout << std::endl;
      }


    
  }

  void compute_preconditioner(const rocsparse_clients_preconditioner& kind)
  {

    switch(kind)
      {
      case rocsparse_clients_preconditioner::ILUK:
	{
	  iluk();
	  break;
	}
      case rocsparse_clients_preconditioner::JACOBI:
      case rocsparse_clients_preconditioner::ILU0:
	{
	  this->m_P = new rocsparse_clients_csr_matrix_t();
	  this->m_P->m = this->m_A->m;
	  this->m_P->n = this->m_A->n;
	  this->m_P->nnz = this->m_A->nnz;
	  if (kind == rocsparse_clients_preconditioner::JACOBI)
	    {
	      this->m_P->ptr.set_indextype(rocsparse_indextype_i32);
	      this->m_P->ind.set_indextype(this->m_A->ind.get_indextype());
	      this->m_P->val.set_datatype(this->m_A->val.get_datatype());

	      this->m_P->ptr.resize(this->m_m+1);
	      this->m_P->ind.resize(this->m_m);
	      this->m_P->val.resize(this->m_m);
	      
	      rocsparse_create_identity_permutation(this->m_sparse_handle, this->m_m+1,(int32_t*) this->m_P->ptr);
	      rocsparse_create_identity_permutation(this->m_sparse_handle, this->m_m,(int32_t*) this->m_P->ind);
	      
	      hipLaunchKernelGGL((extract_diag<512,double>),
				 dim3( (this->m_m + 512 - 1) / 512),
				 dim3(512),
				 0,
				 0,
				 (int32_t)this->m_m,			
				 (const int32_t *)this->m_A->ptr,
				 (const int32_t *)this->m_A->ind,
				 (const double*)this->m_A->val,
				 (double*)this->m_P->val,
				 double(1));
	    }
	  else
	    {
	      
	      this->m_P->ptr = this->m_A->ptr;
	      this->m_P->ind = this->m_A->ind;
	      this->m_P->val.set_datatype(this->m_A->val.get_datatype());
	      this->m_P->val.resize(this->m_nnz);
	      HIP_CHECK(hipMemcpy(this->m_P->val,this->m_A->val,sizeof(double)*this->m_nnz, hipMemcpyDefault));
	    }
	  
	  if (m_P_descr_L == nullptr)
	    rocsparse_create_mat_descr(&m_P_descr_L);

	  if (m_P_descr_U == nullptr)
	    rocsparse_create_mat_descr(&m_P_descr_U);
	  if (m_P_info == nullptr)
	    rocsparse_create_mat_info(&m_P_info);

	  rocsparse_set_mat_fill_mode(m_P_descr_L,rocsparse_fill_mode_lower);
	  rocsparse_set_mat_diag_type(m_P_descr_L,rocsparse_diag_type_unit);

	  rocsparse_set_mat_fill_mode(m_P_descr_U,rocsparse_fill_mode_upper);
	  rocsparse_set_mat_diag_type(m_P_descr_U,rocsparse_diag_type_non_unit);
	  
	  this->m_P->to_device();
	  
  // Obtain required buffer size
	  rocsparse_mat_descr P_descr;
	  rocsparse_create_mat_descr(&P_descr);
	  size_t buffer_size_ilu0 = 0;
	  
	  ROCSPARSE_CHECK(rocsparse_dcsrilu0_buffer_size(this->m_sparse_handle,
							 this->m_m,
							 this->m_nnz,
							 P_descr,
							 (double*)this->m_P->val,
							 (const int32_t*)this->m_P->ptr,
							 (const int32_t*)this->m_P->ind,
							 this->m_P_info,
							 &buffer_size_ilu0));
	  HIP_CHECK(hipDeviceSynchronize());
	  
	  void* dbuffer_ilu0 = nullptr;
	  HIP_CHECK(hipMalloc(&dbuffer_ilu0,
			      buffer_size_ilu0));
	  
	  ROCSPARSE_CHECK(rocsparse_dcsrilu0_analysis(m_sparse_handle,
						      this->m_m,
						      this->m_nnz,
						      P_descr,
						      (double*)this->m_P->val,
						      (const int32_t*)this->m_P->ptr,
						      (const int32_t*)this->m_P->ind,
						      this->m_P_info,
						      rocsparse_analysis_policy_reuse,
						      rocsparse_solve_policy_auto,
						      dbuffer_ilu0));
	  HIP_CHECK(hipDeviceSynchronize());
	  
	  ROCSPARSE_CHECK(rocsparse_dcsrilu0(m_sparse_handle,
					     this->m_m,
					     this->m_nnz,
					     P_descr,
					     (double*)this->m_P->val,
					     (const int32_t*)this->m_P->ptr,
					     (const int32_t*)this->m_P->ind,
					     this->m_P_info,
					     rocsparse_solve_policy_auto,
					     dbuffer_ilu0));
	  HIP_CHECK(hipDeviceSynchronize());
	  rocsparse_destroy_mat_descr(P_descr);

	break;
      }
    case rocsparse_clients_preconditioner::NONE:
      {
	break;
      }
      }
    
  }

  rocsparse_sparse_operators_(rocsparse_clients_preconditioner& preconditioner,
			      rocsparse_clients_csr_matrix A,
			      rocsparse_index_base index_base,
			      rocsparse_clients_data_array_t&x,
			      rocsparse_clients_data_array_t&y)
    : m_A(A)
  {
    this->m_preconditioner = preconditioner;
    const auto m = A->m;
    const auto n = A->n;
    this->m_m = A->m;
    if (this->m_datatype == rocsparse_datatype_f32_r)
      {
	float one = 1.0;
	memcpy(this->m_one, &one, sizeof(float));
      }
    else
      {
	double one = 1.0;
	memcpy(this->m_one, &one, sizeof(double));
      }

    this->m_datatype = A->val.get_datatype();

    rocsparse_create_handle(&this->m_sparse_handle);
    rocsparse_create_dnvec_descr(&this->m_dnvec_x,
				 n,
				 x,
				 x.get_datatype());

    this->m_tmp = new rocsparse_clients_data_array_t(n, this->m_datatype);

    rocsparse_create_dnvec_descr(&this->m_dnvec_tmp,
				 this->m_tmp->size(),
				 this->m_tmp,
				 this->m_tmp->get_datatype());
    
    rocsparse_create_dnvec_descr(&this->m_dnvec_y,
				 m,
				 y,
				 y.get_datatype());

    this->m_nnz = A->ind.size();
    
    compute_preconditioner(m_preconditioner);
    

    if (m_preconditioner == rocsparse_clients_preconditioner::ILU0 ||
	m_preconditioner == rocsparse_clients_preconditioner::JACOBI)
      {	
	this->create_L(this->m_sparse_handle,
		       &this->m_spmat_L);
    
	this->create_U(this->m_sparse_handle,
		       &this->m_spmat_U);
      }
    
    rocsparse_create_const_csr_descr(&this->m_spmat_A,
				     this->m_m,
				     this->m_m,
				     this->m_nnz,
				     this->m_A->ptr,
				     this->m_A->ind,
				     this->m_A->val,
				     A->ptr.get_indextype(),
				     A->ind.get_indextype(),			     
				     index_base,
				     A->val.get_datatype());
    
    rocsparse_spmv(this->m_sparse_handle,		   
		   rocsparse_operation_none,
		   this->m_one,
		   this->m_spmat_A,		   
		   this->m_dnvec_x,
		   this->m_zero,
		   this->m_dnvec_y,
		   this->m_datatype,		   
		   rocsparse_spmv_alg_default,
		   rocsparse_spmv_stage_buffer_size,
		   &this->m_buffer_size,
		   nullptr);
    

    
    size_t buffer_size_spsv_L{};
    size_t buffer_size_spsv_U{};
    if (m_preconditioner == rocsparse_clients_preconditioner::ILU0 ||
	m_preconditioner == rocsparse_clients_preconditioner::JACOBI)
      {
	this->m_P->to_device();

	rocsparse_operation trans = rocsparse_operation_none;
#ifdef RAW_SV
	size_t buffer_size;
	this->m_buffer_size = 0;
	(rocsparse_dcsrsv_buffer_size(m_sparse_handle,
				      rocsparse_operation_none,		   
				      (int32_t)this->m_m,
				      (int32_t)this->m_nnz,
				      this->m_P_descr_L,
				      (const double*)this->m_P->val,
				      (const int32_t*)this->m_P->ptr,
				      (const int32_t*)this->m_P->ind,
				      this->m_P_info,
				      &buffer_size));
	this->m_buffer_size = std::max(this->m_buffer_size,buffer_size);
	(rocsparse_dcsrsv_buffer_size(m_sparse_handle,
				      rocsparse_operation_none,		   
				      (int32_t)this->m_m,
				      (int32_t)this->m_nnz,
				      this->m_P_descr_U,
				      (const double*)this->m_P->val,
				      (const int32_t*)this->m_P->ptr,
				      (const int32_t*)this->m_P->ind,
				      this->m_P_info,
				      &buffer_size));
	this->m_buffer_size = std::max(this->m_buffer_size,buffer_size);
	
#else
	ROCSPARSE_CHECK(rocsparse_spsv(this->m_sparse_handle,
				       rocsparse_operation_none,		   
				       this->m_one,
				       this->m_spmat_L,
				       this->m_dnvec_x,
				       this->m_dnvec_y,
				       this->m_datatype,
				       rocsparse_spsv_alg_default,
				       rocsparse_spsv_stage_buffer_size,
				       &buffer_size_spsv_L,
				       nullptr));
	ROCSPARSE_CHECK(rocsparse_spsv(this->m_sparse_handle,
				       rocsparse_operation_none,		   
				       this->m_one,
				       this->m_spmat_U,
				       this->m_dnvec_x,
				       this->m_dnvec_y,
				       this->m_datatype,
				       rocsparse_spsv_alg_default,
				       rocsparse_spsv_stage_buffer_size,
				       &buffer_size_spsv_U,
				       nullptr));

   this->m_buffer_size = std::max(std::max(buffer_size_spsv_L,
					   buffer_size_spsv_U),
				  this->m_buffer_size);
#endif
      }   
   
   HIP_CHECK(hipMalloc(&this->m_buffer,this->m_buffer_size));
   
   rocsparse_spmv(this->m_sparse_handle,		   
		   rocsparse_operation_none,		   
				      
		  this->m_one,
		  this->m_spmat_A,
		  this->m_dnvec_x,
		  this->m_zero,
		  this->m_dnvec_y,
		  this->m_datatype,
		  rocsparse_spmv_alg_default,
		  rocsparse_spmv_stage_preprocess,
		  &this->m_buffer_size,
		  this->m_buffer);
   

    if (m_preconditioner == rocsparse_clients_preconditioner::ILU0 ||
	m_preconditioner == rocsparse_clients_preconditioner::JACOBI)
      {
#ifdef RAW_SV

	(rocsparse_dcsrsv_analysis(m_sparse_handle,
				   rocsparse_operation_none,		   
				   (int32_t)this->m_m,
				   (int32_t)this->m_nnz,
				   this->m_P_descr_L,
				   (const double*)this->m_P->val,
				   (const int32_t*)this->m_P->ptr,
				   (const int32_t*)this->m_P->ind,
				   this->m_P_info,
				   rocsparse_analysis_policy_reuse,
				   rocsparse_solve_policy_auto, 
				   this->m_buffer));


    (rocsparse_dcsrsv_analysis(m_sparse_handle,
			       rocsparse_operation_none,
			       (int32_t)this->m_m,
			       (int32_t)this->m_nnz,
			       this->m_P_descr_U,
			       (const double*)this->m_P->val,
			       (const int32_t*)this->m_P->ptr,
			       (const int32_t*)this->m_P->ind,
			       this->m_P_info,
			       rocsparse_analysis_policy_reuse,
			       rocsparse_solve_policy_auto, 
			       this->m_buffer));
#else
	ROCSPARSE_CHECK(rocsparse_spsv(this->m_sparse_handle,
				  rocsparse_operation_none,
				  this->m_one,
				  this->m_spmat_L,
				  this->m_dnvec_x,
				  this->m_dnvec_y,
				  this->m_datatype,
				  rocsparse_spsv_alg_default,
				  rocsparse_spsv_stage_preprocess,
				  &this->m_buffer_size,
				  this->m_buffer));
   
   ROCSPARSE_CHECK(rocsparse_spsv(this->m_sparse_handle,
				  rocsparse_operation_none,
				  this->m_one,
				  this->m_spmat_U,
				  this->m_dnvec_x,
				  this->m_dnvec_y,
				  this->m_datatype,
				  rocsparse_spsv_alg_default,
				  rocsparse_spsv_stage_preprocess,
				  &this->m_buffer_size,
				  this->m_buffer));
#endif
      }   

  }
  
  void mv(void * in, void * out)
  {
    rocsparse_dnvec_set_values(this->m_dnvec_x, in);
    rocsparse_dnvec_set_values(this->m_dnvec_y, out);
if (0)    {
    rocsparse_clients_data_array_t tmpx(this->m_m,rocsparse_datatype_f64_r, in,rocsparse_pointer_mode_device);
    std::cout << "MV X ------ " << std::endl;
    tmpx.print();
    std::cout << "MV X DONE -" << std::endl;
    }
    rocsparse_spmv(this->m_sparse_handle,		   
		   rocsparse_operation_none,
		   this->m_one,
		   this->m_spmat_A,
		   this->m_dnvec_x,
		   this->m_zero,
		   this->m_dnvec_y,
		   this->m_datatype,
		   rocsparse_spmv_alg_default,
		   rocsparse_spmv_stage_compute,
		   &this->m_buffer_size,
		   this->m_buffer);
    if (0){
      rocsparse_clients_data_array_t tmpy(this->m_m,rocsparse_datatype_f64_r    , out,rocsparse_pointer_mode_device);
    std::cout << "MV Y ------ " << std::endl;
    tmpy.print();
    std::cout << "MV DONE --- " << std::endl;
    
    }
  }

  void sv(void * in, void * out)
  {
    if (0)
    {
    rocsparse_clients_data_array_t tmpx(this->m_m,rocsparse_datatype_f64_r, in,rocsparse_pointer_mode_device);
    std::cout << "SV X ------ " << std::endl;
    tmpx.print();
    std::cout << "SV X DONE -" << std::endl;
    }
    switch(this->m_preconditioner)
      {
      case rocsparse_clients_preconditioner::ILUK:
      case rocsparse_clients_preconditioner::NONE:
	{
	  HIP_CHECK(hipMemcpy(out,in,sizeof(double)*this->m_m, hipMemcpyDefault ));
	  break;
	}
	
      case rocsparse_clients_preconditioner::ILU0:
      case rocsparse_clients_preconditioner::JACOBI:
	{

#if 0
	  {	      double * p1 = new double[this->m_m];
	    hipMemcpy(p1,in, this->m_m*sizeof(double),hipMemcpyDefault);
	    for (int i=0;i<this->m_m;++i)
	      {
		std::cout << "in[" << i << "] = " <<p1[i] << std::endl;
	      }
	    delete[]p1;}
#endif
	  
#ifdef RAW_SV
	  (rocsparse_dcsrsv_solve(m_sparse_handle,
				  rocsparse_operation_none,
				  (int32_t)this->m_m,
				  (int32_t)this->m_nnz,
				  this->m_one,
				  this->m_P_descr_L,
				  (const double*)this->m_P->val,
				  (const int32_t*)this->m_P->ptr,
				  (const int32_t*)this->m_P->ind,
				  this->m_P_info,
				  (const double*)in,
				  (double*)this->m_tmp[0],
				  rocsparse_solve_policy_auto, 
				  this->m_buffer));
	  
	  (rocsparse_dcsrsv_solve(m_sparse_handle,
				  rocsparse_operation_none,
				  (int32_t)this->m_m,
				  (int32_t)this->m_nnz,
				  this->m_one,
				  this->m_P_descr_U,
				  (const double*)this->m_P->val,
				  (const int32_t*)this->m_P->ptr,
				  (const int32_t*)this->m_P->ind,
				  this->m_P_info,
				  (const double*)this->m_tmp[0],
				  (double*)out,
				  rocsparse_solve_policy_auto, 
				  this->m_buffer));
#else
	  
	  rocsparse_dnvec_set_values(this->m_dnvec_x, in);
	  rocsparse_dnvec_set_values(this->m_dnvec_y, out);
	  ROCSPARSE_CHECK(rocsparse_spsv(this->m_sparse_handle,
					 rocsparse_operation_none,
					 this->m_one,
					 this->m_spmat_L,
					 this->m_dnvec_x,
					 this->m_dnvec_tmp,
					 this->m_datatype,					 
					 rocsparse_spsv_alg_default,
					 rocsparse_spsv_stage_compute,
					 &this->m_buffer_size,
					 this->m_buffer));    
	  
	  ROCSPARSE_CHECK(rocsparse_spsv(this->m_sparse_handle,
					 rocsparse_operation_none,
					 this->m_one,
					 this->m_spmat_U,
					 this->m_dnvec_tmp,
					 this->m_dnvec_y,
					 this->m_datatype,					 
					 rocsparse_spsv_alg_default,
					 rocsparse_spsv_stage_compute,
					 &this->m_buffer_size,
					 this->m_buffer));
#endif
#if 0
	  	      {	      double * p1 = new double[this->m_m];
	      hipMemcpy(p1,out, this->m_m*sizeof(double),hipMemcpyDefault);
	      for (int i=0;i<this->m_m;++i)
		{
		  std::cout << "out[" << i << "] = " <<p1[i] << std::endl;
		}
	      delete[]p1;}
#endif
	  break;
	}
	
      }
    if (0){        rocsparse_clients_data_array_t tmpy(this->m_m,rocsparse_datatype_f64_r    , out,rocsparse_pointer_mode_device);
    std::cout << "SV Y ------ " << std::endl;
    tmpy.print();
    std::cout << "SV DONE --- " << std::endl;}

  }
  
  void create_L(rocsparse_handle handle,
		rocsparse_spmat_descr * L)
  {
    rocsparse_create_csr_descr(L,
			       this->m_m,
			       this->m_m,
			       this->m_nnz,
			       (void*)this->m_P->ptr,
			       (void*)this->m_P->ind,
			       (void*)this->m_P->val,
			       rocsparse_indextype_i32,
			       rocsparse_indextype_i32,
			       rocsparse_index_base_zero,
			       rocsparse_datatype_f64_r);
    rocsparse_fill_mode fill_mode = rocsparse_fill_mode_lower;
    rocsparse_spmat_set_attribute(L[0],
				  rocsparse_spmat_fill_mode,
				  &fill_mode,
				  sizeof(fill_mode));
    
    rocsparse_diag_type diag_type = rocsparse_diag_type_unit;
    rocsparse_spmat_set_attribute(L[0],
				  rocsparse_spmat_diag_type,
				  &diag_type,
				  sizeof(diag_type));

  }


  void create_U(rocsparse_handle handle,
		rocsparse_spmat_descr * U)
  {
    rocsparse_create_csr_descr(U,
			       this->m_m,
			       this->m_m,
			       this->m_nnz,
			       (void*)this->m_P->ptr,
			       (void*)this->m_P->ind,
			       (void*)this->m_P->val,
			       rocsparse_indextype_i32,
			       rocsparse_indextype_i32,
			       rocsparse_index_base_zero,
			       rocsparse_datatype_f64_r);
    const rocsparse_fill_mode fill_mode = rocsparse_fill_mode_upper;
    rocsparse_spmat_set_attribute(U[0],
				  rocsparse_spmat_fill_mode,
				  &fill_mode,
				  sizeof(fill_mode));
    
    const rocsparse_diag_type diag_type = rocsparse_diag_type_non_unit;
    rocsparse_spmat_set_attribute(U[0],
				  rocsparse_spmat_diag_type,
				  &diag_type,
				  sizeof(diag_type));    
  }

};












typedef struct rocsparse_clients_graph_dimension_t
{
  int64_t m {-1};
  int64_t n {-1};
} *rocsparse_clients_graph_dimension;

typedef struct rocsparse_clients_graph_coo_t
{
  rocsparse_clients_graph_dimension_t dimension;
  rocsparse_clients_index_array row_ind{};
  rocsparse_clients_index_array col_ind{};
  int64_t nnz {-1};    
} * rocsparse_clients_graph_coo;

typedef struct rocsparse_clients_graph_coo_aos_t
{
  rocsparse_clients_graph_dimension_t dimension;
  rocsparse_clients_index_array ind{};
  int64_t nnz {-1};    
} * rocsparse_clients_graph_coo_aos;

typedef struct rocsparse_clients_graph_csr_t
{
  rocsparse_clients_graph_dimension_t dimension;
  rocsparse_clients_index_array row_ptr{};
  rocsparse_clients_index_array col_ind{};
  int64_t nnz {-1};    
} *rocsparse_clients_graph_csr;

typedef struct rocsparse_clients_graph_csc_t
{
  rocsparse_clients_graph_dimension_t dimension;
  rocsparse_clients_index_array row_ind{};
  rocsparse_clients_index_array col_ptr{};
  int64_t m {-1};
  int64_t n {-1};
  int64_t nnz {-1};    
} *rocsparse_clients_graph_csc;

typedef union rocsparse_clients_graph_union_t
{
  rocsparse_clients_graph_dimension_t dimension;
  rocsparse_clients_graph_csr_t csr;
  rocsparse_clients_graph_csc_t csc;
  rocsparse_clients_graph_coo_t coo;
  rocsparse_clients_graph_coo_aos_t coo_aos;
} * rocsparse_clients_graph_union;

typedef union rocsparse_clients_graph_t
{
  rocsparse_format format;
  rocsparse_clients_graph_union_t u;
} * rocsparse_clients_graph;

struct rocsparse_clients_sparse_matrix_t
{
  rocsparse_clients_graph graph{};
  rocsparse_clients_data_array val{};

  rocsparse_clients_sparse_matrix_t()
  {
  }
  rocsparse_clients_sparse_matrix_t(rocsparse_clients_graph g)
  {
  }
  
#if 0
  rocsparse_clients_sparse_csr(int64_t m_,
			       int64_t n_,
			       int64_t nnz_,				  
			       rocsparse_indextype indextype,
			       rocsparse_datatype datatype)
    :    m(m_),
	 n(n_),
	 nnz(nnz_),
	 ptr( (m_>0) ? m_ :0,
	     (nnz_ > std::numeric_limits<int32_t>::max())
	? rocsparse_indextype_i64
	: rocsparse_indextype_i32),    
	 ind(nnz_,
	     indextype),
	 
	 val(nnz_,
	     datatype),
	 
	 x(n,
	   datatype),
	 
	 b(m,
	   datatype)
  {
  }
#endif  
};

int main(int argc, const char ** argv)
  {
    rocsparse_clients_sparse_matrix_t S; 
    rocsparse_clients_generate_alg alg = (rocsparse_clients_generate_alg)-1;
    rocsparse_clients_preconditioner kind = (rocsparse_clients_preconditioner)-1;
    rocsparse_clients_iterative_method kind_iterative_method = (rocsparse_clients_iterative_method)-1;

    const char * ifilename = nullptr;
    const char * b_ifilename = nullptr;
    const char * ofilename = nullptr;
    
    for (int i=0;i<argc;++i)
      {
	std::cout << "> " <<  argv[i]<<std::endl;
      }
    for (int i=1;i<argc;++i)
      {
	if (!strcmp(argv[i],"-o"))
	  {
	    if (i+1 >= argc)
	      {
		std::cerr << "missing argument for -o "<<std::endl;
		return 1;
	      }
	    ofilename = argv[i+1];
	    for (int j=i;j+2<argc;++j)
	      argv[j] = argv[j+2];	    
	    argc-=2;
	    break;
	  }
      }
    for (int i=1;i<argc;++i)
      {
	if (!strcmp(argv[i],"-b"))
	  {
	    if (i+1 >= argc)
	      {
		std::cerr << "missing argument for -b "<<std::endl;
		return 1;
	      }
	    b_ifilename = argv[i+1];
	    for (int j=i;j+2<argc;++j)
	      argv[j] = argv[j+2];	    
	    argc-=2;
	    break;
	  }
      }


    for (int i=1;i<argc;++i)
      {
    	if (!strcmp(argv[i],"-m"))
	  {
	    if (i+1 >= argc)
	      {
		std::cerr << "missing argument for -m"<<std::endl;
		return 1;
	      }
	    kind_iterative_method = rocsparse_clients_iterative_method(argv[i+1]);
	    if (kind_iterative_method.is_invalid())
	      {
		std::cerr << "invalid argument for -m: '"<< argv[i+1] << "'"<<std::endl;
		std::cerr  << "list of valid arguments is:" << std::endl;
		for (const auto&name : rocsparse_clients_iterative_method::names)
		  {
		    std::cerr  << " - '" << name << "'"<< std::endl;
		  }
		return rocsparse_status_invalid_value;
	      }

	    for (int j=i;j+2<argc;++j)
	      argv[j] = argv[j+2];	    
	    argc-=2;
	    --i;
	    break;
	  }
      }
    
	if (kind_iterative_method.is_invalid())
	  {
	    std::cerr << "missing -m"  << std::endl;
	    exit(1);
	  }
    
    for (int i=1;i<argc;++i)
      {
	if (!strcmp(argv[i],"-a"))
	  {
	    if (i+1 >= argc)
	      {
		std::cerr << "missing argument for -a "<<std::endl;
		return 1;
	      }
	    alg = (rocsparse_clients_generate_alg)atoi(argv[i+1]);
	    for (int j=i;j+2<argc;++j)
	      argv[j] = argv[j+2];	    
	    argc-=2;
	    --i;
	    break;
	  }
      }
    for (int i=1;i<argc;++i)
      {

	if (!strcmp(argv[i],"-p"))
	  {
	    if (i+1 >= argc)
	      {
		std::cerr << "missing argument for -p"<<std::endl;
		return 1;
	      }
	    kind = rocsparse_clients_preconditioner(argv[i+1]);
	    if (kind.is_invalid())
	      {
		std::cerr << "invalid argument for -p: '"<< argv[i+1] << "'"<<std::endl;
		std::cerr  << "list of valid arguments is:" << std::endl;
		for (const auto&name : rocsparse_clients_preconditioner::names)
		  {
		    std::cerr  << " - '" << name << "'"<< std::endl;
		  }
		return rocsparse_status_invalid_value;
	      }

	    for (int j=i;j+2<argc;++j)
	      argv[j] = argv[j+2];	    
	    argc-=2;
	    --i;
	    break;
	  }
	
      }
  
    
    if (kind.is_invalid())
      {
	kind = rocsparse_clients_preconditioner::NONE;
      }
    if (argc==1+1)
      {
	ifilename = argv[1];
	std::cout << "ifilename " << ifilename << std::endl;
	if (alg == (rocsparse_clients_generate_alg)-1)
	  {
	    alg = rocsparse_clients_generate_alg_file;
	  }
      }
    else
      {
	if (alg == (rocsparse_clients_generate_alg)-1)
	  {
	    alg = rocsparse_clients_generate_alg_tridiagonal;
	  }
	else if (alg == rocsparse_clients_generate_alg_file)
	  {
	    std::cout << "require only one argument " << std::endl;
	    for (int i=1;i<argc;++i)
	      std::cout << "! " <<  argv[i]<<std::endl;
	    return 1;
	  }
      }

    rocsparse_clients_csr_matrix A = new rocsparse_clients_csr_matrix_t();

    switch(alg)
      {
      case rocsparse_clients_generate_alg_file:
	{
	  const rocsparse_clients_generate_file_seed_ seed{ ifilename };
	  A[0] >> seed;
	  break;
	}
      case rocsparse_clients_generate_alg_tridiagonal:
	{
	  
	  const rocsparse_clients_generate_tridiagonal_seed_ seed{ 4 };
	  A[0] >> seed;
	  break;
	}
      }
    
    const int64_t m = A->m;
    const int64_t n = A->n;
    auto& ptr = A->ptr;
    auto& ind = A->ind;
    auto& val = A->val;

    rocsparse_clients_data_array_t x(n,val.get_datatype());
    rocsparse_clients_data_array_t b(m,val.get_datatype());
    
    //    if (b_ifilename)
    //      b >> b_ifilename;

#if 0
    {
      double * tmp = b;
      tmp[0] = 5;
      tmp[1] = 7;
      tmp[2] = 15;
      tmp[3] = 10;

    }

    {
      double * tmp = val;
      tmp[0] = 4;
      tmp[1] = 1;
      tmp[2] = 1;
      tmp[3] = 3;
      tmp[4] = 2;
      tmp[5] = 2;
      tmp[6] = 7;
      tmp[7] = 1;
      tmp[8] = 1;
      tmp[9] = 5;
    }
#endif
    bool ss = A->is_sym();
    std::cout << "symmetric : " << ss << std::endl;
    for (int i=1;i<32;++i)
        spy_terminal(m,
		     n,
		 ind.size(),
		 ptr,
		 ind,
		 i);
    exit(1);

    
    x.to_device();
    b.to_device();
    A->to_device();
    
    int64_t maxIters = n + 1;
    double tol   = 1e-12;
    //    A->info();
    //    ptr.print();
    //    ind.print();
    //    val.print();
    rocsparse_sparse_operators_ op(kind,
				   A,
				   rocsparse_index_base_zero,
				   b,
				   x);

    //
    // Generate b.
    // 
    //    x.one();    
    //    op.mv(x, b);    
    x.zero();
    rocsparse_itsol_descr itsol_descr{};
    
    rocsparse_create_itsol_descr(&itsol_descr);


    const auto datatype = val.get_datatype();
    rocsparse_itsol_set_input(itsol_descr,
				  rocsparse_itsol_input_datatype_rhs,
				  &datatype,
				  sizeof(datatype));
    rocsparse_itsol_set_input(itsol_descr,
				  rocsparse_itsol_input_datatype_sol,
				  &datatype,
				  sizeof(datatype));
    rocsparse_itsol_set_input(itsol_descr,
				  rocsparse_itsol_input_datatype_compute,
				  &datatype,
				  sizeof(datatype));
  
    rocsparse_itsol_set_input(itsol_descr,
				  rocsparse_itsol_input_dimension,
				  &m,
				  sizeof(m));
    
    rocsparse_itsol_set_input(itsol_descr,
			       rocsparse_itsol_input_nmaxiter,
			       &maxIters,
			       sizeof(maxIters));
    
    rocsparse_itsol_set_input(itsol_descr,
			       rocsparse_itsol_input_tolerance,
			       &tol,
			       sizeof(tol));
    
    rocsparse_itsol_set_input(itsol_descr,
			      rocsparse_itsol_input_alg,
			      &kind_iterative_method,
			      sizeof(alg));
    size_t buffer_size;
    void * buffer;
    rocsparse_itsol_buffer_size(itsol_descr,
				 &buffer_size);
    
    HIP_CHECK(hipMalloc(&buffer, buffer_size));
    
    // 2) RCI iteration loop
    // We'll store matvec result in a temporary array
    //    b.print("bbbbb ");
    //    x.print("xxxxx");
    rocsparse_itsol_request request;
    do
      {
	// Pass the result back to the solver
	rocsparse_itsol(itsol_descr,
			 b,
			 x,
			 buffer_size,
			 buffer);
	
	rocsparse_itsol_get_request(itsol_descr, &request);	
	switch(request)
	  {
	    
	  case rocsparse_itsol_request_matrix_vector:
	    {
	      void * in{};
	      void * out{};	
	      rocsparse_itsol_get_request_input(itsol_descr, &in);
	      rocsparse_itsol_get_request_output(itsol_descr, &out);
	      
	      op.mv(in, out);
	      break;
	    }
	  case rocsparse_itsol_request_preconditioner:
	    {
	      void * in{};
	      void * out{};	
	      rocsparse_itsol_get_request_input(itsol_descr, &in);
	      rocsparse_itsol_get_request_output(itsol_descr, &out);	      
	      op.sv(in, out);
	      break;
	    }
	  case rocsparse_itsol_request_finished:
	    {
	      int64_t niter,nmaxiter;
	      rocsparse_itsol_get_output(itsol_descr,
					 rocsparse_itsol_output_niter,
					 &niter,
					 sizeof(niter));
	      rocsparse_itsol_get_input(itsol_descr,
					 rocsparse_itsol_input_nmaxiter,
					 &nmaxiter,
					 sizeof(nmaxiter));
	      if(niter >= nmaxiter)
		{
		  std::cout << "ITSOL did not reach convergence within max iterations.\n";
		}
	      else
		{
		  std::cout << "ITSOL reached convergence within "<< niter << " iterations.\n";
		}
	      break;
	    }
	  case rocsparse_itsol_request_error:
	    {
	      std::cout << "ITSOL error.\n";
	      break;
	    }
	  }	
      } while(request != rocsparse_itsol_request_finished);

    HIP_CHECK(hipFree(buffer));

    rocsparse_destroy_itsol_descr(itsol_descr);
    x.to_host();
    double * xx = x;
    double s=0;
    for (int i=0;i<m;++i)
      {
	s=std::max(s,(double(1)-xx[i]));
      }
    std::cout << "mx : " << s << std::endl;
    if (ofilename)
      x << ofilename;
    //    x.print("X",10);
    return 0;
  }
