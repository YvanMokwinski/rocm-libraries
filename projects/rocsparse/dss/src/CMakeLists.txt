# ########################################################################
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# ########################################################################

# hipDSS source
set(hipdss_source
  src/c_api/hipdssConfigCreate.cpp
  src/c_api/hipdssConfigDestroy.cpp
  src/c_api/hipdssConfigGet.cpp
  src/c_api/hipdssConfigSet.cpp
  src/c_api/hipdssCreate.cpp
  src/c_api/hipdssCreateMg.cpp
  src/c_api/hipdssDataCreate.cpp
  src/c_api/hipdssDataDestroy.cpp
  src/c_api/hipdssDataGet.cpp
  src/c_api/hipdssDataSet.cpp
  src/c_api/hipdssDestroy.cpp
  src/c_api/hipdssExecute.cpp
  src/c_api/hipdssGetDeviceMemHandler.cpp
  src/c_api/hipdssGetPinnedMemHandler.cpp
  src/c_api/hipdssGetProperty.cpp
  src/c_api/hipdssLoggerForceDisable.cpp
  src/c_api/hipdssLoggerOpenFile.cpp
  src/c_api/hipdssLoggerSetCallback.cpp
  src/c_api/hipdssLoggerSetFile.cpp
  src/c_api/hipdssLoggerSetLevel.cpp
  src/c_api/hipdssLoggerSetMask.cpp
  src/c_api/hipdssMatrixCreateBatchCsr.cpp
  src/c_api/hipdssMatrixCreateBatchDn.cpp
  src/c_api/hipdssMatrixCreateCsr.cpp
  src/c_api/hipdssMatrixCreateDn.cpp
  src/c_api/hipdssMatrixDestroy.cpp
  src/c_api/hipdssMatrixGetBatchCsr.cpp
  src/c_api/hipdssMatrixGetBatchDn.cpp
  src/c_api/hipdssMatrixGetCsr.cpp
  src/c_api/hipdssMatrixGetDistributionRow1d.cpp
  src/c_api/hipdssMatrixGetDn.cpp
  src/c_api/hipdssMatrixGetFormat.cpp
  src/c_api/hipdssMatrixSetBatchCsrPointers.cpp
  src/c_api/hipdssMatrixSetBatchValues.cpp
  src/c_api/hipdssMatrixSetCsrPointers.cpp
  src/c_api/hipdssMatrixSetDistributionRow1d.cpp
  src/c_api/hipdssMatrixSetValues.cpp
  src/c_api/hipdssSetCommLayer.cpp
  src/c_api/hipdssSetDeviceMemHandler.cpp
  src/c_api/hipdssSetJitCacheDir.cpp
  src/c_api/hipdssSetMgStreams.cpp
  src/c_api/hipdssSetPinnedMemHandler.cpp
  src/c_api/hipdssSetStream.cpp
  src/c_api/hipdssSetThreadingLayer.cpp
  ####
  src/rf/hipdss_rf_descr.cpp
  src/rf/hipdss_rf_descr_create.cpp
  src/rf/hipdss_rf_descr_destroy.cpp
  src/rf/hipdss_rf.cpp
  src/rf/hipdss_rf_set_config.cpp
  src/rf/hipdss_rf_set_input.cpp # empty
  src/rf/hipdss_rf_host_symbolic_factor_calculation.cpp
  src/rf/hipdss_rf_symbolic_factorization.cpp
  src/rf/hipdss_rf_factorization.cpp
  src/rf/hipdss_rf_refactorization.cpp
  src/rf/hipdss_rf_reordering.cpp
  src/rf/hipdss_rf_scaling.cpp
  src/rf/hipdss_rf_solve.cpp
  src/rf/hipdss_rf_solve_diagonal.cpp
  src/rf/hipdss_rf_solve_forward.cpp
  src/rf/hipdss_rf_solve_backward.cpp
  src/rf/hipdss_rf_info.cpp
  ####
  ####
  src/spic0/hipdss_spic0.cpp
  src/spic0/hipdss_spic0_config_t.cpp
  ####
  src/spildlt0/hipdss_spildlt0.cpp
  src/spildlt0/hipdss_spildlt0_config_t.cpp
  ####
  src/spilu0/hipdss_spilu0.cpp
  src/spilu0/hipdss_spilu0_config_t.cpp
  ####
  src/spmat_info/hipdss_spmat_info.cpp
  ####
  src/hipdssext/hipdssextMatrixImportConfigCreate.cpp
  src/hipdssext/hipdssextMatrixImportConfigDestroy.cpp
  src/hipdssext/hipdssextMatrixImportConfig.cpp
  src/hipdssext/hipdssextMatrixImportConfigFileSet.cpp
  src/hipdssext/hipdssextMatrixImport.cpp
  src/hipdssext/hipdssextInfo.cpp
  ####
  src/sptranspose/hipdss_sptranspose.cpp
  ####
  src/spreordering/hipdss_spreordering.cpp
  src/spreordering/rcmk/hipdss_spreordering_rcmk.cpp
  src/spreordering/nd_metis/hipdss_spreordering_nd_metis.cpp
  ####
  src/spequilibrate/hipdss_spequilibrate_alg.cpp
  src/spequilibrate/hipdss_spequilibrate_stage.cpp
  src/spequilibrate/hipdss_spequilibrate_input.cpp
  src/spequilibrate/hipdss_spequilibrate_output.cpp
  src/spequilibrate/hipdss_spequilibrate_descr.cpp
  src/spequilibrate/hipdss_spequilibrate_get_output.cpp
  src/spequilibrate/hipdss_spequilibrate_set_input.cpp
  src/spequilibrate/hipdss_spequilibrate_buffer_size.cpp
  src/spequilibrate/hipdss_spequilibrate.cpp
  src/spequilibrate/ruiz/hipdss_spequilibrate_ruiz.cpp

  #  src/spequilibrate/rocdss_spmat_scale.cpp
  #  src/spequilibrate/rocdss_dnvec_rsqrt.cpp
  #  src/spequilibrate/rocdss_dnvec_hadamard.cpp
  #  src/spequilibrate/rocdss_dnvec_inv.cpp
  #
  src/sptrsm/hipdss_sptrsm.cpp
  src/sptrsm/hipdss_sptrsm_config_t.cpp
  #
  src/common/hipdss_idvec_descr.cpp
  src/common/hipdss_working_row_t.cpp
  src/common/hipdss_timer.cpp
  src/common/rocsparse_indextype_utils.cpp
  src/common/rocsparse_datatype_utils.cpp
  src/common/hipdss_datatype_utils.cpp
  src/common/hipdss_mem_handler.cpp
  src/common/hipdss_envariables.cpp
  src/common/hipdss_debug.cpp
  src/common/hipdss_argdescr.cpp
  src/common/hipdss_message.cpp
  #
  src/common/hipdssMatrixViewType_t.cpp
  src/common/hipdssIndexBase_t.cpp
  src/common/hipdssLayout_t.cpp
  src/common/hipdssMatrixType_t.cpp
  src/common/hipdssConfigParam_t.cpp
  src/common/hipdssDataType_t.cpp
  src/common/hipdssSolveMode_t.cpp
  src/common/hipdssMemoryEstimateState_t.cpp
  src/common/hipdssDowncastFactorsMode_t.cpp
  src/common/hipdssMemoryOptimizationFlags_t.cpp
  src/common/hipdssDataParam_t.cpp
  src/common/hipdssPhase_t.cpp
  src/common/hipdssStatus_t.cpp
  src/common/hipdssReorderingAlg_t.cpp
  src/common/hipdssFactorizationAlg_t.cpp
  src/common/hipdssPivotEpsilonAlg_t.cpp
  src/common/hipdssSolveAlg_t.cpp
  src/common/hipdssMatchingAlg_t.cpp
  src/common/hipdssPivotType_t.cpp
  src/common/hipdssMatrixFormat_t.cpp
  src/common/libraryPropertyType.cpp
  src/common/hipdssContext.cpp
  src/common/hipdssMatrix.cpp
  src/common/hipdssMatrixRowPermutation.cpp
  src/common/hipdssMatrixSpyTerminal.cpp
  src/common/hipdssMatrixPrint.cpp
  src/common/hipdssConfig.cpp
  #
  src/conversion/hipDataType2rocsparse_datatype.cpp
  src/conversion/hipDataType2rocsparse_indextype.cpp
  src/conversion/rocsparse_indextype2hipDataType.cpp
  src/conversion/rocsparse_datatype2hipDataType.cpp
  #
  src/conversion/hipdssIndexBase_t2rocsparse_index_base.cpp
  src/conversion/rocsparse_index_base2hipdssIndexBase_t.cpp
  src/conversion/hipdssLayout_t2rocsparse_order.cpp
  src/conversion/hipdssMatrixType_t2rocsparse.cpp
  src/conversion/hipdssMatrixViewType_t2rocsparse.cpp
  #
  src/conversion/rocsparse_order2hipdssLayout_t.cpp
  src/conversion/rocsparse2hipdssMatrixType_t.cpp
  src/conversion/rocsparse2hipdssMatrixViewType_t.cpp
  src/conversion/hipdss_phase2hipdss_rf_stage.cpp

)
