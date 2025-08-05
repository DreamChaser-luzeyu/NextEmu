#include <climits>

#include "sdk/symbol_attr.h"

//#include "module_manifest.hpp"
#include "rvcore_minicpu/MiniCPUCore.h"

// using MiniCPU_ns::FuncReturnFeedback_e;

bool MiniCPU_ns::RV64Core::memRead(uint64_t start_addr, uint64_t size, uint8_t *buffer) {
    if (start_addr % size != 0) {
//            CSReg_Cause_t cause;
//            cause.cause = exec_load_misalign;
//            cause.interrupt = 0;
        raiseTrap(/*cause*/ { .cause = exec_load_misalign, .interrupt = 0 } , start_addr);
        return false;
    }
    int va_err = doLoad(start_addr, size, buffer);
    if (va_err == Interface_ns::FB_SUCCESS) {
        return true;
    }
    else {
        RV64_ExecFeedbackCode_e feedback = (va_err == VADDR_ACCESS_FAULT) ? exec_load_acc_fault :
                                           (va_err == VADDR_MIS_ALIGN) ? exec_load_misalign :
                                           (va_err == VADDR_PAGE_FAULT) ? exec_load_pgfault : exec_ok;
        assert(feedback != exec_ok);
        raiseTrap({ .cause = (uint64_t)feedback, .interrupt = 0 }, start_addr);
        return false;
    }
}

bool MiniCPU_ns::RV64Core::memWrite(uint64_t start_addr, uint64_t size, const uint8_t *buffer) {
    if (start_addr % size != 0) {
        raiseTrap({ .cause = exec_store_misalign, .interrupt = 0 },start_addr);
        return false;
    }
    int va_err = doStore(start_addr,size,buffer);
    if (va_err == Interface_ns::FB_SUCCESS) {
        return true;
    }
    else {
        RV64_ExecFeedbackCode_e feedback = (va_err == VADDR_ACCESS_FAULT) ? exec_store_acc_fault :
                                           (va_err == VADDR_MIS_ALIGN) ? exec_store_misalign :
                                           (va_err == VADDR_PAGE_FAULT) ? exec_store_pgfault : exec_ok;
        assert(feedback != exec_ok);
        raiseTrap({ .cause = (uint64_t)feedback, .interrupt = 0 }, start_addr);
        return false;
    }
}

FuncReturnFeedback_e MiniCPU_ns::RV64Core::WriteProgramCounter_DebugAPI(RegItemVal_t reg_val)
{
    currentProgramCounter = reg_val.u64_val;
    return MEMU_OK;
}

FuncReturnFeedback_e MiniCPU_ns::RV64Core::setGPRByIndex(uint8_t gpr_index, int64_t val)
{
    assert(gpr_index < 32);
    if(unlikely(gpr_index == 0)) { return MEMU_OK; }
    ctx[currentCtx].GPR[gpr_index] = val;
    return MEMU_OK;
}



FuncReturnFeedback_e MiniCPU_ns::RV64Core::DumpProgramCounter_DebugAPI(RegisterItem_t &reg)
{
    reg.val.u64_val = currentProgramCounter;
    reg.reg_id = 33;
    reg.size = 8;
    strcpy(reg.disp_name, "pc");
    return MEMU_OK;
}

void MiniCPU_ns::RV64Core::reset() {
    trapProgramCounter = 0;
    needTrap = false;
    currentPrivMode = M_MODE;
    nextPrivMode = M_MODE;
    status = 0;
    CSReg_MStatus_t* mstatus = (CSReg_MStatus_t*)&status;
    mstatus->sxl = 2;
    mstatus->uxl = 2;
    csrMachineISA.ext = rv_ext('i') | rv_ext('m') | rv_ext('a') | rv_ext('c') | rv_ext('s') | rv_ext('u');
    csrMachineISA.mxl = 2;
    csrMachineISA.blank = 0;
    csrMExceptionDelegation = 0;
    csrMIntDelegation = 0;
    csrIntEnable = 0;
    csrMTrapVecBaseAddr.val = 0;
    csrMscratch = 0;
    csrMExceptionPC = 0;
    csrMachineCause.val = 0;
    csrMachineTrapVal = 0;
    csrMCounterEN.val = 0;
    csrIntPending.val = 0;
    csrMCycleNum = 0;
    minstret = 0;
    csrSTrapVecBaseAddr.val = 0;
    csrSscratch = 0;
    csrSExceptionPC = 0;
    csrSupervisorCause.val = 0;
    csrSupervisorTrapVal = 0;
    satp.val = 0;
    csrSCounterEN.val = 0;
    for (size_t curr_ctx = 0; curr_ctx < NR_CTX; curr_ctx++) {
        ctx[curr_ctx].GPR[0] = 0;
        for(int i = 0; i < 32; i ++) ctx[curr_ctx].GPR[i] = 0;
    }

    // GPR[2] = 0x180000100;
}

int MiniCPU_ns::RV64Core::run() {
    while(true) this->step();
}

//void RVCore_ns::RV64Core::step() {
//    this->Step_CoreAPI();
//}










