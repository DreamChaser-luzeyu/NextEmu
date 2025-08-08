#include "rvcore_minicpu/MiniCPUCore.h"

void MiniCPU_ns::RV64Core::preExec() {
    // TODO: fill the raise trap
    // ----- Update CSRs
    this->csrMCycleNum++;
    csrIntPending.m_e_ip = this->intStatus->meip;
    csrIntPending.m_s_ip = this->intStatus->msip;
    csrIntPending.m_t_ip = this->intStatus->mtip;
    csrIntPending.s_e_ip = this->intStatus->seip;
    // ----- Update inner states
    this->needTrap = false;
    this->currentPrivMode = this->nextPrivMode;
    // ----- Check interrupt bits
    const uint64_t int_bits = (csrIntPending.val) & csrIntEnable;
    if(likely(bits_to_int_type(int_bits) == int_no_int)) { return; }  // Certainly no need to trap, so return directly
    // ----- Raise trap
    // 1-- For M_MODE
    if(currentPrivMode == M_MODE) {
        CSReg_MStatus_t* csrMachineStatus = (CSReg_MStatus_t *)(&status);
        uint64_t m_mode_int_bits = int_bits & (~csrMIntDelegation);  // Interrupts that are not delegated to S_MODE
        if(csrMachineStatus->mie && m_mode_int_bits) {
            // M_MODE interrupt enabled && there exists M_MODE interrupt
            // then we want to raise a trap
            // No need to perform mode switching
            CSReg_Cause_t cause;
            cause.cause = bits_to_int_type(m_mode_int_bits);
            cause.interrupt = 1;
            return raiseTrap(cause);
            // **ends here
        }
        else {
            // ???
            return;
        }
    }
    // 2-- For other mode
    else {
        IntType_e int_type = bits_to_int_type(int_bits);
        CSReg_MStatus_t* csrMachineStatus = (CSReg_MStatus_t *)(&status);
        // Check if exists interrupt to be delegated to S_MODE
        if(int_bits & csrMIntDelegation) {
            // There exists interrupts to be delegated to S_MODE
            if(currentPrivMode < S_MODE) {
                // We are at lower privilege level than S_MODE
                // A trap is needed to go to S_MODE <TO BE VERIFIED>
                CSReg_Cause_t cause;
                cause.cause = int_type;
                cause.interrupt = 1;
                return raiseTrap(cause);
                // **ends here
            }
            else if(csrMachineStatus->sie) {
                // We have S_MODE interrupt enabled, so we want to raise a trap
                CSReg_Cause_t cause;
                cause.cause = int_type;
                cause.interrupt = 1;
                return raiseTrap(cause);
                // ??? What if we are not at S_MODE, but S_MODE interrupt is disabled? Should we raise a trap?
                // I think we need a trap to switch to S_MODE anyway. <TO BE VERIFIED>
                // We need two traps ???
                // **ends here
            }
            else {
                // ???
                return;
            }
        }
        else {
            // Does not exist interrupts to be delegated to S_MODE
            // We should trap to M_MODE by default
            if(currentPrivMode < M_MODE) { // TODO: Is this correct?
                // Same as above, we need a trap to switch to M_MODE
                CSReg_Cause_t cause;
                cause.cause = int_type;
                cause.interrupt = 1;
                return raiseTrap(cause);
                // ** ends here
            }
            else if(csrMachineStatus->mie) {
                // If we have M_MODE interrupt enabled
                // then we use the int_bits directly
                CSReg_Cause_t cause;
                cause.cause = int_type;
                cause.interrupt = 1;
                return raiseTrap(cause);
                // **ends here
            }
            else {
                // ???
                return;
            }
        }
    }
    // Why reach here?
    assert(0);  // Should never reach here
}

