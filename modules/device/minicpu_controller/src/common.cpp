#include <condition_variable>
#include <cstdint>
#include <memory>
#include <thread>
#include <sdk/base/GDBStub.h>

#include "sdk/console.h"

#include "MiniCPUController.h"
// #include "../../../platform/rvcore_minicpu/include/rvcore_minicpu/MiniCPUCore.h"

enum class Builtin_ns::MiniCPUMemController::EventType {
    ASYNC_READ,
    ASYNC_WRITE
};

class Builtin_ns::MiniCPUMemController::MemCtrlEvent {
public:
    EventType type;
    int64_t chnID;
    uint64_t srcAddr;
    uint64_t dstAddr;
};

int Builtin_ns::MiniCPUMemController::load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer)  {
    LOG_INFO("Load  MiniCPU Controller %s at dev addr %016lx len %016lx", devDesc, begin_addr, len);

    // --- Store value to reg struct
    assert(len == 8);
    uint64_t *ptr = (uint64_t*)((uint8_t*)(&this->reg) + begin_addr);
    memcpy(buffer, ptr, 8);

    if (begin_addr == 0x10) {
        // assert(len == 8);
        // LOG_INFO("Mem Controller: Read 0x10 rd chnPending register");
        // memcpy(buffer, (void*)&(this->reg.rd_pending_chn_bits), 8); // assuming little endian
        // --- Dump bytes to load
        STDOUT_ACQUIRE_LOCK;
        for(size_t i = 0; i < len; i++) {
            if(i % 8 == 0) printf("%s[INFO ] ", STYLE_TEXT_BLUE);
            printf("%02x ", buffer[i]);
            printf("%s", STYLE_RST);
        }
        printf("\n");
        STDOUT_RELEASE_LOCK;
    }

    if (begin_addr == 0x18) {
        // assert(len == 8);
        // LOG_INFO("Mem Controller: Read 0x18 wr chnPending register");
        // memcpy(buffer, (void*)&(this->reg.wr_pending_chn_bits), 8); // assuming little endian
        // --- Dump bytes to load
        STDOUT_ACQUIRE_LOCK;
        for(size_t i = 0; i < len; i++) {
            if(i % 8 == 0) printf("%s[INFO ] ", STYLE_TEXT_BLUE);
            printf("%02x ", buffer[i]);
            printf("%s", STYLE_RST);
        }
        printf("\n");
        STDOUT_RELEASE_LOCK;
    }

    return Interface_ns::FB_SUCCESS;
}

