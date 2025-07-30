
#include "sdk/test.h"

#include "AXILiteAdapter.h"
#include "sdk/base/AddrBus.hpp"

REGISTER_TEST_UNIT(AXILite_TestUnit)

#include "VTop.h"
TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello AXILite_TestUnit Test!");
    auto* top_mod = new VTop();
    top_mod->eval();
    top_mod->ACLK = 0;
    top_mod->ACLK = 1;
    top_mod->AWVALID = 0;
    top_mod->AWADDR = 0;
}

#include <chrono>
TEST_CASE(axilite_pftest, "A performance test of axi lite rtl") {
    using namespace std::chrono;
    using std::chrono::steady_clock;
   
    auto* axilite_adapter = new Base_ns::AXILiteAdapter(32);
    auto *bus = new Base_ns::AddrBus();
    bus->addDev(axilite_adapter, 0x80000000);

    uint32_t data;
    auto last = steady_clock::now();
    for(long long i = 0; i < 65536; i++) {
        bus->store(0x80000000 + 4 * (i % 4), 4, (uint8_t*)&data);
    }
    uint64_t duration_ns = duration_cast<nanoseconds>(steady_clock::now() - last).count();
    LOG_INFO("%lu", duration_ns);

}

//#include "VAXIMaster.h"
//#include "verilated_vcd_c.h"
//TEST_CASE(axilite_test, "A test of axi lite rtl") {
//    Verilated::traceEverOn(true);
//    VerilatedVcdC tfp;
//    auto rtl_module = VAXIMaster();
//    uint64_t sim_time = 0;
//    rtl_module.trace(&tfp, 0);
//    tfp.open("axilite_test.vcd");
//    uint8_t clk = 0;
//
//    // --- Module init
//    rtl_module.ACLK = clk;
//    rtl_module.ARESETn = 0;
//    rtl_module.eval();
//    tfp.dump(sim_time);
//    sim_time ++;
//    // --- Write Address Channel Test
//    rtl_module.AWREADY = 0;
//    rtl_module.ARESETn = 1;
//    rtl_module.awaddr = 0xaaaaaaaa;
//    rtl_module.eval();
//    tfp.dump(sim_time);
//    while(sim_time < 100) {
//        rtl_module.ACLK = clk;
//        rtl_module.eval();
//        tfp.dump(sim_time);
//        sim_time ++;
//        clk = !clk;
//    }
//
//
//    rtl_module.final();
//    tfp.flush();
//    tfp.dump(sim_time);
//    tfp.close();
//}
