#pragma once

#include "sdk/interface/dev_if.h"
#include "sdk/base/AddrBus.hpp"




class SpikePlatform : public Interface_ns::MasterIO_I,
        public Interface_ns::InterruptController_I , public Interface_ns::Runnable_I {
public:
    class MySim;
private:
    MySim *sim = nullptr;
    std::mutex setIntMutex;
public:
    explicit SpikePlatform(Base_ns::AddrBus *bus);

    void setInt(uint32_t int_id, bool level) override;

    int run() override;

    void step() override {
        assert(0); // Not implemented
    }


};

