#include "sdk/console.h"
#include "sdk/test.h"

#include "SpikePlatform.hpp"

REGISTER_TEST_UNIT(RVCoreSpike_TestUnit)

TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello Spike Test!");
}



//#include "processor.h"
//#include "isa_parser.h"
//#include "cfg.h"
//#include "sim.h"
TEST_CASE(test_sim_construct, "A test of Spike sim_t construct") {
//    isa_parser_t isa("", "");
//    cfg_t cfg;
//
//    processor_t p(&isa, cfg, );

//    cfg_t cfg;
//
//
//    sim_t s();
}

//
//#include "spike_obj_factory.h"
//#include "SpikePlatform.hpp"
//#include "../../device/mem/include/Mem.h"
//#include "../../device/uartlite/include/Uartlite.hpp"
//#include "../../waveform_encoder/uart_encoder/include/UartEncoder.h"
//#include "sdk/base/ClkDrive.hpp"
//#include "../uart_rtl/include/UartRtl.h"
//
//class UartTester : public UartEncoder {
//private:
//    const char *data = "hello";
//
//public:
//    UartTester(uint64_t clkFreqHz, const uint8_t *data, uint64_t dataLenBytes, uint64_t baurdRate = 115200,
//               uint64_t payloadBits = 8,
//               uint64_t cyclesDelay = 0, uint64_t bytesDelay = 0) : UartEncoder(clkFreqHz, data, dataLenBytes,
//                                                                                baurdRate,
//                                                                                payloadBits, cyclesDelay, bytesDelay) {}
//
//    void tick(uint64_t nr_ticks) override {
//        if (UartEncoder::finished()) {
//            resetData((uint8_t *) data, 5);
//        }
//        UartEncoder::tick(nr_ticks);
//    }
//};
//TEST_CASE(test_spike_main, "A test of Spike main() func") {
//    auto* bus = new Base_ns::AddrBus();
//
//
//    const char *load_path = "/home/luzeyu/temp/memu_linux/opensbi-1.3.1/build/platform/generic/firmware/fw_payload.bin";
//    auto* mem = new Mem(load_path, 4096l*1024l*1024l);
////    bus->addDev(mem, 0x80000000);
////    bus->addDev(mem, 0x80000000, true);
//    bus->setMem(0x80000000, 1024l * 1024l * 1024l * 4, load_path);
//
//
//
//
//
//    auto* p = new SpikePlatform(bus);
//
//    auto* uart = new Builtin_ns::Uartlite(p);
//    bus->addDev(uart, 0x60100000);
//    uart->spawnInputThread();
//
//    auto* cd = new Base_ns::ClkDrive(200);
//    cd->regTickObj(uart);
//    cd->spawn();
//
//    uint8_t data[] = {0xaa, 0xcc, 0x01, 0xcc, 0x01, 0xaa, 0xcc, 0x01, 0xcc, 0x01};
//    auto ue = UartTester(1000000, data, 10, 9600);
//    UartRtl_ns::ModuleIf_t moduleIf;
//    moduleIf.sig_uart_rx = ue.getWire(0);
//    auto uart_rtl = UartRtl_ns::UartRtl(moduleIf);
//
//    bus->addDev((Interface_ns::SlaveIO_I*)(&uart_rtl), 0x60200000);
//
//
//    // --- ClockDrive used for uart encoder
//    using Base_ns::ClkDrive;
//    ClkDrive clk1(1000, true, true);
//    clk1.regTickObj((Interface_ns::Triggerable_I *) (&ue));
//    // --- ClockDrive used for uart rtl
//    using Base_ns::ClkDrive;
//    ClkDrive clk2(2000, true, true);
//    clk2.regTickObj((Interface_ns::Triggerable_I *) (&uart_rtl));
//    // --- Let them run
//    clk2.spawn();
//    clk1.spawn();
//
//
//    p->run();
////    const char* argv[] = { "./spike", nullptr };
////    create_sim(1, argv, bus);
//}


