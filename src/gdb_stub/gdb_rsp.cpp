#include <cassert>
#include <map>
#include <vector>
#include "include/gdb_rsp.h"

std::map<GDB_RSP_PacketType_e_t, std::vector<std::string> > PacketTypeRegexStr_GV = {
        /**
         * Command Enum,   { Whole-command Regex,   Param Regex }
         * Note: the `Whole-command Regex` should ONLY match the corresponding command
         * */
        // {CMD_Q_SUPPORTED,       { R"((qSupported):(?:[a-zA-Z-]+\+?;?)+)", R"((-|\w)+(?=\+))" } },
        // {CMD_Q_SUPPORTED,       { R"((qSupported):([a-zA-Z-]+[+;]?)+)", R"((-|\w)+(?=\+))" } },
        {CMD_Q_SUPPORTED,       { R"(^qSupported:.*)", R"((-|\w)+(?=\+))" } },
        {CMD_QUERY_HALT_REASON, { R"((\?))" } },
        {CMD_REG_READ_ALL,      { R"((g))" } },
        {CMD_REG_WRITE_ALL,     { R"((G)([0-9A-Fa-f]+))", R"(([0-9A-Fa-f]+))" } },
        {CMD_REG_READ_INDEX,    { R"((p)([0-9A-Fa-f]+))", R"(([0-9A-Fa-f]+))" } },
        {CMD_MEM_READ,          { R"((m)([0-9A-Fa-f]+),([0-9A-Fa-f]+))", R"(([0-9A-Fa-f]+))" } },
        {CMD_MEM_WRITE,         { R"((M)([0-9A-Fa-f]+),([0-9A-Fa-f]+):([0-9A-Fa-f]+))", R"(([0-9A-Fa-f]+))" } },
        {CMD_INSERT_BREAKPOINT, { R"((Z)([0-9A-Fa-f]+),([0-9A-Fa-f]+),([0-9A-Fa-f]+))", R"(([0-9A-Fa-f]+))"} },
        {CMD_REMOVE_BREAKPOINT, { R"((z)([0-9A-Fa-f]+),([0-9A-Fa-f]+),([0-9A-Fa-f]+))", R"(([0-9A-Fa-f]+))"} },
        // C++ regex does not support positive lookbehind (?<=c)([0-9A-Fa-f]+) so use a workaround
        {CMD_CONTINUE,          { R"(((c)([0-9A-Fa-f]+))|(c))", R"(.([0-9A-Fa-f]+))" } },
//        {CMD_Q_ATTACHED,        { R"((qAttached)$)" } }
};

void CalCheckSum_GDB_RSP_Utils(const char* data, size_t len, uint8_t* chk_sum) {
    uint64_t sum = 0;
    for(int i = 0; i < len; i++) sum += data[i];
    (*chk_sum) = sum % 256;
}

void ParsePacket_GDB_RSP_Utils(const char* data, size_t len, GDB_RSP_Packet_t* ret_packet) {
    // --- Check if EMPTY_PACKET
    if(len == 0) {
        ret_packet->packetType = EMPTY_PACKET;
        ret_packet->cmd = std::string("");
    }
    // --- Copy data from buffer to str
    char* data_str = (char*)alloca(len + 1);
    data_str[len] = '\0';
    // --- Match cmd type
    for(auto p : PacketTypeRegexStr_GV) {
        memcpy(data_str, data, len);
        // --- Try matching regex
        if(std::regex_match(data_str, std::regex(p.second.at(0)))) {
            // Regex matches!
            // --- Set packet type
            ret_packet->packetType = p.first;
            // --- Match & parse params
            if(p.second.size() > 1) {
                std::string data_string(data_str);
                std::regex param_regex(p.second.at(1));
                std::sregex_iterator it(data_string.begin(), data_string.end(), param_regex);
                std::sregex_iterator end;
                while (it != end) {
                    const std::smatch& match = *it;
                    std::string matched_str = match.str();
                    ret_packet->paramList.push_back(matched_str);
                    it++;
                }
            }
            return;
        }
    }
    ret_packet->packetType = CMD_UNKNOWN;
    ret_packet->cmd = std::string(data_str);
}

void ParseByteStreamStr(const char* byte_stream, size_t len, uint8_t* buffer) {
    assert(len % 2 == 0);
    for(int i = 0; i < (len / 2); i++) {
        // --- Split byte str
        char* byte_str = (char*)alloca(3);
        byte_str[2] = '\0';
        memcpy(byte_str, &(byte_stream[2 * i]), 2);
        uint8_t byte_data = strtoul(byte_str, nullptr, 16);
        buffer[i] = byte_data;
    }
}
