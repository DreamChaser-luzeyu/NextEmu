#pragma once

#include "sdk/interface/dev_if.h"
#include "spike_obj_factory.h"
#include "mysim.h"

class SpikePlatform : public Interface_ns::MasterIO_I,
        public Interface_ns::InterruptController_I , public Interface_ns::Runnable_I {
private:
    MySim *sim = nullptr;

public:
    explicit SpikePlatform(Base_ns::AddrBus *bus) : Interface_ns::MasterIO_I(
            (Interface_ns::SlaveIO_I *) bus) {
        /**
         * @note Spike's command line params could be passed here.
         */
        const char *argv[] = {"./spike",
                              "-p2",
                              "--isa=rv64imac_zicsr",
                              "--pc=0x80000000",
                              "--pmpregions=0",
                              "--disable-dtb",          // Must set. Make our framework in charge of mmio devs.
                              "none",                   // Must be `none` since we don't want spike to load elf/bin
                              nullptr};

        /**
         * @note Function `create_sim` is almost exactly the same as the `main` func in spike.
         * @note We want to keep as many spike's functions as possible.
         */
        sim = create_sim(7, argv, bus);
//        sim->get_harts();
    }

    void setInt(uint32_t int_id, bool level) override {
        sim->plic->set_interrupt_level(int_id, (int) level);
    }

    int run() override {
        return sim->run();
    }

    void step() override {
        assert(0); // Not implemented
    }

};

