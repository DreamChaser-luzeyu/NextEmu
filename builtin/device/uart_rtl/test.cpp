#include <iostream>
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "sdk/test.h"

#include "VUartRx.h"
#include "sdk/base/ClkDrive.hpp"
#include "../../waveform_encoder/uart_encoder/include/UartEncoder.h"
#include "sdk/base/LogicAnalyzer.hpp"

REGISTER_TEST_UNIT(UartRTL_TestUnit)

TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello UART RTL Test!");
}

TEST_CASE(uart_rtl_rx_test, "A test of the uart rx") {
    Verilated::traceEverOn(true);
    VUartRx rx_module;
    VerilatedVcdC tfp;
    rx_module.trace(&tfp, 0);
    tfp.open("wave.vcd");

    rx_module.IO_Rx_I = 1;
    rx_module.IO_Clk_I = 1;
    rx_module.IO_Rst_I = 1;
    rx_module.eval();
    uint64_t time_cntr = 0;
    tfp.dump(time_cntr);
    time_cntr++;
    while(!rx_module.IO_Debug_Sample_O) {
        rx_module.IO_Clk_I = 0;
        rx_module.eval();
        rx_module.IO_Clk_I = 1;
        rx_module.eval();
        tfp.dump(time_cntr);
        time_cntr++;
    }
    rx_module.final();
    tfp.flush();
    tfp.close();
}

/**
 * Case not working on newer version of verilator.
 * Because cannot read inner reg value.
 */
//TEST_CASE(uart_rtl_rx_test_2, "A test of the uart rx data") {
//    Verilated::traceEverOn(true);
//    VUartRx rx_module;
//    VerilatedVcdC tfp;
//    rx_module.trace(&tfp, 0);
//    tfp.open("wave2.vcd");
//
//    rx_module.IO_Rx_I = 1;
//    rx_module.IO_Clk_I = 1;
//    rx_module.IO_Rst_I = 1;
//    rx_module.eval();
//    uint64_t time_cntr = 0;
//    tfp.dump(time_cntr);
//    time_cntr++;
//    while(!rx_module.IO_Debug_Sample_O) {
//        rx_module.IO_Clk_I = 0;
//        rx_module.eval();
//        rx_module.IO_Clk_I = 1;
//        rx_module.eval();
//        tfp.dump(time_cntr);
//        time_cntr++;
//    }
//
//    rx_module.IO_Rx_I = 0;
//
//    LOG_DEBUG("UartRTL: Rx Data Begin!");
//
//    while(rx_module.UartRx__DOT__reg_bit_counter < 7) {
//        rx_module.IO_Clk_I = 0;
//        rx_module.eval();
//        rx_module.IO_Clk_I = 1;
//        rx_module.eval();
//        tfp.dump(time_cntr);
//        time_cntr++;
//    }
//
//    LOG_DEBUG("UartRTL: Rx Data Done!");
//
//    rx_module.IO_Rx_I = 1;
//
//    while(!rx_module.IO_RxDone_O) {
//        rx_module.IO_Clk_I = 0;
//        rx_module.eval();
//        rx_module.IO_Clk_I = 1;
//        rx_module.eval();
//        tfp.dump(time_cntr);
//        time_cntr++;
//    }
//
//    LOG_DEBUG("UartRTL: Rx found stop bit! Finish!");
//
//    for(int i=0; i<5; i++) {
//        rx_module.IO_Clk_I = 0;
//        rx_module.eval();
//        rx_module.IO_Clk_I = 1;
//        rx_module.eval();
//        tfp.dump(time_cntr);
//        time_cntr++;
//    }
//
//    rx_module.final();
//    tfp.flush();
//    tfp.close();
//}


