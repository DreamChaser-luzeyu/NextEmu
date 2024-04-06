#pragma once

#include "sdk/base/AddrBus.hpp"
#include "abstract_device.h"
#include <sys/syscall.h>
#include "sim.h"


class DevAdapter : public Interface_ns::SlaveIO_I, public Interface_ns::Triggerable_I {
private:
    abstract_device_t *spikeDev;
public:
    explicit DevAdapter(abstract_device_t *spike_dev) : spikeDev(spike_dev) {}

private:
    int load(addr_t begin_addr, uint64_t len, uint8_t *buffer) override {
        if (spikeDev->load(begin_addr, len, buffer)) return 0;
        return 1;
    }

    int store(addr_t begin_addr, uint64_t len, const uint8_t *buffer) override {
        if (spikeDev->store(begin_addr, len, buffer)) return 0;
        return 1;
    }

    void tick(uint64_t nr_ticks) override {
        spikeDev->tick(nr_ticks);
    }
};

class SpikePlatform::MySim : public sim_t {
public:

    MySim(const cfg_t *cfg, bool halted,
          std::vector<std::pair<reg_t, abstract_mem_t *>> mems,
          std::vector<device_factory_t *> plugin_device_factories,
          const std::vector<std::string> &args,
          const debug_module_config_t &dm_config,
          const char *log_path,
          bool dtb_enabled, const char *dtb_file,
          bool socket_enabled,
          FILE *cmd_file,
          Base_ns::AddrBus *nextemu_bus) : sim_t(cfg, halted, mems, plugin_device_factories, args, dm_config, log_path,
                                                 dtb_enabled,
                                                 dtb_file, socket_enabled, cmd_file), nextEmuBus(nextemu_bus) {
//        this->get_core()
        clint_t* clint = new clint_t(this, sim_t::CPU_HZ / sim_t::INSNS_PER_RTC_TICK / 100, this->get_cfg().real_time_clint);
        plic = new plic_t(this, 4);  // Maximum num of interrupts supported (guess?)
        add_device(0x2000000, clint, "Spike CLInt");
        add_device(0xc000000, plic, "Spike PLIC");
    }


    /**
     * @note This method is not virtual. So we also needs to override all methods calling it.
     * @brief Add device to NextEmu addr bus
     * @param addr base address
     * @param dev ptr of the io handle
     */
    void add_device(reg_t addr, abstract_device_t* dev, const char* desc = "No description") {
        nextEmuBus->addDev((Interface_ns::SlaveIO_I *) (new DevAdapter(dev)), addr);
        // Also need to call the original `add_device` func, in order to tick the spike built-in devices
        // like clint, plic
        sim_t::add_device(addr, std::shared_ptr<abstract_device_t>(dev));
        LOG_INFO("SpikePlatform: Implicit added device `%s` at address %016lx", desc, addr);
    }

    bool mmio_fetch(reg_t paddr, size_t len, uint8_t* bytes) override { return mmio_load(paddr, len, bytes); }

    // no need to override this func
//    bool mmio_fetch(reg_t paddr, size_t len, uint8_t *bytes) override {
//
//    }

    Base_ns::AddrBus *nextEmuBus = nullptr;
    plic_t* plic = nullptr;   ///< Interrupt Controller

private:

    /**
     * Get pointer of this paddr, if the addr is in a memory region
     * @note We do not support this way of mem accessing. Just fallback to mmio load/store instead.
     * @param paddr
     * @return pointer to this paddr, nullptr if not memory region
     */
    char *addr_to_mem(reg_t paddr) override {
        return nullptr;
    }

    /**
     * Get whether an address is "load reservable" or not
     * @note In the original spike impl, only memory address is reservable
     * @note In my modification all address access are fallbacked to mmio load/store
     * @note So for simplicity, let's assume all addresses are reservable for now.
     * @param paddr
     * @return
     */
    bool reservable(reg_t paddr) override { return true; }

    bool mmio_load(reg_t paddr, size_t len, uint8_t *bytes) override {
        assert(nextEmuBus);

        if(nextEmuBus->load(paddr, len, bytes) == 0) return true;
        LOG_WARN("MySim: `mmio_load()`: addr %016lx failed", paddr);
        return false;
    }

    bool mmio_store(reg_t paddr, size_t len, const uint8_t *bytes) override {
        assert(nextEmuBus);

        if(nextEmuBus->store(paddr, len, bytes) == 0) return true;
        LOG_WARN("MySim: `mmio_store()`: addr %016lx false", paddr);
        return false;
    }

};
