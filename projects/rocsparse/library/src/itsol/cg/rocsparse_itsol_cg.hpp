#pragma once

#include "internal/itsol/rocsparse_itsol_cg.h"
#include "rocsparse_itsol_cg_inputs.hpp"
#include "rocsparse_itsol_cg_outputs.hpp"
#include "../rocsparse_itsol.hpp"
#include "../rocsparse_scalar.hpp"

typedef enum rocsparse_cg_internal_state_ {
  rocsparse_cg_internal_state_initial,
  rocsparse_cg_internal_state_init_mv,
  rocsparse_cg_internal_state_init_prec,
  rocsparse_cg_internal_state_iter_mv,
  rocsparse_cg_internal_state_iter_prec,
  rocsparse_cg_internal_state_post_iteration
} rocsparse_cg_internal_state;

struct rocsparse_itsol_cg_descr_  : public rocsparse_itsol_impl_
{
private:
  rocsparse_cg_outputs m_outputs{};
  rocsparse_cg_inputs m_inputs{};
  rocsparse_itsol_descr m_itsol{};
protected:
  rocsparse_itsol_descr get_itsol() {return this->m_itsol;}
  rocsparse_cg_inputs get_inputs() {return this->m_inputs;}
public:
  
  rocsparse_cg_outputs get_outputs() {return this->m_outputs;}

  void set_itsol(rocsparse_itsol_descr value) {this->m_itsol = value;}
  void set_inputs(rocsparse_cg_inputs value) {this->m_inputs= value;}
  void set_outputs(rocsparse_cg_outputs value) {this->m_outputs= value;}

  //  rocsparse_cg_internal_state m_internal_state{rocsparse_cg_internal_state_initial};
    rocsparse_cg_internal_state m_internal_state;
  bool m_initialized{};
  void * r{};
  void * p{};  

  rocsparse_scalar_ m_r_z{};
  rocsparse_scalar_ m_nrm2{};
  rocsparse_scalar_ m_pAp{};
  rocsparse_scalar_ m_scalar{};
  rocsparse_scalar_ m_next_r_z{};
  rocsparse_scalar_ m_alpha{};
  rocsparse_scalar_ m_beta{};
  rocsparse_scalar_ m_one{};
  rocsparse_scalar_ m_negative_one{};
  void * get_r();
  void * get_p();
  
  rocsparse_status set_input(rocsparse_itsol_cg_input input,
			     const void * data,
			     size_t size);

  rocsparse_status get_input(rocsparse_itsol_cg_input input,
			     void * data,
			     size_t size);
  
  rocsparse_status set_output(rocsparse_itsol_cg_output output,
			     const void * data,
			     size_t size);
  
  rocsparse_status get_output(rocsparse_itsol_cg_output output,
			      void * data,
			      size_t size);

  virtual rocsparse_status buffer_size(rocsparse_handle handle, rocsparse_itsol_descr  descr,
				       size_t*buffer_size);
  
  virtual rocsparse_status run(rocsparse_handle handle, rocsparse_itsol_descr descr,
			       const void * b,
			       void * x,	    
			       size_t buffer_size,
			       void * buffer);
  
  rocsparse_itsol_cg_descr_(rocsparse_itsol_descr descr);
  
  ~rocsparse_itsol_cg_descr_();
  
  
};


