
#include "VUartRx.h"
#include "UartRtl.h"

struct UartRtl_ns::UartRTL::RTLImpl {
    VUartRx* uartRx;
};

UartRtl_ns::UartRTL::UartRTL(ModuleIf interfaceSig): sigUartRx(interfaceSig.sig_uart_rx) {
    rtlImpl = new RTLImpl();
    rtlImpl->uartRx = new VUartRx();
    sramAdapter = new Base_ns::SRAMAdapter();
    uartRxMutex.unlock();

    // ----- Configure your register address map here
    //   --- `RxDone` Register @0x00,ro
    sramAdapter->regAddr(&(rtlImpl->uartRx->IO_RxDone_O), 0, 1);
    //   --- `RxData` Register @0x01,ro
    sramAdapter->regAddr(&(rtlImpl->uartRx->IO_RxData_O), 1, 1);
    //   --- `RxRecvDone` Register @0x02,rw
    sramAdapter->regAddr(&(rtlImpl->uartRx->IO_RxRecvDone_I), 2, 1);

    // ----- Reset rx module
    uartRxMutex.lock();
    rtlImpl->uartRx->IO_Rx_I = sigUartRx->getBit(0);
    rtlImpl->uartRx->IO_Rst_I = 0;
    rtlImpl->uartRx->IO_RxRecvDone_I = 0;
    rtlImpl->uartRx->IO_Clk_I = 1;
    rtlImpl->uartRx->eval();
    rtlImpl->uartRx->IO_Rst_I = 1;
    rtlImpl->uartRx->IO_Clk_I = 0;
    rtlImpl->uartRx->eval();
    uartRxMutex.unlock();
}

UartRtl_ns::UartRTL::~UartRTL() {
    delete rtlImpl->uartRx;
    delete rtlImpl;
    delete sramAdapter;
}

void UartRtl_ns::UartRTL::tick(uint64_t nr_ticks) {
    uartRxMutex.lock();
    rtlImpl->uartRx->IO_Clk_I = !(rtlImpl->uartRx->IO_Clk_I);
    rtlImpl->uartRx->IO_Rx_I = sigUartRx->getBit(0);
    rtlImpl->uartRx->eval();
    uartRxMutex.unlock();
}

int UartRtl_ns::UartRTL::load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) {
    uartRxMutex.lock();
    int fb = sramAdapter->load(begin_addr, len, buffer);
    uartRxMutex.unlock();
    return fb;
}

int UartRtl_ns::UartRTL::store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) {
    uartRxMutex.lock();
    int fb = sramAdapter->store(begin_addr, len, buffer);
    rtlImpl->uartRx->eval();
    uartRxMutex.unlock();
    return fb;
}
