#include "sdk/console.h"
#include "RV64Core.h"

void RVCore_ns::RV64Core::step() {
//    LOG_DEBUG("CURRENT PC: %016lx", currentProgramCounter);
    bool is_new_pc_set = false;
    bool is_compressed_inst = false;
    bool is_instr_illegal = false;
    // --- Instruction Fetch
    RVInst_t inst;
    inst.val = 0;
    VAddr_RW_Feedback feedback;
    preExec();
    if(needTrap) goto exception;
    if(currentProgramCounter % PC_ALIGN) {
        raiseTrap({ .cause = exec_instr_misalign, .interrupt = 0 }, currentProgramCounter);
        goto exception;
    }
    feedback = ((RV64SV39_MMU*)subBus)->VAddr_InstFetch_MMU_API(currentProgramCounter, 4, &inst);
    if(feedback != INST_ACCESS_OK) {
        CSReg_Cause_t cause;
        cause.cause = (feedback == INST_PAGE_FAULT) ? exec_instr_pgfault :
                      (feedback == INST_ACCESS_FAULT) ? exec_instr_acc_fault : 0;
        assert(cause.cause);
        cause.interrupt = 0;
        raiseTrap(cause, currentProgramCounter);
        goto exception;
    }
    // --- Decode & exec
    is_compressed_inst = (inst.r_type.opcode & 0b11) != 0b11;
    if (!is_compressed_inst) {
        switch (inst.r_type.opcode) {
            case OPCODE_LUI: {
                // Load Upper Immediate, x[rd] = sext(immediate[31:12] << 12)
                setGPR(inst.u_type.rd, ((int64_t) inst.u_type.imm_31_12) << 12);
                break;
            }
            case OPCODE_AUIPC: {
                // Add Upper Immediate to PC, x[rd] = pc+sext(immediate[31:12] << 12)
                setGPR(inst.u_type.rd, (((int64_t) inst.u_type.imm_31_12) << 12) + currentProgramCounter);
                break;
            }
            case OPCODE_JAL: {
                // Jump And Link, x[rd] = pc+4; pc += sext(offset)
                uint64_t npc = currentProgramCounter + ((inst.j_type.imm_20 << 20) | (inst.j_type.imm_19_12 << 12) | (inst.j_type.imm_11 << 11) | (inst.j_type.imm_10_1 << 1));
                if (npc % PC_ALIGN) {
                    raiseTrap({ .cause = exec_instr_misalign, .interrupt = 0 }, npc);
                }
                else {
                    setGPR(inst.j_type.rd, currentProgramCounter + 4);
                    currentProgramCounter = npc;
                    is_new_pc_set = true;
                }
                break;
            }
            case OPCODE_JALR: {
                // Jump And Link Register, t = pc+4; pc=(x[rs1]+sext(offset))&(~1); x[rd]=t
                uint64_t npc = GPR[inst.i_type.rs1] + (inst.i_type.imm12 & (~1));
                uint64_t tmp = inst.i_type.imm12;
                if(tmp & 1) tmp ^= 1;
                assert(npc == (GPR[inst.i_type.rs1] + tmp));
                if (npc % PC_ALIGN) {
                    raiseTrap({ .cause = exec_instr_misalign, .interrupt = 0 }, npc);
                }
                else {
                    setGPR(inst.j_type.rd, currentProgramCounter + 4);
                    currentProgramCounter = npc;
                    is_new_pc_set = true;
                }
                break;
            }
                // B-Type Instructions
            case OPCODE_BRANCH: {
                int64_t offset = (inst.b_type.imm_12 << 12) | (inst.b_type.imm_11 << 11) | (inst.b_type.imm_10_5 << 5) | (inst.b_type.imm_4_1 << 1);
                uint64_t npc;
                switch (inst.b_type.funct3) {
                    case FUNCT3_BEQ:
                        // Branch if Equal, if (rs1 == rs2) pc += sext(offset)
                        if (GPR[inst.b_type.rs1] == GPR[inst.b_type.rs2]) {
                            npc = currentProgramCounter + offset;
                            is_new_pc_set = true;
                        }
                        break;
                        // Are you finding BEQZ instruction? It is a `pesudoinstruction` and is implemented using BEQ inst.
                        // STFW for detailed explanation about pesudoinstruction.
                    case FUNCT3_BNE:
                        // Branch if Not Equal, if (rs1 ≠ rs2) pc += sext(offset)
                        if (GPR[inst.b_type.rs1] != GPR[inst.b_type.rs2]) {
                            npc = currentProgramCounter + offset;
                            is_new_pc_set = true;
                        }
                        break;
                    case FUNCT3_BLT:
                        // Branch if Less Than, if (rs1 <s rs2) pc += sext(offset)
                        if (GPR[inst.b_type.rs1] < GPR[inst.b_type.rs2]) {
                            npc = currentProgramCounter + offset;
                            is_new_pc_set = true;
                        }
                        break;
                    case FUNCT3_BGE:
                        // Branch if Greater than or Equal, if (rs1 ≥s rs2) pc += sext(offset)
                        if (GPR[inst.b_type.rs1] >= GPR[inst.b_type.rs2]) {
                            npc = currentProgramCounter + offset;
                            is_new_pc_set = true;
                        }
                        break;
                    case FUNCT3_BLTU:
                        // Branch if Less Than Unsigned, if (rs1 <u rs2) pc += sext(offset)
                        if ((uint64_t)GPR[inst.b_type.rs1] < (uint64_t)GPR[inst.b_type.rs2]) {
                            npc = currentProgramCounter + offset;
                            is_new_pc_set = true;
                        }
                        break;
                    case FUNCT3_BGEU:
                        // Branch if Greater Than or Equal Unsigned, if (rs1 ≥u rs2) pc += sext(offset)
                        if ((uint64_t)GPR[inst.b_type.rs1] >= (uint64_t)GPR[inst.b_type.rs2]) {
                            npc = currentProgramCounter + offset;
                            is_new_pc_set = true;
                        }
                        break;
                    default:
                        is_instr_illegal = true;
                        break;
                }
                if (is_new_pc_set) {
                    if (npc % PC_ALIGN) raiseTrap({ .cause = exec_instr_misalign, .interrupt = 0 }, npc);
                    else currentProgramCounter = npc;
                }
                break;
            }
            case OPCODE_LOAD: {
                uint64_t mem_addr = GPR[inst.i_type.rs1] + (inst.i_type.imm12);
                switch (inst.i_type.funct3) {
                    case FUNCT3_LB: {
                        // Load Byte, x[rd] = sext(M[x[rs1] + sext(offset)][7:0])
                        int8_t buf;
                        bool ok = memRead(mem_addr, 1, (unsigned char *) &buf);
                        if (ok) setGPR(inst.i_type.rd, buf);
                        // TODO: What if not-ok?
                        break;
                    }
                    case FUNCT3_LH: {
                        // Load Halfword, x[rd] = sext(M[x[rs1] + sext(offset)][15:0])
                        int16_t buf;
                        bool ok = memRead(mem_addr, 2, (unsigned char *) &buf);
                        if (ok) setGPR(inst.i_type.rd, buf);
                        break;
                    }
                    case FUNCT3_LW: {
                        // Load Word, x[rd] = sext(M[x[rs1] + sext(offset)][31:0])
                        int32_t buf;
                        bool ok = memRead(mem_addr, 4, (unsigned char *) &buf);
                        if (ok) setGPR(inst.i_type.rd, buf);
                        break;
                    }
                    case FUNCT3_LD: {
                        // Load Doubleword, x[rd] = M[x[rs1] + sext(offset)][63:0]
                        int64_t buf;
                        bool ok = memRead(mem_addr, 8, (unsigned char *) &buf);
                        if (ok) setGPR(inst.i_type.rd, buf);
                        break;
                    }
                    case FUNCT3_LBU: {
                        // Load Byte Unsigned, x[rd] = M[x[rs1] + sext(offset)][7:0]
                        uint8_t buf;
                        bool ok = memRead(mem_addr, 1, (unsigned char *) &buf);
                        if (ok) setGPR(inst.i_type.rd, buf);
                        break;
                    }
                    case FUNCT3_LHU: {
                        // Load Halfword Unsigned, x[rd] = M[x[rs1] + sext(offset)][15:0]
                        uint16_t buf;
                        bool ok = memRead(mem_addr, 2, (unsigned char *) &buf);
                        if (ok) setGPR(inst.i_type.rd, buf);
                        break;
                    }
                    case FUNCT3_LWU: {
                        // Load Word Unsigned, x[rd] = M[x[rs1] + sext(offset)][31:0]
                        uint32_t buf;
                        bool ok = memRead(mem_addr, 4, (unsigned char *) &buf);
                        if (ok) setGPR(inst.i_type.rd, buf);
                        break;
                    }
                    default:
                        is_instr_illegal = true;
                }
                break;
            }
                // S-Type Instructions
            case OPCODE_STORE: {
                uint64_t mem_addr = GPR[inst.s_type.rs1] + ( (inst.s_type.imm_11_5 << 5) | (inst.s_type.imm_4_0));
                switch (inst.i_type.funct3) {
                    case FUNCT3_SB: {
                        // Store Byte, M[x[rs1]+sext(offset)=x[rs2][7:0]
                        memWrite(mem_addr, 1, (unsigned char *) &GPR[inst.s_type.rs2]);
                        break;
                    }
                    case FUNCT3_SH: {
                        // Store Halfword, M[x[rs1]+sext(offset)=x[rs2][15:0]
                        memWrite(mem_addr, 2, (unsigned char *) &GPR[inst.s_type.rs2]);
                        break;
                    }
                    case FUNCT3_SW: {
                        // Store Word, M[x[rs1]+sext(offset)=x[rs2][31:0]
                        memWrite(mem_addr, 4, (unsigned char *) &GPR[inst.s_type.rs2]);
                        break;
                    }
                    case FUNCT3_SD: {
                        // Store Doubleword, M[x[rs1]+sext(offset)=x[rs2][63:0]
                        memWrite(mem_addr, 8, (unsigned char *) &GPR[inst.s_type.rs2]);
                        break;
                    }
                    default:
                        is_instr_illegal = true;
                }
                break;
            }
                // I-Type Instructions
            case OPCODE_OPIMM: {
                int64_t imm = inst.i_type.imm12;
                Funct6_Op_enum fun6 = (Funct6_Op_enum)((inst.r_type.funct7) >> 1);
                ALU_Op_enum op;
                switch (inst.i_type.funct3) {
                    case FUNCT3_ADD_SUB:
                        op = ALU_ADD;
                        break;
                    case FUNCT3_SLT:
                        // Set if Less Than Immediate, x[rd] = (x[rs1] <𝑠 sext(immediate))
                        op = ALU_SLT;
                        break;
                    case FUNCT3_SLTU:
                        // Set if Less Than Unsigned Immediate
                        op = ALU_SLTU;
                        break;
                    case FUNCT3_XOR:
                        op = ALU_XOR;
                        break;
                    case FUNCT3_OR:
                        op = ALU_OR;
                        break;
                    case FUNCT3_AND:
                        op = ALU_AND;
                        break;
                    case FUNCT3_SLL:
                        // slli Shift Left Logical Immediate, x[rd] = x[rs1] << shamt
                        if (imm >= 64) is_instr_illegal = true;
                        op = ALU_SLL;
                        break;
                    case FUNCT3_SRL_SRA:
                        // srli/srai Shift Right Logical/Arithmetic Immediate, x[rd] = (x[rs1] >> shamt)
                        imm = imm & ((1 << 6) - 1);
                        op = ( (fun6 == FUNCT6_SRA) ? ALU_SRA : ALU_SRL);
                        if (fun6 != FUNCT6_NORMAL && fun6 != FUNCT6_SRA) is_instr_illegal = true;
                        break;
                    default:
                        assert(false);
                }
                if (!is_instr_illegal) {
                    int64_t result = alu_exec(GPR[inst.r_type.rs1],imm,op);
                    setGPR(inst.r_type.rd, result);
                }
                break;
            }
                // I-Type Instructions, would be cut to 32 bits
            case OPCODE_OPIMM32: {
                int64_t imm = inst.i_type.imm12;
                Funct7_Op_enum fun7 = (Funct7_Op_enum)((inst.r_type.funct7));
                ALU_Op_enum op;
                switch (inst.i_type.funct3) {
                    case FUNCT3_ADD_SUB:
                        op = ALU_ADD;
                        break;
                    case FUNCT3_SLL:
                        // Shift Left Logical Immediate Word
                        if (imm >= 64) is_instr_illegal = true;
                        op = ALU_SLL;
                        if (fun7 != FUNCT7_NORMAL) is_instr_illegal = true;
                        break;
                    case FUNCT3_SRL_SRA:
                        // Shift Right Logical/Arithmetic Word
                        imm = imm & ((1 << 6) - 1);
                        op = ( (fun7 == FUNCT7_SUB_SRA) ? ALU_SRA : ALU_SRL);
                        if (fun7 != FUNCT7_NORMAL && fun7 != FUNCT7_SUB_SRA) is_instr_illegal = true;
                        break;
                    default:
                        is_instr_illegal = true;
                }
                if (!is_instr_illegal) {
                    int64_t result = alu_exec(GPR[inst.r_type.rs1],imm,op,true);
                    setGPR(inst.r_type.rd, result);
                }
                break;
            }
                // R-Type Instructions
            case OPCODE_OP: {
                ALU_Op_enum op = ALU_NOP;
                switch (inst.r_type.funct7) {
                    case FUNCT7_NORMAL: {
                        switch (inst.r_type.funct3) {
                            case FUNCT3_ADD_SUB:
                                op = ALU_ADD;
                                break;
                            case FUNCT3_SLL:
                                op = ALU_SLL;
                                break;
                            case FUNCT3_SLT:
                                op = ALU_SLT;
                                break;
                            case FUNCT3_SLTU:
                                op = ALU_SLTU;
                                break;
                            case FUNCT3_XOR:
                                op = ALU_XOR;
                                break;
                            case FUNCT3_SRL_SRA:
                                op = ALU_SRL;
                                break;
                            case FUNCT3_OR:
                                op = ALU_OR;
                                break;
                            case FUNCT3_AND:
                                op = ALU_AND;
                                break;
                            default:
                                assert(false);
                        }
                        break;
                    }
                    case FUNCT7_SUB_SRA: {
                        switch (inst.r_type.funct3) {
                            case FUNCT3_ADD_SUB:
                                op = ALU_SUB;
                                break;
                            case FUNCT3_SRL_SRA:
                                op = ALU_SRA;
                                break;
                            default:
                                is_instr_illegal = true;
                        }
                        break;
                    }
                    case FUNCT7_MUL: {
                        switch (inst.r_type.funct3) {
                            case FUNCT3_MUL:
                                op = ALU_MUL;
                                break;
                            case FUNCT3_MULH:
                                op = ALU_MULH;
                                break;
                            case FUNCT3_MULHSU:
                                op = ALU_MULHSU;
                                break;
                            case FUNCT3_MULHU:
                                op = ALU_MULHU;
                                break;
                            case FUNCT3_DIV:
                                op = ALU_DIV;
                                break;
                            case FUNCT3_DIVU:
                                op = ALU_DIVU;
                                break;
                            case FUNCT3_REM:
                                op = ALU_REM;
                                break;
                            case FUNCT3_REMU:
                                op = ALU_REMU;
                                break;
                            default:
                                assert(false);
                        }
                        break;
                    }
                    default:
                        is_instr_illegal = true;
                }
                if (!is_instr_illegal) {
                    int64_t result = alu_exec(GPR[inst.r_type.rs1],GPR[inst.r_type.rs2],op);
                    setGPR(inst.r_type.rd, result);
                }
                break;
            }
                // R-Type Instructions, would be cut to 32 bits
            case OPCODE_OP32: {
                ALU_Op_enum op = ALU_NOP;
                switch (inst.r_type.funct7) {
                    case FUNCT7_NORMAL: {
                        switch (inst.r_type.funct3) {
                            case FUNCT3_ADD_SUB:
                                op = ALU_ADD;
                                break;
                            case FUNCT3_SLL:
                                op = ALU_SLL;
                                break;
                            case FUNCT3_SRL_SRA:
                                op = ALU_SRL;
                                break;
                            default:
                                is_instr_illegal = true;
                        }
                        break;
                    }
                    case FUNCT7_SUB_SRA: {
                        switch (inst.r_type.funct3) {
                            case FUNCT3_ADD_SUB:
                                op = ALU_SUB;
                                break;
                            case FUNCT3_SRL_SRA:
                                op = ALU_SRA;
                                break;
                            default:
                                is_instr_illegal = true;
                        }
                        break;
                    };
                    case FUNCT7_MUL: {
                        switch (inst.r_type.funct3) {
                            case FUNCT3_MUL:
                                op = ALU_MUL;
                                break;
                            case FUNCT3_DIV:
                                op = ALU_DIV;
                                break;
                            case FUNCT3_DIVU:
                                op = ALU_DIVU;
                                break;
                            case FUNCT3_REM:
                                op = ALU_REM;
                                break;
                            case FUNCT3_REMU:
                                op = ALU_REMU;
                                break;
                            default:
                                is_instr_illegal = true;
                        }
                        break;
                    }
                    default:
                        is_instr_illegal = true;
                }
                if (!is_instr_illegal) {
                    int64_t result = alu_exec(GPR[inst.r_type.rs1],GPR[inst.r_type.rs2],op,true);
                    setGPR(inst.r_type.rd, result);
                }
                break;
            }
            // R-Type Instructions, Atomic Memory Operations
            case OPCODE_AMO: {
                uint8_t funct5 = (inst.r_type.funct7) >> 2;
                if (inst.r_type.funct3 != 0b010 && inst.r_type.funct3 != 0b011) {
                    is_instr_illegal = true;
                    break;
                }
                switch (funct5) {
                    case AMOLR: {
                        // Load Reserved <Size>, x[rd] = LoadReserved(M[x[rs1]])
                        if (inst.r_type.rs2 != 0) is_instr_illegal = true;
                        else {
                            if (inst.r_type.funct3 == 0b011) {
                                // Load Reserved Doubleword, x[rd] = LoadReserved64(M[x[rs1]])
                                int64_t result;
                                RV64_ExecFeedbackCode_e exc = ((RV64SV39_MMU*)subBus)->vaddrLoadReserved(GPR[inst.r_type.rs1],
                                                                                         (1<<inst.r_type.funct3),
                                                                                         (uint8_t*)&result);

                                if (exc == exec_ok) setGPR(inst.r_type.rd, result);
                                else {
                                    raiseTrap({ .cause = exc, .interrupt = 0 },GPR[inst.r_type.rs1]);
                                }
                            }
                            else/* if(inst.r_type.funct3 == 0b010)*/ {
                                // Load-Reserved Word, x[rd] = LoadReserved32(M[x[rs1]])
                                assert(inst.r_type.funct3 == 0b010);
                                int32_t result;
                                RV64_ExecFeedbackCode_e exc = ((RV64SV39_MMU*)subBus)->vaddrLoadReserved(GPR[inst.r_type.rs1],
                                                                                         (1<<inst.r_type.funct3),
                                                                                         (uint8_t*)&result);
                                if (exc == exec_ok) setGPR(inst.r_type.rd, result);
                                else {
                                    CSReg_Cause_t cause;
                                    cause.cause = exc;
                                    cause.interrupt = 0;
                                    raiseTrap(cause, GPR[inst.r_type.rs1]);
                                }
                            }
                        }
                        break;
                    }
                    case AMOSC: {
                        // Store-Conditional <Size>, x[rd] = StoreConditional(M[x[rs1]], x[rs2])
                        bool result;
                        RV64_ExecFeedbackCode_e exc = ((RV64SV39_MMU*)subBus)->vaddrStoreConditional(GPR[inst.r_type.rs1],
                                                                                     (1<<inst.r_type.funct3),
                                                                                     (uint8_t*)&GPR[inst.r_type.rs2],
                                                                                     result);
                        if (exc == exec_ok) setGPR(inst.r_type.rd, result);
                        else {
                            raiseTrap({ .cause = exc, .interrupt = 0 }, GPR[inst.r_type.rs1]);
                        }
                        break;
                    }
                    case AMOSWAP: case AMOADD: case AMOXOR: case AMOAND: case AMOOR: case AMOMIN: case AMOMAX: case AMOMINU: case AMOMAXU: {
                        // Atomic Memory Operation: <Op> <Size> , x[rd] = AMO(M[x[rs1]] op x[rs2])
                        int64_t result;
                        RV64_ExecFeedbackCode_e exc = ((RV64SV39_MMU*)subBus)->vaddrAtomicMemOperation(GPR[inst.r_type.rs1],
                                                                                       (1<<inst.r_type.funct3),
                                                                                       (AMO_Funct_enum)(funct5),
                                                                                       GPR[inst.r_type.rs2],
                                                                                       result);
                        if (exc == exec_ok) setGPR(inst.r_type.rd, result);
                        else {
                            raiseTrap({ .cause = exc, .interrupt = 0 }, GPR[inst.r_type.rs1]);
                        }
                        break;
                    }
                    default:
                        is_instr_illegal = true;
                }
                break;
            }
            case OPCODE_FENCE: {
                break;
            }
            case OPCODE_SYSTEM: {
                switch (inst.i_type.funct3) {
                    case FUNCT3_PRIV: {
                        uint64_t funct7 = ((inst.i_type.imm12) & ((1<<12)-1)) >> 5;
                        uint64_t rs2 = (inst.i_type.imm12) & ((1<<5)-1);
                        switch (funct7) {
                            case FUNCT7_ECALL_EBREAK: {
                                if (inst.r_type.rs1 == 0 && inst.r_type.rd == 0) {
                                    switch (rs2) {
                                        case 0: {
                                            // ecall Environment Call
                                            ecall();
                                            break;
                                        }
                                        case 1:
                                            // ebreak Environment Break
                                            ebreak();
                                            break;
                                        default:
                                            is_instr_illegal = true;
                                            break;
                                    }
                                }
                                else is_instr_illegal = true;
                                break;
                            }
                            case FUNCT7_SRET_WFI: {
                                if (inst.r_type.rs1 == 0 && inst.r_type.rd == 0) {
                                    switch (rs2) {
                                        case 0b00010:
                                            // sret Supervisor-mode Exception Return
                                            is_instr_illegal = !sret();
                                            break;
                                        case 0b00101:
                                            // wfi Wait For Interrupt
                                            break;
                                        default:
                                            is_instr_illegal = true;
                                    }
                                }
                                break;
                            }
                            case FUNCT7_MRET: {
                                // mret Machine-mode Exception Return
                                if (rs2 == 0b00010 && inst.r_type.rs1 == 0 && inst.r_type.rd == 0) {
                                    is_instr_illegal = !mret();
                                }
                                else is_instr_illegal = true;
                                break;
                            }
                            case FUNCT7_SFENCE_VMA:
                                // sfence.vma Signal the processor that software has modified the page table
                                // TLB cache is needed to refresh
                                is_instr_illegal = !sfence_vma(GPR[inst.r_type.rs1], GPR[inst.r_type.rs2] & 0xffff);
                                break;
                            default:
                                is_instr_illegal = true;
                        }
                        break;
                    }
                    case FUNCT3_CSRRW: {
                        // csrrw CSR Read and Write, t=CSRs[csr]; CSRs[csr]=x[rs1]; x[rd]=t
                        RV_CSR_Addr_enum csr_index = static_cast<RV_CSR_Addr_enum>(inst.i_type.imm12&((1<<12)-1));
                        is_instr_illegal = !csr_op_permission_check(csr_index, true);
                        uint64_t csr_result;
                        if (!is_instr_illegal) is_instr_illegal = !csrRead(csr_index, csr_result);
                        if (!is_instr_illegal) is_instr_illegal = !csrWrite(csr_index, GPR[inst.i_type.rs1]);
                        if (!is_instr_illegal && inst.i_type.rd) setGPR(inst.i_type.rd, csr_result);
                        break;
                    }
                    case FUNCT3_CSRRS: {
                        // csrrs CSR Read and Set, t=CSRs[csr]; CSRs[csr] = t|x[rs1]; x[rd]=t
                        RV_CSR_Addr_enum csr_index = static_cast<RV_CSR_Addr_enum>(inst.i_type.imm12&((1<<12)-1));
                        is_instr_illegal = !csr_op_permission_check(csr_index, inst.i_type.rs1 != 0);
                        uint64_t csr_result;
                        if (!is_instr_illegal) is_instr_illegal = !csrRead(csr_index, csr_result);
                        if (!is_instr_illegal && inst.i_type.rs1) is_instr_illegal = !csrWrite(csr_index, csr_result |
                                                                                                          GPR[inst.i_type.rs1]);
                        if (!is_instr_illegal && inst.i_type.rd) setGPR(inst.i_type.rd, csr_result);
                        break;
                    }
                    case FUNCT3_CSRRC: {
                        // csrrc CSR Read and Clear, t=CSRs[csr]; CSRs[csr] = t & ~x[rs1]; x[rd] = t
                        RV_CSR_Addr_enum csr_index = static_cast<RV_CSR_Addr_enum>(inst.i_type.imm12&((1<<12)-1));
                        is_instr_illegal = !csr_op_permission_check(csr_index, inst.i_type.rs1 != 0);
                        uint64_t csr_result;
                        if (!is_instr_illegal) is_instr_illegal = !csrRead(csr_index, csr_result);
                        if (!is_instr_illegal && inst.i_type.rs1) is_instr_illegal = !csrWrite(csr_index, csr_result &
                                                                                                          (~GPR[inst.i_type.rs1]));
                        if (!is_instr_illegal && inst.i_type.rd) setGPR(inst.i_type.rd, csr_result);
                        break;
                    }
                    case FUNCT3_CSRRWI: {
                        RV_CSR_Addr_enum csr_index = static_cast<RV_CSR_Addr_enum>(inst.i_type.imm12&((1<<12)-1));
                        is_instr_illegal = !csr_op_permission_check(csr_index, true);
                        uint64_t csr_result;
                        if (!is_instr_illegal) is_instr_illegal = !csrRead(csr_index, csr_result);
                        if (!is_instr_illegal) is_instr_illegal = !csrWrite(csr_index, inst.i_type.rs1);
                        if (!is_instr_illegal && inst.i_type.rd) setGPR(inst.i_type.rd, csr_result);
                        break;
                    }
                    case FUNCT3_CSRRSI: {
                        RV_CSR_Addr_enum csr_index = static_cast<RV_CSR_Addr_enum>(inst.i_type.imm12&((1<<12)-1));
                        is_instr_illegal = !csr_op_permission_check(csr_index, inst.i_type.rs1 != 0);
                        uint64_t csr_result;
                        if (!is_instr_illegal) is_instr_illegal = !csrRead(csr_index, csr_result);
                        if (!is_instr_illegal && inst.i_type.rs1) is_instr_illegal = !csrWrite(csr_index, csr_result |
                                                                                                          inst.i_type.rs1);
                        if (!is_instr_illegal && inst.i_type.rd) setGPR(inst.i_type.rd, csr_result);
                        break;
                    }
                    case FUNCT3_CSRRCI: {
                        RV_CSR_Addr_enum csr_index = static_cast<RV_CSR_Addr_enum>(inst.i_type.imm12&((1<<12)-1));
                        is_instr_illegal = !csr_op_permission_check(csr_index, inst.i_type.rs1 != 0);
                        uint64_t csr_result;
                        if (!is_instr_illegal) is_instr_illegal = !csrRead(csr_index, csr_result);
                        if (!is_instr_illegal && inst.i_type.rs1) is_instr_illegal = !csrWrite(csr_index, csr_result &
                                                                                                          (~(inst.i_type.rs1)));
                        if (!is_instr_illegal && inst.i_type.rd) setGPR(inst.i_type.rd, csr_result);
                        break;
                    }
                    default:
                        is_instr_illegal = true; // HLV
                }
                break;
            }
            default: {
                is_instr_illegal = true;
                break;
            }
        }
    }
    else {
        // rvc
        uint8_t rvc_opcode = ( (inst.val & 0b11) << 3) | ((inst.val >> 13) & 0b111);
        uint8_t rs2 = (inst.val >> 2) & 0x1f;
        uint8_t rs2_c = (inst.val >> 2) & 0x7;
        switch (rvc_opcode) {
            case OPCODE_C_ADDI4SPN: {
                uint8_t rd = 8 + binary_concat(inst.val,4,2,0);
                int64_t imm = binary_concat(inst.val,12,11,4) | binary_concat(inst.val,10,7,6) | binary_concat(inst.val,6,6,2) | binary_concat(inst.val,5,5,3);
                int64_t value = GPR[2] + imm;
                if (imm) setGPR(rd, value); // nzimm
                else is_instr_illegal = true;
                break;
            }
            case OPCODE_C_LW: {
                uint64_t imm = (binary_concat(inst.val,6,6,2) | binary_concat(inst.val,5,5,6) | binary_concat(inst.val,12,10,3));
                uint8_t rs1 = 8 + binary_concat(inst.val,9,7,0);
                uint64_t mem_addr = GPR[rs1] + imm;
                uint8_t rd = 8 + binary_concat(inst.val,4,2,0);
                int32_t buf;
                bool ok = memRead(mem_addr, 4, (unsigned char *) &buf);
                if (ok) setGPR(rd, buf);
                break;
            }
            case OPCODE_C_LD: {
                uint64_t imm = (binary_concat(inst.val,6,5,6) | binary_concat(inst.val,12,10,3));
                uint8_t rs1 = 8 + binary_concat(inst.val,9,7,0);
                uint64_t mem_addr = GPR[rs1] + imm;
                uint8_t rd = 8 + binary_concat(inst.val,4,2,0);
                int64_t buf;
                bool ok = memRead(mem_addr, 8, (unsigned char *) &buf);
                if (ok) setGPR(rd, buf);
                break;
            }
            case OPCODE_C_SW: {
                uint64_t imm = (binary_concat(inst.val,6,6,2) | binary_concat(inst.val,5,5,6) | binary_concat(inst.val,12,10,3));
                uint8_t rs1 = 8 + binary_concat(inst.val,9,7,0);
                uint64_t mem_addr = GPR[rs1] + imm;
                uint8_t rs2 = 8 + binary_concat(inst.val,4,2,0);
                memWrite(mem_addr, 4, (unsigned char *) &GPR[rs2]);
                break;
            }
            case OPCODE_C_SD: {
                uint64_t imm = binary_concat(inst.val,6,5,6) | binary_concat(inst.val,12,10,3);
                uint8_t rs1 = 8 + binary_concat(inst.val,9,7,0);
                uint64_t mem_addr = GPR[rs1] + imm;
                uint8_t rs2 = 8 + binary_concat(inst.val,4,2,0);
                memWrite(mem_addr, 8, (unsigned char *) &GPR[rs2]);
                break;
            }
            case OPCODE_C_ADDI: {
                uint64_t imm = binary_concat(inst.val,12,12,5) | binary_concat(inst.val,6,2,0);
                if (imm >> 5) imm |= 0xffffffffffffffc0u; // sign extend[5]
                uint8_t rd = binary_concat(inst.val,11,7,0);
                if (imm) setGPR(rd, alu_exec(GPR[rd], imm, ALU_ADD)); // nzimm
                break;
            }
            case OPCODE_C_ADDIW: {
                uint64_t imm = binary_concat(inst.val,12,12,5) | binary_concat(inst.val,6,2,0);
                if (imm >> 5) imm |= 0xffffffffffffffc0u; // sign extend[5]
                uint8_t rd = binary_concat(inst.val,11,7,0);
                setGPR(rd, alu_exec(GPR[rd], imm, ALU_ADD, true));
                break;
            }
            case OPCODE_C_LI: {
                int64_t imm = binary_concat(inst.val,12,12,5) | binary_concat(inst.val,6,2,0);
                if (imm >> 5) imm |= 0xffffffffffffffc0u; // sign extend[5]
                uint8_t rd = binary_concat(inst.val,11,7,0);
                setGPR(rd, imm);
                break;
            }
            case OPCODE_C_ADDI16SPN_LUI: {
                uint8_t rd = binary_concat(inst.val,11,7,0);
                if (rd == 2)  { // ADDI16SPN
                    int64_t imm = binary_concat(inst.val,12,12,9) | binary_concat(inst.val,6,6,4) | binary_concat(inst.val,5,5,6) | binary_concat(inst.val,4,3,7) | binary_concat(inst.val,2,2,5);
                    if (imm >> 9) imm |= 0xfffffffffffffc00u; // sign extend[9]
                    uint64_t value = GPR[rd] + imm;
                    if (imm) setGPR(rd, value); // nzimm
                }
                else { // LUI
                    int64_t imm = binary_concat(inst.val,12,12,17) | binary_concat(inst.val,6,2,12);
                    if (imm >> 17) imm |= 0xfffffffffffc0000u; // sign extend[17]
                    if (imm) setGPR(rd, imm); // nzimm
                }
                break;
            }
            case OPCODE_C_ALU: {
                bool is_srli_srai = !(binary_concat(inst.val,11,11,0));
                uint8_t rs1 = 8 + binary_concat(inst.val,9,7,0);
                if (is_srli_srai) { // SRLI, SRAI
                    bool is_srai = binary_concat(inst.val,10,10,0);
                    uint64_t imm = binary_concat(inst.val,12,12,5) | binary_concat(inst.val,6,2,0);
                    if (imm) { // nzimm
                        if (is_srai) {
                            setGPR(rs1, alu_exec(GPR[rs1], imm, ALU_SRA));
                        }
                        else {
                            setGPR(rs1, alu_exec(GPR[rs1], imm, ALU_SRL));
                        }
                    }
                }
                else {
                    bool is_andi = !binary_concat(inst.val,10,10,0);
                    if (is_andi) {
                        int64_t imm = binary_concat(inst.val,12,12,5) | binary_concat(inst.val,6,2,0);
                        if (imm >> 5) imm |= 0xffffffffffffffc0u; // sign extend[5]
                        setGPR(rs1, alu_exec(GPR[rs1], imm, ALU_AND));
                    }
                    else {
                        uint8_t rs2 = 8 + binary_concat(inst.val,4,2,0);
                        uint8_t funct2 = binary_concat(inst.val,6,5,0);
                        bool instr_12 = binary_concat(inst.val,12,12,0);
                        switch (funct2) {
                            case FUNCT2_SUB:
                                setGPR(rs1, alu_exec(GPR[rs1], GPR[rs2], ALU_SUB, instr_12));
                                break;
                            case FUNCT2_XOR_ADDW:
                                setGPR(rs1, alu_exec(GPR[rs1], GPR[rs2], instr_12 ? ALU_ADD : ALU_XOR, instr_12));
                                break;
                            case FUNCT2_OR: {
                                if (instr_12) is_instr_illegal = true;
                                else setGPR(rs1, alu_exec(GPR[rs1], GPR[rs2], ALU_OR));
                                break;
                            }
                            case FUNCT2_AND: {
                                if (instr_12) is_instr_illegal = true;
                                else setGPR(rs1, alu_exec(GPR[rs1], GPR[rs2], ALU_AND));
                                break;
                            }
                        }
                    }
                }
                break;
            }
            case OPCODE_C_J: {
                int64_t imm = binary_concat(inst.val,12,12,11) | binary_concat(inst.val,11,11,4) |
                              binary_concat(inst.val,10,9,8) | binary_concat(inst.val,8,8,10) |
                              binary_concat(inst.val,7,7,6) | binary_concat(inst.val,6,6,7) |
                              binary_concat(inst.val,5,3,1) | binary_concat(inst.val,2,2,5);
                if (imm >> 11) imm |= 0xfffffffffffff000u; // sign extend [11]
                uint64_t npc = currentProgramCounter + imm;
                currentProgramCounter = npc;
                is_new_pc_set = true;
                break;
            }
            case OPCODE_C_BEQZ: {
                int64_t imm =   binary_concat(inst.val,12,12,8) | binary_concat(inst.val,11,10,3) | binary_concat(inst.val,6,5,6) | binary_concat(inst.val,4,3,1) | binary_concat(inst.val,2,2,5);
                if (imm>>8) imm |= 0xfffffffffffffe00u; // sign extend [8]
                uint8_t rs1 = 8 + binary_concat(inst.val,9,7,0);
                if (GPR[rs1] == 0) {
                    currentProgramCounter = currentProgramCounter + imm;
                    is_new_pc_set = true;
                }
                break;
            }
            case OPCODE_C_BNEZ: {
                int64_t imm =   binary_concat(inst.val,12,12,8) | binary_concat(inst.val,11,10,3) | binary_concat(inst.val,6,5,6) | binary_concat(inst.val,4,3,1) | binary_concat(inst.val,2,2,5);
                if (imm>>8) imm |= 0xfffffffffffffe00u; // sign extend [8]
                uint8_t rs1 = 8 + binary_concat(inst.val,9,7,0);
                if (GPR[rs1] != 0) {
                    currentProgramCounter = currentProgramCounter + imm;
                    is_new_pc_set = true;
                }
                break;
            }
            case OPCODE_C_SLLI: {
                int64_t imm =   binary_concat(inst.val,12,12,5) | binary_concat(inst.val,6,2,0);
                uint8_t rs1 = binary_concat(inst.val,11,7,0);
                if (imm) setGPR(rs1, alu_exec(GPR[rs1], imm, ALU_SLL)); // nzimm
                break;
            }
            case OPCODE_C_LWSP: {
                uint64_t imm = (binary_concat(inst.val,6,4,2) | binary_concat(inst.val,3,2,6) | binary_concat(inst.val,12,12,5));
                uint64_t mem_addr = GPR[2] + imm;
                uint8_t rd = binary_concat(inst.val,11,7,0);
                int32_t buf;
                bool ok = memRead(mem_addr, 4, (unsigned char *) &buf);
                if (ok) setGPR(rd, buf);
                // TODO: rd != 0
                break;
            }
            case OPCODE_C_LDSP: {
                uint64_t imm = (binary_concat(inst.val,6,5,3) | binary_concat(inst.val,4,2,6) | binary_concat(inst.val,12,12,5));
                uint64_t mem_addr = GPR[2] + imm;
                uint8_t rd = binary_concat(inst.val,11,7,0);
                int64_t buf;
                bool ok = memRead(mem_addr, 8, (unsigned char *) &buf);
                if (ok) setGPR(rd, buf);
                // TODO: rd != 0
                break;
            }
            case OPCODE_C_JR_MV_EB_JALR_ADD: {
                bool is_ebreak_jalr_add = binary_concat(inst.val,12,12,0);
                uint8_t rs2 = binary_concat(inst.val,6,2,0);
                uint8_t rs1 = binary_concat(inst.val,11,7,0);
                if (is_ebreak_jalr_add) {
                    if (rs2 == 0) { // EBREAK, JALR
                        if (rs1 == 0) { // EBREAK
                            ebreak();
                        }
                        else { // JALR
                            uint64_t npc = GPR[rs1];
                            if (npc & 1) npc ^= 1;
                            CSReg_Cause_t cause;
                            cause.interrupt = 0;
                            cause.cause = exec_instr_misalign;
                            if (npc % PC_ALIGN) raiseTrap(cause,npc);
                            else {
                                setGPR(1, currentProgramCounter + 2);
                                currentProgramCounter = npc;
                                is_new_pc_set = true;
                            }
                        }
                    }
                    else { // ADD
                        setGPR(rs1, alu_exec(GPR[rs1], GPR[rs2], ALU_ADD));
                    }
                }
                else {
                    if (rs2 == 0) { // JR
                        if (rs1 == 0) is_instr_illegal = true;
                        else {
                            uint64_t npc = GPR[rs1];
                            if (npc & 1) npc ^= 1;
                            CSReg_Cause_t cause;
                            cause.interrupt = 0;
                            cause.cause = exec_instr_misalign;
                            if (npc % PC_ALIGN) raiseTrap(cause,npc);
                            else {
                                currentProgramCounter = npc;
                                is_new_pc_set = true;
                            }
                        }
                    }
                    else { // MV
                        setGPR(rs1, GPR[rs2]);
                        // TODO: rs1(rd) != 0
                    }
                }
                break;
            }
            case OPCODE_C_SWSP: {
                uint64_t imm = (binary_concat(inst.val,12,9,2) | binary_concat(inst.val,8,7,6));
                uint64_t mem_addr = GPR[2] + imm;
                uint8_t rs2 = binary_concat(inst.val,6,2,0);
                memWrite(mem_addr, 4, (unsigned char *) &GPR[rs2]);
                break;
            }
            case OPCODE_C_SDSP: {
                uint64_t imm = (binary_concat(inst.val,12,10,3) | binary_concat(inst.val,9,7,6));
                uint64_t mem_addr = GPR[2] + imm;
                uint8_t rs2 = binary_concat(inst.val,6,2,0);
                memWrite(mem_addr, 8, (unsigned char *) &GPR[rs2]);
                break;
            }
            default: {
                is_instr_illegal = true;
                break;
            }
        }
    }

exception:
    if(is_instr_illegal) {
        raiseTrap({ .cause = exec_illegal_instr, .interrupt = 0 }, inst.val);
    }
    if(needTrap) {
        currentProgramCounter = trapProgramCounter;
    }
    else if(!is_new_pc_set){
        currentProgramCounter = currentProgramCounter + (is_compressed_inst ? 2 : 4);
    }
post_exec:
    if(!needTrap) { minstret++; }
    needTrap = false;
    CSReg_MStatus_t* mstatus = (CSReg_MStatus_t*)&status;
    // seems useless, to be removed
    assert(mstatus->blank0 == 0);
    assert(mstatus->blank1 == 0);
    assert(mstatus->blank2 == 0);
    assert(mstatus->blank3 == 0);
    assert(mstatus->blank4 == 0);
    return;
}

