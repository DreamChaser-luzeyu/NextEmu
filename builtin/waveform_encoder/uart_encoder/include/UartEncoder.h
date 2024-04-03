#pragma once

#include <atomic>
#include <cassert>
#include <mutex>
#include "sdk/interface/waveform_generator.h"
#include "sdk/console.h"

// WARN RISK SAMPLING MOMENT, OPTIMIZE YOUR RTL!

class UartEncoder : public Interface_ns::WaveformGenerator_I {
private:
    const uint64_t clkFreqHz;
    const uint64_t baudRate;
    const uint8_t *txData;
    uint64_t txDataBytes;
    uint64_t cycleCounter = 0;
    uint64_t bitCounter = 0;
    uint64_t txBitCounter = 0;
    const uint64_t cyclesPerBit;
    uint64_t cyclesToDelay;
    const uint64_t bytesDelay;
    const uint64_t payloadBits;

    std::mutex ueMutex;

    Interface_ns::signal_bit_val_t currentLogicVal;
    Interface_ns::WireSignal* wireSignal;

    const static uint8_t STATE_IDLE = 0;
    const static uint8_t STATE_START_BIT = 1;
    const static uint8_t STATE_DATA_BITS = 2;
    const static uint8_t STATE_STOP_BIT = 3;
    uint8_t currentState;
    uint8_t nextState;

    static uint8_t get_tx_data_bit(uint64_t bit_index, const uint8_t *data) {
        uint64_t byte_index = bit_index >> 3;
        uint8_t bit_index_in_byte = bit_index % 8;
        return !!((data[byte_index] >> bit_index_in_byte) & 0b00000001);
    }

    /**
     * Get bit according to current state
     * @return
     */
    uint8_t get_bit() {
        switch (currentState) {
            case STATE_IDLE:
                return LOGIC_HIGH;
            case STATE_START_BIT:
                return LOGIC_LOW;
            case STATE_DATA_BITS:
                return get_tx_data_bit(txBitCounter, txData);
            case STATE_STOP_BIT:
                return LOGIC_HIGH;
        }
        return 0;
    }

    /**
     * @note Called every specified baud rate cycle duration
     */
    void on_update() {
        currentState = nextState;
        currentLogicVal.store(get_bit());
        wireSignal->setBit(0, (uint8_t)(currentLogicVal));
        // do state transition
        switch (currentState) {
            case STATE_IDLE:
                nextState = STATE_START_BIT;
                break;
            case STATE_START_BIT:
                nextState = STATE_DATA_BITS;
                break;
            case STATE_DATA_BITS:
                bitCounter++;
                txBitCounter++;
                if (bitCounter == payloadBits) {
                    bitCounter = 0;
                    nextState = STATE_STOP_BIT;
                }
                break;
            case STATE_STOP_BIT:
                nextState = STATE_IDLE;
        }
    }

public:

    explicit UartEncoder(uint64_t clk_freq_hz, const uint8_t *data, uint64_t data_len_bytes, uint64_t baurd_rate = 115200,
                         uint64_t payload_bits = 8, uint64_t cycles_delay = 0, uint64_t bytes_delay = 0)
            : clkFreqHz(clk_freq_hz), txData(data), txDataBytes(data_len_bytes), baudRate(baurd_rate),
              cyclesPerBit(clkFreqHz / baudRate),
              payloadBits(payload_bits), cyclesToDelay(cycles_delay), bytesDelay(bytes_delay),
              wireSignal(new Interface_ns::WireSignal(1, 0)) {
        if (clkFreqHz % baudRate) {
            LOG_WARN("Cannot accurately divide the frequency using clk freq %lu and baurd rate %lu", clkFreqHz,
                     baudRate);
        }
        currentLogicVal.store(LOGIC_HIGH);
        wireSignal->setBit(0, Interface_ns::WireSignal::BIT_POS);
        nextState = STATE_IDLE;
        ueMutex.unlock();
    }

    /**
     * Update level value of all signal channels, to be called in clock tick.
     * @param nr_ticks
     */
    void tick(uint64_t nr_ticks) override {
        ueMutex.lock();
        if (cyclesToDelay > 0) {
            cyclesToDelay--;
            return;
        }
        if ((cycleCounter % cyclesPerBit) == 0) {
            if((txBitCounter < 8 * txDataBytes) || (currentState != STATE_IDLE)) {
                on_update();
            }
            else LOG_INFO("UartEncoder: Tx Done");
        }
        cycleCounter++;
        ueMutex.unlock();
    }

    void resetData(const uint8_t *data, size_t data_len_bytes) {
        ueMutex.lock();
        txBitCounter = 0;
        txDataBytes = data_len_bytes;
        txData = data;
        nextState = STATE_IDLE;
        ueMutex.unlock();
    }

    bool finished() override {
        return (txBitCounter >= 8 * txDataBytes) && (currentState == STATE_IDLE);
    }

    /**
     * Get the LEVEL of the signal at the specified channel at the moment.
     * To be called by rtl-level devices.
     * @param channel Channel of the signal
     * @return level of the signal at channel `channel`
     */
    Interface_ns::signal_bit_val_t *getBitValRef(uint32_t channel) {
        assert(channel == 0);   // We have only 1 channel for uart tx
//        LOG_DEBUG("Current State: %d", currentState);
        return &currentLogicVal;
    }

    Interface_ns::WireSignal *getWire(uint32_t channel) override {
        assert(channel == 0);   // We have only 1 channel for uart tx
        return wireSignal;
    }

};
