#pragma once

#include <cstdint>
#include <cstring>
#include <mutex>
#include "unistd.h"
#include "sdk/console.h"
#include "sdk/interface/digital_if.h"
#include "sdk/interface/dev_if.h"
#include "sdk/base/SRAMAdapter.hpp"
#include "sdk/symbol_attr.h"


class VUartRx;

namespace UartRtl_ns {

typedef struct ModuleIf {
    Interface_ns::WireSignal *sig_uart_rx;
} ModuleIf_t;

class UartRTL : public Interface_ns::SlaveIO_I, public Interface_ns::Triggerable_I {
private:
    VUartRx* uartRx;
    std::mutex uartRxMutex;
    Base_ns::SRAMAdapter* sramAdapter;
    Interface_ns::WireSignal *sigUartRx;

public:
    explicit UartRTL(ModuleIf interfaceSig): sigUartRx(interfaceSig.sig_uart_rx) {
        uartRx = new VUartRx();
        sramAdapter = new Base_ns::SRAMAdapter();
        uartRxMutex.unlock();
        // ----- Configure your register address map here
        sramAdapter->regAddr(&(uartRx->IO_RxDone_O), 0, 1);
        sramAdapter->regAddr(&(uartRx->IO_RxData_O), 1, 1);

        // --- Reset rx module
        uartRxMutex.lock();
        uartRx->IO_Rx_I = sigUartRx->getBit(0);
        uartRx->IO_Rst_I = 0;
        uartRx->IO_Clk_I = 1;
        uartRx->eval();
        uartRx->IO_Rst_I = 1;
        uartRx->IO_Clk_I = 0;
        uartRx->eval();
        uartRxMutex.unlock();
    }

    ~UartRTL() {
        delete uartRx;
        delete sramAdapter;
    }

    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override {
        uartRxMutex.lock();
        int fb = sramAdapter->load(begin_addr, len, buffer);
        uartRxMutex.unlock();
        return fb;
    }

    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override {
        uartRxMutex.lock();
        int fb = sramAdapter->store(begin_addr, len, buffer);
        uartRx->eval();
        uartRxMutex.unlock();
        return fb;
    }

    void tick(uint64_t nr_ticks) override {
        uartRxMutex.lock();
        uartRx->IO_Clk_I = !(uartRx->IO_Clk_I);
        uartRx->IO_Rx_I = sigUartRx->getBit(0);
        uartRx->eval();
        uartRxMutex.unlock();
    }

};

}