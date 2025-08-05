#pragma once

#include <vector>

#include "status_enum.h"
#include "struct_defs.h"

namespace Interface_ns {

class Debuggable_I {
public:
    /**
     * @brief Dump core registers to an array
     * @param regs The array to store the dump of core registers
     */
    virtual FuncReturnFeedback_e DumpAllRegister_DebugAPI(std::vector<RegisterItem_t>& regs) = 0;

    virtual FuncReturnFeedback_e WriteAllRegister_DebugAPI(const std::vector<RegisterItem_t>& regs) = 0;

    virtual FuncReturnFeedback_e MemRead_DebugAPI(uint64_t start_addr, uint64_t size, uint8_t *buffer) = 0;

    virtual FuncReturnFeedback_e MemWrite_DebugAPI(uint64_t start_addr, uint64_t size, const uint8_t *buffer) = 0;

    virtual FuncReturnFeedback_e DumpProgramCounter_DebugAPI(RegisterItem_t& reg) = 0;

    virtual FuncReturnFeedback_e WriteProgramCounter_DebugAPI(RegItemVal_t reg_val) = 0;

    virtual FuncReturnFeedback_e Step_DebugAPI() = 0;

    virtual int GetISABitLen_DebugAPI() { return 64; }
};

}