int Builtin_ns::MiniCPUMemController::store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer)  {
    LOG_INFO("Store MiniCPU Controller %s at dev addr %016lx len %016lx", devDesc, begin_addr, len);
    // --- Dump bytes to store
    STDOUT_ACQUIRE_LOCK;
    for(size_t i = 0; i < len; i++) {
        if(i % 8 == 0) printf("%s[INFO ] ", STYLE_TEXT_BLUE);
        printf("%02x ", buffer[i]);
        printf("%s", STYLE_RST);
    }
    printf("\n");
    STDOUT_RELEASE_LOCK;

    // --- Store value to reg struct
    assert(len == 8);
    uint64_t val_to_store;
    memcpy(&val_to_store, buffer, 8);
    uint64_t *ptr = (uint64_t*)((uint8_t*)(&this->reg) + begin_addr);
    *ptr = val_to_store;

    if (begin_addr == 0x10) {
        assert(len == 8);
        LOG_INFO("Mem Controller: Write 0x10 rd chnPending register");
        regMutex.lock();
        val_to_store &= (~(this->reg.rd_pending_chn_bits)); // Only allow setting `0` bits in chnPendingBit
        uint64_t chn_to_start = (val_to_store ^ (this->reg.rd_pending_chn_bits));
        if (!chn_to_start) {
            Interface_ns::FB_SUCCESS; // No channel to start
        }
        (this->reg.rd_pending_chn_bits) |= val_to_store;
        regMutex.unlock();

        for (size_t i = 0; i < NR_CHN; i++) {
            uint64_t mask = 1ul << (uint64_t)i;
            if (mask & chn_to_start) {
                auto event = std::make_shared<MemCtrlEvent>();
                event->type = EventType::ASYNC_READ;
                event->chnID = i;
                event->srcAddr = this->reg.rd_chn_descs[i].src_addr;
                event->dstAddr = this->reg.rd_chn_descs[i].dst_addr;

                emitEvent(event);
                LOG_INFO("Mem Controller: Start read channel %lu", i);
            }
        }
    }

    if (begin_addr == 0x18) {
        assert(len == 8);
        LOG_INFO("Mem Controller: Write 0x18 wr chnPending register");
        regMutex.lock();
        val_to_store &= (~(this->reg.wr_pending_chn_bits)); // Only allow setting `0` bits in chnPendingBit
        uint64_t chn_to_start = (val_to_store ^ (this->reg.wr_pending_chn_bits));
        if (!chn_to_start) {
            Interface_ns::FB_SUCCESS; // No channel to start
        }
        (this->reg.wr_pending_chn_bits) |= val_to_store;
        regMutex.unlock();

        for (size_t i = 0; i < NR_CHN; i++) {
            uint64_t mask = 1ul << (uint64_t)i;
            if (mask & chn_to_start) {
                auto event = std::make_shared<MemCtrlEvent>();
                event->type = EventType::ASYNC_WRITE;
                event->chnID = i;
                event->srcAddr = this->reg.wr_chn_descs[i].src_addr;
                event->dstAddr = this->reg.wr_chn_descs[i].dst_addr;

                emitEvent(event);
                LOG_INFO("Mem Controller: Start write channel %lu", i);
            }
        }
    }


    return Interface_ns::FB_SUCCESS;
}

void Builtin_ns::MiniCPUMemController::emitEvent(std::shared_ptr<Builtin_ns::MiniCPUMemController::MemCtrlEvent> event) {
    {
        std::lock_guard<std::mutex> lock(this->eventQueueMutex);
        eventQueue.emplace(event);
    }
    queueCond.notify_one();
}

Builtin_ns::MiniCPUMemController::MiniCPUMemController(Interface_ns::SlaveIO_I *bus, const char* desc)
        : bus(bus), devDesc(desc ? desc : "default"), stop(false) {
    // ----- Register Event Handler
    handler[EventType::ASYNC_READ] = [this](std::shared_ptr<MemCtrlEvent> event) {
        // assert(0x200000000ul <= event->dstAddr && event->dstAddr <= 0x200000000ul + 128ul * 1024ul * 1024ul - 8);
        LOG_DEBUG("Handling read event...");
        uint64_t data = 0;
        this->bus->load(event->srcAddr, 8, (uint8_t*)&data);
        this->bus->store(event->dstAddr, 8, (uint8_t*)&data);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        {
            std::lock_guard<std::mutex> lock(this->regMutex);
            uint64_t mask = ~(1ul << event->chnID);
            (this->reg.rd_pending_chn_bits) &= mask;           // clear pending bit
        }
        LOG_DEBUG("Handle done");
    };
    handler[EventType::ASYNC_WRITE] = [](std::shared_ptr<MemCtrlEvent>) {
        LOG_DEBUG("Handling write event...");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        LOG_DEBUG("Handle done");
    };


    eventThread = std::make_unique<std::thread>([this]() {
        while (true) {
            std::shared_ptr<MemCtrlEvent> event;
            {
                std::unique_lock<std::mutex> queue_lock(eventQueueMutex);
                queueCond.wait(queue_lock, [this]() { return stop || (!eventQueue.empty()); });
                event = eventQueue.front();
                eventQueue.pop();
            }
            auto event_handler = (this->handler)[event->type];
            event_handler(event);
            // --- Clear bit
            {
                std::unique_lock<std::mutex> pendingBitLock(regMutex);
                uint64_t mask = ~(1ul << (event->chnID));
                (this->reg.rd_pending_chn_bits) &= mask;
            }
        }
    });
}

