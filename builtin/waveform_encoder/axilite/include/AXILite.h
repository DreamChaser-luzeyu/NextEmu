#pragma once

#include "sdk/interface/dev_if.h"
#include "sdk/interface/digital_if.h"

class AXILite : public Interface_ns::Triggerable_I {

public:
    void tick(uint64_t nr_ticks) override {

    }
};