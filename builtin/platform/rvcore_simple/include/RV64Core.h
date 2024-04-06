#pragma once

#include <stdint.h>
#include <cstring>
#include <vector>
#include <fstream>

#include "sdk/console.h"
#include "sdk/interface/dev_if.h"
#include "sdk/symbol_attr.h"
#include "sdk/interface/struct_defs.h"
#include "RV64SV39_MMU.h"
#include "misc/rv64_structs.h"
#include "misc/rv64_csr_structs.h"
#include "misc/status_enum.h"

namespace RVCore_ns {
enum ALU_Op_enum {
    ALU_ADD, ALU_SUB, ALU_SLL, ALU_SLT, ALU_SLTU, ALU_XOR, ALU_SRL, ALU_SRA, ALU_OR, ALU_AND,
    ALU_MUL, ALU_MULH, ALU_MULHU, ALU_MULHSU, ALU_DIV, ALU_DIVU, ALU_REM, ALU_REMU,
    ALU_NOP
};

struct IntStatus {
    bool meip;
    bool msip;
    bool mtip;
    bool seip;
};
typedef IntStatus IntStatus_t;


class RV64Core : public Interface_ns::MasterAtomicIO_I, public Interface_ns::Runnable_I {
    // ----- Fields
public:
    IntStatus_t *intStatus;

//    std::unordered_set<uint64_t> watchPointPCs;
private:
    // ----- Constants
    const uint64_t S_MODE_INT_MASK = (1ull << int_s_ext) | (1ull << int_s_sw) | (1ull << int_s_timer);
    const uint64_t COUNTER_MASK = (1 << 0) | (1 << 2);
    const uint64_t S_MODE_EXC_MASK = (1 << 16) - 1 - (1 << exec_ecall_from_machine);
    const uint64_t M_MODE_INT_MASK = S_MODE_INT_MASK | (1ull << int_m_ext) | (1ull << int_m_sw) | (1ull << int_m_timer);
    const uint8_t PC_ALIGN = 2;
    // the name GDB should use when describing these register
    const std::vector<std::string> GPR_NAME_ARR = {
            "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
            "fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
            "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
            "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
    };
    const std::string PC_NAME = "pc";

//    Interface_ns::SlaveAtomicIO_I* sv39MMU = nullptr;

    // ----- Registers
    int64_t GPR[32];                                   // General purpose registers
    // ----- CSRs
    // --- M_MODE CSRs
    uint64_t csrMCycleNum;

    uint64_t csrMIntDelegation;         ///< mideleg, Machine-level Interrupt Delegation Register
    ///< Used as a mask, indicating which traps to be routed to S_MODE
    uint64_t csrMExceptionDelegation;   // medeleg, Machine-level Exception Delegation Register
    uint64_t csrMachineTrapVal;         // mtval,   Machine Trap Value Register
    CSReg_Cause_t csrMachineCause;           // mcause
    uint64_t csrMExceptionPC;           // mepc
    CSReg_TrapVector_t csrMTrapVecBaseAddr;       // mtvec
    CSR_CounterEN_t csrMCounterEN;             // mcounteren
    CSR_MISA_t csrMachineISA;             // misa
    uint64_t csrMscratch;               // mscratch
    // --- S_MODE CSRs
    uint64_t csrSupervisorTrapVal;      // stval
    CSReg_Cause_t csrSupervisorCause;        // scause
    uint64_t csrSExceptionPC;           // sepc
    CSReg_TrapVector_t csrSTrapVecBaseAddr;       // stvec
    CSR_CounterEN_t csrSCounterEN;             // scounteren
    uint64_t csrSscratch;               // sscratch
    // --- Shared CSRs
    uint64_t minstret;
    // The sstatus register is a subset of the mstatus register.
    // In a straightforward implementation, reading or writing any field in sstatus is equivalent to
    // reading or writing the homonymous field in mstatus.
    uint64_t status;                    // mstatus, sstatus
    uint64_t rvHartID;
    SATP_Reg_t satp;
    // Restricted views of the mip and mie registers appear as the sip and sie registers for supervisor
    // level.
    CSReg_IntPending_t csrIntPending;             // mip, sip
    uint64_t csrIntEnable;              // mie, sie TODO: mask?
    uint64_t trapProgramCounter;
    uint64_t currentProgramCounter;

    // ----- Inner states
    bool needTrap;
    PrivMode_e currentPrivMode;
    PrivMode_e nextPrivMode;

public:
    void step() override;

    int run() override;

    FuncReturnFeedback_e DumpProgramCounter_CoreAPI(RegisterItem_t &reg);

    FuncReturnFeedback_e writeProgramCounter(RegItemVal_t reg_val);

    FuncReturnFeedback_e setGPRByIndex(uint8_t gpr_index, int64_t val);

    FuncReturnFeedback_e MemRead_CoreDebugAPI(uint64_t start_addr, uint64_t size, uint8_t *buffer);

    FuncReturnFeedback_e MemWrite_CoreDebugAPI(uint64_t start_addr,
                                               uint64_t size, const uint8_t *buffer);

    FuncReturnFeedback_e DumpRegister_CoreAPI(std::vector<RegisterItem_t> &regs);

    FuncReturnFeedback_e WriteAllRegister_CoreAPI(const std::vector<RegisterItem_t> &regs);

    RV64Core(Interface_ns::SlaveAtomicIO_I *bus, uint16_t hart_id) :
            rvHartID(hart_id), csrMCycleNum(0), needTrap(false),
            Interface_ns::MasterAtomicIO_I(new RV64SV39_MMU(bus, currentPrivMode, satp, status, rvHartID)) {
        this->intStatus = new IntStatus_t;
        intStatus->mtip = false;
        intStatus->msip = false;
        intStatus->meip = false;
        intStatus->seip = false;
        reset();
    }

private:
    void preExec();

    int64_t alu_exec(int64_t a, int64_t b, ALU_Op_enum op, bool op_32 = false);

    void raiseTrap(CSReg_Cause_t cause, uint64_t tval = 0);

    void setGPR(uint8_t GPR_index, int64_t value) {
        assert(GPR_index >= 0 && GPR_index < 32);
        if (likely(GPR_index)) GPR[GPR_index] = value;
    }

    bool memRead(uint64_t start_addr, uint64_t size, uint8_t *buffer);

    bool memWrite(uint64_t start_addr, uint64_t size, const uint8_t *buffer);

    void ecall();

    void ebreak();

    bool mret();

    bool sret();

    bool sfence_vma(uint64_t vaddr, uint64_t asid);

    bool csrRead(RV_CSR_Addr_enum csr_index, uint64_t &csr_result);

    bool csrWrite(RV_CSR_Addr_enum csr_index, uint64_t csr_data);

    bool csrSetbit(RV_CSR_Addr_enum csr_index, uint64_t csr_mask);

    bool csrClearbit(RV_CSR_Addr_enum csr_index, uint64_t csr_mask);

    bool csr_op_permission_check(uint16_t csr_index, bool write);

    void reset();
};
}