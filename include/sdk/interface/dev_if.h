#pragma once

#include <stdint.h>
#include <cassert>

namespace Interface_ns {

typedef uint64_t addr_t;
typedef uint64_t size_t;

const int FB_SUCCESS = 0;
const int FB_OUT_OF_RANGE = 1;
const int FB_DEV_NOT_FOUND = 2;
const int FB_MISALIGNED = 3;
const int FB_UNKNOWN_ERR = 4;

class Triggerable_I {
public:
    virtual void tick(uint64_t nr_ticks) = 0;
};

class SlaveIO_I {
public:
    virtual int load(addr_t begin_addr, uint64_t len, uint8_t* buffer) = 0;

    virtual int store(addr_t begin_addr, uint64_t len, const uint8_t *buffer) = 0;

};

class MasterIO_I {
public:
    MasterIO_I() = delete;
protected:
    SlaveIO_I* subBus;

public:
    explicit MasterIO_I(SlaveIO_I* bus) : subBus(bus) {
        assert(bus);
    }

    virtual int doLoad(addr_t begin_addr, uint64_t len, uint8_t* buffer) {
        return subBus->load(begin_addr, len, buffer);
    }

    virtual int doStore(addr_t begin_addr, uint64_t len, const uint8_t* buffer) {
        return subBus->store(begin_addr, len, buffer);
    }
};

class MasterAtomic_I {
public:
    virtual void doReserve(addr_t begin_addr, size_t len, size_t hart_id) = 0;
    virtual bool doCheck(addr_t begin_addr, size_t len, size_t hart_id) = 0;
};

class SlaveAtomic_I {
protected:
    bool reservationValid = false;
    size_t reserveForHart;
    addr_t reserveBeginAddr;
    size_t reserveLen;

public:
    virtual void reserve(addr_t begin_addr, size_t len, size_t hart_id) {
        reserveBeginAddr = begin_addr;
        reserveLen = len;
        reserveForHart = hart_id;
        reservationValid = true;
    }

    virtual bool check(addr_t begin_addr, size_t len, size_t hart_id) {
        if(!reservationValid) return false;
        reservationValid = false;
        return (begin_addr == reserveBeginAddr) && (hart_id == reserveForHart)
            && (len == reserveLen);
    }
};

class SlaveAtomicIO_I : public SlaveAtomic_I, public SlaveIO_I {};

class MasterAtomicIO_I : public MasterIO_I, public MasterAtomic_I {
public:
    MasterAtomicIO_I() = delete;
    MasterAtomicIO_I(SlaveAtomicIO_I* bus) : MasterIO_I(bus) {}

    virtual void doReserve(addr_t begin_addr, size_t len, size_t hart_id) override {
        ((SlaveAtomicIO_I*)subBus)->reserve(begin_addr, len, hart_id);
    }

    virtual bool doCheck(addr_t begin_addr, size_t len, size_t hart_id) override {
        return ((SlaveAtomicIO_I*)subBus)->check(begin_addr, len, hart_id);
    }

};

class Runnable_I {
public:
    virtual int run() = 0;
    virtual void step() = 0;

};

class InterruptController_I {
public:
    virtual void setInt(uint32_t int_id, bool triggered) = 0;
    virtual void regIntContext(uint32_t hart_id, void* ptr) {};
};

}

