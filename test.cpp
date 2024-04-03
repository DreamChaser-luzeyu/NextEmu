#include <csignal>
#include "sdk/test.h"
#include "sdk/base/ClkDrive.hpp"
#include "sdk/interface/waveform_generator.h"
#include "sdk/interface/digital_if.h"
#include "sdk/base/LogicAnalyzer.hpp"
#include "builtin/waveform_encoder/uart_encoder/include/UartEncoder.h"

REGISTER_TEST_UNIT(Main_TestUnit)

TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello test framework!");
}

TEST_CASE(test_bus_mem, "Test of mem bus and mem") {

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
    value.store((uint64_t) WaveformGenerator_I::LOGIC_HIGH); // SET HIGH
    la.tick(0);
    value.store((uint64_t) WaveformGenerator_I::LOGIC_LOW); // SET LOW
    la.tick(1);
    value.store((uint64_t) WaveformGenerator_I::LOGIC_HIGH);
    la.tick(2);                                       // Should trigger at here
    value.store((uint64_t) WaveformGenerator_I::LOGIC_HIGH);
    la.tick(3);
    value.store((uint64_t) WaveformGenerator_I::LOGIC_LOW);
    la.tick(4);
    value.store((uint64_t) WaveformGenerator_I::LOGIC_LOW);
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
#include "UartRtl.h"

class UartTester : public UartEncoder {
private:
    const char *data = "hello\n";

public:
    UartTester(uint64_t clkFreqHz, uint64_t baurdRate = 115200,
               uint64_t payloadBits = 8, uint64_t cyclesDelay = 0, uint64_t bytesDelay = 0)
            : UartEncoder(clkFreqHz, nullptr, 0,
                          baurdRate,
                          payloadBits, cyclesDelay, bytesDelay) {
        resetData((uint8_t*)data, 6);
    }

    void tick(uint64_t nr_ticks) override {
        if (UartEncoder::finished()) {
            resetData((uint8_t *) data, 6);
        }
        UartEncoder::tick(nr_ticks);
    }
};

TEST_CASE(test_spike_main, "A test of Spike main() func") {
    std::mutex test_mtx;
    test_mtx.unlock();
    test_mtx.lock();
//    test_mtx.lock();


    auto *bus = new Base_ns::AddrBus();


    const char *load_path = "/home/luzeyu/temp/memu_linux/opensbi-1.3.1/build/platform/generic/firmware/fw_payload.bin";
    // auto *mem = new Mem(load_path, 4096l * 1024l * 1024l);
    // bus->addDev(mem, 0x80000000);
    // bus->addDev(mem, 0x80000000, true);
    // @note Use `setMem()` for better performance when accessing RAM
    bus->setMem(0x80000000, 1024l * 1024l * 1024l * 4, load_path);


    auto *p = new SpikePlatform(bus);

    auto *uart = new Builtin_ns::Uartlite(p);
    bus->addDev(uart, 0x60100000);
    uart->spawnInputThread();

    auto *cd = new Base_ns::ClkDrive(400);
    cd->regTickObj(uart);
    cd->spawn();

//    uint8_t data[] = {0xaa, 0xcc, 0x01, 0xcc, 0x01, 0xaa, 0xcc, 0x01, 0xcc, 0x01};
    auto ue = UartTester(1000000, 9600);
    UartRtl_ns::ModuleIf_t moduleIf;
    moduleIf.sig_uart_rx = ue.getWire(0);
//    auto uart_rtl = UartRtl_ns::UartRtl(moduleIf, p);
    auto uart_rtl = UartRtl_ns::UartRTL(moduleIf, p);
//    uart_rtl.spawnInputThread();

    bus->addDev((Interface_ns::SlaveIO_I *) (&uart_rtl), 0x60200000);
//    bus->addDev((Interface_ns::SlaveIO_I*)(uart), 0x60200000);


    // --- ClockDrive used for uart encoder
    using Base_ns::ClkDrive;
    ClkDrive clk1(10000, true, true);
    clk1.regTickObj((Interface_ns::Triggerable_I *) (&ue));
    // --- ClockDrive used for uart rtl
    using Base_ns::ClkDrive;
    ClkDrive clk2(20000, true, true);
    clk2.regTickObj((Interface_ns::Triggerable_I *) (&uart_rtl));
    // --- Let them run
    clk2.spawn();
    clk1.spawn();


    p->run();
//    const char* argv[] = { "./spike", nullptr };
//    create_sim(1, argv, bus);
}