bool MiniCPU_ns::RV64Core::csrRead(RV_CSR_Addr_enum csr_index, uint64_t &csr_result) {
    switch (csr_index) {
        case csr_mvendorid:
            csr_result = 0;
            break;
        case csr_marchid:
            csr_result = 0;
            break;
        case csr_mimpid:
            csr_result = 0;
            break;
        case csr_mhartid:
            csr_result = rvHartID;
            break;
        case csr_mconfigptr:
            csr_result = 0;
            break;
        case csr_mstatus:
            csr_result = status;
            break;
        case csr_misa:
            csr_result = csrMachineISA.val;
            break;
        case csr_medeleg:
            csr_result = csrMExceptionDelegation;
            break;
        case csr_mideleg:
            csr_result = csrMIntDelegation;
            break;
        case csr_mie:
            csr_result = csrIntEnable;
            break;
        case csr_mtvec:
            csr_result = csrMTrapVecBaseAddr.val;
            break;
        case csr_mcounteren:
            csr_result = csrMCounterEN.val;
            break;
        case csr_mscratch:
            csr_result = csrMscratch;
            break;
        case csr_mepc:
            csr_result = csrMExceptionPC;
            break;
        case csr_mcause:
            csr_result = csrMachineCause.val;
            break;
        case csr_mtval:
            csr_result = csrMachineTrapVal;
            break;
        case csr_mip:
            csr_result = csrIntPending.val;
            break;
        case csr_mcycle:
            csr_result = csrMCycleNum;
            break;
        case csr_minstret:
            csr_result = minstret;
            break;
        case csr_sstatus: {
            // This register has WPRI fields,
            // which means Reserved Writes Preserve Values, Reads Ignore Values (WPRI)
            csr_result = 0;
            CSReg_SStatus_t *sstatus = (CSReg_SStatus_t*)&csr_result;
            CSReg_MStatus_t *mstatus = (CSReg_MStatus_t*)&status;
            sstatus->sie = mstatus->sie;
            sstatus->spie = mstatus->spie;
            sstatus->ube = mstatus->ube;
            sstatus->spp = mstatus->spp;
            sstatus->vs = mstatus->vs;
            sstatus->fs = mstatus->fs;
            sstatus->xs = mstatus->xs;
            sstatus->sum = mstatus->sum;
            sstatus->mxr = mstatus->mxr;
            sstatus->uxl = mstatus->uxl;
            sstatus->sd = mstatus->sd;
            break;
        }
        case csr_sie:
            csr_result = csrIntEnable & S_MODE_INT_MASK;
            break;
        case csr_stvec:
            csr_result = csrSTrapVecBaseAddr.val;
            break;
        case csr_scounteren:
            csr_result = csrSCounterEN.val;
            break;
        case csr_sscratch:
            csr_result = csrSscratch;
            break;
        case csr_sepc:
            csr_result = csrSExceptionPC;
            break;
        case csr_scause:
            csr_result = csrSupervisorCause.val;
            break;
        case csr_stval:
            csr_result = csrSupervisorTrapVal;
            break;
        case csr_sip:
            csr_result = csrIntPending.val & S_MODE_INT_MASK;
            break;
        case csr_satp: {
            const CSReg_MStatus_t *mstatus = (CSReg_MStatus_t*) &status;
            if (currentPrivMode == S_MODE && mstatus->tvm) return false;
            csr_result = satp.val;
            break;
        }
        case csr_cycle: {
            CSR_CounterEN_t *mcen = (CSR_CounterEN_t*)&csrMCounterEN;
            CSR_CounterEN_t *scen = (CSR_CounterEN_t*)&csrSCounterEN;
            if (currentPrivMode <= S_MODE && (!mcen->cycle || !scen->cycle)) return false;
            csr_result = csrMCycleNum;
            break;
        }
        case csr_tselect:
            csr_result = 1;
            break;
        case csr_tdata1:
            csr_result = 0;
            break;
        case csr_time:
            // csr_result = csrMCycleNum / 10;
            csr_result = csrMCycleNum;
            break;
        default:
            return false;
    }
    return true;
}

