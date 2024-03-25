#pragma once

#include <cstdint>
#include <cstring>
#include <queue>
#include "VTop.h"
#include "VUartRx.h"
#include "sdk/console.h"
#include "sdk/interface/digital_if.h"
#include "sdk/interface/dev_if.h"
#include "sdk/symbol_attr.h"

namespace UartRtl_ns {

typedef struct ModuleIf {
    Interface_ns::WireSignal *sig_uart_rx;
} ModuleIf_t;

/**
 * Let's make it compatible with uartlite
 */
class UartRtl : public Interface_ns::SlaveIO_I, public Interface_ns::Triggerable_I {
private:
    VUartRx *uartRx;
    Interface_ns::WireSignal *sigUartRx;
    Interface_ns::InterruptController_I *intc;
    std::queue<char> rx;
    std::queue<char> tx;
    uint32_t rx_fifo;
    uint32_t tx_fifo;
    uint32_t status;
    uint32_t control;
    bool txDone = false;
    const static uint32_t CTRL_RST_TX = 0x01;
    const static uint32_t CTRL_RST_RX = 0x02;
    const static uint32_t RX_FIFO_VALID_DATA = (1 << 0);  ///< Indicates if the receive FIFO has valid data
    const static uint32_t RX_FIFO_FULL = (1 << 1);        ///< Indicates if the receive FIFO is full.
    const static uint32_t TX_FIFO_EMPTY = (1 << 2);       ///< Indicates if the transmit FIFO is empty.
    const static uint32_t TX_FIFO_FULL = (1 << 3);        ///< Indicates if the transmit FIFO is full.
    const static uint32_t INTR_ENABLED = (1 << 4);        ///< Indicates if interrupts is enabled.

public:
    explicit UartRtl(ModuleIf interfaceSig, Interface_ns::InterruptController_I *intc = nullptr)
            : intc(intc), status(TX_FIFO_EMPTY), rx_fifo(0), tx_fifo(0), control(0) {
        uartRx = new VUartRx();
        sigUartRx = interfaceSig.sig_uart_rx;

        uartRx->IO_Rx_I = sigUartRx->getBit(0);
        uartRx->IO_Rst_I = 0;
        uartRx->IO_Clk_I = 1;
        uartRx->eval();

        uartRx->IO_Rst_I = 1;
        uartRx->IO_Clk_I = 0;
        uartRx->eval();

//        uartRx->IO_Rst_I = 1;
//        uartRx->IO_Clk_I = 1;
//        uartRx->eval();

    }

    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override {
        if (unlikely(begin_addr + len > 16)) {
            LOG_ERR("Uartlite: Invalid address 0x%08lx", begin_addr);
            assert(false);
            return Interface_ns::FB_OUT_OF_RANGE;
        }
        if ((begin_addr % 4) || (len % 4)) {
            LOG_ERR("Uartlite: Misaligned access addr 0x%08lx, len 0x%08lx", begin_addr, len);
            assert(false);
            return Interface_ns::FB_MISALIGNED;
        }

        if (!rx.empty()) {
            rx_fifo = (uint32_t) ((uint8_t) rx.front());
            status |= RX_FIFO_VALID_DATA;
        }

        // --- `rx_fifo`
        if (begin_addr == 0) {
            *(uint32_t *) (buffer) = rx_fifo;
            rx.pop();
        }
        // --- `tx_fifo`
        if (begin_addr == 4) { *(uint32_t *) (buffer) = (uint32_t) 0; }
        // --- `status`
        if (begin_addr == 8) { *(uint32_t *) (buffer) = status; }
        // --- `control`
        if (begin_addr == 12) { *(uint32_t *) (buffer) = (uint32_t) 0; }

        // --- Do the rest of access
        if (len > 4) load(begin_addr + 4, len - 4, buffer + 4);

        return Interface_ns::FB_SUCCESS;
    }

    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override {
        if (unlikely(begin_addr + len > 16)) {
            LOG_ERR("Uartlite: Invalid address 0x%08lx", begin_addr);
            assert(false);
            return Interface_ns::FB_OUT_OF_RANGE;
        }
        if ((begin_addr % 4) || (len % 4)) {
            LOG_ERR("Uartlite: Misaligned access addr 0x%08lx, len 0x%08lx", begin_addr, len);
            assert(false);
            return Interface_ns::FB_MISALIGNED;
        }

        // --- `rx_fifo`
        if (begin_addr == 0) { /* Read only, do nothing */ }
        // --- `tx_fifo`
        if (begin_addr == 4) {
            tx_fifo = *((uint32_t*)buffer);
            tx.push((char)tx_fifo);
        }
        // --- `status`
        if (begin_addr == 8) { status = *((uint32_t*)buffer); }
        // --- `control`
        if (begin_addr == 12) {
            if (control & CTRL_RST_TX) {
                while (!tx.empty()) tx.pop();
            }
            if (control & CTRL_RST_RX) {
                while (!rx.empty()) rx.pop();
            }
        }

        // --- Do the rest of access
        if (len > 4) store(begin_addr + 4, len - 4, buffer + 4);

        return Interface_ns::FB_SUCCESS;
    }

    void tick(uint64_t nr_ticks) override {
//        LOG_DEBUG("!");

        if(intc) {
            intc->setInt(2, (!rx.empty()) || txDone);
        }

        STDOUT_ACQUIRE_LOCK;
        while(!tx.empty()) {
            LOG_DEBUG("Tx byte %02x", tx.front());
            tx.pop();
            if(tx.empty()) txDone = true;
        }
        fflush(stdout);
        STDOUT_RELEASE_LOCK;

        uartRx->IO_Clk_I = !(uartRx->IO_Clk_I);
        uartRx->IO_Rx_I = sigUartRx->getBit(0);
        uartRx->eval();

        if (uartRx->IO_RxDone_O && (uartRx->IO_Clk_I)) {
            LOG_DEBUG("Push byte %02x to rx fifo", uartRx->IO_RxData_O);
            rx.push((char) (uartRx->IO_RxData_O));
        }
    }


};

}