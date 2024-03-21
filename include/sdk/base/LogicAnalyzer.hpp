#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include "sdk/console.h"
#include "sdk/interface/dev_if.h"
#include "sdk/interface/waveform_generator.h"


namespace Base_ns {

class LogicAnalyzer : public Interface_ns::Triggerable_I {
private:
    typedef Interface_ns::signal_val_t *signal_val_ptr_t;
    typedef uint8_t trigger_mode_t;
    typedef struct Signal {
        const char *desc_name;
        const char *sig_id;
        signal_val_ptr_t signal_ref;
        Interface_ns::signal_val_raw_t last_val;

        Signal(const char *desc_name, const char *sig_id, signal_val_ptr_t signal_ref)
                : desc_name(desc_name), sig_id(sig_id), signal_ref(signal_ref) {}

        Signal(const Signal &signal) {
            this->desc_name = signal.desc_name;
            this->sig_id = signal.sig_id;
            this->signal_ref = signal.signal_ref;
            this->last_val = signal.last_val;
        }
    } Signal_t;
    const char *vcdPath;
    uint64_t timeScaleNs;
    uint64_t cycleDurationNs;
    uint64_t currentMomentNs;
    uint64_t freqHz;
    uint64_t sampleCounter;
    uint64_t sampleTimes;
    trigger_mode_t triggerMode;
    bool triggered;
    signal_val_ptr_t triggerSignalRef;
    Interface_ns::signal_val_raw_t lastTriggerSignalVal;

    std::vector<Signal_t> signals;
    std::stringstream vcdStrStream;

public:

    const static trigger_mode_t TRIGGER_MODE_LOW = 0;
    const static trigger_mode_t TRIGGER_MODE_HIGH = 1;
    const static trigger_mode_t TRIGGER_MODE_POSEDGE = 2;
    const static trigger_mode_t TRIGGER_MODE_NEGEDGE = 3;
    const static trigger_mode_t TRIGGER_MODE_EDGE = 4;
    const static trigger_mode_t TRIGGER_MODE_ANALOG = 5;    ///< Begin sampling at once, do not wait for trigger

    explicit LogicAnalyzer(const char *vcd_path, uint64_t time_scale_ns, uint8_t trigger_mode, uint64_t freq_hz,
                           uint64_t sample_times = 100)
            : vcdPath(vcd_path), timeScaleNs(time_scale_ns), currentMomentNs(0),
              freqHz(freq_hz), cycleDurationNs(1000000000 / freq_hz), sampleTimes(sample_times), sampleCounter(0),
              triggerSignalRef(nullptr), triggered(false), triggerMode(trigger_mode),
              lastTriggerSignalVal(Interface_ns::WaveformGenerator_I::LOGIC_UNDEFINED) {
        vcdStrStream << "$date  $end\n"
                        "$version NextEmu LogicAnalyzer $end\n";
        // --- File header
        vcdStrStream << "$comment\n"
                        "  Acquisition with " << signals.size() << " channels at " << freq_hz << " Hz\n"
                        "$end\n"
                        "$timescale " << timeScaleNs << " ns $end\n";
        if (1000000000 / freq_hz) LOG_WARN("Not divisible freq.");
        if(trigger_mode == TRIGGER_MODE_ANALOG) triggered = true;
    }

    /**
     * Attach a channel
     * @param channel_desc_name Description of a signal
     * @param sig_uid As short as possible, a uid to describe a signal
     * @param signal Pointer to signal_val_t
     * @param is_trigger Whether to use this channel as trigger or not
     */
    void addChannel(const char *channel_desc_name, const char *sig_uid, signal_val_ptr_t signal,
                    bool is_trigger = false) {
        Signal_t s(channel_desc_name, sig_uid, signal);
        signals.emplace_back(s);
        if(is_trigger) triggerSignalRef = s.signal_ref;
    }



    static bool isTriggered(Interface_ns::signal_val_raw_t last_val, Interface_ns::signal_val_raw_t current_val,
                            trigger_mode_t trigger_mode) {
        switch (trigger_mode) {
            case TRIGGER_MODE_LOW:
                return current_val == Interface_ns::WaveformGenerator_I::LOGIC_LOW;
            case TRIGGER_MODE_HIGH:
                return current_val == Interface_ns::WaveformGenerator_I::LOGIC_HIGH;
            case TRIGGER_MODE_POSEDGE:
                return last_val == Interface_ns::WaveformGenerator_I::LOGIC_LOW
                       && current_val == Interface_ns::WaveformGenerator_I::LOGIC_HIGH;
            case TRIGGER_MODE_NEGEDGE:
                return last_val == Interface_ns::WaveformGenerator_I::LOGIC_HIGH
                       && current_val == Interface_ns::WaveformGenerator_I::LOGIC_LOW;
            case TRIGGER_MODE_ANALOG:
                return true;
            default:
                return false;
        }
        return false;
    }

public:
    /**
     * @brief Generate the signal declaration of the Value Change Dump file.
     * @note Should only be called ONCE, after finish adding all signal channels
     */
    void buildVcdHeader() {
        // --- Signal ID Declaration
        vcdStrStream << "$scope module la $end";
        for (const auto &s: signals) {
            vcdStrStream << "$var wire 1 " << s.sig_id << " " << s.desc_name << " $end\n";
        }
        vcdStrStream << "$upscope $end\n"
                        "$enddefinitions $end\n";
    }

    void dumpVcd() {
        LOG_DEBUG("%s", vcdStrStream.str().c_str());
    }

    void tick(uint64_t nr_ticks) override {
        // --- Check if triggered when not triggered
        Interface_ns::signal_val_raw_t current_trigger_signal_val = triggerSignalRef->load();
        if(!triggered) {
            if (triggerMode == TRIGGER_MODE_ANALOG) triggered = true;
            if ((!triggerSignalRef) && (triggerMode != TRIGGER_MODE_ANALOG)) {
                triggered = true;
                LOG_WARN("LogicAnalyzer: No trigger signal set, start sampling at once.");
            }
            triggered = isTriggered(lastTriggerSignalVal, current_trigger_signal_val, triggerMode);
        }
        // --- Sample signals & write value change line if triggered
        if(triggered) {
            std::stringstream moment_dump;
            moment_dump << "#" << sampleCounter * cycleDurationNs << " ";
            for (auto &p: signals) {
                Interface_ns::signal_val_raw_t current_val = p.signal_ref->load();
                // Dump value change
                moment_dump << current_val << p.sig_id << " ";
            }
            moment_dump << "\n";
            // --- Append value change to vcd string stream
            vcdStrStream << moment_dump.rdbuf();
            sampleCounter ++;
        }
        // --- Update current moment
        currentMomentNs += cycleDurationNs;

        lastTriggerSignalVal = current_trigger_signal_val;

//        LOG_DEBUG("%s", vcdStrStream.str().c_str());
    }
};


}
