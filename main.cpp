#include <DummyDev.h>
#include <iostream>
#include <mutex>
#include <SpikePlatform.hpp>
#include <Uartlite.hpp>
#include <example/minicpu_full_sys.h>
#include <sdk/base/ByteStreamStub.h>

#include "sdk/base/AddrBus.hpp"
#include "sdk/test.h"
#include "example/simple_full_sys.h"

namespace Builtin_ns {

std::mutex STDOutMutex_GV;

}


void spike_cosim() {
    RUN_TEST_UNIT(Full_TestUnit);
}

void heterogeneous_minicpu() {

}

int main() {
    // ----- Unit Test
    // (new sim_t())->hello();
    // (new processor_t())->hello();
    // (new VTop())->hello();

    // RUN_TEST_UNIT(Main_TestUnit);
    // RUN_TEST_UNIT(Full_TestUnit);

    // RUN_TEST_UNIT(AXILite_TestUnit);
    // RUN_TEST_UNIT(UartliteRTL_TestUnit);
    // RUN_TEST_UNIT(UartRTL_TestUnit);
    // RUN_TEST_UNIT(UartEncoder_TestUnit);
    // RUN_TEST_UNIT(RVCoreSpike_TestUnit);
    // RUN_TEST_UNIT(RVCore_TestUnit);

    // ----- Run example
    // simple_full_sys();
    // simple_full_sys_spike();
    simple_full_sys_minicpu();

    return 0;
}
