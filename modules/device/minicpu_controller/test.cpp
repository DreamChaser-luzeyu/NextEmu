#include <cassert>

#include "sdk/console.h"
#include "sdk/test.h"

#include <MiniCPUController.h>

#include "../mem/include/Mem.h"
#include "sdk/base/AddrBus.hpp"

REGISTER_TEST_UNIT(MemCtrl_TestUnit)

TEST_CASE(test_test, "A test of test framework") {
    LOG_DEBUG("Hello Mem Ctrl Test!");
}

TEST_CASE(mem_ctrl_read, "A test of Memory Controller Read") {
    using namespace Builtin_ns;
    using namespace Base_ns;
    const char *load_path = "/media/luzeyu/LinuxApps/MasterSrc/NextEmu/misc/opensbi/build/platform/generic/firmware/fw_payload.bin";
    auto* bus = new Base_ns::AddrBus();
    auto* mem = new Mem(load_path, 3 * 1024l*1024l*1024l);
    bus->addDev(mem, 0x80000000);

    auto * minicpuMem = new Mem("/media/luzeyu/LinuxApps/MasterSrc/NextEmu/misc/minicpu_app/start.bin", 128l * 1024l * 1024l);
    bus->addDev(minicpuMem, 0x180000000);

    auto * miniCPUCtrl = new Builtin_ns::MiniCPUMemController(bus);
    bus->addDev(miniCPUCtrl, 0x200000000);



    // ---
    uint64_t src_addr = 0x100000000;
    uint64_t dst_addr = 0x180000000;
    bus->store(0x200000100, 8, (uint8_t*)&src_addr);
    bus->store(0x200000108, 8, (uint8_t*)&dst_addr);
    uint64_t chn_bits = 0b01; // channel 0 enable
    bus->store(0x200000010, 8, (uint8_t*)&chn_bits);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    extern std::atomic<uint64_t> global_tick;
    global_tick += 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    // while (1);
    uint64_t pending_bits;
    bus->load(0x200000010, 8, (uint8_t*)&pending_bits);
    assert((pending_bits & chn_bits) == 0ul);
    uint64_t src_data;
    bus->load(src_addr, 8, (uint8_t*)&src_data);
    uint64_t dst_data;
    bus->load(src_addr, 8, (uint8_t*)&dst_data);
    assert(src_data == dst_data);


    src_addr = 0x100000000;
    dst_addr = 0x180000000;
    bus->store(0x200000110, 8, (uint8_t*)&src_addr);
    bus->store(0x200000118, 8, (uint8_t*)&dst_addr);
    bus->store(0x200000100, 8, (uint8_t*)(&src_addr));
    bus->store(0x200000108, 8, (uint8_t*)(&src_addr));
    chn_bits = 0b10; // channel 1 enable
    bus->store(0x200000010, 8, (uint8_t*)&chn_bits);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    extern std::atomic<uint64_t> global_tick;
    global_tick += 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bus->load(0x200000010, 8, (uint8_t*)&pending_bits);
    assert((pending_bits & chn_bits) == 0ul);
    bus->load(src_addr, 8, (uint8_t*)&src_data);
    bus->load(src_addr, 8, (uint8_t*)&dst_data);
    assert(src_data == dst_data);

    // while (1);
}
