#pragma once
// ----- SYS
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
// ----- STL
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
// ----- SDK
#include "sdk/interface/interconnect.h"
#include "sdk/console.h"

namespace Base_ns {

class ByteStreamStub : public Interface_ns::StreamIO_I {

private:
    int serverFd;
    int commFd;
    FILE* rxFile;
    FILE* txFile;
    struct sockaddr_in address;

public:
    explicit ByteStreamStub(uint16_t port, const char *in_addr = nullptr) : serverFd(-1), commFd(-1), txFile(nullptr),
                                                                            rxFile(nullptr), address({}) {
        // --- Create LISTEN socket
        if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        // --- Init server addr struct
        address.sin_family = AF_INET;
        if (in_addr) { inet_pton(AF_INET, in_addr, &(address.sin_addr.s_addr)); }
        else { address.sin_addr.s_addr = INADDR_ANY; }
        address.sin_port = htons(port);
        // --- Bind address
        int optval = 1;
        if((setsockopt(serverFd,SOL_SOCKET, SO_REUSEPORT, &optval,sizeof(optval)))<0) {
            perror("setsockopt failed");
            exit(EXIT_FAILURE);
        }
        if (bind(serverFd, (struct sockaddr *) &address, sizeof(address)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(serverFd, 1) < 0) {  // Let's allow only 1 client to connect
            perror("listen");
            exit(EXIT_FAILURE);
        }
    }

    void waitForConnection() {
        int addrlen = sizeof(address);
        LOG_INFO("ByteStreamStub: Waiting for connections...\n");
        if ((commFd = accept(serverFd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        txFile = fdopen(commFd, "r+");
        rxFile = fdopen(commFd, "r+");
        if (!txFile) LOG_ERRNO_AND_EXIT("fdopen() failed");
        if (!rxFile) LOG_ERRNO_AND_EXIT("fdopen() failed");
    }

    int putc(int c) override {
        if (!txFile) {
            LOG_WARN("Try writing %02x to bytestream %s", (uint8_t) c, "default desc");
            return EOF;
        }
        int fb = fputc(c, txFile);
        fflush(txFile);
        return fb;
    }

    int getc() override {
        if (commFd == -1) waitForConnection();
        return fgetc(rxFile);
    }

    ~ByteStreamStub() {
        if (commFd != -1) close(commFd);
        if (serverFd != -1) close(serverFd);
    }
};

}
