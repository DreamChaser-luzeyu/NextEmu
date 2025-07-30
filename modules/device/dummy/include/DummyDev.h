#pragma once

#include <cstdint>
#include "sdk/interface/dev_if.h"

namespace Builtin_ns {

class DummyDev: public Interface_ns::SlaveIO_I {
private:
    const char* devDesc;
public:
    explicit DummyDev(const char* desc = nullptr);
    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override;
    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override;
};

}

