#include "sdk/base/GDBStub.h"
#include "include/gdb_rsp.h"

FuncReturnFeedback_e GDBStub::recvAndCheck() {
    // --- Accept
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    if(recvFD == -1) {
        recvFD = accept(listenFD, (struct sockaddr *) &clientAddr, &client_addr_size);
        sendFD = recvFD;
    }
    if(recvFD == -1) {
        printf("accept error: %s(errno:%d)\n", strerror(errno), errno);
        exit(EXIT_FAILURE);
    }
    // --- Read
    int packet_begin_index = -1;
    size_t packet_size = 0;
    bool packet_finish = false;
    FILE* fp = fdopen(recvFD, "r");
    LOG_DEBUG("getc() blocked");
    for(int i = 0; i < BUFFER_SIZE; i++) {
        char ch = getc(fp);
        buffer[i] = ch;
        if(ch == PACKET_BEGIN_CHAR) packet_begin_index = i + 1;
        if(ch == PACKET_END_CHAR) {
            packet_size = i - packet_begin_index;
            packet_finish = true;
            break;
        }
    }
    if(!packet_finish) {
        LOG_ERR("GDB RSP packet too long");
        exit(EXIT_FAILURE);
    }
    char chk_sum_str[3];
    chk_sum_str[0] = getc(fp);
    chk_sum_str[1] = getc(fp);
    chk_sum_str[2] = '\0';
    // --- Check
    uint8_t chk_sum_recv;
    sscanf(chk_sum_str, "%02x", (uint32_t*)(&chk_sum_recv));
    if(packet_begin_index == -1) return MEMU_PROTOCOL_ERR;
    uint8_t chk_sum_cal;
    CalCheckSum_GDB_RSP_Utils(&(buffer[packet_begin_index]), packet_size, &chk_sum_cal);
#if !CONFIG_DEBUG_ENABLE
    // Let's bypass the chksum validation while developing...
    if(chk_sum_recv != chk_sum_cal) return MEMU_CHKSUM_ERR;
#endif
    // --- Save packet info
    packetBeginIndex = packet_begin_index;
    packetSize = packet_size;
    return MEMU_OK;
}

FuncReturnFeedback_e GDBStub::recvPacket() {
    int nr_retry_times = 0;
    FuncReturnFeedback_e feedback = recvAndCheck();

    while(feedback == MEMU_CHKSUM_ERR) {
        if(nr_retry_times >= 10) LOG_ERRNO_AND_EXIT("Invalid checksum, maximum number of retries reached");
        sendNAK();
        feedback = recvAndCheck();
        nr_retry_times ++;
    }
    while(feedback == MEMU_PROTOCOL_ERR) {
        if(nr_retry_times >= 10) LOG_ERRNO_AND_EXIT("Invalid protocol, maximum number of retries reached");
        write(sendFD, NAK_PACKET_DATA, strlen(ACK_PACKET_DATA));
        feedback = recvAndCheck();
        nr_retry_times ++;
    }

    sendACK();

    return feedback;
}

void GDBStub::sendACK() {
    write(sendFD, ACK_PACKET_DATA, strlen(ACK_PACKET_DATA));
}

void GDBStub::sendNAK() {
    write(sendFD, NAK_PACKET_DATA, strlen(NAK_PACKET_DATA));
}

void GDBStub::packingAndSend(const char *buffer, size_t len) {
    char* buffer_to_send = (char*)alloca(len + 5);
    // --- Warp package data with '$' and '#'
    memcpy(buffer_to_send + 1, buffer, len);
    buffer_to_send[0] = PACKET_BEGIN_CHAR;
    buffer_to_send[len+1] = PACKET_END_CHAR;
    // --- Append chksum
    uint8_t chksum;
    CalCheckSum_GDB_RSP_Utils(buffer, len, &chksum);
    sprintf(&(buffer_to_send[len+2]), "%02x", chksum);
    // --- Write
    write(sendFD, buffer_to_send, len + 4);
    char* buffer_to_send_str = (char*)alloca(len + 5);
    memcpy(buffer_to_send_str, buffer_to_send, len + 4);
    buffer_to_send_str[len + 4] = '\0';
    LOG_DEBUG("Send packet info-> %s", buffer_to_send_str);
    // --- Check ACK & re-send
    char ack = '-';
    int nr_retry_times = 0;
    read(recvFD, &ack, 1);
    while(ack != '+') {
        if(nr_retry_times >= 10) break;
        write(sendFD, buffer_to_send, len + 4);
        read(recvFD, &ack, 1);
        nr_retry_times ++;
    }
}




