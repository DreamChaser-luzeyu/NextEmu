
#include <csignal>
#include "sdk/sdkconfig.h"
#include "sdk/console.h"
#include "sdk/test.h"
#include "UartEncoder.h"
#include "UartFIFOEncoder.h"
#include "sdk/base/LogicAnalyzer.hpp"
#include "sdk/base/ClkDrive.hpp"

REGISTER_TEST_UNIT(UartEncoder_TestUnit)

TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello UartEncoder Test!");
}

//TEST_CASE(encoding_test, "Test of UART Encoding") {
//    uint8_t data[] = {0xaa, 0xaa, 0x01};
//    auto ue = UartEncoder(9600, data, 3, 9600);
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//    ue.tick(0);
//    LOG_DEBUG("UartEncoder: current val %lu", ue.getCurrentVal(0)->load());
//}

TEST_CASE(async_logic_analyzer_test, "Test of uart encoding & logic analyzer, async") {
    // --- UartEncoder
    uint8_t data[] = {0xaa, 0xcc, 0x01, 0xcc, 0x01, 0xaa, 0xcc, 0x01, 0xcc, 0x01};
    auto ue = UartEncoder(9600, data, 10, 9600);
    using Interface_ns::signal_bit_val_t;
    using Interface_ns::WaveformGenerator_I;
    // --- LogicAnalyzer
    auto la = Base_ns::LogicAnalyzer("test.vcd", 1, Base_ns::LogicAnalyzer::TRIGGER_MODE_ANALOG, 38400);
    la.addChannel("uart", "u", ue.getWire(0), 0);
    la.buildVcdHeader();
    // --- ClockDrive used for uart encoder
    using Base_ns::ClkDrive;
    ClkDrive clk1(100, true, true);
    clk1.regTickObj((Interface_ns::Triggerable_I *)(&ue));
    // --- ClockDrive used for logic analyzer
    using Base_ns::ClkDrive;
    ClkDrive clk2(400, true, true);
    clk2.regTickObj((Interface_ns::Triggerable_I *)(&la));

    // --- Let them run
    clk2.spawn();
    clk1.spawn();
    // --- Sleep for 3s
    sleep(5);
    // --- Terminate threads
    clk2.terminate();
    clk1.terminate();

    la.dumpVcd();
}







