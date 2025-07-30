#pragma once

#include <stdint.h>
#include <cstring>
#include <filesystem>
#include <fstream>

#include "sdk/console.h"
#include "sdk/interface/dev_if.h"
#include "sdk/symbol_attr.h"


class Mem : public Interface_ns::SlaveIO_I {
private:
    uint8_t* mem;
    uint64_t memSize;
public:
    explicit Mem(uint64_t mem_size = 1024l * 1024l * 128) {
        mem = new uint8_t[mem_size];
        memSize = mem_size;
    }

    explicit Mem(const char* init_bin_path, uint64_t mem_size = 1024l * 1024l * 128) {
        mem = new uint8_t[mem_size];
        memSize = mem_size;

        uint64_t file_size = std::filesystem::file_size(init_bin_path);
        if (file_size > mem_size) {
            LOG_WARN("Mem: mem size is not big enough for init file.");
            file_size = memSize;
        }

        std::ifstream init_bin_file(init_bin_path, std::ios::in | std::ios::binary);
        init_bin_file.read((char*)mem, file_size);

    }

    int load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer) override {
        if(likely(begin_addr + len - 1 < memSize)) {
            memcpy(buffer, mem + begin_addr, len);
            return Interface_ns::FB_SUCCESS;
        }
        return Interface_ns::FB_OUT_OF_RANGE;
    }

    int store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer) override {
        if(likely(begin_addr + len - 1 < memSize)) {
            memcpy(mem + begin_addr, buffer, len);
            return Interface_ns::FB_SUCCESS;
        }
        return Interface_ns::FB_OUT_OF_RANGE;
    }

    ~Mem() { delete mem; }
};
