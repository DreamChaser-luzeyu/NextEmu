#include "sdk/console.h"
#include "sdk/test.h"



REGISTER_TEST_UNIT(RVCore_TestUnit)

TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello Core Test!");
}

#include "RV64Core.h"
#include "CLInt.h"
#include "PLIC.h"
//#include "ref/rv_core.hpp"
#include "sdk/base/AddrBus.hpp"
#include "../../device/mem/include/Mem.h"
#include "../../device/uartlite/include/Uartlite.hpp"
TEST_CASE(core_test, "A test of the core") {
    auto* bus = new Base_ns::AddrBus();


    const char *load_path = "/home/luzeyu/temp/memu_linux/opensbi-1.3.1/build/platform/generic/firmware/fw_payload.bin";
    auto* mem = new Mem(load_path, 4096l*1024l*1024l);
//    bus->addDev(mem, 0x80000000);
    bus->addDev(mem, 0x80000000);
    bus->addDev(new Builtin_ns::Uartlite(), 0x60100000);


    RVCore_ns::RV64Core* core = new RVCore_ns::RV64Core(bus, 0);

    core->writeProgramCounter({.u64_val = 0x80000000});
    core->setGPRByIndex(10, 0);

//    rv_core* core = new rv_core(bus, 0);
//    core->jump(0x80000000);
//    core->set_GPR(10, 0);

    PLIC<4,4>* plic = new PLIC<4,4>();
    RVCore_ns::CLInt<2>* clint = new RVCore_ns::CLInt<2>();
    RVCore_ns::IntStatus* int_status = new RVCore_ns::IntStatus;
    int_status->msip = false;
    int_status->mtip = false;
    int_status->meip = false;
    int_status->seip = false;
    clint->regIntContext(0, /*int_status*/core->intStatus);
    clint->regIntContext(1, new RVCore_ns::IntStatus);
    bool test0; bool test1; bool test2; bool test3;
    plic->regIntContext(0, /*&test0*/&(core->intStatus->meip));
    plic->regIntContext(1, /*&test1*/&(core->intStatus->seip));
    plic->regIntContext(2, &test2);
    plic->regIntContext(3, &test3);

    bus->addDev(plic, 0xc000000);
    bus->addDev(clint, 0x2000000);

//    core->run();

    while(true) {
        clint->tick(0);
        plic->tick(0);
        core->step();
//        core->step(test0, int_status->msip, int_status->mtip, test1);




    }
}

