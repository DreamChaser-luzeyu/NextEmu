#include <csignal>
#include "sdk/test.h"
#include "sdk/base/ClkDrive.hpp"
#include "sdk/interface/waveform_generator.h"
#include "sdk/interface/digital_if.h"
#include "sdk/base/LogicAnalyzer.hpp"
#include "builtin/rtl_adapter/uart_encoder/include/UartEncoder.h"
#include "sdk/base/SRAMAdapter.hpp"


REGISTER_TEST_UNIT(Main_TestUnit)

TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello test framework!");
}

TEST_CASE(test_bus_mem, "Test of mem bus and mem") {

}

#include "sdk/base/SRAMAdapter.hpp"

TEST_CASE(test_sram_adapter, "Test of SRAM Adapter") {
    auto sa = Base_ns::SRAMAdapter();
    uint64_t val_1 = 0l;
    void *ptr = &val_1;
    sa.regAddr(ptr, 0, 8);

    uint64_t test_val = 0x8877665544332211;
    assert(sa.store(0, 8, (uint8_t *) (&test_val)) == Interface_ns::FB_SUCCESS);
    assert(test_val == val_1);
    assert(sa.store(0, 9, (uint8_t *) (&test_val)) == Interface_ns::FB_OUT_OF_RANGE);
    assert(sa.store(1, 8, (uint8_t *) (&test_val)) == Interface_ns::FB_OUT_OF_RANGE);


    uint8_t region_2 = 0;
    sa.regAddr(&region_2, 1, 1);
    uint16_t test_val_2 = 0x2211;
    assert(sa.store(0, 2, (uint8_t *) &test_val_2) == Interface_ns::FB_SUCCESS);
    assert(region_2 == *(((uint8_t *) &test_val_2) + 1));
    assert(*((uint8_t *) &val_1) == *((uint8_t *) &test_val_2));

    auto sa2 = Base_ns::SRAMAdapter();
    uint8_t reg_0 = 0;
    sa2.regAddr(&reg_0, 0, 1);
    uint8_t reg_1 = 0;
    sa2.regAddr(&reg_1, 1, 1);
    uint8_t reg_2 = 0;
    sa2.regAddr(&reg_2, 2, 1);
    uint8_t reg_3 = 0;
    sa2.regAddr(&reg_3, 3, 1);
    uint32_t data_4bytes = 0x44332211;
    assert(sa2.store(0, 4, (uint8_t*)&data_4bytes) == Interface_ns::FB_SUCCESS);
    assert(reg_0 == ((uint8_t*)(&data_4bytes))[0]);
    assert(reg_1 == ((uint8_t*)(&data_4bytes))[1]);
    assert(reg_2 == ((uint8_t*)(&data_4bytes))[2]);
    assert(reg_3 == ((uint8_t*)(&data_4bytes))[3]);



}

class TickTester : public Interface_ns::SlaveIO_I, public Interface_ns::Triggerable_I {
public:
    uint64_t nrTick = 0;
    uint64_t currTick;

    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override {
        return 0;
    }

    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override {
        return 0;
    }

    void tick(uint64_t nr_ticks) override {
        nrTick++;
        currTick = nr_ticks;
        LOG_DEBUG("Tick %lu", nr_ticks);
    }
};

TEST_CASE(test_clk_drive, "Test of clock drive") {
    using Base_ns::ClkDrive;
    ClkDrive clk1(4, true, true);
    TickTester tick_tester;
    clk1.regTickObj(&tick_tester);
    clk1.spawn();
    usleep(3000000);
    clk1.terminate();
    LOG_DEBUG("%lu, %lu", tick_tester.nrTick, tick_tester.currTick);
    assert(tick_tester.nrTick == 12);
    assert(tick_tester.currTick == 12);     // Not 100% pass
}

TEST_CASE(test_logic_analyzer, "Test of virtual logic analyzer") {
    using Interface_ns::signal_bit_val_t;
    using Interface_ns::WaveformGenerator_I;

    auto la = Base_ns::LogicAnalyzer("", 1, Base_ns::LogicAnalyzer::TRIGGER_MODE_POSEDGE, 100);
    signal_bit_val_t value;
    la.addChannel("test_signal", "t", &value, true);
    la.buildVcdHeader();
    value.store((uint64_t) Interface_ns::BIT_POS); // SET HIGH
    la.tick(0);
    value.store((uint64_t) Interface_ns::BIT_NEG); // SET LOW
    la.tick(1);
    value.store((uint64_t) Interface_ns::BIT_POS);
    la.tick(2);                                       // Should trigger at here
    value.store((uint64_t) Interface_ns::BIT_POS);
    la.tick(3);
    value.store((uint64_t) Interface_ns::BIT_NEG);
    la.tick(4);
    value.store((uint64_t) Interface_ns::BIT_NEG);
    la.tick(5);
    // Seq should be 1 1 0 0
}