bool MiniCPU_ns::RV64Core::csrWrite(RV_CSR_Addr_enum csr_index, uint64_t csr_data) {
    switch (csr_index) {
        case csr_mstatus: {
            CSReg_MStatus_t *nstatus = (CSReg_MStatus_t*)&csr_data;
            CSReg_MStatus_t *mstatus = (CSReg_MStatus_t*)&status;
            mstatus->sie = nstatus->sie;
            mstatus->mie = nstatus->mie;
            mstatus->spie = nstatus->spie;
            mstatus->mpie = nstatus->mpie;
            assert(mstatus->spie != 2);
            assert(mstatus->mpie != 2);
            mstatus->spp = nstatus->spp;
            mstatus->mpp = nstatus->mpp;
            mstatus->mprv = nstatus->mprv;
            mstatus->sum = nstatus->sum; // always true
            mstatus->mxr = nstatus->mxr; // always true
            mstatus->tvm = nstatus->tvm;
            mstatus->tw = nstatus->tw; // not supported but wfi impl as nop
            mstatus->tsr = nstatus->tsr;
            break;
        }
        case csr_misa: {
            break;
        }
        case csr_medeleg:
            csrMExceptionDelegation = csr_data & S_MODE_EXC_MASK;
            break;
        case csr_mideleg:
            csrMIntDelegation = csr_data & S_MODE_INT_MASK;
            break;
        case csr_mie:
            csrIntEnable = csr_data & M_MODE_INT_MASK;
            break;
        case csr_mtvec: {
            CSReg_TrapVector_t *tvec = (CSReg_TrapVector_t*)&csr_data;
            assert(tvec->mode <= 1);
            csrMTrapVecBaseAddr.val = csr_data;
            break;
        }
        case csr_mcounteren: {
            csrMCounterEN.val = csr_data & COUNTER_MASK;
            break;
        }
        case csr_mscratch:
            csrMscratch = csr_data;
            break;
        case csr_mepc:
            csrMExceptionPC = csr_data;
            break;
        case csr_mcause:
            csrMachineCause.val = csr_data;
            break;
        case csr_mtval:
            csrMachineTrapVal = csr_data;
            break;
        case csr_mip:
            csrIntPending.val = csr_data & M_MODE_INT_MASK;
            break;
        case csr_mcycle:
            csrMCycleNum = csr_data;
            break;
        case csr_sstatus: {
            CSReg_SStatus_t* nstatus = (CSReg_SStatus_t*)&csr_data;
            CSReg_SStatus_t* sstatus = (CSReg_SStatus_t*)&status;
            sstatus->sie = nstatus->sie;
            sstatus->spie = nstatus->spie;
            assert(sstatus->spie != 2);
            sstatus->spp = nstatus->spp;
            sstatus->sum = nstatus->sum;
            sstatus->mxr = nstatus->mxr;
            break;
        }
        case csr_sie:
            csrIntEnable = (csrIntEnable & (~S_MODE_INT_MASK)) | (csr_data & S_MODE_INT_MASK);
            break;
        case csr_stvec: {
            CSReg_TrapVector_t* tvec = (CSReg_TrapVector_t*)&csr_data;
            if (tvec->mode > 1) tvec->mode = 0;
            csrSTrapVecBaseAddr.val = csr_data;
            break;
        }
        case csr_scounteren:
            csrSCounterEN.val = csr_data & COUNTER_MASK;
            break;
        case csr_sscratch:
            csrSscratch = csr_data;
            break;
        case csr_sepc:
            csrSExceptionPC = csr_data;
            break;
        case csr_scause:
            csrSupervisorCause.val = csr_data;
            break;
        case csr_stval:
            csrSupervisorTrapVal = csr_data;
            break;
        case csr_sip:
            csrIntPending.val = (csrIntPending.val & (~S_MODE_INT_MASK)) | (csr_data & S_MODE_INT_MASK);
            break;
        case csr_satp: {
            const CSReg_MStatus_t* mstatus = (CSReg_MStatus_t*)&status;
            if (unlikely(currentPrivMode == S_MODE && mstatus->tvm)) return false;
            SATP_Reg_t *satp_reg = (SATP_Reg_t*)&csr_data;
            if (satp_reg->mode !=0 && satp_reg->mode != 8)
                satp_reg->mode = 0;
            satp.val = csr_data;
            break;
        }
        case csr_tselect:
            break;
        case csr_tdata1:
            break;
        case csr_time:
            break;
        default:
            return false;
    }
    return true;
}

bool MiniCPU_ns::RV64Core::csrSetbit(RV_CSR_Addr_enum csr_index, uint64_t csr_mask) {
    uint64_t tmp;
    bool ret = csrRead(csr_index, tmp);
    if (unlikely(!ret)) return false;
    tmp |= csr_mask;
    ret &= csrWrite(csr_index, tmp);
    return ret;
}

bool MiniCPU_ns::RV64Core::csrClearbit(RV_CSR_Addr_enum csr_index, uint64_t csr_mask) {
    uint64_t tmp;
    bool ret = csrRead(csr_index, tmp);
    if (unlikely(!ret)) return false;
    tmp &= ~csr_mask;
    ret &= csrWrite(csr_index, tmp);
    return ret;
}

bool MiniCPU_ns::RV64Core::csr_op_permission_check(uint16_t csr_index, bool write) {
    if (unlikely(((csr_index >> 8) & 3) > currentPrivMode)) return false;
    if (unlikely((((csr_index >> 10) & 3) == 3) && write)) return false;
    return true;
}

