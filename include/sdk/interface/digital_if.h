#pragma once

#include <mutex>
#include <atomic>

namespace Interface_ns {

typedef std::atomic<uint64_t> signal_val_t;
typedef uint64_t signal_val_raw_t;
typedef uint8_t bit_val_t;

class WireSignal {
private:
    uint8_t *bitVals;
    size_t busWidth;
    std::mutex signalMutex;
public:
    const static bit_val_t BIT_POS = 1;
    const static bit_val_t BIT_NEG = 0;

    static uint8_t bit_mask(size_t bit_index) {
        uint8_t val = 1;
        val <<= bit_index;
        return val;
    }

    explicit WireSignal(size_t bus_width, uint64_t val) : bitVals(new uint8_t[(bus_width / 8 + 1)]),
                                                          busWidth(bus_width) {
        size_t nr_bytes = bus_width / 8 + 1;
        // @note Let's force little-endian write
        for(size_t i = 0; i < nr_bytes; i++) {
            bitVals[i] = (uint8_t)(val & 0xff);
            val >>= 8;
        }
        signalMutex.unlock();
    }

    ~WireSignal() { delete[] bitVals; }

    bool setBit(size_t bit_index, bit_val_t bit_val) {
        if(bit_index >= busWidth) return false;
        if(bit_val == BIT_POS) {
            signalMutex.lock();
            bitVals[bit_index / 8] |= bit_mask(bit_index % 8);
            signalMutex.unlock();
        } else {
            signalMutex.lock();
            bitVals[bit_index / 8] &= (!bit_mask(bit_index % 8));
            signalMutex.unlock();
        }
        return true;
    }

    bit_val_t getBit(size_t bit_index) {
        signalMutex.lock();
        uint8_t masked_val = (bitVals[bit_index / 8] & bit_mask(bit_index % 8));
        signalMutex.unlock();
        if(masked_val) return BIT_POS;
        return BIT_NEG;
    }

    size_t getBusWidth() const { return busWidth; }
};

}