#pragma once

#include <mutex>
#include <atomic>

#include "sdk/utils.h"
#include "sdk/interface/dev_if.h"

namespace Interface_ns {

typedef uint8_t bit_val_t;
typedef std::atomic<bit_val_t> signal_bit_val_t;

const static bit_val_t BIT_UNDEF = 2;
const static bit_val_t BIT_POS = 1;
const static bit_val_t BIT_NEG = 0;

class WireSignal {
private:
    uint8_t *bitVals;
    size_t busWidth;
    std::mutex signalMutex;
    bool isVerilatorSig;
public:
    const static bit_val_t BIT_POS = 1;
    const static bit_val_t BIT_NEG = 0;

    static inline uint8_t bit_mask(size_t bit_index) {
        uint8_t val = 1;
        val <<= bit_index;
        return val;
    }

    explicit WireSignal(size_t bus_width, uint64_t val) : bitVals(new uint8_t[(bus_width / 8 + 1)]),
                                                          busWidth(bus_width),
                                                          isVerilatorSig(false) {
        size_t nr_bytes = bus_width / 8 + 1;
        // @note Let's force little-endian write
        for(size_t i = 0; i < nr_bytes; i++) {
            bitVals[i] = (uint8_t)(val & 0xff);
            val >>= 8;
        }
        signalMutex.unlock();
    }

    void setVerilatorSig(void* sig_ptr) {
        assert(sig_ptr);
        // Test little endian
//        assert(is_little_endian());
//        if(!is_little_endian()) {
//            LOG_ERR("Host ISA endian error: requires little endian");
//            exit(-1);
//        }

        bitVals = (uint8_t*)sig_ptr;
        delete[] bitVals;
        isVerilatorSig = true;
    }

    ~WireSignal() {
        if(!isVerilatorSig) delete[] bitVals;
    }

    bool setBit(size_t bit_index, bit_val_t bit_val) {
        if(bit_index >= busWidth) return false;

        if(isVerilatorSig) {
            if((1 <= busWidth) && (busWidth <= 8)) {
                uint8_t* ptr = bitVals;
                signalMutex.lock();
                if(bit_val) (*ptr) |= ((uint8_t)(0x1) << bit_index);
                else (*ptr) &= ~((uint8_t)(0x1) << bit_index);
                signalMutex.unlock();
                return true;
            }
            if((9 <= busWidth) && (busWidth <= 16)) {
                auto* ptr = (uint16_t*)bitVals;
                signalMutex.lock();
                if(bit_val) (*ptr) |= ((uint16_t)(0x1) << bit_index);
                else (*ptr) &= ~((uint16_t)(0x1) << bit_index);
                signalMutex.unlock();
                return true;
            }
            if((17 <= busWidth) && (busWidth <= 32)) {
                auto* ptr = (uint32_t*)bitVals;
                signalMutex.lock();
                if(bit_val) (*ptr) |= ((uint32_t)(0x1) << bit_index);
                else (*ptr) &= ~((uint32_t)(0x1) << bit_index);
                signalMutex.unlock();
                return true;
            }
            if((33 <= busWidth) && (busWidth <= 64)) {
                auto* ptr = (uint64_t*)bitVals;
                signalMutex.lock();
                if(bit_val) (*ptr) |= ((uint64_t)(0x1) << bit_index);
                else (*ptr) &= ~((uint64_t)(0x1) << bit_index);
                signalMutex.unlock();
                return true;
            }
            if(busWidth > 64) {
                auto* ptr = (uint32_t*)bitVals;
                signalMutex.lock();
                if(bit_val) (ptr[bit_index / 32]) |= ((uint32_t)(0x1) << (bit_index % 32));
                else (ptr[bit_index / 32]) &= ~((uint32_t)(0x1) << (bit_index % 32));
                signalMutex.unlock();
                return true;
            }
            assert(false);
        }

        if(bit_val == BIT_POS) {
            signalMutex.lock();
            bitVals[bit_index / 8] |= bit_mask(bit_index % 8);
            signalMutex.unlock();
        } else {
            signalMutex.lock();
            bitVals[bit_index / 8] &= (~bit_mask(bit_index % 8));
            signalMutex.unlock();
        }
        return true;
    }

    bit_val_t getBit(size_t bit_index) {
        if(bit_index >= busWidth) return -1;

        if(isVerilatorSig) {
            if((1 <= busWidth) && (busWidth <= 8)) {
                uint8_t* ptr = bitVals;
                signalMutex.lock();
                if((*ptr) & ((uint8_t)(0x1) << bit_index)) {
                    signalMutex.unlock();
                    return BIT_POS;
                }
                signalMutex.unlock();
                return BIT_NEG;
            }
            if((9 <= busWidth) && (busWidth <= 16)) {
                auto* ptr = (uint16_t*)bitVals;
                signalMutex.lock();
                if((*ptr) & ((uint16_t)(0x1) << bit_index)) {
                    signalMutex.unlock();
                    return BIT_POS;
                }
                signalMutex.unlock();
                return BIT_NEG;
            }
            if((17 <= busWidth) && (busWidth <= 32)) {
                auto* ptr = (uint32_t*)bitVals;
                signalMutex.lock();
                if((*ptr) & ((uint32_t)(0x1) << bit_index)) {
                    signalMutex.unlock();
                    return BIT_POS;
                }
                signalMutex.unlock();
                return BIT_NEG;
            }
            if((33 <= busWidth) && (busWidth <= 64)) {
                auto* ptr = (uint64_t*)bitVals;
                signalMutex.lock();
                if((*ptr) & ((uint64_t)(0x1) << bit_index)) {
                    signalMutex.unlock();
                    return BIT_POS;
                }
                signalMutex.unlock();
                return BIT_NEG;
            }
            if(busWidth > 64) {
                auto* ptr = (uint32_t*)bitVals;
                signalMutex.lock();
                if((ptr[bit_index / 32]) & ((uint32_t)(0x1) << (bit_index % 32))) {
                    signalMutex.unlock();
                    return BIT_POS;
                }
                signalMutex.unlock();
                return BIT_NEG;
            }
            assert(false);
        }

        signalMutex.lock();
        uint8_t masked_val = (bitVals[bit_index / 8] & bit_mask(bit_index % 8));
        signalMutex.unlock();
        if(masked_val) return BIT_POS;
        return BIT_NEG;
    }

//    uint8_t getU8() { return bitVals[0]; }
//
//    uint16_t getU16() {
//        uint16_t val;
//        uint8_t* ptr = (uint8_t*)&val;
//        ptr[0] = bitVals[0];
//        ptr[1] = bitVals[1];
//        return val;
//    }


    size_t getBusWidth() const { return busWidth; }
};

class WaveformGenerator_I : public Interface_ns::Triggerable_I {
public:
    virtual Interface_ns::WireSignal *getSignal(uint32_t channel) = 0;
    virtual bool finished() = 0;
};

}