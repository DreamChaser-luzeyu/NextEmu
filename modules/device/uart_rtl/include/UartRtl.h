#pragma once

#include <cstdint>
#include <cstring>
#include <mutex>
#include "sdk/interface/dev_if.h"
#include "sdk/interface/digital_if.h"
#include "sdk/base/SRAMAdapter.hpp"

namespace UartRtl_ns {

typedef struct ModuleIf {
    Interface_ns::WireSignal *sig_uart_rx;
} ModuleIf_t;

class UartRTL : public Interface_ns::SlaveIO_I, public Interface_ns::Triggerable_I {
private:
    struct RTLImpl;         // Hide class `VUartRx`
    RTLImpl* rtlImpl;
    std::mutex uartRxMutex;
    Base_ns::SRAMAdapter* sramAdapter;
    Interface_ns::WireSignal *sigUartRx;

public:
    explicit UartRTL(ModuleIf interfaceSig);
    ~UartRTL();

    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override;
    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override;

    void tick(uint64_t nr_ticks) override;

};



}