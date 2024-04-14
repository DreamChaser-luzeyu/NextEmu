
#include "sdk/test.h"

#include "AXILiteAdapter.h"

REGISTER_TEST_UNIT(AXILite_TestUnit)

TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello AXILite_TestUnit Test!");
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
