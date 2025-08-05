#pragma once

#include "sdk/interface/dev_if.h"

// TODO: Replace those magic numbers
// We need 2 context corresponding to meip and seip per hart.
template<int nr_source = 1, int nr_context = 2>
class PLIC : public Interface_ns::SlaveIO_I,
             public Interface_ns::InterruptController_I,
             public Interface_ns::Triggerable_I {
public:
    PLIC() {
        for (int i = 0; i < (nr_source + 1); i++) {
            priority[i] = 0;
        }
        for (int i = 0; i < ((nr_source + 1 + 31) / 32); i++) {
            pending[i] = 0;
            claimed[i] = 0;
        }
        for (int i = 0; i < nr_context; i++) {
            for (int j = 0; j < ((nr_source + 1 + 31) / 32); j++) enable[i][j] = 0;
            threshold[i] = 0;
            claim[i] = 0;
        }
    }

    void setInt(uint32_t source_id, bool fired) override {
        if (fired) pending[source_id / 32] |= 1u << (source_id % 32);
    }

    bool get_int(int context_id) {
        uint64_t max_priority = 0;
        uint64_t max_priority_int = 0;
        for (int i = 1; i <= nr_source; i++) {
            if (priority[i] >= threshold[context_id] && ((pending[i / 32] >> (i % 32)) & 1) &&
                ((enable[context_id][i / 32] >> (i % 32)) & 1) && !((claimed[i / 32] >> (i % 32)) & 1)) {
                if (priority[i] > max_priority) {
                    max_priority = priority[i];
                    max_priority_int = i;
                }
            }
        }
        if (max_priority_int) claim[context_id] = max_priority_int;
        else claim[context_id] = 0;
        return claim[context_id] != 0;
    }

    int load(uint64_t start_addr, uint64_t size, uint8_t *buffer) override {
        start_addr %= 0x4000000;
        assert(size == 4);
        if (start_addr + size <= 0x1000) { // [0x4,0x1000] interrupt source priority
            if (start_addr == 0) return false;
            if (start_addr > 4 * nr_source || start_addr + size > 4 * (nr_source + 1)) return false;
            *((uint32_t *) buffer) = priority[start_addr / 4];
            return Interface_ns::FB_SUCCESS;
        } else if (start_addr + size <= 0x1080) { // [0x1000,0x1080] interrupt pending bits
            uint64_t idx = (start_addr - 0x1000) / 4;
            if (idx > nr_source) return false;
            *((uint32_t *) buffer) = pending[idx];
            return Interface_ns::FB_SUCCESS;
        } else if (start_addr + size <= 0x2000) {
            return Interface_ns::FB_OUT_OF_RANGE;   // error
        } else if (start_addr + size <= 0x200000) { // enable bits for sources on context
            uint64_t context_id = (start_addr - 0x2000) / 0x80;
            uint64_t pos = start_addr % 0x80;
            if (context_id >= nr_context) return false;
            if (pos > nr_source) return false;
            *((uint32_t *) buffer) = enable[context_id][pos];
            return Interface_ns::FB_SUCCESS;
        } else { // priority threshold and claim/complete
            uint64_t context_id = (start_addr - 0x200000) / 0x1000;
            if (context_id > nr_context) return false;
            uint64_t offset = start_addr % 0x1000;
            if (offset == 0) { // priority threshold
                *((uint32_t *) buffer) = threshold[context_id];
                return Interface_ns::FB_SUCCESS;
            } else if (offset == 4) { // claim/complete
                (*((uint32_t *) buffer)) = claim[context_id];
                claimed[claim[context_id] / 32] |= 1u << (claim[context_id] % 32);
                return Interface_ns::FB_SUCCESS;
            } else return Interface_ns::FB_OUT_OF_RANGE;
        }
        return Interface_ns::FB_SUCCESS;
    }

    int store(uint64_t start_addr, uint64_t size, const uint8_t *buffer) {
        start_addr %= 0x4000000;
        if (start_addr + size <= 0x1000) { // [0x4,0x1000] interrupt source priority
            if (start_addr == 0) return false;
            if (start_addr > 4 * nr_source || start_addr + size > 4 * (nr_source + 1)) return false;
            priority[start_addr / 4] = *((uint32_t *) buffer);
            return Interface_ns::FB_SUCCESS;
        } else if (start_addr + size <= 0x1080) { // [0x1000,0x1080] interrupt pending bits
            return Interface_ns::FB_SUCCESS;
        } else if (start_addr + size <= 0x2000) {
            return Interface_ns::FB_OUT_OF_RANGE;   // error
        } else if (start_addr + size <= 0x200000) { // enable bits for sources on context
            uint64_t context_id = (start_addr - 0x2000) / 0x80;
            uint64_t pos = start_addr % 0x80;
            if (context_id >= nr_context) return false;
            if (pos > nr_source) return false;
            enable[context_id][pos] = *((uint32_t *) buffer);
            return Interface_ns::FB_SUCCESS;
        } else { // priority threshold and claim/complete
            uint64_t context_id = (start_addr - 0x200000) / 0x1000;
            if (context_id > nr_context) return false;
            uint64_t offset = start_addr % 0x1000;
            if (offset == 0) { // priority threshold
                threshold[context_id] = *((uint32_t *) buffer);
                return Interface_ns::FB_SUCCESS;
            } else if (offset == 4) { // claim/complete
                claimed[(*((uint32_t *) buffer)) / 32] &= ~(1u << ((*((uint32_t *) buffer) % 32)));
                pending[(*((uint32_t *) buffer)) / 32] &= ~(1u << ((*((uint32_t *) buffer) % 32)));
                return Interface_ns::FB_SUCCESS;
            } else return Interface_ns::FB_OUT_OF_RANGE;
        }
        return Interface_ns::FB_SUCCESS;
    }

    void tick(uint64_t nr_ticks) override {
        for (uint32_t i = 0; i < nr_context; i++) {
            bool *ptr = (bool *) (intContexts[i]);
            *ptr = get_int(i);
        }
    }

    void regIntContext(uint32_t ctx_id, void *ptr) override {
        assert(ctx_id < nr_context);
        intContexts[ctx_id] = ptr;
    }

private:
    // source 0 is reserved.
    uint32_t priority[nr_source + 1];
    uint32_t pending[(nr_source + 1 + 31) / 32];
    uint32_t enable[nr_context][(nr_source + 1 + 31) / 32];
    uint32_t threshold[nr_context];
    uint32_t claim[nr_context];
    uint32_t claimed[(nr_source + 1 + 31) / 32];
    void *intContexts[nr_context];
};