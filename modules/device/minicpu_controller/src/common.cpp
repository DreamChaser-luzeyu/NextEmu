#include <cstdint>
#include <memory>
#include <thread>
#include <sdk/base/GDBStub.h>

#include "sdk/console.h"

#include "MiniCPUController.h"
// #include "../../../platform/rvcore_minicpu/include/rvcore_minicpu/MiniCPUCore.h"

int Builtin_ns::MiniCPUController::load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer)  {
    LOG_INFO("Load  MiniCPU Controller %s at dev addr %016lx len %016lx", devDesc, begin_addr, len);
    // --- Dump bytes to load
    STDOUT_ACQUIRE_LOCK;
    for(size_t i = 0; i < len; i++) {
        if(i % 8 == 0) printf("%s[INFO ] ", STYLE_TEXT_BLUE);
        printf("%02x ", buffer[i]);
        printf("%s", STYLE_RST);
    }
    printf("\n");
    STDOUT_RELEASE_LOCK;
    return Interface_ns::FB_SUCCESS;
}

int Builtin_ns::MiniCPUController::store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer)  {
    LOG_INFO("Store MiniCPU Controller %s at dev addr %016lx len %016lx", devDesc, begin_addr, len);
    // --- Dump bytes to store
    STDOUT_ACQUIRE_LOCK;
    for(size_t i = 0; i < len; i++) {
        if(i % 8 == 0) printf("%s[INFO ] ", STYLE_TEXT_BLUE);
        printf("%02x ", buffer[i]);
        printf("%s", STYLE_RST);
    }
    printf("\n");
    STDOUT_RELEASE_LOCK;
    if(begin_addr == 0x0ul) {
        uint8_t val = buffer[0];
        if(val) {
            LOG_INFO("MiniCPU Start!");
            auto *minicpu_thread = new std::thread([&]() {
                // dev->run();
                GDBStub gdbStub("127.0.0.1", 54321);
                gdbStub.Debug_GDBStub((Interface_ns::Debuggable_I*)(dev));
            });
        }
        else {
            LOG_INFO("MiniCPU Stop!");
            // to be implemented
        }
    }
    else if(begin_addr == 0x1ul) {
        uint8_t val = buffer[0];
        if(val) {
            LOG_INFO("MiniCPU Step!");
            dev->step();
        }
    }
    else if(begin_addr == 0x2ul) {
        uint8_t val = buffer[0];
        if(val) {
            LOG_INFO("MiniCPU Reset!");
            // (static_cast<MiniCPU_ns::RV64Core*>(dev))->reset();
        }
    }


    return Interface_ns::FB_SUCCESS;
}

Builtin_ns::MiniCPUController::MiniCPUController(Interface_ns::Runnable_I * _dev, const char* desc) 
        : dev(_dev), devDesc(desc ? desc : "default") {}

