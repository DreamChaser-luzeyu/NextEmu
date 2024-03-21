#include <csignal>
#include "sdk/test.h"
#include "sdk/base/ClkDrive.hpp"
#include "sdk/interface/waveform_generator.h"
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
    using Interface_ns::signal_val_t;
    using Interface_ns::WaveformGenerator_I;

    auto la = Base_ns::LogicAnalyzer("", 1, Base_ns::LogicAnalyzer::TRIGGER_MODE_POSEDGE, 100);
    signal_val_t value;
    la.addChannel("test_signal", "t", &value, true);
    la.buildVcdHeader();
    value.store((uint64_t)WaveformGenerator_I::LOGIC_HIGH); // SET HIGH
    la.tick(0);
    value.store((uint64_t)WaveformGenerator_I::LOGIC_LOW); // SET LOW
    la.tick(1);
    value.store((uint64_t)WaveformGenerator_I::LOGIC_HIGH);
    la.tick(2);                                       // Should trigger at here
    value.store((uint64_t)WaveformGenerator_I::LOGIC_HIGH);
    la.tick(3);
    value.store((uint64_t)WaveformGenerator_I::LOGIC_LOW);
    la.tick(4);
    value.store((uint64_t)WaveformGenerator_I::LOGIC_LOW);
    la.tick(5);
    // Seq should be 1 1 0 0
}



