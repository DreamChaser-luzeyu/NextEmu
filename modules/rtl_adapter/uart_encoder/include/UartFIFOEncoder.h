#pragma once

#include "UartEncoder.h"

class UartFIFOEncoder : public UartEncoder {
public:
    void tick(uint64_t nr_ticks) override {
        UartEncoder::tick(nr_ticks);
    }

    bool finished() override {
        return UartEncoder::finished();
    }
};