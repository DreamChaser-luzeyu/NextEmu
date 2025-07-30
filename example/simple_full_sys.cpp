//
// Created by luzeyu on 25-7-29.
//

#include "example/simple_full_sys.h"

#include "SpikePlatform.hpp"
#include "Uartlite.hpp"

#include "sdk/base/AddrBus.hpp"
#include "sdk/base/ByteStreamStub.h"

void simple_full_sys_spike() {
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
    // ----- Run
    p->run();
}

void simple_full_sys() {

}
