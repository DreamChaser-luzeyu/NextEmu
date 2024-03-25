#include <iostream>
#include <mutex>
#include "sdk/base/AddrBus.hpp"
#include "sdk/test.h"

namespace Builtin_ns {

std::mutex STDOutMutex_GV;

}

// Should not conflict here
class sim_t {
public:
    void hello() { LOG_DEBUG("Test if linked correctly"); }
};

class processor_t {
public:
    void hello() { LOG_DEBUG("Test if linked correctly"); }
};
//class sim_t {};

int main() {
    (new sim_t())->hello();
    (new processor_t())->hello();

//    RUN_TEST_UNIT(Main_TestUnit);
    RUN_TEST_UNIT(UartRTL_TestUnit);
//    RUN_TEST_UNIT(UartEncoder_TestUnit);
//    RUN_TEST_UNIT(RVCoreSpike_TestUnit);
//    RUN_TEST_UNIT(RVCore_TestUnit);
    // --- Initialize Threads



    return 0;
}
