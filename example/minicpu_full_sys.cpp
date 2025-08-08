//
// Created by luzeyu on 25-7-31.
//

#include "example/minicpu_full_sys.h"

#include "DummyDev.h"
#include "sdk/symbol_attr.h"

#include <CLInt.h>
#include <Mem.h>
#include <PLIC.h>

#include <RV64Core.h>
#include <Uartlite.hpp>
#include <rvcore_minicpu/MiniCPUCore.h>
#include <MiniCPUController.h>
#include <sdk/base/GDBStub.h>

#include "sdk/base/AddrBus.hpp"

void simple_full_sys_minicpu() {
// ┌────┐
// ├────┤
// │....│0x002000000  MainCPU
// │....│             CLINT
// ├────┤
// ├────┤
// │....│0x00c000000  MainCPU
// │....│             PLIC
// ├────┤
// │    │
// │    │
// ├────┤
// │....│0x060100000  MainCPU
// │....│             Uartlite
// ├────┤
// │    │
// │    │
// ├────┼────────────┬─
// │....│0x080000000 ▲
// │....│            │
// │....│            │
// │....│            │CPU Memory
// │....│            │(2GB Managed by Linux)
// │....│            │
// │....│            │
// │....│            ▼
// ├────┼────────────┼─
// │....│0x100000000 ▲
// │....│            │
// │....│            │
// │....│            │CPU Memory
// │....│            │(2GB Reserved)
// │....│            │
// │....│            │
// │....│            ▼
// ├────┼────────────┴─
// │    │                                       ┌───────────┐
// │    │                                       │0x200000000│Memory Controller Base (Reserved)
// │    │                                       ├───────────┤
// │    │                                       │0x200000008│(Reserved)
// ├────┼────────────┬─                         ├───────────┤
// │....│0x180000000 ▲                          │0x200000010│Read Pending channel bitmap (Set to 1 to start the channel, auto clear upon finish)
// │....│            │MiniCPU Memory            ├───────────┤
// │....│            │(128MB)                   │    ...    │(Reserved)
// │....│            │                          ├───────────┤
// │....│0x188000000 ▼                          │0x200000100│Channel 0 source address (rw)
// ├────┼────────────┴─                         │           │
// │    │                                       │0x200000108│Channel 0 destination address (rw)
// │    │                                       ├───────────┤
// │    │                                       │0x200000110│Channel 1 source address (rw)
// ├────┤                                       │           │
// │....│0x200000000  MiniCPU                   │0x200000118│Channel 1 destination address (rw)
// │....│             Memory Controller         ├───────────┤
// │....│                                       │    ...    │
// ├────┤                                       └───────────┘
// │    │
// │    │
// └────┘

    auto* bus = new Base_ns::AddrBus();

    const char *load_path = "/media/luzeyu/LinuxApps/MasterSrc/NextEmu/misc/opensbi/build/platform/generic/firmware/fw_payload.bin";
    bus->setMem(0x80000000, 1024l * 1024l * 1024l * 3, load_path);

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

    // ----- MiniCPU
    auto * minicpuMem = new Mem("/media/luzeyu/LinuxApps/MasterSrc/NextEmu/misc/minicpu_app/start.bin", 128l * 1024l * 1024l);
    bus->addDev(minicpuMem, 0x180000000);
    // uint8_t val = 0xaa;
    // bus->store(0x180000000, 1, &val);
    auto * miniCPUCore = new MiniCPU_ns::RV64Core(bus, 1);
    // miniCPUCore->WriteProgramCounter_DebugAPI({.u64_val = 0x180000000});
    miniCPUCore->WriteProgramCounterForAllCtx({.u64_val = 0x180000000});
    auto * miniCPUCtrl = new Builtin_ns::MiniCPUMemController(bus);
    bus->addDev(miniCPUCtrl, 0x200000000);

    GDBStub gdbStub("127.0.0.1", 55556);
    gdbStub.Debug_GDBStub((Interface_ns::Debuggable_I*)(miniCPUCore));

    while(true) {
        clint->tick(0);
        plic->tick(0);
        uart->tick(0);
        core->step();
    }
}
