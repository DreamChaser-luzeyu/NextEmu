#pragma once

#include <cstdint>
#include <atomic>
#include "sdk/interface/dev_if.h"
#include "sdk/interface/samplable.h"


namespace Interface_ns {

typedef std::atomic<uint64_t> signal_val_t;
typedef uint64_t signal_val_raw_t;

class WaveformGenerator_I : public Triggerable_I {
public:

    const static uint8_t LOGIC_LOW = 0;
    const static uint8_t LOGIC_HIGH = 1;
    const static uint8_t LOGIC_UNDEFINED = 2;

    void tick(uint64_t nr_ticks) override = 0;
    virtual signal_val_t* getCurrentVal(uint32_t channel) = 0;
    virtual bool finished() = 0;
};

}