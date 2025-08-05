#include <iomanip>
#include <cassert>

#include "sdk/base/GDBStub.h"
#include "include/gdb_rsp.h"

void GDBStub::cmd_ReadAllRegister() {
    // --- Get RegisterItem from core
    std::vector<RegisterItem_t> regs;
    debuggedCore->DumpAllRegister_DebugAPI(regs);
    // --- Convert to GDB format
    std::stringstream dump_stringstream;
    for(auto reg : regs) {
        // Dump the register in little-endian mode
        for(int i = 0; i < reg.size; i++) {
            uint8_t* ptr = (uint8_t*)&reg.val.u64_val;
            dump_stringstream << std::hex
                              << std::setw(2)
                              << std::setfill('0')
                              << (uint32_t)(*(ptr + i));
        }
    }
    string dump_str = dump_stringstream.str();
    char *data = (char*)alloca(dump_str.length() + 1);
    strcpy(data, dump_str.c_str());
    packingAndSend(data, strlen(data));
}

void GDBStub::cmd_WriteAllRegister(const GDB_RSP_Packet &packet) {
    assert(packet.packetType == CMD_REG_WRITE_ALL);
    // --- Check if param valid & parse param
    if(packet.paramList.size() != 1) {
        // Send empty packet & return
        packingAndSend("", 0);
        return;
    }
    // Check if reg data stream length valid
    if((packet.paramList.at(0).length() % (debuggedCore->GetISABitLen_DebugAPI() / 4)) != 0) {
        // Send empty packet & return
        packingAndSend("", 0);
        return;
    }
    // --- Cal reg num
    // One char ('0' ~ 'F') could represent 4 bit of data
    int reg_str_len = (debuggedCore->GetISABitLen_DebugAPI() / 4);
    int reg_num = (packet.paramList.at(0).length()) / reg_str_len;
    int reg_size = debuggedCore->GetISABitLen_DebugAPI() / 8;
    const char* all_reg_str = packet.paramList.at(0).c_str();
    uint8_t* all_reg_data = (uint8_t*)alloca(reg_num * reg_size);
    ParseByteStreamStr(all_reg_str, strlen(all_reg_str), all_reg_data);
    vector<RegisterItem_t> regs;
    for(int i = 0; i < reg_num; i++) {
        RegisterItem_t reg_item;
        reg_item.size = reg_size;
        reg_item.reg_id = i;
        memcpy(&(reg_item.val.u64_val), &(all_reg_data[i * reg_size]), reg_size);
        regs.push_back(reg_item);
    }
    FuncReturnFeedback_e feedback = debuggedCore->WriteAllRegister_DebugAPI(regs);
    if(feedback != MEMU_OK) {
        const char* err = "E01";
        packingAndSend(err, strlen(err));
    }
}

void GDBStub::cmd_GetHaltReason() {
    const char* halt_reason = "S05";
    packingAndSend(halt_reason, strlen(halt_reason));
}

void GDBStub::cmd_QSupported(const GDB_RSP_Packet &packet) {
    std::stringstream supported_stringstream;
    for(const auto& str : packet.paramList) {
        if(strcmp(str.c_str(), "hwbreak") == 0) supported_stringstream << "hwbreak+";
    }
    string supported_str = supported_stringstream.str();
    char* data = (char*)alloca(supported_str.length() + 1);
    strcpy(data, supported_str.c_str());
    packingAndSend(data, strlen(data));
}

void GDBStub::cmd_Unknown() {
    packingAndSend("", 0);
}

