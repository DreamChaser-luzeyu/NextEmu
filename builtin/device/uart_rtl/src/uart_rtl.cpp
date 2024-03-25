#include "VUartRx.h"
#include "UartRtl.h"
#include "module_manifest.h"


UartRtl_ns::UartRtl::UartRtl(ModuleIf interfaceSig, Interface_ns::InterruptController_I *intc)
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

void UartRtl_ns::UartRtl::tick(uint64_t nr_ticks) {
//        LOG_DEBUG("!");

    if (intc) {
        intc->setInt(2, (!rx.empty()) || txDone);
    }

    STDOUT_ACQUIRE_LOCK;
    while (!tx.empty()) {
        LOG_DEBUG("Tx byte %02x", tx.front());
        tx.pop();
        if (tx.empty()) txDone = true;
    }
    fflush(stdout);
    STDOUT_RELEASE_LOCK;

    uartRx->IO_Clk_I = !(uartRx->IO_Clk_I);
    uartRx->IO_Rx_I = sigUartRx->getBit(0);
    uartRx->eval();

    if (uartRx->IO_RxDone_O && (uartRx->IO_Clk_I)) {
        LOG_DEBUG("Push byte %02x, %c to rx fifo", uartRx->IO_RxData_O, uartRx->IO_RxData_O);
        rx.push((char) (uartRx->IO_RxData_O));
    }
}