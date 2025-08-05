#pragma once

#include <cstdint>

#define binary_concat(value, r, l, shift) ( ( ((value) >> (l)) & ((1<<((r)-(l)+1))-1)) << (shift) )
namespace MiniCPU_ns {
typedef union {
    struct rv_r {
        unsigned int opcode: 7;
        unsigned int rd: 5;
        unsigned int funct3: 3;
        unsigned int rs1: 5;
        unsigned int rs2: 5;
        unsigned int funct7: 7;
    } r_type;
    struct rv_i {
        unsigned int opcode: 7;
        unsigned int rd: 5;
        unsigned int funct3: 3;
        unsigned int rs1: 5;
        int imm12: 12;
    } i_type;
    struct rv_s {
        unsigned int opcode: 7;
        unsigned int imm_4_0: 5;
        unsigned int funct3: 3;
        unsigned int rs1: 5;
        unsigned int rs2: 5;
        int imm_11_5: 7;
    } s_type;
    struct rv_b {
        unsigned int opcode: 7;
        unsigned int imm_11: 1;
        unsigned int imm_4_1: 4;
        unsigned int funct3: 3;
        unsigned int rs1: 5;
        unsigned int rs2: 5;
        unsigned int imm_10_5: 6;
        int imm_12: 1;
    } b_type;
    struct rv_u {
        unsigned int opcode: 7;
        unsigned int rd: 5;
        int imm_31_12: 20;
    } u_type;
    struct rv_j {
        unsigned int opcode: 7;
        unsigned int rd: 5;
        unsigned int imm_19_12: 8;
        unsigned int imm_11: 1;
        unsigned int imm_10_1: 10;
        int imm_20: 1;
    } j_type;
    uint32_t val;
} RVInst_t;

enum RV64I_OpCode_enum {
    OPCODE_LUI = 0b0110111,
    OPCODE_AUIPC = 0b0010111,
    OPCODE_JAL = 0b1101111,
    OPCODE_JALR = 0b1100111,
    OPCODE_BRANCH = 0b1100011,
    OPCODE_LOAD = 0b0000011,
    OPCODE_STORE = 0b0100011,
    // --- Custom OP Code Begins
    OPCODE_LOAD_ASYNC = 0b0001011,
    OPCODE_STORE_ASYNC = 0b0101011,
    // --- Custom OP Code Ends
    OPCODE_OPIMM = 0b0010011,
    OPCODE_OPIMM32 = 0b0011011,
    OPCODE_OP = 0b0110011,
    OPCODE_OP32 = 0b0111011,
    OPCODE_FENCE = 0b0001111,
    OPCODE_SYSTEM = 0b1110011,
    OPCODE_AMO = 0b0101111
};

// Concat(instr(1,0),instr(15,13))
enum RV64C_OpCode_enum { // without f/d
    OPCODE_C_ADDI4SPN = 0b00000,
    OPCODE_C_LW = 0b00010,
    OPCODE_C_LD = 0b00011,
    OPCODE_C_SW = 0b00110,
    OPCODE_C_SD = 0b00111,
    OPCODE_C_ADDI = 0b01000,
    OPCODE_C_ADDIW = 0b01001,
    OPCODE_C_LI = 0b01010,
    OPCODE_C_ADDI16SPN_LUI = 0b01011,
    OPCODE_C_ALU = 0b01100,
    OPCODE_C_J = 0b01101,
    OPCODE_C_BEQZ = 0b01110,
    OPCODE_C_BNEZ = 0b01111,
    OPCODE_C_SLLI = 0b10000,
    OPCODE_C_LWSP = 0b10010,
    OPCODE_C_LDSP = 0b10011,
    OPCODE_C_JR_MV_EB_JALR_ADD = 0b10100,
    OPCODE_C_SWSP = 0b10110,
    OPCODE_C_SDSP = 0b10111
};

enum RV64C_Funct2_enum {
    FUNCT2_SUB = 0b00,
    FUNCT2_XOR_ADDW = 0b01,
    FUNCT2_OR = 0b10,
    FUNCT2_AND = 0b11
};

enum Funct3_Branch_enum {
    FUNCT3_BEQ = 0b000,
    FUNCT3_BNE = 0b001,
    FUNCT3_BLT = 0b100,
    FUNCT3_BGE = 0b101,
    FUNCT3_BLTU = 0b110,
    FUNCT3_BGEU = 0b111
};

enum Funct3_Load_enum {
    FUNCT3_LB = 0b000,
    FUNCT3_LH = 0b001,
    FUNCT3_LW = 0b010,
    FUNCT3_LD = 0b011,
    FUNCT3_LBU = 0b100,
    FUNCT3_LHU = 0b101,
    FUNCT3_LWU = 0b110
};

enum Funct3_Store_enum {
    FUNCT3_SB = 0b000,
    FUNCT3_SH = 0b001,
    FUNCT3_SW = 0b010,
    FUNCT3_SD = 0b011
};

enum Funct3_Op_enum {
    FUNCT3_ADD_SUB = 0b000,
    FUNCT3_SLL = 0b001,
    FUNCT3_SLT = 0b010,
    FUNCT3_SLTU = 0b011,
    FUNCT3_XOR = 0b100,
    FUNCT3_SRL_SRA = 0b101,
    FUNCT3_OR = 0b110,
    FUNCT3_AND = 0b111
};

enum Funct3_M_enum {
    FUNCT3_MUL = 0b000,
    FUNCT3_MULH = 0b001,
    FUNCT3_MULHSU = 0b010,
    FUNCT3_MULHU = 0b011,
    FUNCT3_DIV = 0b100,
    FUNCT3_DIVU = 0b101,
    FUNCT3_REM = 0b110,
    FUNCT3_REMU = 0b111
};

enum Funct3_System_enum {
    FUNCT3_PRIV = 0b000,
    FUNCT3_CSRRW = 0b001,
    FUNCT3_CSRRS = 0b010,
    FUNCT3_CSRRC = 0b011,
    FUNCT3_HLSV = 0b100,
    FUNCT3_CSRRWI = 0b101,
    FUNCT3_CSRRSI = 0b110,
    FUNCT3_CSRRCI = 0b111
};

enum Funct7_Priv_enum {
    FUNCT7_ECALL_EBREAK = 0b0000000,
    FUNCT7_SRET_WFI = 0b0001000,
    FUNCT7_MRET = 0b0011000,
    FUNCT7_SFENCE_VMA = 0b0001001
};

enum Funct7_Op_enum {
    FUNCT7_NORMAL = 0b0000000,
    FUNCT7_SUB_SRA = 0b0100000,
    FUNCT7_MUL = 0b0000001
};

enum Funct6_Op_enum {
    FUNCT6_NORMAL = 0b000000,
    FUNCT6_SRA = 0b010000
};

enum AMO_Funct_enum {
    AMOLR = 0b00010,
    AMOSC = 0b00011,
    AMOSWAP = 0b00001,
    AMOADD = 0b00000,
    AMOXOR = 0b00100,
    AMOAND = 0b01100,
    AMOOR = 0b01000,
    AMOMIN = 0b10000,
    AMOMAX = 0b10100,
    AMOMINU = 0b11000,
    AMOMAXU = 0b11100
};

}