void GDBStub::cmd_MemRead(const GDB_RSP_Packet &packet) {
    assert(packet.packetType == CMD_MEM_READ);
    // --- Check if param valid & parse param
    if(packet.paramList.size() != 2) {
        // Send empty packet & return
        packingAndSend("", 0);
        return;
    }
    uint64_t addr = strtoll(packet.paramList.at(0).c_str(), nullptr, 16);
    uint64_t len = strtoll(packet.paramList.at(1).c_str(), nullptr, 16);
    // --- Read data
    uint8_t* data = (uint8_t*)alloca(len);
    FuncReturnFeedback_e feedback = debuggedCore->MemRead_DebugAPI(addr, len, data);
    // --- Encode data & send
    if(feedback == MEMU_OK) {
        // 2 char represents 1 byte
        char *data_str = (char *) alloca(2 * len + 1);
        data_str[2 * len] = '\0';
        for (int i = 0; i < len; i++) {
            sprintf(&(data_str[2 * i]), "%02x", data[i]);
        }
        packingAndSend(data_str, strlen(data_str));
    }
    else {
        const char* err = "E01";
        packingAndSend(err, strlen(err));
    }
}

void GDBStub::cmd_MemWrite(const GDB_RSP_Packet &packet) {
    assert(packet.packetType == CMD_MEM_WRITE);
    // --- Check if param valid & parse param
    if((packet.paramList.size() != 3)) {
        // Send empty packet & return
        packingAndSend("", 0);
        return;
    }
    uint64_t addr = strtoll(packet.paramList.at(0).c_str(), nullptr, 16);
    uint64_t len = strtoll(packet.paramList.at(1).c_str(), nullptr, 16);
    if((2 * len) != packet.paramList.at(2).length()) {
        packingAndSend("", 0);
        return;
    }
    uint8_t* data = (uint8_t*)alloca(len);
    ParseByteStreamStr(packet.paramList.at(2).c_str(), packet.paramList.at(2).length(), data);
    // --- Write data to mem
    FuncReturnFeedback_e feedback = debuggedCore->MemWrite_DebugAPI(addr, len, data);
    if(feedback != MEMU_OK) {
        const char* err = "E01";
        packingAndSend(err, strlen(err));
    }
}

void GDBStub::cmd_InsertBreakpoint(const GDB_RSP_Packet &packet) {
    assert(packet.packetType == CMD_INSERT_BREAKPOINT);
    // --- Check if param valid & parse param
    if((packet.paramList.size() != 3)) {
        // Send empty packet & return
        packingAndSend("", 0);
        return;
    }
    int watchpoint_type = strtol(packet.paramList.at(0).c_str(), nullptr, 16);
    if((watchpoint_type != 0) && (watchpoint_type != 1)) {
        packingAndSend("", 0);
        return;
    }
    uint64_t addr = strtoll(packet.paramList.at(1).c_str(), nullptr, 16);
    this->insertWatchPoint({ .u64_val = addr });
    LOG_DEBUG("Insert watchpoint at %016lx", addr);
    // neglect the `kind` param
    const char* resp = "OK";
    packingAndSend(resp, strlen(resp));
}

void GDBStub::cmd_RemoveBreakpoint(const GDB_RSP_Packet &packet) {
    assert(packet.packetType == CMD_REMOVE_BREAKPOINT);
    // --- Check if param valid & parse param
    if((packet.paramList.size() != 3)) {
        // Send empty packet & return
        packingAndSend("", 0);
        return;
    }
    int watchpoint_type = strtol(packet.paramList.at(0).c_str(), nullptr, 16);
    if((watchpoint_type != 0) && (watchpoint_type != 1)) {
        packingAndSend("", 0);
        return;
    }
    uint64_t addr = strtoll(packet.paramList.at(1).c_str(), nullptr, 16);
    this->removeWatchPoint({ .u64_val = addr });
    LOG_DEBUG("Remove watchpoint at %016lx", addr);
    // neglect the `kind` param
    const char* resp = "OK";
    packingAndSend(resp, strlen(resp));
}

void GDBStub::cmd_Continue(const GDB_RSP_Packet &packet) {
    assert(packet.packetType == CMD_CONTINUE);
    // --- Check if param valid & parse param
    if((packet.paramList.size() != 0)) {
        // Send empty packet & return
        packingAndSend("", 0);
        return;
    }
    while(!hitsWatchPoint()) {
        debuggedCore->Step_DebugAPI();
//        usleep(100 * 1000);
    }
    LOG_DEBUG("Hit watchpoint!");
    const char* halt_reason = "S05";
    packingAndSend(halt_reason, strlen(halt_reason));
}


