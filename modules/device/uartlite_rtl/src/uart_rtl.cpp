#include "VUartRx.h"
#include "UartliteRtl.h"
#include "module_manifest.h"

//
//UartliteRtl_ns::UartRtl::UartRtl(ModuleIf interfaceSig, Interface_ns::InterruptController_I *intc)
//        : intc(intc), status(TX_FIFO_EMPTY), rx_fifo(0), tx_fifo(0), control(0) {
//    uartRx = new VUartRx();
//    sigUartRx = interfaceSig.sig_uart_rx;
//
//    uartRx->IO_Rx_I = sigUartRx->getBit(0);
//    uartRx->IO_Rst_I = 0;
//    uartRx->IO_Clk_I = 1;
//    uartRx->eval();
//
//    uartRx->IO_Rst_I = 1;
//    uartRx->IO_Clk_I = 0;
//    uartRx->eval();
//
////        uartRx->IO_Rst_I = 1;
////        uartRx->IO_Clk_I = 1;
////        uartRx->eval();
//
//    rxMutex.unlock();
//    txMutex.unlock();
//}
//
//void UartliteRtl_ns::UartRtl::tick(uint64_t nr_ticks) {
////        LOG_DEBUG("!");
//
//    txMutex.lock();
//
//
//    STDOUT_ACQUIRE_LOCK;
//    while (!tx.empty()) {
//        LOG_DEBUG("Tx byte %02x", tx.front());
//        tx.pop();
//        if (tx.empty()) txDone = true;
//    }
//    fflush(stdout);
//    STDOUT_RELEASE_LOCK;
//    txMutex.unlock();
//
//    uartRx->IO_Clk_I = !(uartRx->IO_Clk_I);
//    uartRx->IO_Rx_I = sigUartRx->getBit(0);
//    uartRx->eval();
//
//    rxMutex.lock();
//    if (uartRx->IO_RxDone_O && (uartRx->IO_Clk_I)) {
//        char ch = (char)(uartRx->IO_RxData_O);
//        LOG_DEBUG("Push byte %02x, %c to rx fifo", ch, ch);
//
//        rx.push(ch);
//
//    }
//
////    rx.push('a');
//    if (intc) {
//        intc->setInt(2, (!rx.empty()) || txDone);
//    }
//
//    rxMutex.unlock();
//
//}


UartliteRtl_ns::UartRTL::UartRTL(ModuleIf interfaceSig, Interface_ns::InterruptController_I *intc, size_t dev_size)
        : waitACK(false),
          devSize(dev_size),
          intc(intc) {
    regs.rx_fifo = 0;
    regs.tx_fifo = 0;
    regs.status = TX_FIFO_EMPTY;
    regs.control = 0;

    uartRx = new VUartRx();
    sigUartRx = interfaceSig.sig_uart_rx;

    uartRx->IO_Rx_I = sigUartRx->getBit(0);
    uartRx->IO_Rst_I = 0;
    uartRx->IO_Clk_I = 1;
    uartRx->eval();

    uartRx->IO_Rst_I = 1;
    uartRx->IO_Clk_I = 0;
    uartRx->eval();

    txMutex.unlock();
    rxMutex.unlock();
}

void UartliteRtl_ns::UartRTL::tick(UNUSED uint64_t nr_ticks) {
//    LOG_DEBUG("!");
    if (intc) {
        intc->setInt(2, hasIRQ());
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


    uartRx->IO_Clk_I = !(uartRx->IO_Clk_I);
    uartRx->IO_Rx_I = sigUartRx->getBit(0);
    uartRx->eval();


    if (uartRx->IO_RxDone_O && (uartRx->IO_Clk_I)) {
        std::unique_lock<std::mutex> lock_rx(rxMutex);
        char ch = (char)(uartRx->IO_RxData_O);
//        LOG_DEBUG("Push byte %02x, %c to rx fifo", ch, ch);
        rx.push(ch);
    }

}