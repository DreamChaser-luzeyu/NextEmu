#include <cassert>
#include "sdk/base/GDBStub.h"
#include "include/gdb_rsp.h"
#include "sdk/interface/status_enum.h"


FuncReturnFeedback_e GDBStub::Debug_GDBStub(Interface_ns::Debuggable_I* core) {
    LOG_INFO("Waiting for debugger...");
    debuggedCore = core;
    while(true) {
        // --- Receive packet
        FuncReturnFeedback_e recv_packet_feedback = recvPacket();
        if (recv_packet_feedback != MEMU_OK) LOG_ERRNO_AND_EXIT("Receive packet error");
        // --- Print packet info
        char *packet = new char[packetSize + 1];
        memcpy(packet, &(buffer[packetBeginIndex]), packetSize);
        packet[packetSize] = '\0';
        LOG_DEBUG("Recv packet Info<- %s", packet);
        // --- Parse packet & perform action
        GDB_RSP_Packet_t parsed_packet;
        ParsePacket_GDB_RSP_Utils(&(buffer[packetBeginIndex]), packetSize, &parsed_packet);

        switch(parsed_packet.packetType) {
            case CMD_QUERY_HALT_REASON:
                LOG_DEBUG("CMD_QUERY_HALT_REASON received");
                sendACK();
                cmd_GetHaltReason();
                break;
            case CMD_REG_READ_ALL:
                LOG_DEBUG("CMD_REG_READ_ALL received");
                sendACK();
                cmd_ReadAllRegister();
                break;
            case CMD_REG_WRITE_ALL:
                LOG_DEBUG("CMD_REG_WRITE_ALL received");
                sendACK();
                cmd_WriteAllRegister(parsed_packet);
                break;
            case CMD_Q_SUPPORTED:
                LOG_DEBUG("CMD_Q_SUPPORTED received");
                sendACK();
                cmd_QSupported(parsed_packet);
                break;
            case CMD_MEM_READ:
                LOG_DEBUG("CMD_MEM_READ received");
                sendACK();
                cmd_MemRead(parsed_packet);
                break;
            case CMD_MEM_WRITE:
                LOG_DEBUG("CMD_MEM_WRITE received");
                sendACK();
                cmd_MemWrite(parsed_packet);
                break;
            case CMD_INSERT_BREAKPOINT:
                LOG_DEBUG("CMD_INSERT_BREAKPOINT received");
                sendACK();
                cmd_InsertBreakpoint(parsed_packet);
                break;
            case CMD_REMOVE_BREAKPOINT:
                LOG_DEBUG("CMD_REMOVE_BREAKPOINT received");
                sendACK();
                cmd_RemoveBreakpoint(parsed_packet);
                break;
            case CMD_CONTINUE:
                LOG_DEBUG("CMD_CONTINUE received");
                sendACK();
                cmd_Continue(parsed_packet);
                break;
            case EMPTY_PACKET:
                LOG_DEBUG("Empty packet received");
                sendACK();
                break;
            case CMD_UNKNOWN:
                LOG_DEBUG("Unknown cmd received");
                sendACK();
                cmd_Unknown();
                break;
            default: // Should never reach here
                assert(0);
        }


    }
    return MEMU_OK;
}