void MiniCPU_ns::RV64Core::raiseTrap(CSReg_Cause_t cause, uint64_t tval)
{
    assert(false);
    assert(!this->needTrap);
    assert(cause.cause != exec_ok);
    needTrap = true;
    uint8_t trap_dest = 0;  // 0 - trap to M_MODE   1 - trap to S_MODE

    // --- Decide which mode to trap to
    if(likely(currentPrivMode != M_MODE)) {
        if(cause.interrupt) {
            // Trap caused by interrupt
            if(csrMIntDelegation & (1 << cause.cause)) {
                trap_dest = 1;
            }
        }
        else {
            // Trap caused by exception
            if(csrMExceptionDelegation & (1 << cause.cause)) {
                trap_dest = 1;
            }
        }
    }
    // If in M_MODE, the core do not care the delegated traps
    // so the trap destination should always be M_MODE

    // --- Do trap procedure
    if(trap_dest == 1) {
        // Trap to S_MODE
        csrSupervisorTrapVal = tval;
        csrSupervisorCause.val = cause.val;
        csrSExceptionPC = ctx[currentCtx].currentProgramCounter;
        CSReg_SStatus_t* csrSupervisorStatus = (CSReg_SStatus_t *)(&status);
        csrSupervisorStatus->spie = csrSupervisorStatus->sie;  // TODO: ???
        csrSupervisorStatus->sie = 0;
        csrSupervisorStatus->spp = (uint64_t)currentPrivMode;
        trapProgramCounter = (csrSTrapVecBaseAddr.base << 2) +
                             ((csrSTrapVecBaseAddr.mode == 1) ? (csrSupervisorCause.cause * 4) : 0);
        nextPrivMode = S_MODE;
    }
    else if(trap_dest == 0) {
        // Trap to M_MODE
        csrMachineTrapVal = tval;
        csrMachineCause.val = cause.val;
        csrMExceptionPC = ctx[currentCtx].currentProgramCounter;
        CSReg_MStatus_t* csrMachineStatus = (CSReg_MStatus_t *)(&status);
        csrMachineStatus->mpie = csrMachineStatus->mie;
        csrMachineStatus->mie = 0;
        csrMachineStatus->mpp = currentPrivMode;
        trapProgramCounter = (csrMTrapVecBaseAddr.base << 2) +
                             ((csrMTrapVecBaseAddr.mode == 1) ? (csrMachineCause.cause * 4) : 0);
        nextPrivMode = M_MODE;
    }
    else {
        assert(0);  // Never reach here
    }

    if(cause.cause == exec_instr_pgfault && tval == trapProgramCounter) { assert(false); }
}

void MiniCPU_ns::RV64Core::ecall() {
//    assert((currentPrivMode | 0b100) == (currentPrivMode + 8));
//    raiseTrap({ .cause = (uint64_t)(currentPrivMode | 0b100), .interrupt = 0 });
    raiseTrap({ .cause = (uint64_t)(currentPrivMode + 8), .interrupt = 0 });
}

void MiniCPU_ns::RV64Core::ebreak() {
    raiseTrap({ .cause = exec_breakpoint, .interrupt = 0 });
}

bool MiniCPU_ns::RV64Core::mret() {
    if(currentPrivMode != M_MODE) { return false; }
    CSReg_MStatus_t* csrMachineStatus = (CSReg_MStatus_t *)(&status);
    csrMachineStatus->mie = csrMachineStatus->mpie;
    nextPrivMode = (PrivMode_e)(csrMachineStatus->mpp);
    csrMachineStatus->mpie = 1;
    if(csrMachineStatus->mpp != M_MODE) { csrMachineStatus->mprv = 0; }
    csrMachineStatus->mpp = U_MODE;
    needTrap = true;
    trapProgramCounter = csrMExceptionPC;
    return true;
}

bool MiniCPU_ns::RV64Core::sret() {
    if(unlikely(currentPrivMode < S_MODE)) { return false; }
    CSReg_MStatus_t* csrMachineStatus = (CSReg_MStatus_t *)(&status);
    CSReg_SStatus_t* csrSupervisorStatus = (CSReg_SStatus_t *)(&status);
    if(unlikely(csrMachineStatus->tsr)) { return false; }
    csrSupervisorStatus->sie = csrSupervisorStatus->spie;
    nextPrivMode = (PrivMode_e)(csrSupervisorStatus->spp);
    csrSupervisorStatus->spie = 1;
    if(csrSupervisorStatus->spp != M_MODE) { csrMachineStatus->mprv = 0; }
    csrSupervisorStatus->spp = U_MODE;
    needTrap = true;
    trapProgramCounter = csrSExceptionPC;
    return true;
}

bool MiniCPU_ns::RV64Core::sfence_vma(uint64_t vaddr, uint64_t asid) {
    const CSReg_MStatus_t *mstatus = (CSReg_MStatus_t *)&status;
    if (unlikely(currentPrivMode < S_MODE || (currentPrivMode == S_MODE && mstatus->tvm))) return false;
    ((RV64SV39_MMU*)subBus)->SV39_FlushTLB_sfence_vma(vaddr,asid);
    return true;
}
