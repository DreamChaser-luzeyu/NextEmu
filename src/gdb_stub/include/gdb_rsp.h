#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <vector>
#include <string>
#include <map>
#include <regex>

const static char PACKET_BEGIN_CHAR  = '$';
const static char PACKET_END_CHAR    = '#';

const static char* CMD_HALT_REASON   = "?";
const static char* ACK_PACKET_DATA   = "+";
const static char* NAK_PACKET_DATA   = "-";
const static char* EMPTY_PACKET_DATA = "$#00";

typedef enum GDB_RSP_PacketType_e {
    // --- Other
    CMD_UNKNOWN = 0,
    // --- Mem
    CMD_MEM_READ,             ///< `m` Mandatory, `m addr,length` memory read
    CMD_MEM_WRITE,            ///< `M` Mandatory, `M addr,length:XX...` memory write
    // --- Reg
    CMD_REG_READ_INDEX,       ///< `p` `p n` read the value of reg n
                              ///< Reply: 'XX...' / 'E NN'
    CMD_REG_WRITE_INDEX,      ///< `P` `P n...=r...`
                              ///< Reply: 'OK' / 'E NN'
    CMD_REG_READ_ALL,         ///< `g` Mandatory, dump values of all registers
    CMD_REG_WRITE_ALL,        ///< `G` Mandatory, 'G XX...' write general registers.
    // --- Ctrl
    CMD_Q_TRACE_STATUS,       ///< `qTStatus` Query whether a trace experiment is
                              ///< running
    CMD_QUERY_HALT_REASON,    ///< `?` Mandatory, query a reason why the target halts
    CMD_SINGLE_STEP,          ///< `s` Mandatory, 's [addr]' single step, resume at addr
                              ///< Resume at same address if `addr` is omitted
    CMD_INSERT_BREAKPOINT,    ///< 'Z' Mandatory, `Z type,addr,kind` insert a watchpoint
    CMD_REMOVE_BREAKPOINT,    ///< 'z' Mandatory, `z type,addr,kind` remove a watchpoint
    CMD_CONTINUE,             ///< `c` Mandatory, `c addr` Continue at addr, which is
                              ///< the address to resume. If addr is omitted, resume
                              ///< at current address.
    // --- Multi-thread
    CMD_SPECIFY_THREAD_FOR_G, ///< `Hg` Specify the thread ID for all following `g`
                              ///< commands, `0` to pick any threads, `-1` to pick
                              ///< all threads
    CMD_SPECIFY_THREAD_FOR_C, ///< `Hg` Specify the thread ID for all following `c`
                              ///< commands, `0` to pick any threads, `-1` to pick
                              ///< all threads
    CMD_Q_ATTACHED,           ///< `qAttached` Mandatory Query whether the server is
                              ///< attached to a core or a thread
    CMD_Q_SUPPORTED,          ///< `qSupported` Tell the server what the gdb client
                              ///< supports, and query what the server supports
    CMD_Q_THREAD_INFO,        ///< `qfThreadInfo` Query which thread is active
    // --- Other
    EMPTY_PACKET,
} GDB_RSP_PacketType_e_t;

extern std::map<GDB_RSP_PacketType_e_t, std::vector<std::string> > PacketTypeRegexStr_GV;

typedef struct GDB_RSP_Packet {
    GDB_RSP_PacketType_e_t packetType;
    std::string cmd;
    std::vector<std::string> paramList;
} GDB_RSP_Packet_t;


/**
 * Calculate the check sum of the data.
 * @param data
 * @param len
 * @param chk_sum
 */
void CalCheckSum_GDB_RSP_Utils(const char* data, size_t len, uint8_t* chk_sum);

void ParsePacket_GDB_RSP_Utils(const char* data, size_t len, GDB_RSP_Packet_t* ret_packet);

void ParseByteStreamStr(const char* byte_stream, size_t len, uint8_t* buffer);
