//#pragma once
//
//#include "processor.h"
//#include "isa_parser.h"
//#include "cfg.h"
//#include "simif.h"
//
//
//class SpikeCoreIfAdapter : simif_t {
//    char *addr_to_mem(reg_t paddr) override {
//        return nullptr;
//    }
//
//    bool mmio_load(reg_t paddr, size_t len, uint8_t *bytes) override {
//        return false;
//    }
//
//    bool mmio_store(reg_t paddr, size_t len, const uint8_t *bytes) override {
//        return false;
//    }
//
//    void proc_reset(unsigned int id) override {
//
//    }
//
//    const cfg_t &get_cfg() const override {
//        assert(0);  // Never called in core
//        return <#initializer#>;
//    }
//
//    const std::map<size_t, processor_t *> &get_harts() const override {
//        assert(0);  // Never called in core
//        return <#initializer#>;
//    }
//
//    const char *get_symbol(uint64_t paddr) override {
//        assert(0);  // Never called in core
//        return nullptr;
//    }
//};
//
//class SpikeCore : public Interface_ns::MasterAtomicIO_I, public Interface_ns::Runnable_I {
//private:
//    processor_t* spikeProcessor;
//    isa_parser_t isa;
//    cfg_t cfg;
//
//public:
//
//    explicit SpikeCore(Interface_ns::SlaveAtomicIO_I* mmu, reg_t hart_id):
//            isa("", ""),
//            Interface_ns::MasterAtomicIO_I(mmu) {
//        spikeProcessor = new processor_t(&isa, &cfg, );
//    }
//
//};