//TEST_CASE(test_async_uart_rx, "Test of UART Rx using waveform generator") {
//    using Base_ns::ClkDrive;
//    ClkDrive clk1(100, true, true);
//    uint8_t data[] = {0xaa, 0xaa, 0x01};
//    auto ue = UartEncoder(96000, data, 3, 9600);
//    using Interface_ns::signal_val_t;
//    using Interface_ns::WaveformGenerator_I;
//    auto la = Base_ns::LogicAnalyzer("", 1, Base_ns::LogicAnalyzer::TRIGGER_MODE_ANALOG, 100);
//    la.addChannel("uart", "u", ue.getCurrentVal(0), true);
//    la.buildVcdHeader();
//
//
//
//
////    Verilated::traceEverOn(true);
////    VUartRx rx_module;
////    VerilatedVcdC tfp;
////    rx_module.trace(&tfp, 0);
////    tfp.open("wave2.vcd");
//
//
//}

#include "UartRtl.h"

class UartRTLTicker : Interface_ns::Triggerable_I {
public:
    VUartRx rxModule;
    VerilatedVcdC tfp;
    Interface_ns::signal_val_t* signalRef;
    uint8_t currentClk = 0;
    uint64_t time_cntr = 0;

    UartRTLTicker(Interface_ns::signal_val_t* signal_ref) : rxModule(), tfp(), signalRef(signal_ref) {
        Verilated::traceEverOn(true);
        rxModule.trace(&tfp, 0);
        tfp.open("wave_uart_test.vcd");
        rxModule.IO_Rx_I = signalRef->load();
        rxModule.IO_Rst_I = 0;
        rxModule.IO_Clk_I = 1;
        rxModule.eval();
        tfp.dump(time_cntr++);
        rxModule.IO_Rst_I = 1;
        rxModule.IO_Clk_I = 0;
        rxModule.eval();
        tfp.dump(time_cntr++);
//        rxModule.IO_Rst_I = 1;
//        rxModule.IO_Clk_I = 0;
//        rxModule.eval();
//        tfp.dump(time_cntr++);
    }

    void tick(uint64_t nr_ticks) override {
//        rxModule.IO_Rst_I = 0;
        rxModule.IO_Clk_I = currentClk;
        rxModule.IO_Rx_I = signalRef->load();
        rxModule.eval();
        tfp.dump(time_cntr);
        currentClk = (!currentClk);
        time_cntr ++;
    }

    virtual ~UartRTLTicker() {
        rxModule.final();
        tfp.flush();
        tfp.close();
    }
};
TEST_CASE(async_uart_rtl_test, "Test of uart encoding & uart rtl, aync") {
    // --- UartEncoder
    uint8_t data[] = {0xaa, 0xcc, 0x01, 0xcc, 0x01, 0xaa, 0xcc, 0x01, 0xcc, 0x01};
    auto ue = UartEncoder(1000000, data, 10, 9600);
    using Interface_ns::signal_val_t;
    using Interface_ns::WaveformGenerator_I;
    // --- Uart Rx RTL
//    auto ut = UartRTLTicker(ue.getCurrentVal(0));
    UartRtl_ns::ModuleIf_t moduleIf;
    moduleIf.sig_uart_rx = ue.getWire(0);
    auto uart_rtl = UartRtl_ns::UartRtl(moduleIf);

    // --- ClockDrive used for uart encoder
    using Base_ns::ClkDrive;
    ClkDrive clk1(1000, true, true);
    clk1.regTickObj((Interface_ns::Triggerable_I *)(&ue));
    // --- ClockDrive used for uart rx rtl
    using Base_ns::ClkDrive;
    ClkDrive clk2(2000, true, true);
//    clk2.regTickObj((Interface_ns::Triggerable_I *)(&ut));
    clk2.regTickObj((Interface_ns::Triggerable_I *)(&uart_rtl));

    // --- Let them run
    clk2.spawn();
//    sleep(1);
    clk1.spawn();
    // --- Sleep for 3s
    sleep(10);
    while(!ue.finished());
    // --- Terminate threads
    clk2.terminate();
    clk1.terminate();

}
