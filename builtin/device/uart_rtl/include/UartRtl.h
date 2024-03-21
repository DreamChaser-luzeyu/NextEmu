#pragma once

#include <stdint.h>
#include <cstring>
#include "VTop.h"
#include "VUartRx.h"
#include "sdk/interface/dev_if.h"
#include "sdk/symbol_attr.h"

namespace UartRtl_ns {

typedef struct UartliteReg {
    uint32_t rx_fifo;
    uint32_t tx_fifo;
    uint32_t status;
    uint32_t control;
} UartliteReg_t;

}

class UartRtl : public Interface_ns::SlaveIO_I, public Interface_ns::Triggerable_I {
private:
    VTop* top;
    VUartRx* uartRx;
public:
    explicit UartRtl() {
        top = new VTop();
        uartRx = new VUartRx();
    }

    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override {

        return Interface_ns::FB_SUCCESS;
    }

    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override {

        return Interface_ns::FB_SUCCESS;
    }

    void tick(uint64_t nr_ticks) override {



    }


};
