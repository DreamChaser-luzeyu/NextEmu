#include <climits>

#include "rvcore_minicpu/MiniCPUCore.h"

/**
 * Do calculations
 * @param a operand 1
 * @param b operand 2
 * @param op operation
 * @param op_32 If to cut off result to 32 bits
 * @return Calculation result
 */
int64_t MiniCPU_ns::RV64Core::alu_exec(int64_t a, int64_t b, ALU_Op_enum op, bool op_32) {
    if (op_32) {
        a = (int32_t)a;
        b = (int32_t)b;
    }
    int64_t result = 0;
    switch (op) {
        case ALU_ADD:
            result = a + b;
            break;
        case ALU_SUB:
            result = a - b;
            break;
        case ALU_SLL:
            result = a << (b & (op_32 ? 0x1f : 0x3f));
            break;
        case ALU_SLT:
            result = a < b;
            break;
        case ALU_SLTU:
            result = (uint64_t)a < (uint64_t)b;
            break;
        case ALU_XOR:
            result = a ^ b;
            break;
        case ALU_SRL:
            result = (uint64_t)(op_32 ? (a&0xffffffff) : a) >> (b & (op_32 ? 0x1f : 0x3f));
            break;
        case ALU_SRA:
            result = a >> (b & (op_32 ? 0x1f : 0x3f));
            break;
        case ALU_OR:
            result = a | b;
            break;
        case ALU_AND:
            result = a & b;
            break;
        case ALU_MUL:
            result = a * b;
            break;
        case ALU_MULH:
            result = ((__int128_t)a * (__int128_t)b) >> 64;
            break;
        case ALU_MULHU:
            result = (static_cast<__uint128_t>(static_cast<uint64_t>(a))*static_cast<__uint128_t>(static_cast<uint64_t>(b))) >> 64;
            break;
        case ALU_MULHSU:
            result = (static_cast<__int128_t>(a)*static_cast<__uint128_t>(static_cast<uint64_t>(b))) >> 64;
            break;
        case ALU_DIV:
            if (b == 0) result = -1;
            else if (a == LONG_MIN && b == -1) result = LONG_MIN;
            else result = a / b;
            break;
        case ALU_DIVU:
            if (b == 0) result = ULONG_MAX;
            else if (op_32) result = (uint32_t)a / (uint32_t)b;
            else result = ((uint64_t)a) / ((uint64_t)b);
            break;
        case ALU_REM:
            if (b == 0) result = a;
            else if (a == LONG_MIN && b == -1) result = 0;
            else result = a % b;
            break;
        case ALU_REMU:
            if (b == 0) result = a;
            else if (op_32) result = (uint32_t)a % (uint32_t)b;
            else result = (uint64_t)a % (uint64_t)b;
            break;
        default:
            assert(false);
    }
    if (op_32) result = (int32_t)result;
    return result;
}


