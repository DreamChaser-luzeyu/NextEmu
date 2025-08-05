// #include <regex>
// #include <fstream>
// #include <unordered_set>
// #include "sdk/test/test.h"
// #include "sdk/gdb_stub/gdb_rsp.h"
// #include "sdk/gdb_stub/GDBStub.h"
//
// #include "misc/obj_factory.h"
//
// REGISTER_TEST_UNIT(GDB_RSP_Test);
//
// TEST_CASE(test_gdb_wait_for_debugger, "GDB Wait For Debugger Test") {
//     // --- Bus
//     MMIOBus_I* sysBus;
//     // --- Cores
//     ProcessorCore_I* core0 = nullptr;
//     ProcessorCore_I* core1 = nullptr;
//     // --- Intcs
//     IntCtrl_I* rvPLIC = nullptr;
//     IntCtrl_I* rvCLInt = nullptr;
//     // --- Devs
//     MMIODev_I* uartlite = nullptr;
//     MMIODev_I* ram = nullptr;
//     // ----- Get used instances from factory
//     sysBus = Bus_GetInstance({ .desc_str = "mmio_bus" });
//     core0 = Core_GetInstance({ .desc_str = "rv64core_simple" }, sysBus, 0);
//     core1 = Core_GetInstance({ .desc_str = "rv64core_simple" }, sysBus, 1);
//     rvCLInt = IntCtrl_GetInstance({ .desc_str = "rv_clint" });
//     rvPLIC = IntCtrl_GetInstance({ .desc_str = "rv_plic" });
//     ram = MMIO_Dev_GetInstance({ .desc_str = "simple_ram" });
//     uartlite = MMIO_Dev_GetInstance({ .desc_str = "uart_lite" });
//     // ----- Init the platform
//     // --- Init cores
//     core0->WriteProgramCounter_CoreAPI({ .u64_val = 0x80000000 });
//     core1->WriteProgramCounter_CoreAPI({ .u64_val = 0x80000000 });
//     core1->SetGPRByIndex_CoreAPI(10, 1);
//     // --- Init image
//     const char* init_file = "/home/luzeyu/temp/memu_linux/opensbi-1.3.1/build/platform/generic/firmware/fw_payload.bin";
//     std::ifstream file(init_file,std::ios::in | std::ios::binary);
//     uint64_t file_size = std::filesystem::file_size(init_file);
//     uint8_t* bin_data = new uint8_t[file_size];
//     file.read((char*)bin_data,file_size);
//     ram->WriteBuffer_MMIODev_API(0, file_size, bin_data);
//     // --- Register devs to bus
//     sysBus->RegisterMMIODev_MMIOBus_API(ram, 0x80000000);
//     sysBus->RegisterMMIODev_MMIOBus_API(uartlite, 0x60100000);
//     // --- Connect peripherals&cores to intcs
//     rvCLInt->AttachCoreIntStatus(core0->GetIntStatusPtr_Core(), 0);
//     rvCLInt->AttachCoreIntStatus(core1->GetIntStatusPtr_Core(), 1);
//     rvPLIC->AttachCoreIntStatus(core0->GetIntStatusPtr_Core(), 0);
//     rvPLIC->AttachCoreIntStatus(core1->GetIntStatusPtr_Core(), 1);
//     rvPLIC->RegisterDev_IntCtrl_API(1, uartlite);
//     // --- Register intcs to bus
//     sysBus->RegisterMMIODev_MMIOBus_API(rvCLInt, 0x2000000);
//     sysBus->RegisterMMIODev_MMIOBus_API(rvPLIC, 0xc000000);
//
//     GDBStub gdbStub("127.0.0.1", 5679);
// //    GDBStub gdbStub(fileno(stdin), fileno(stdout));
// //    gdbStub.Debug_GDBStub(core0);
// }
//
// TEST_CASE(reg_format_test, "Reg format test") {
//     std::stringstream dump_stringstream;
//
//     RegisterItem_t reg = {
//             .reg_id = 0,
//             .disp_name = "pc",
//             .size = 8,
//             .val = { .u64_val = 0xf }
//     };
//     dump_stringstream << std::hex
//                       << std::setw(reg.size * 2)
//                       << std::setfill('0')
//                       << reg.val.u64_val;
//     LOG_DEBUG("%s", dump_stringstream.str().c_str());
//     assert(strcmp(dump_stringstream.str().c_str(), "000000000000000f") == 0);
// }
//
// TEST_CASE(regex_test, "regex test") {
//     using std::regex_match;
//     using std::regex_search;
//     using std::regex;
//
//     string raw = R"Ignored(BuZhuanYi\n\r\t)Ignored";
//     LOG_DEBUG("%s", raw.c_str());
//
//     assert(std::regex_match("qSupported:"
//                             "multiprocess+;"
//                             "swbreak+;"
//                             "hwbreak+;"
//                             "qRelocInsn+;"
//                             "fork-events+;"
//                             "vfork-events+;"
//                             "exec-events+;"
//                             "vContSupported+;"
//                             "QThreadEvents+;"
//                             "no-resumed+;"
//                             "memory-tagging+",
//                             regex(PacketTypeRegexStr_GV.at(CMD_Q_SUPPORTED).at(0))));
//
//     assert(std::regex_match("?",
//                             regex(PacketTypeRegexStr_GV.at(CMD_QUERY_HALT_REASON).at(0))));
// //    assert(std::regex_match("qAttached",
// //                            regex(PacketTypeRegexStr_GV.at(CMD_Q_ATTACHED).at(0))));
//
//     // --- CMD_MEM_READ
//     {
//         const char *str = "maa,bb";
//         GDB_RSP_Packet mem_read_packet;
//         ParsePacket_GDB_RSP_Utils(str, strlen(str), &mem_read_packet);
//         assert(mem_read_packet.packetType == CMD_MEM_READ);
//         assert(strcmp(mem_read_packet.paramList.at(0).c_str(), "aa") == 0);
//         assert(strcmp(mem_read_packet.paramList.at(1).c_str(), "bb") == 0);
//     }
//
//     // --- CMD_MEM_WRITE
//     {
//         const char *str = "Maa,bb:cccc";
//         GDB_RSP_Packet mem_read_packet;
//         ParsePacket_GDB_RSP_Utils(str, strlen(str), &mem_read_packet);
//         assert(mem_read_packet.packetType == CMD_MEM_WRITE);
//         assert(strcmp(mem_read_packet.paramList.at(0).c_str(), "aa") == 0);
//         assert(strcmp(mem_read_packet.paramList.at(1).c_str(), "bb") == 0);
//         assert(strcmp(mem_read_packet.paramList.at(2).c_str(), "cccc") == 0);
//     }
// }
//
// TEST_CASE(regex_split_token, "Regex Test Split Token") {
//
// //    string msg = "Hg0";
// //    string msg = "hh+ss+aa+bb+";
// //    static std::regex reg(
// //            R"(^(\?)|(D)|(g))"
// //            R"(|(c)([0-9]*))"
// //            R"(|(G)([0-9A-Fa-f]+))"
// //            R"(|(M)([0-9A-Fa-f]+),([0-9A-Fa-f]+):([0-9A-Fa-f]+))"
// //            R"(|(m)([0-9A-Fa-f]+),([0-9A-Fa-f]+))"
// //            R"(|([zZ])([0-1]),([0-9A-Fa-f]+),([0-9]))"
// //            R"(|(qAttached)$)"
// //            R"(|(qSupported):((?:[a-zA-Z-]+\+?;?)+))");
// //    std::vector<string> res;
// //    std::smatch sm;
// //    regex_match(msg, sm, reg);
// //    for (uint i = 1; i < sm.size(); ++i) {
// //        if (sm[i].str() != "") {
// //            res.push_back(sm[i].str());
// //            LOG_DEBUG("Split: %s", sm[i].str().c_str());
// //        }
// //    }
//     string msg = "qSupported:multiprocess+;swbreak+;hwbreak+;qRelocInsn+;"
//                  "fork-events+;vfork-events+;exec-events+;vContSupported+;"
//                  "QThreadEvents+;no-resumed+;memory-tagging+";
//     GDB_RSP_Packet_t packet = {};
//     ParsePacket_GDB_RSP_Utils(msg.c_str(), msg.length(), &packet);
//     assert(packet.packetType == CMD_Q_SUPPORTED);
//     for(const auto& s : packet.paramList) LOG_DEBUG("%s", s.c_str());
// //    std::regex test_regex(R"(((-|\w)+(?=\+)))");
// //    std::sregex_iterator it(msg.begin(), msg.end(), test_regex);
// //    std::sregex_iterator end;
// //
// //    while (it != end) {
// //        std::smatch match = *it;
// //        std::string matchedString = match.str();
// ////        std::string word1 = match[1].str(); // 第一个捕获组
// ////        std::string word2 = match[2].str(); // 第二个捕获组
// //        /*for(const auto& sm : match)*/ LOG_DEBUG("%s", matchedString.c_str());
// //        ++it;
// //    }
//
//
// }
//
// TEST_CASE(data_encode_test, "Data Encode Test") {
//     uint64_t data_content = 0xfedcba9876543210;
//     uint8_t* data = (uint8_t*)&data_content;
//     uint64_t len = 8;
//     // --- Encode data & send
//     // 2 char represents 1 byte
//     char* data_str = (char*)alloca(2*len + 1);
//     data_str[2 * len] = '\0';
//     for(int i = 0; i < len; i++) {
//         sprintf(&(data_str[2*i]), "%02x", data[i]);
//     }
//     assert(strcmp("1032547698badcfe", data_str) == 0);
// }
//
// TEST_CASE(byte_parse_test, "Byte Parse Test") {
//     for(int i = 0; i < 256; i++) {
//         uint8_t u8_val = i;
//         char* byte_str = (char*)alloca(3);
//         byte_str[2] = '\0';
//         sprintf(byte_str, "%02x", u8_val);
//         uint8_t u8_val_parsed = strtoul(byte_str, nullptr, 16);
//         assert(u8_val == u8_val_parsed);
//     }
// }
//
// TEST_CASE(write_all_reg, "Write All Regs Test") {
//     GDB_RSP_Packet packet = {
//             .packetType = CMD_REG_READ_ALL,
//             .paramList = {
//                     "000000000000000011111111111111110123456789abcdef"
//             }
//     };
//     assert(packet.packetType == CMD_REG_READ_ALL);
//     // --- Check if param valid & parse param
//     if(packet.paramList.size() != 1) {
//         // Send empty packet & return
//         assert(0);
//         return;
//     }
//     // Check if reg data stream length valid
//     if((packet.paramList.at(0).length() % (64 / 4)) != 0) {
//         // Send empty packet & return
//         assert(0);
//         return;
//     }
//     // --- Cal reg num
//     // One char ('0' ~ 'F') could represent 4 bit of data
//     int reg_str_len = (64 / 4);
//     int reg_num = (packet.paramList.at(0).length()) / reg_str_len;
//     int reg_size = 64 / 8;
//     const char* all_reg_str = packet.paramList.at(0).c_str();
//     uint8_t* all_reg_data = (uint8_t*)alloca(reg_num * reg_size);
//     ParseByteStreamStr(all_reg_str, strlen(all_reg_str), all_reg_data);
//     vector<RegisterItem_t> regs;
//     for(int i = 0; i < reg_num; i++) {
//         RegisterItem_t reg_item;
//         reg_item.size = reg_size;
//         reg_item.reg_id = i;
//         memcpy(&(reg_item.val.u64_val), &(all_reg_data[i * reg_size]), reg_size);
//         regs.push_back(reg_item);
//     }
//
//     assert(regs.size() == 3);
//     assert(regs.at(0).val.u64_val == 0x0000000000000000);
//     assert(regs.at(1).val.u64_val == 0x1111111111111111);
//     assert(regs.at(2).val.u64_val == 0xefcdab8967452301);
// }
//
// TEST_CASE(set_test, "Set Test") {
//     std::unordered_set<uint64_t> watchPointPCs;
//     auto it = watchPointPCs.find(233);
//     assert(it == watchPointPCs.end());
//     watchPointPCs.insert(233);
//     it = watchPointPCs.find(233);
//     assert(it != watchPointPCs.end());
//
//     watchPointPCs.insert(233);
//     it = watchPointPCs.find(233);
//     assert(it != watchPointPCs.end());
//
//     watchPointPCs.erase(233);
//     it = watchPointPCs.find(233);
//     assert(it == watchPointPCs.end());
//     it = watchPointPCs.find(666);
//     assert(it == watchPointPCs.end());
//     watchPointPCs.erase(666);
//
// }
