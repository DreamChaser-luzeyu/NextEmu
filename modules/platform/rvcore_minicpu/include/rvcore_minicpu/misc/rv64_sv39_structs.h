#pragma once

#include <stdint.h>
namespace MiniCPU_ns {
// This struct is not mapped to a 32-bit or 64-bit register.
typedef struct {
    uint64_t ppa;      // physical page addr ?
    uint64_t vpa;      // virtual page addr ?
    uint16_t asid;
    uint8_t pagesize; // 0: invalid, 1: 4KB , 2: 2M, 3: 1G
    bool R;        // read
    bool W;        // write
    bool X;        // execute
    bool U;        // user
    bool G;        // global
    bool A;        // access
    bool D;        // dirty
} SV39_TLB_Entry_t;

// Page Table Entry
typedef struct {
    uint64_t V: 1;    // valid
    uint64_t R: 1;    // read
    uint64_t W: 1;    // write
    uint64_t X: 1;    // execute
    uint64_t U: 1;    // user
    uint64_t G: 1;    // global
    uint64_t A: 1;    // access
    uint64_t D: 1;    // dirty
    uint64_t RSW: 2;    // reserved for use by supervisor program
    uint64_t PPN0: 9;
    uint64_t PPN1: 9;
    uint64_t PPN2: 26;
    uint64_t reserved: 7;
    uint64_t PBMT: 2;    // Svpbmt is not implemented, return 0
    uint64_t N: 1;
} SV39_PageTableEntry_t;
static_assert(sizeof(SV39_PageTableEntry_t) == 8, "SV39_PageTableEntry_t shoule be 8 bytes.");
// Note: A and D not implement. So use them as permission bit and raise page fault.

typedef union {
    struct {
        uint64_t page_off: 12;
        uint64_t vpn_0: 9;
        uint64_t vpn_1: 9;
        uint64_t vpn_2: 9;
        uint64_t blank: 25;
    };
    uint64_t val;
} SV39_VAddr_t;

}