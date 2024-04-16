#include <iostream>
#include "sdk/test.h"
#include "sdk/base/ClkDrive.hpp"
#include "sdk/base/LogicAnalyzer.hpp"
#include "UartEncoder.h"

REGISTER_TEST_UNIT(UartRTL_TestUnit)

TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello UART RTL Test!");
}

#include "UartRtl.h"
TEST_CASE(async_uart_rtl_test, "Test of uart encoding & uart rtl, aync") {
    // ----- UartEncoder
    uint8_t data[] = {0xaa, 0xcc, 0x01, 0xcc, 0x01, 0xaa, 0xcc, 0x01, 0xcc, 0x01};
    auto ue = UartEncoder(1000000, data, 10, 9600);
    using Interface_ns::WaveformGenerator_I;
    // ----- Uart Rx RTL
    UartRtl_ns::ModuleIf_t moduleIf;
    moduleIf.sig_uart_rx = ue.getSignal(0);
    auto uart_rtl = UartRtl_ns::UartRTL(moduleIf);

    // ----- ClockDrive used for uart encoder
    using Base_ns::ClkDrive;
    ClkDrive clk1(1000, true, true);
    clk1.regTickObj((Interface_ns::Triggerable_I *) (&ue));
    // ----- ClockDrive used for uart rx rtl
    using Base_ns::ClkDrive;
    ClkDrive clk2(2000, true, true);
    clk2.regTickObj((Interface_ns::Triggerable_I *) (&uart_rtl));

    // ----- Let them run
    clk2.spawn();
    clk1.spawn();
    // ----- Test `Slave_IO` interface
    while (!ue.finished()) {
        uint8_t rx_done_reg;
        uart_rtl.load(0, 1, &rx_done_reg);
        if(rx_done_reg) {
            uint8_t rx_data_reg;
            uart_rtl.load(1, 1, &rx_data_reg);
            LOG_INFO("Parsed byte %02x", rx_data_reg);
            uint8_t rx_recv_done = 0;
            uart_rtl.store(2, 1, &rx_recv_done);
            rx_recv_done = 1;
            uart_rtl.store(2, 1, &rx_recv_done);
            rx_recv_done = 0;
            uart_rtl.store(2, 1, &rx_recv_done);
        }
    }
    // ----- Terminate threads
    clk2.terminate();
    clk1.terminate();
}
