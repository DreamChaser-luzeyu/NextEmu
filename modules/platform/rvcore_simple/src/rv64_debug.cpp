#include "sdk/console.h"

#include "RV64Core.h"

//FuncReturnFeedback_e RVCore_ns::RV64Core::VAddrRead_CoreDebugAPI(uint64_t start_addr, uint64_t size, uint8_t *buffer) {
//    if(currentPrivMode == M_MODE) return PAddrRead_CoreDebugAPI(start_addr, size, buffer);
//    // --- Read data byte singly to avoid cross-page mis-align
//    for(uint64_t i = 0; i < size; i++) {
//        VAddr_RW_Feedback feedback = sv39MMU->VAddr_ReadBuffer_MMU_API(start_addr + i, 1, &(buffer[i]));
//        if(feedback != VADDR_ACCESS_OK) break;
//    }
//    return MEMU_OK;
//}
//
//FuncReturnFeedback_e RVCore_ns::RV64Core::VAddrWrite_CoreDebugAPI(uint64_t start_addr, uint64_t size, const uint8_t *buffer) {
//    if(currentPrivMode == M_MODE) return PAddrWrite_CoreDebugAPI(start_addr, size, buffer);
//    // --- Write data byte singly to avoid cross-page mis-align
//    for(uint64_t i = 0; i < size; i++) {
//        VAddr_RW_Feedback feedback = sv39MMU->VAddr_WriteBuffer_MMU_API(start_addr + i, 1, &(buffer[i]));
//        if(feedback != VADDR_ACCESS_OK) break;
//    }
//    return MEMU_OK;
//}

FuncReturnFeedback_e RVCore_ns::RV64Core::MemRead_CoreDebugAPI(uint64_t start_addr, uint64_t size, uint8_t *buffer) {
//    if(currentPrivMode == M_MODE || satp.mode == 0) {
//        LOG_DEBUG("MMU not enabled, currently reading PADDR");

        if(doLoad(start_addr, size, buffer) == Interface_ns::FB_SUCCESS) return MEMU_OK;
        return MEMU_UNKNOWN;
//    }
//    LOG_DEBUG("MMU enabled, currently reading VADDR");
//    for(uint64_t i = 0; i < size; i++) {
//        VAddr_RW_Feedback feedback = sv39MMU->VAddr_ReadBuffer_MMU_API(start_addr + i, 1, &(buffer[i]));
//        if(feedback != VADDR_ACCESS_OK) return MEMU_NOT_ACCESSIBLE;
//    }
//    return MEMU_OK;
}

FuncReturnFeedback_e RVCore_ns::RV64Core::MemWrite_CoreDebugAPI(uint64_t start_addr, uint64_t size, const uint8_t *buffer) {
//    if(currentPrivMode == M_MODE || satp.mode == 0) {
//        LOG_DEBUG("MMU not enabled, currently writing PADDR");
//        return sv39MMU->getSysBus_Debug()->PAddr_WriteBuffer_MMIOBus_API(start_addr, size, buffer);
//    }
//    LOG_DEBUG("MMU enabled, currently writing VADDR");
//    for(uint64_t i = 0; i < size; i++) {
//        VAddr_RW_Feedback feedback = sv39MMU->VAddr_WriteBuffer_MMU_API(start_addr + i, 1, &(buffer[i]));
//        if(feedback != VADDR_ACCESS_OK) return MEMU_NOT_ACCESSIBLE;
//    }
//    return MEMU_OK;
    if(doStore(start_addr, size, buffer) == Interface_ns::FB_SUCCESS) return MEMU_OK;
    return MEMU_UNKNOWN;

}

FuncReturnFeedback_e RVCore_ns::RV64Core::DumpRegister_CoreAPI(std::vector<RegisterItem_t> &regs)
{
    // --- Dump GPRs
    for(int i = 0; i < 32; i++) {
        const char* name = GPR_NAME_ARR[i].c_str();
        RegisterItem_t reg_item = {
                .reg_id = (uint16_t)i,
                .size = 8,
                .val = { .u64_val = (uint64_t)GPR[i] }
        };
        strcpy(reg_item.disp_name, name);
        regs.push_back(reg_item);
    }
    // --- Dump PC
    RegisterItem_t reg_item = {
            .reg_id = (uint16_t)32,
            .size = 8,
            .val = { .u64_val = currentProgramCounter }
    };
    strcpy(reg_item.disp_name, PC_NAME.c_str());
    regs.push_back(reg_item);
    return MEMU_OK;
}

FuncReturnFeedback_e RVCore_ns::RV64Core::WriteAllRegister_CoreAPI(const std::vector<RegisterItem_t> &regs) {
    if(regs.size() != 33) { return MEMU_INVALID_PARAM; }
    // --- Write GPRs
    for(int i = 0; i < 32; i++) {
        GPR[i] = regs.at(i).val.u64_val;
    }
    // --- Write PC
    currentProgramCounter = regs.at(32).val.u64_val;
    return MEMU_OK;
}
