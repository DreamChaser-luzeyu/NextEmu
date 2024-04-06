#pragma once

#include <unordered_map>
#include <map>
#include <cstring>
#include "sdk/interface/dev_if.h"


namespace Base_ns {

using Interface_ns::addr_t;
using Interface_ns::size_t;
using Interface_ns::FB_SUCCESS;
using Interface_ns::FB_OUT_OF_RANGE;

class SRAMAdapter : public Interface_ns::SlaveIO_I {

    std::map<addr_t, void *> addrRegMap;

public:
    void regAddr(void *ptr, addr_t dev_addr, size_t len) {
        for (size_t i = 0; i < len; i++) {
            addrRegMap[dev_addr + i] = ((uint8_t *) ptr) + i;
        }
    }

    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override {
        // --- Do nothing if out of range
        try {
            for (size_t i = 0; i < len; i++) {
                const void *ptr = addrRegMap.at(begin_addr + i);
            }
        } catch (std::out_of_range &e) {
            return FB_OUT_OF_RANGE;
        }
        // --- Access after checking
        for (size_t i = 0; i < len; i++) {
            const void *ptr = addrRegMap.at(begin_addr + i);
            memcpy(buffer + i, ptr, 1);
        }
        return FB_SUCCESS;
    }

    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override {
        // --- Do nothing if out of range
        try {
            for (size_t i = 0; i < len; i++) {
                void *ptr = addrRegMap.at(begin_addr + i);
            }
        } catch (std::out_of_range &e) {
            return FB_OUT_OF_RANGE;
        }

        for (size_t i = 0; i < len; i++) {
            void *ptr = addrRegMap.at(begin_addr + i);
            memcpy(ptr, buffer + i, 1);
        }
        return FB_SUCCESS;
    }

};

}
