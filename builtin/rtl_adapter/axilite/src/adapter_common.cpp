#include "AXILiteAdapter.h"
#include "VTop.h"
#include "ModuleIf.h"
#include "sdk/console.h"


struct Base_ns::AXILiteAdapter::TopImpl {
public:
    VTop* topModule;
};


Base_ns::AXILiteAdapter::AXILiteAdapter(int bit_width) : BIT_WIDTH(bit_width), byteSize(bit_width / 8),
                                                         useTickClock(false) {
    assert(bit_width == 32 || bit_width == 64);
    assert(is_little_endian());
    if(!is_little_endian()) {
        LOG_ERR("Host ISA endian error: requires little endian");
        exit(-1);
    }
    moduleIf = new ModuleIf(bit_width);

    topImpl = new TopImpl;
    topImpl->topModule = new VTop();
    topImpl->topModule = new VTop();
}

Base_ns::AXILiteAdapter::~AXILiteAdapter() {
    delete moduleIf;
    delete topImpl->topModule;
    delete topImpl;
}

int Base_ns::AXILiteAdapter::load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) {


    return Interface_ns::FB_OUT_OF_RANGE;
}

Base_ns::AXILiteAdapter::axilite_resp_t
Base_ns::AXILiteAdapter::axiliteStore4B(uint32_t addr, uint8_t wstrb, uint32_t val) {
    // ----- Transmit Address
    // --- Send Address to Store
    topImpl->topModule->AWADDR = (uint32_t) addr;
    topImpl->topModule->AWVALID = Interface_ns::BIT_POS;
    topImpl->topModule->eval();
    // --- Wait for Ready & Clear Store Addr Valid Flag
    while (!topImpl->topModule->AWREADY) axiClkCycle();
    topImpl->topModule->AWVALID = Interface_ns::BIT_NEG;

    // ----- Transmit Data
    // --- Send Data to Store
    topImpl->topModule->WDATA = val;
    topImpl->topModule->WSTRB = wstrb;
    topImpl->topModule->WVALID = Interface_ns::BIT_POS;
    topImpl->topModule->eval();
    // --- Wait for Ready & Clear Store Data Valid Flag
    while (!topImpl->topModule->WREADY) axiClkCycle();
    topImpl->topModule->WVALID = Interface_ns::BIT_NEG;

    // ----- Get Write Response
    while (!topImpl->topModule->BVALID) axiClkCycle();
    uint8_t axilite_write_resp = topImpl->topModule->BRESP;
    topImpl->topModule->BREADY = Interface_ns::BIT_POS;
    axiClkCycle();

    return axilite_write_resp;
}

Base_ns::AXILiteAdapter::axilite_resp_t
Base_ns::AXILiteAdapter::axiliteStore8B(uint64_t addr, uint8_t wstrb, uint64_t val) {
    // ----- Transmit Address
    // --- Send Address to Store
    topImpl->topModule->AWADDR = (uint64_t) addr;
    topImpl->topModule->AWVALID = Interface_ns::BIT_POS;
    topImpl->topModule->eval();
    // --- Wait for Ready & Clear Store Addr Valid Flag
    while (!topImpl->topModule->AWREADY) axiClkCycle();
    topImpl->topModule->AWVALID = Interface_ns::BIT_NEG;

    // ----- Transmit Data
    // --- Send Data to Store
    topImpl->topModule->WDATA = val;
    topImpl->topModule->WSTRB = wstrb;
    topImpl->topModule->WVALID = Interface_ns::BIT_POS;
    topImpl->topModule->eval();
    // --- Wait for Ready & Clear Store Data Valid Flag
    while (!topImpl->topModule->WREADY) axiClkCycle();
    topImpl->topModule->WVALID = Interface_ns::BIT_NEG;

    // ----- Get Write Response
    while (!topImpl->topModule->BVALID) axiClkCycle();
    uint8_t axilite_write_resp = topImpl->topModule->BRESP;
    topImpl->topModule->BREADY = Interface_ns::BIT_POS;
    axiClkCycle();

    return axilite_write_resp;
}

int Base_ns::AXILiteAdapter::store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) {
    uint8_t axilite_write_resp = AXI_WRITE_FB_SUCCESS;
    // --- Store 4or8-byte-aligned len
    size_t aligned_len = len - (len % byteSize);
    for (size_t bytes_stored = 0; bytes_stored < aligned_len; bytes_stored += byteSize) {
        if (BIT_WIDTH == 32) {
            axilite_write_resp = axiliteStore4B(begin_addr + bytes_stored, 0b1111,
                                                *((uint32_t *) (buffer + bytes_stored)));
        } else {
            axilite_write_resp = axiliteStore8B(begin_addr + bytes_stored, 0b11111111,
                                                *((uint64_t *) (buffer + bytes_stored)));
        }
        if (axilite_write_resp != AXI_WRITE_FB_SUCCESS) {
            if (axilite_write_resp == AXI_WRITE_FB_DECODE_ERR) return Interface_ns::FB_OUT_OF_RANGE;
            return Interface_ns::FB_UNKNOWN_ERR;
        }
    }

    // --- Store bytes left
    size_t len_left = len % byteSize;
    if (BIT_WIDTH == 32) {
        uint32_t val = 0;
        memcpy(&val, buffer + aligned_len, len_left);
        axilite_write_resp = axiliteStore4B(begin_addr + aligned_len, (uint8_t)len_left, val);
    } else {
        uint64_t val = 0;
        memcpy(&val, buffer + aligned_len, len_left);
        axilite_write_resp = axiliteStore8B(begin_addr + aligned_len, (uint8_t)len_left, val);
    }

    if (axilite_write_resp != AXI_WRITE_FB_SUCCESS) {
        if (axilite_write_resp == AXI_WRITE_FB_DECODE_ERR) return Interface_ns::FB_OUT_OF_RANGE;
        return Interface_ns::FB_UNKNOWN_ERR;
    }
    return Interface_ns::FB_SUCCESS;
}

void Base_ns::AXILiteAdapter::axiClkCycle() {
    topImpl->topModule->ACLK = Interface_ns::BIT_NEG;
    topImpl->topModule->eval();
    topImpl->topModule->ACLK = Interface_ns::BIT_POS;
    topImpl->topModule->eval();
}










