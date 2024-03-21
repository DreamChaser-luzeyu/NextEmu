#include "sdk/console.h"
#include "sdk/test.h"

#include "SpikePlatform.hpp"
#include "SpikeCore.h"

REGISTER_TEST_UNIT(RVCoreSpike_TestUnit)

TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello Spike Test!");
}



#include "processor.h"
#include "isa_parser.h"
#include "cfg.h"
#include "sim.h"
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


#include "spike_obj_factory.h"
#include "SpikePlatform.hpp"
#include "../../device/mem/include/Mem.h"
#include "../../device/uartlite/include/Uartlite.hpp"
#include "sdk/base/ClkDrive.hpp"
TEST_CASE(test_spike_main, "A test of Spike main() func") {
    auto* bus = new Base_ns::AddrBus();


    const char *load_path = "/home/luzeyu/temp/memu_linux/opensbi-1.3.1/build/platform/generic/firmware/fw_payload.bin";
    auto* mem = new Mem(load_path, 4096l*1024l*1024l);
//    bus->addDev(mem, 0x80000000);
    bus->addDev(mem, 0x80000000);

    auto* uart = new Builtin_ns::Uartlite();
    bus->addDev(uart, 0x60100000);



    auto* p = new SpikePlatform(bus);

    auto* cd = new Base_ns::ClkDrive(200);
    cd->regTickObj(uart);
    cd->spawn();

    p->run();
//    const char* argv[] = { "./spike", nullptr };
//    create_sim(1, argv, bus);
}


