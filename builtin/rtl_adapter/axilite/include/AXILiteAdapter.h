#pragma once

#include "sdk/interface/dev_if.h"
#include "sdk/interface/digital_if.h"

namespace Base_ns {

class AXILiteAdapter : public Interface_ns::Triggerable_I, public Interface_ns::SlaveIO_I {
public:
    const static uint8_t AXI_WRITE_FB_SUCCESS = 0b00;
    const static uint8_t AXI_WRITE_FB_SLAVE_ERR = 0b10;
    const static uint8_t AXI_WRITE_FB_DECODE_ERR = 0b11;

    explicit AXILiteAdapter(int bit_width = 32);
    virtual ~AXILiteAdapter();

private:
    typedef uint8_t axilite_resp_t;
    const int BIT_WIDTH;
    size_t byteSize;
    class ModuleIf;
    struct TopImpl;
    TopImpl* topImpl;       // Use Pointer to Implementation to hide class `VTop`
    ModuleIf* moduleIf;
    bool useTickClock;
    void axiClkCycle();

    axilite_resp_t axiliteStore4B(uint32_t addr, uint8_t wstrb, uint32_t val);
    axilite_resp_t axiliteStore8B(uint64_t addr, uint8_t wstrb, uint64_t val);

public:
    // ----- Implementation of interface `SlaveIO_I`
    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override;
    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override;
    // ----- Implementation of interface `Triggerable_I`
    /*
     * @note The adapter will not depend on the `tick` function, it is reserved for the module's main clock.
     * @note The bus clock is handled independently by the adapter, in the `load`/`store` function.
     */
    void tick(uint64_t nr_ticks) override = 0;
};

}