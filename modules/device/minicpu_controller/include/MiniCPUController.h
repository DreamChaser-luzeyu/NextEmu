#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <queue>
#include <unordered_map>
#include <condition_variable>
#include <thread>

#include "sdk/interface/dev_if.h"

namespace Builtin_ns {



class MiniCPUMemController: public Interface_ns::SlaveIO_I {
private:
    const static size_t NR_CHN = 16;

    const char* devDesc;
    Interface_ns::SlaveIO_I * bus;

    uint64_t miniCPUMemBase = 0x180000000;
    size_t miniCPUMemSize = 128ul * 1024ul * 1024ul;


    // volatile uint64_t rdPendingChnBit = 0x0ul;

    class MemCtrlEvent;
    enum class EventType;

    std::pmr::unordered_map<EventType, std::function<void(std::shared_ptr<MemCtrlEvent>)> > handler;

    std::unique_ptr<std::thread> eventThread;

    std::mutex eventQueueMutex;
    std::condition_variable queueCond;
    std::queue<std::shared_ptr<MemCtrlEvent> > eventQueue;
    std::atomic<bool> stop;

    void emitEvent(std::shared_ptr<MemCtrlEvent>);

#pragma pack(8)
    struct inner_state {
        uint64_t base_reserved;
        uint64_t reserved_0;
        uint64_t rd_pending_chn_bits;
        uint64_t wr_pending_chn_bits;
        uint8_t  reserved_1[224];
        struct {
            uint64_t src_addr;
            uint64_t dst_addr;
        } rd_chn_descs[NR_CHN];  // RAM -> MiniCPU
        struct {
            uint64_t src_addr;
            uint64_t dst_addr;
        } wr_chn_descs[NR_CHN];  // MiniCPU -> RAM
    };
#pragma pack()
    std::mutex regMutex;
    volatile struct inner_state reg;


public:
    explicit MiniCPUMemController(Interface_ns::SlaveIO_I *bus, const char* desc = nullptr);

    // ----- MMIO Load/Store for peripheral registers

    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override;
    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override;

    ~MiniCPUMemController() {
        stop = true;
        queueCond.notify_all();
        eventThread->join();
    }


    // -----
};

}

