#pragma once

#include <cstdint>
#include <cassert>
#include <queue>
#include <mutex>
#include <cstring>
#include <thread>
#include "sdk/console.h"
#include "sdk/interface/dev_if.h"
#include "sdk/symbol_attr.h"

namespace Builtin_ns {

class Uartlite : public Interface_ns::SlaveIO_I, public Interface_ns::Triggerable_I {
private:
    typedef struct UartliteReg {
        uint32_t rx_fifo;    ///< offset 0  Reading this register will result in reading the data word from the top of
                             ///< the FIFO
        uint32_t tx_fifo;    ///< offset 4  This is a write-only location. Issuing a read request to the transmit data
                             ///< FIFO generates the read acknowledgement with zero data.
        uint32_t status;     ///< offset 8
        uint32_t control;    ///< offset 12
    } UartliteReg_t;
    static_assert(offsetof(UartliteReg_t, rx_fifo) == 0);
    static_assert(offsetof(UartliteReg_t, tx_fifo) == 4);
    static_assert(offsetof(UartliteReg_t, status) == 8);
    static_assert(offsetof(UartliteReg_t, control) == 12);

    UartliteReg_t regs;
    size_t devSize;
    std::queue<char> tx;
    std::queue<char> rx;
    std::mutex rxMutex;
    std::mutex txMutex;
    bool waitACK;            ///< Initialized with FALSE, TRUE when all elems in queue `tx` are popped

    const static uint32_t CTRL_RST_TX = 0x01;
    const static uint32_t CTRL_RST_RX = 0x02;
    const static uint32_t RX_FIFO_VALID_DATA = (1 << 0);  ///< Indicates if the receive FIFO has valid data
    const static uint32_t RX_FIFO_FULL = (1 << 1);        ///< Indicates if the receive FIFO is full.
    const static uint32_t TX_FIFO_EMPTY = (1 << 2);       ///< Indicates if the transmit FIFO is empty.
    const static uint32_t TX_FIFO_FULL = (1 << 3);        ///< Indicates if the transmit FIFO is full.
    const static uint32_t INTR_ENABLED = (1 << 4);        ///< Indicates if interrupts is enabled.

    std::thread *inputThread = nullptr;
    Interface_ns::InterruptController_I * intc = nullptr;

public:

    Uartlite(Interface_ns::InterruptController_I *intc = nullptr, size_t dev_size = 1024 * 1024)
            : waitACK(false),
              devSize(dev_size),
              intc(intc) {
        regs.rx_fifo = 0;
        regs.tx_fifo = 0;
        regs.status = TX_FIFO_EMPTY;
        regs.control = 0;
    }

    void tick(UNUSED uint64_t nr_ticks) override {
        if(intc) {
            intc->setInt(1, hasIRQ());
        }
        std::unique_lock<std::mutex> lock(txMutex);
        STDOUT_ACQUIRE_LOCK;
        while (!tx.empty()) {
            char c = tx.front();
            tx.pop();
            if (tx.empty()) waitACK = true;
            putchar(c);
        }
        fflush(stdout);
        STDOUT_RELEASE_LOCK;
    }

    void readInput() {
        while (1) {
            char c = getchar();
//            if (c == 10) c = 13;   // convert lf to cr
            rxMutex.lock();
            rx.push(c);
            rxMutex.unlock();
        }
    }

    void spawnInputThread() {
        inputThread = new std::thread(&Uartlite::readInput, this);
    }

    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override {
        begin_addr %= devSize;
//        LOG_INFO("Uartlite: Loading address 0x%08lx", begin_addr);
        std::unique_lock<std::mutex> locker(rxMutex); // To be released on return
        if (unlikely(begin_addr + len > sizeof(regs))) {
            LOG_ERR("Uartlite: Invalid address 0x%08lx", begin_addr);
            assert(false);
            return Interface_ns::FB_OUT_OF_RANGE;
        }

        if (!rx.empty()) {
            regs.status |= RX_FIFO_VALID_DATA;   // Set bit
            regs.rx_fifo = (uint8_t) (rx.front());
        } else regs.status &= ~RX_FIFO_VALID_DATA; // Clear bit

        memcpy(buffer, ((uint8_t *) (&regs)) + begin_addr, len);
        waitACK = false;  // Clear the bit which is causing irq

        // --- Check if accessed `regs.rx_fifo`
        if (begin_addr <= offsetof(UartliteReg_t, rx_fifo) &&
            offsetof(UartliteReg_t, rx_fifo) <= begin_addr + len - 1) {
            static_assert(offsetof(UartliteReg_t, rx_fifo) == 0);
            // `regs.rx_fifo` accessed
            if (!rx.empty()) rx.pop();
        }
//        tick(0);

        return Interface_ns::FB_SUCCESS;
    }

    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override {
        begin_addr %= devSize;
//        LOG_INFO("Uartlite: Storing address 0x%08lx", begin_addr);
//        std::unique_lock<std::mutex> tx_locker(txMutex);
        txMutex.lock();
        rxMutex.lock();
//        std::unique_lock<std::mutex> rx_locker(rxMutex);
        if (unlikely(begin_addr + len > sizeof(regs))) {
            LOG_ERR("Uartlite: Invalid address 0x%08lx", begin_addr);
            assert(false);
            return Interface_ns::FB_OUT_OF_RANGE;
        }

        memcpy(((uint8_t *) (&regs)) + begin_addr, buffer, len);

        // --- Check if accessed `regs.tx_fifo`
        if (begin_addr <= offsetof(UartliteReg_t, tx_fifo) &&
            offsetof(UartliteReg_t, tx_fifo) <= begin_addr + len - 1) {
            static_assert(offsetof(UartliteReg_t, tx_fifo) == 4);
            tx.push((char) (regs.tx_fifo));
        }
        // --- Check if accessed `regs.control`
        if (begin_addr <= offsetof(UartliteReg_t, control) &&
            offsetof(UartliteReg_t, control) <= begin_addr + len - 1) {
            if (regs.control & CTRL_RST_TX) {
                while (!tx.empty()) tx.pop();
            }
            if (regs.control & CTRL_RST_RX) {
                while (!rx.empty()) rx.pop();
            }
        }

        txMutex.unlock();
        rxMutex.unlock();
//        tick(0);

        return Interface_ns::FB_SUCCESS;
    }

    bool hasIRQ() {
        std::unique_lock<std::mutex> lock(txMutex);
        return (!rx.empty()) || waitACK;     // IRQ when rx buffer not empty (has data to read)
        // or tx buffer empty (all data has been transmitted)
    }

};

}

