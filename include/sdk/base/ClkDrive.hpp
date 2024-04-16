#pragma once

#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include "sdk/interface/dev_if.h"

namespace Base_ns {

using Interface_ns::SlaveIO_I;

typedef struct ClkDriveCfg {
    uint64_t freq_hz;
    bool realtime;

} ClkDriveCfg_t;

class ClkDrive {
private:
    const char *clkDesc;
    std::vector<Interface_ns::Triggerable_I *> objs;
    uint64_t nrTicks;
    uint64_t cycleIntervalNs;           ///< Interval in nanoseconds (1/1000,000,000 s)
    bool realtime;                      ///< Whether to sync with host time or not.
    bool tickAtOnce;                    ///< Whether to delay at first tick. Set to `true` to tick once thread begins.
    std::atomic<bool> terminateThread = false;
    std::thread *thread = nullptr;

    void do_tick() {
        nrTicks++;
        for (auto o: objs) o->tick(nrTicks);
    }

    void run() {
        if (tickAtOnce) do_tick();
        if (realtime) {
            using namespace std::chrono;
            using std::chrono::steady_clock;
            auto last = steady_clock::now(); // The moment when last tick finishes, initialize with now moment
            while (!terminateThread) {
                // Calculate interval in ns
                uint64_t duration_ns = duration_cast<nanoseconds>(steady_clock::now() - last).count();
                // Do tick
                if (duration_ns >= cycleIntervalNs) {
                    // Cal tick time consumption
                    auto moment_before_tick = steady_clock::now();
                    do_tick();
                    auto moment_after_tick = steady_clock::now();
                    uint64_t tick_time_ns = duration_cast<nanoseconds>(moment_after_tick - moment_before_tick).count();
                    // Check if potential miss tick
                    if (tick_time_ns >= cycleIntervalNs) LOG_WARN("Clk %s tick miss, cannot run realtime", clkDesc);
                    last = steady_clock::now();  // Update last moment
                }
            }
        } else {
            while (!terminateThread) {
                do_tick();
                std::this_thread::sleep_for(std::chrono::nanoseconds(cycleIntervalNs));
            }
        }
    }

public:
    explicit ClkDrive(uint64_t freq_hz = 200000000, bool real_time = true, bool tick_at_once = false,
                      const char *desc = "Default description")
            : nrTicks(0), cycleIntervalNs(1000000000 / freq_hz), realtime(real_time), tickAtOnce(tick_at_once),
              clkDesc(desc) {}

    void regTickObj(Interface_ns::Triggerable_I *tick_obj) {
        objs.push_back(tick_obj);
    }

    void spawn() {
        thread = new std::thread(&ClkDrive::run, this);
    }

    void terminate() {
        terminateThread = true;
        thread->join();
        delete thread;
    }
};

}