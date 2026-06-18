#pragma once

#include "internal/itsol/rocsparse_itsol_fgmres.h"
#include "rocsparse_itsol_fgmres_inputs.hpp"
#include "rocsparse_itsol_fgmres_outputs.hpp"
#include "../rocsparse_itsol.hpp"
#include "../rocsparse_scalar.hpp"

typedef enum rocsparse_fgmres_internal_state_ {
  rocsparse_fgmres_internal_state_initial,
  rocsparse_fgmres_internal_state_init_mv,
  rocsparse_fgmres_internal_state_iter_mv,
  rocsparse_fgmres_internal_state_iter_mv_1,
  rocsparse_fgmres_internal_state_iter_mv_2,
  rocsparse_fgmres_internal_state_iter_prec,
  rocsparse_fgmres_internal_state_start
} rocsparse_fgmres_internal_state;
#include <iostream>

struct rocsparse_itsol_fgmres_descr_  : public rocsparse_itsol_impl_
{
private:
  rocsparse_fgmres_outputs m_outputs{};
  rocsparse_fgmres_inputs m_inputs{};
  rocsparse_itsol_descr m_itsol{};
protected:
  rocsparse_itsol_descr get_itsol() {return m_itsol;}
  rocsparse_fgmres_inputs get_inputs() {return m_inputs;}
  rocsparse_status request(rocsparse_itsol_request request_,
			   void * in_,
			   void * out_,
			   rocsparse_fgmres_internal_state state_)
  {
    this->get_itsol()->set_request_input(in_);
    this->get_itsol()->set_request_output(out_);
    this->get_itsol()->set_request(request_);
    this->m_internal_state = state_;
    return rocsparse_status_success;
  }
public:
  
  rocsparse_fgmres_outputs get_outputs() {return m_outputs;}

  void set_itsol(rocsparse_itsol_descr value) {m_itsol = value;}
  void set_inputs(rocsparse_fgmres_inputs value) {m_inputs= value;}
  void set_outputs(rocsparse_fgmres_outputs value) {m_outputs= value;}

  rocsparse_fgmres_internal_state m_internal_state{rocsparse_fgmres_internal_state_initial};

  int32_t arnoldi_iter{};
  bool m_initialized{};

  void * m_c{};
  void  *  m_s{};
  void  *  m_r{};
  void  *  m_H{};  
  void  *  m_v{};  
  void  *  m_z{};  
  
  rocsparse_scalar_ m_one{};
  rocsparse_scalar_ m_negative_one{};

  void * get_c();
  void * get_s();
  void * get_r();
  void * get_H();
  void * get_v();
  void * get_z();
  
  rocsparse_status set_input(rocsparse_itsol_fgmres_input input,
			     const void * data,
			     size_t size);

  rocsparse_status get_input(rocsparse_itsol_fgmres_input input,
			     void * data,
			     size_t size);
  
  rocsparse_status set_output(rocsparse_itsol_fgmres_output output,
			     const void * data,
			     size_t size);
  
  rocsparse_status get_output(rocsparse_itsol_fgmres_output output,
			      void * data,
			      size_t size);

  virtual rocsparse_status buffer_size(rocsparse_handle handle, rocsparse_itsol_descr  descr,
				       size_t*buffer_size);
  
  virtual rocsparse_status run(rocsparse_handle handle, rocsparse_itsol_descr descr,
			       const void * b,
			       void * x,	    
			       size_t buffer_size,
			       void * buffer);
  
  rocsparse_itsol_fgmres_descr_(rocsparse_itsol_descr descr);
  
  ~rocsparse_itsol_fgmres_descr_();
  
  
};


