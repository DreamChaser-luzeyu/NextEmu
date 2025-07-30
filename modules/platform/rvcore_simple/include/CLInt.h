#pragma once

#include "sdk/interface/dev_if.h"

namespace RVCore_ns {

template<unsigned int nr_hart = 1>
class CLInt : public Interface_ns::SlaveIO_I,
              public Interface_ns::Triggerable_I,
              public Interface_ns::InterruptController_I {
public:
    CLInt() {
        mtime = 0;
        for (int i = 0; i < nr_hart; i++) {
            mtimecmp[i] = 0;
            msip[i] = 0;
        }
    }

    bool do_read(uint64_t start_addr, uint64_t size, unsigned char *buffer) {
//        std::cout << "[DEBUG] " << "CLINT READ ADDR " << start_addr << " SIZE " << size << std::endl;
        if (start_addr >= 0x4000) {
            // mtimecmp, mtime
            if (start_addr >= 0xbff8 && start_addr + size <= 0xc000) {
                // mtime
                memcpy(buffer, ((char *) (&mtime)) + start_addr - 0xbff8, size);
                // printf("read mtime\n");
            } else if (start_addr >= 0x4000 && start_addr + size <= 0x4000 + 8 * nr_hart) {
                memcpy(buffer, ((char *) (&mtimecmp)) + start_addr - 0x4000, size);
                // printf("read mtimecmp\n");
            } else return false;
        } else {
            // msip
            if (start_addr + size <= 4 * nr_hart) {
                memcpy(buffer, ((char *) (&msip)) + start_addr, size);
                // printf("read msip\n");
            } else return false;
        }
        return true;
    }

    bool do_write(uint64_t start_addr, uint64_t size, const unsigned char *buffer) {
//        std::cout << "[DEBUG] " << "CLINT WRITE ADDR " << start_addr << " SIZE " << size << std::endl;
        if (start_addr >= 0x4000) {
            // mtimecmp, mtime
            if (start_addr >= 0xbff8 && start_addr + size <= 0xc000) {
                // mtime
                memcpy(((char *) (&mtime)) + start_addr - 0xbff8, buffer, size);
                // printf("write mtime\n");
            } else if (start_addr >= 0x4000 && start_addr + size <= 0x4000 + 8 * nr_hart) {
                memcpy(((char *) (&mtimecmp)) + start_addr - 0x4000, buffer, size);
                // printf("write mtimecmp %lx mtime %lx size %d diff %lx\n",mtimecmp[0],mtime,(int)size,mtimecmp[0]-mtime);
            } else return false;
        } else {
            // msip
            if (start_addr + size <= 4 * nr_hart) {
                memcpy(((char *) (&msip)) + start_addr, buffer, size);
                for (int i = 0; i < nr_hart; i++) msip[i] &= 1;
                // printf("write msip[0] %x\n",msip[0]);
                // printf("write msip[0] %x\n",msip[1]);
            } else return false;
        }
        return true;
    }

    void tick(uint64_t UNUSED nr_ticks) override {
        for (uint32_t i = 0; i < nr_hart; i++) {
            IntStatus *ptr = (IntStatus *) (intContexts[i]);
            ptr->msip = m_s_irq(i);
            ptr->mtip = m_t_irq(i);
        }

        mtime++;
    }

    bool m_s_irq(unsigned int hart_id) { // machine software irq
        if (hart_id >= nr_hart) assert(false);
        else return (msip[hart_id] & 1);
    }

    bool m_t_irq(unsigned int hart_id) { // machine timer irq
        if (hart_id >= nr_hart) assert(false);
        else return mtime > mtimecmp[hart_id];
    }

    void set_cmp(uint64_t new_val) {
        mtimecmp = new_val;
    }

    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override {
        begin_addr %= 0x10000;
        if (do_read(begin_addr, len, buffer)) return Interface_ns::FB_SUCCESS;
        return Interface_ns::FB_OUT_OF_RANGE;
    }

    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override {
        begin_addr %= 0x10000;
        if (do_write(begin_addr, len, buffer)) return Interface_ns::FB_SUCCESS;
        return Interface_ns::FB_OUT_OF_RANGE;
    }

    void setInt(uint32_t int_id, bool triggered) override {
        assert(false);  // CLInt does not support external ints, reg at PLIC instead.
    }

    void regIntContext(uint32_t hart_id, void *ptr) override {
        assert(hart_id < nr_hart);
        intContexts[hart_id] = ptr;
    }

private:
    uint64_t mtime;
    uint64_t mtimecmp[nr_hart];
    uint32_t msip[nr_hart];
    void *intContexts[nr_hart];
};

}