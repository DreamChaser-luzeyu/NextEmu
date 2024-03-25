#include "SpikePlatform.hpp"
#include "mysim.h"

void SpikePlatform::setInt(uint32_t int_id, bool level) {
    setIntMutex.lock();
    sim->plic->set_interrupt_level(int_id, (int) level);
    setIntMutex.unlock();
}

int SpikePlatform::run() {
    return sim->run();
}
