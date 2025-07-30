
#include "sdk/test.h"

REGISTER_TEST_UNIT(Full_TestUnit)


#include "sdk/base/ByteStreamStub.h"
#include "sdk/base/ClkDrive.hpp"
#include "DummyDev.h"
#include "SpikePlatform.hpp"
#include "Uartlite.hpp"
#include "UartEncoder.h"
#include "UartliteRtl.h"
#include "UartRtl.h"
class UartTester : public UartEncoder {
private:
    const char *data = "hello\n";
public:
    explicit UartTester(uint64_t clkFreqHz, uint64_t baurdRate = 115200,
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

    bool finished() override { return false; }
};

TEST_CASE(test_spike_main, "A test of Spike main() func") {
    // ----- ByteStreamStub for external connect to stream-based device
    auto* stream_stub = new Base_ns::ByteStreamStub(12346, "127.0.0.1");
    stream_stub->waitForConnection();

    // ----- Emulator System Bus
    auto *bus = new Base_ns::AddrBus();
    // ----- Emulator Main Mem
    const char *load_path = "/home/luzeyu/temp/memu_linux/opensbi-1.3.1/build/platform/generic/firmware/fw_payload.bin";
    // auto *mem = new Mem(load_path, 4096l * 1024l * 1024l);
    // bus->addDev(mem, 0x80000000);
    // bus->addDev(mem, 0x80000000, true);
    // @note Use `setMem()` for better performance when accessing RAM
    bus->setMem(0x80000000, 1024l * 1024l * 1024l * 4, load_path);

    // ----- Construct Emulator Platform
    auto *p = new SpikePlatform(bus);

    // ----- Construct Emulator Main Console
    auto *uart = new Builtin_ns::Uartlite(stream_stub, p);
    bus->addDev(uart, 0x60100000);
    uart->spawnInputThread();
    //   --- Construct ClkDrive for Emulator Main Console
    auto *cd = new Base_ns::ClkDrive(400);
    cd->regTickObj(uart);
    cd->spawn();

    // ----- Construct `uartlite_rtl` Test Console (input-only)
    //   --- Construct UART Waveform Generator
    auto ue = UartTester(1000000, 9600);
    //   --- Construct ClockDriver for UART Waveform Generator
    using Base_ns::ClkDrive;
    ClkDrive clk1(100, true, true, "UART waveform bench clock");
    clk1.regTickObj((Interface_ns::Triggerable_I *) (&ue));
    //   --- ClockDrive used for uartlite rtl
    using Base_ns::ClkDrive;
    ClkDrive clk2(200, true, true, "uartlite rtl clock");

    // ----- Construct a dummy device for debug
    auto dummy_dev = Builtin_ns::DummyDev("test");
    bus->addDev((Interface_ns::SlaveIO_I *) (&dummy_dev), 0x60300000);

    // --- Let them run
    clk2.spawn();
    clk1.spawn();

    // ----- Test if `UartRTL` works
    //   --- Construct UartRTL
    UartRtl_ns::ModuleIf_t moduleIf_;
    moduleIf_.sig_uart_rx = ue.getSignal(0);
    auto uart_rtl = UartRtl_ns::UartRTL(moduleIf_);
    //   --- Bind to ClockDriver
    clk2.regTickObj((Interface_ns::Triggerable_I *) (&uart_rtl));
    //   --- Add to bus
    bus->addDev((Interface_ns::SlaveIO_I *) (&uart_rtl), 0x60400000);
    //   --- Test with `load`/`store`
//    while (!ue.finished()) {
//        uint8_t rx_done_reg;
//        uart_rtl.load(0, 1, &rx_done_reg);
//        if(rx_done_reg) {
//            uint8_t rx_data_reg;
//            uart_rtl.load(1, 1, &rx_data_reg);
//            LOG_INFO("Parsed byte %02x", rx_data_reg);
//            uint8_t rx_recv_done = 0;
//            uart_rtl.store(2, 1, &rx_recv_done);
//            rx_recv_done = 1;
//            uart_rtl.store(2, 1, &rx_recv_done);
//            rx_recv_done = 0;
//            uart_rtl.store(2, 1, &rx_recv_done);
//        }
//    }
    p->run();
}