TEST_CASE(test_wire_signal, "Test of WireSignal class") {
    auto *ws = new Interface_ns::WireSignal(63, 0);

    assert(ws->setBit(63, Interface_ns::WireSignal::BIT_POS) == false);

    for (int i = 0; i < 63; i++) {
        assert(ws->getBit(i) == Interface_ns::WireSignal::BIT_NEG);
    }

    for (int i = 0; i < 63; i++) {
        ws->setBit(i, Interface_ns::WireSignal::BIT_POS);
        for (int j = 0; j < 63; j++) {
            if (j != i) assert(ws->getBit(j) == Interface_ns::WireSignal::BIT_NEG);
            else {
//                LOG_DEBUG("Check!");
                assert(ws->getBit(j) == Interface_ns::WireSignal::BIT_POS);
            }
        }
        ws->setBit(i, Interface_ns::WireSignal::BIT_NEG);
    }


    delete ws;
}


#include "SpikePlatform.hpp"
#include "Mem.h"
#include "Uartlite.hpp"
#include "UartEncoder.h"
#include "sdk/base/ClkDrive.hpp"
#include "UartliteRtl.h"

class UartTester : public UartEncoder {
private:
    const char *data = "hello\n";

public:
    UartTester(uint64_t clkFreqHz, uint64_t baurdRate = 115200,
               uint64_t payloadBits = 8, uint64_t cyclesDelay = 0, uint64_t bytesDelay = 0)
            : UartEncoder(clkFreqHz, nullptr, 0,
                          baurdRate,
                          payloadBits, cyclesDelay, bytesDelay) {
        resetData((uint8_t *) data, 6);
    }

    void tick(uint64_t nr_ticks) override {
        if (UartEncoder::finished()) {
            resetData((uint8_t *) data, 6);
        }
        UartEncoder::tick(nr_ticks);
    }
};

#include "sdk/interface/interconnect.h"
#include "sdk/base/ByteStreamStub.h"
TEST_CASE(test_spike_main, "A test of Spike main() func") {
    std::mutex test_mtx;
    test_mtx.unlock();
    test_mtx.lock();
//    test_mtx.lock();

    auto* stream_stub = new Base_ns::ByteStreamStub(12346, "127.0.0.1");
    stream_stub->waitForConnection();

    stream_stub->putc('h');
    stream_stub->putc('1');


    auto *bus = new Base_ns::AddrBus();


    const char *load_path = "/home/luzeyu/temp/memu_linux/opensbi-1.3.1/build/platform/generic/firmware/fw_payload.bin";
    // auto *mem = new Mem(load_path, 4096l * 1024l * 1024l);
    // bus->addDev(mem, 0x80000000);
    // bus->addDev(mem, 0x80000000, true);
    // @note Use `setMem()` for better performance when accessing RAM
    bus->setMem(0x80000000, 1024l * 1024l * 1024l * 4, load_path);


    auto *p = new SpikePlatform(bus);

    auto *uart = new Builtin_ns::Uartlite(stream_stub, p);
    bus->addDev(uart, 0x60100000);
    uart->spawnInputThread();
    stream_stub->putc('?');
    stream_stub->putc('?');
    stream_stub->putc('?');
    stream_stub->putc('?');

    auto *cd = new Base_ns::ClkDrive(400);
    cd->regTickObj(uart);
    cd->spawn();
    stream_stub->putc('2');
//    uint8_t data[] = {0xaa, 0xcc, 0x01, 0xcc, 0x01, 0xaa, 0xcc, 0x01, 0xcc, 0x01};
    auto ue = UartTester(1000000, 9600);
    UartliteRtl_ns::ModuleIf_t moduleIf;
    moduleIf.sig_uart_rx = ue.getSignal(0);
//    auto uart_rtl = UartliteRtl_ns::UartRtl(moduleIf, p);
    auto uart_rtl = UartliteRtl_ns::UartRTL(moduleIf, p);
//    uart_rtl.spawnInputThread();

    bus->addDev((Interface_ns::SlaveIO_I *) (&uart_rtl), 0x60200000);
//    bus->addDev((Interface_ns::SlaveIO_I*)(uart), 0x60200000);


    // --- ClockDrive used for uart encoder
    using Base_ns::ClkDrive;
    ClkDrive clk1(1000, true, true);
    clk1.regTickObj((Interface_ns::Triggerable_I *) (&ue));
    // --- ClockDrive used for uart rtl
    using Base_ns::ClkDrive;
    ClkDrive clk2(2000, true, true);
    clk2.regTickObj((Interface_ns::Triggerable_I *) (&uart_rtl));
    // --- Let them run
    clk2.spawn();
    clk1.spawn();


    stream_stub->putc('h');
    p->run();
//    const char* argv[] = { "./spike", nullptr };
//    create_sim(1, argv, bus);
}


