#pragma once

#include "internal/itsol/rocsparse_itsol_bicgstab.h"
#include "rocsparse_itsol_bicgstab_inputs.hpp"
#include "rocsparse_itsol_bicgstab_outputs.hpp"
#include "../rocsparse_itsol.hpp"
#include "../rocsparse_scalar.hpp"

typedef enum rocsparse_bicgstab_internal_state_ {
  rocsparse_bicgstab_internal_state_initial,
  rocsparse_bicgstab_internal_state_init_mv,
  rocsparse_bicgstab_internal_state_init_prec,
  rocsparse_bicgstab_internal_state_iter_mv,
  rocsparse_bicgstab_internal_state_iter_mv_1,
  rocsparse_bicgstab_internal_state_iter_prec,
  rocsparse_bicgstab_internal_state_post_iteration,
  rocsparse_bicgstab_internal_state_finalize_break
} rocsparse_bicgstab_internal_state;


struct rocsparse_itsol_bicgstab_descr_  : public rocsparse_itsol_impl_
{
private:
  rocsparse_bicgstab_outputs m_outputs{};
  rocsparse_bicgstab_inputs m_inputs{};
  rocsparse_itsol_descr m_itsol{};
protected:
  rocsparse_itsol_descr get_itsol() {return m_itsol;}
  rocsparse_bicgstab_inputs get_inputs() {return m_inputs;}
  rocsparse_status request(rocsparse_itsol_request request_,
			   void * in_,
			   void * out_,
			   rocsparse_bicgstab_internal_state state_)
  {
    this->get_itsol()->set_request_input(in_);
    this->get_itsol()->set_request_output(out_);
    this->get_itsol()->set_request(request_);
    this->m_internal_state = state_;
    return rocsparse_status_success;
  }
public:
  
  rocsparse_bicgstab_outputs get_outputs() {return m_outputs;}

  void set_itsol(rocsparse_itsol_descr value) {m_itsol = value;}
  void set_inputs(rocsparse_bicgstab_inputs value) {m_inputs= value;}
  void set_outputs(rocsparse_bicgstab_outputs value) {m_outputs= value;}

  rocsparse_bicgstab_internal_state m_internal_state{rocsparse_bicgstab_internal_state_initial};
  
  bool m_initialized{};
  void * r0{};
  void * r{};
  void * p{};
  void * q{};  
  void * t{};  
  void * v{};  
  void * z{};  
  
  rocsparse_scalar_ m_alpha{};
  rocsparse_scalar_ m_alpha_nom{};
  rocsparse_scalar_ m_alpha_denom{};
  rocsparse_scalar_ m_beta{};
  rocsparse_scalar_ m_beta_nom{};
  rocsparse_scalar_ m_beta_denom{};
  rocsparse_scalar_ m_omega{};
  rocsparse_scalar_ m_omega_nom{};
  rocsparse_scalar_ m_omega_denom{};
  rocsparse_scalar_ m_rho{};
  rocsparse_scalar_ m_rho_old{};
  rocsparse_scalar_ m_scalar{};
  rocsparse_scalar_ m_one{};
  rocsparse_scalar_ m_negative_one{};
  
  void * get_r0();
  void * get_q();
  void * get_r();
  void * get_p();
  void * get_t();  
  void * get_v();
  void * get_z();
  
  rocsparse_status set_input(rocsparse_itsol_bicgstab_input input,
			     const void * data,
			     size_t size);

  rocsparse_status get_input(rocsparse_itsol_bicgstab_input input,
			     void * data,
			     size_t size);
  
  rocsparse_status set_output(rocsparse_itsol_bicgstab_output output,
			     const void * data,
			     size_t size);
  
  rocsparse_status get_output(rocsparse_itsol_bicgstab_output output,
			      void * data,
			      size_t size);

  virtual rocsparse_status buffer_size(rocsparse_handle handle, rocsparse_itsol_descr  descr,
				       size_t*buffer_size);
  
  virtual rocsparse_status run(rocsparse_handle handle, rocsparse_itsol_descr descr,
			       const void * b,
			       void * x,	    
			       size_t buffer_size,
			       void * buffer);
  
  rocsparse_itsol_bicgstab_descr_(rocsparse_itsol_descr descr);
  
  ~rocsparse_itsol_bicgstab_descr_();
  
  
};


