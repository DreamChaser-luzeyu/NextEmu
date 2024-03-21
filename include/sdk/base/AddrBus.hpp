#pragma once

#include <stdint.h>
#include <map>
#include <cstring>

#include "sdk/sdkconfig.h"
#include "sdk/console.h"
#include "sdk/interface/dev_if.h"
#include "sdk/symbol_attr.h"


namespace Base_ns {

using Interface_ns::addr_t;
using Interface_ns::SlaveIO_I;

class AddrBus : public Interface_ns::SlaveAtomicIO_I {
private:
    const static uint32_t MAX_DEV_NUM = 16;

    // --- Store main mem separately, always check if main mem addr first for better performance
    uint8_t *mainMem;
    addr_t memBeginAddr;
    addr_t memEndAddr;

    std::map<addr_t, Interface_ns::SlaveIO_I *> devices; ///< dev begin addr -> dev io handle


public:

    AddrBus() : mainMem(nullptr) {}

    int addDev(Interface_ns::SlaveIO_I *io_handle, uint64_t begin_addr,
               bool is_main_mem = false, size_t main_mem_size = 1024l * 1024l * 128) {
        // Searching devices via lower_bound/upper_bound
        // implicitly relies on the underlying std::map
        // container to sort the keys and provide ordered
        // iteration over this sort, which it does. (python's
        // SortedDict is a good analogy)
        devices[begin_addr] = io_handle;
        return 0;
    }

    int setMem(uint64_t begin_addr, bool main_mem_size = 1024l * 1024l * 128) {
        mainMem = new uint8_t[main_mem_size];
        memBeginAddr = begin_addr;
        memEndAddr = begin_addr + main_mem_size;
        return 0;
    }

    int load(addr_t begin_addr, uint64_t len, uint8_t* buffer) override {
//        LOG_DEBUG("AddrBus: Loading address 0x%08lx", begin_addr);
        // --- Check if addr in main memory first
        if(likely(mainMem && memBeginAddr <= begin_addr && begin_addr <= memEndAddr)) {
            memcpy(buffer, &mainMem[begin_addr - memBeginAddr], len);
            return Interface_ns::FB_SUCCESS;
        }

        // Find the device with the base address closest to but
        // less than addr (price-is-right search)
        auto it = devices.upper_bound(begin_addr);
        if (unlikely(devices.empty() || it == devices.begin())) {
            // Either the bus is empty, or there weren't
            // any items with a base address <= addr
            return Interface_ns::FB_DEV_NOT_FOUND;
        }
        // Found at least one item with base address <= addr
        // The iterator points to the device after this, so
        // go back by one item.
        it--;
        return it->second->load(begin_addr - it->first, len, buffer);
    }

    int store(addr_t begin_addr, uint64_t len, const uint8_t* buffer) override {
        if(reservationValid) {
            if(begin_addr <= reserveBeginAddr && reserveBeginAddr + reserveLen <= begin_addr + len) {
                reservationValid = false;
            }
        }

//        LOG_DEBUG("AddrBus: Storing address 0x%08lx", begin_addr);
        // --- Check if addr in main memory first
        if(likely(mainMem && memBeginAddr <= begin_addr && begin_addr <= memEndAddr)) {
            memcpy(&mainMem[begin_addr - memBeginAddr], buffer, len);
            return Interface_ns::FB_SUCCESS;
        }

        auto it = devices.upper_bound(begin_addr);
        if (unlikely(devices.empty() || it == devices.begin())) {
            return Interface_ns::FB_DEV_NOT_FOUND;
        }
        it--;
        return it->second->store(begin_addr - it->first, len, buffer);
    }

    std::pair<addr_t, Interface_ns::SlaveIO_I*> findDev(addr_t addr) {
        auto it = devices.upper_bound(addr);
        // No dev found
        if(devices.empty() || it == devices.begin()) return std::make_pair(0, nullptr);
        it--;
        return std::make_pair(it->first, it->second);
    }

};

}

