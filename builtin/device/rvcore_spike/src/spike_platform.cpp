#include "SpikePlatform.hpp"
#include "mysim.h"

//class MySim;

SpikePlatform::SpikePlatform(Base_ns::AddrBus *bus) : Interface_ns::MasterIO_I(
        (Interface_ns::SlaveIO_I *) bus) {
/**
 * @note Spike's command line params could be passed here.
 * @note Anything associated with htif should not be used. (Undefined behavior)
 */
    const char *argv[] = {"./spike",
                          "-p2",
                          "--isa=rv64imac_zicsr",
                          "--real-time-clint",
                          "--pc=0x80000000",
                          "--pmpregions=0",
                          "--disable-dtb",          // Must set. Make our framework in charge of mmio devs.
                          "none",                   // Must be `none` since we don't want spike to load elf/bin
                          nullptr};

/**
 * @note Function `create_sim` is almost exactly the same as the `main` func in spike.
 * @note We want to keep as many spike's functions as possible.
 */
    MySim *create_sim(int argc, const char **argv, Base_ns::AddrBus *nextemu_bus);
    sim = create_sim(8, argv, bus);
    setIntMutex.unlock();
}

void SpikePlatform::setInt(uint32_t int_id, bool level) {
    setIntMutex.lock();
    sim->plic->set_interrupt_level(int_id, (int) level);
    setIntMutex.unlock();
}

int SpikePlatform::run() {
    return sim->run();
}
