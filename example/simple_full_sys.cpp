//
// Created by luzeyu on 25-7-29.
//

#include "example/simple_full_sys.h"

#include "SpikePlatform.hpp"
#include "Uartlite.hpp"
#include "RV64Core.h"
#include "CLInt.h"
#include "PLIC.h"

#include "sdk/base/AddrBus.hpp"
#include "sdk/base/ByteStreamStub.h"
#include "sdk/base/ClkDrive.hpp"

namespace RVCore_ns {
struct IntStatus;
class RV64Core;
}

void simple_full_sys_spike() {
    // ----- ByteStreamStub for external connect to stream-based device
    // auto* stream_stub = new Base_ns::ByteStreamStub(12346, "127.0.0.1");
    // stream_stub->waitForConnection();
    // ----- Emulator System Bus
    auto *bus = new Base_ns::AddrBus();
    // ----- Emulator Main Mem
    const char *load_path = "/media/luzeyu/LinuxApps/MasterSrc/NextEmu/misc/opensbi/build/platform/generic/firmware/fw_payload.bin";
    // const char *load_path = "/media/luzeyu/Files/毕业设计/过程文档/fw_payload.bin";
    // auto *mem = new Mem(load_path, 4096l * 1024l * 1024l);
    // bus->addDev(mem, 0x80000000);
    // bus->addDev(mem, 0x80000000, true);
    // @note Use `setMem()` for better performance when accessing RAM
    bus->setMem(0x80000000, 1024l * 1024l * 1024l * 4, load_path);
    // ----- Construct Emulator Platform
    auto *p = new SpikePlatform(bus);
    // ----- Construct Emulator Main Console
    // auto *uart = new Builtin_ns::Uartlite(stream_stub, p);
    auto *uart = new Builtin_ns::Uartlite(nullptr, p);
    bus->addDev(uart, 0x60100000);
    uart->spawnInputThread();

    auto *cd = new Base_ns::ClkDrive(400);
    cd->regTickObj(uart);
    cd->spawn();


    // ----- Run
    p->run();
}

void simple_full_sys() {
    auto* bus = new Base_ns::AddrBus();

    const char *load_path = "/media/luzeyu/LinuxApps/MasterSrc/NextEmu/misc/opensbi/build/platform/generic/firmware/fw_payload.bin";
    // const char *load_path = "/media/luzeyu/Files/毕业设计/过程文档/fw_payload.bin";
    bus->setMem(0x80000000, 1024l * 1024l * 1024l * 4, load_path);


    // bus->addDev(new Builtin_ns::Uartlite(), 0x60100000);

    RVCore_ns::RV64Core* core = new RVCore_ns::RV64Core(bus, 0);

    core->writeProgramCounter({.u64_val = 0x80000000});
    core->setGPRByIndex(10, 0);

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

    // auto* stream_stub = new Base_ns::ByteStreamStub(12346, "127.0.0.1");
    // stream_stub->waitForConnection();
    auto *uart = new Builtin_ns::Uartlite(nullptr, plic);
    bus->addDev(uart, 0x60100000);
    uart->spawnInputThread();

    while(true) {
        clint->tick(0);
        plic->tick(0);
        uart->tick(0);
        core->step();
    }
}
