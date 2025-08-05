#pragma once

#include <stdint.h>

namespace MiniCPU_ns {

typedef enum {
    // Unprivileged Counter/Timers
    csr_cycle = 0xc00,
    csr_time = 0xc01,
    csr_instret = 0xc02,
    // Supervisor Trap Setup
    csr_sstatus = 0x100,
    csr_sie = 0x104,
    csr_stvec = 0x105,
    csr_scounteren = 0x106,
    // Supervisor Trap Handling
    csr_sscratch = 0x140,
    csr_sepc = 0x141,
    csr_scause = 0x142,
    csr_stval = 0x143,
    csr_sip = 0x144,
    // Supervisor Protection and Translation
    csr_satp = 0x180,
    // Machine Information Registers
    csr_mvendorid = 0xf11,
    csr_marchid = 0xf12,
    csr_mimpid = 0xf13,
    csr_mhartid = 0xf14,
    csr_mconfigptr = 0xf15,
    // Machine Trap Setup
    csr_mstatus = 0x300,
    csr_misa = 0x301,
    csr_medeleg = 0x302,
    csr_mideleg = 0x303,
    csr_mie = 0x304,
    csr_mtvec = 0x305,
    csr_mcounteren = 0x306,
    // Machine Trap Handling
    csr_mscratch = 0x340,
    csr_mepc = 0x341,
    csr_mcause = 0x342,
    csr_mtval = 0x343,
    csr_mip = 0x344,
    // Machine Memory Protection
    csr_pmpcfg0 = 0x3a0,
    csr_pmpcfg1 = 0x3a1,
    csr_pmpcfg2 = 0x3a2,
    csr_pmpcfg3 = 0x3a3,
    csr_pmpaddr0 = 0x3b0,
    csr_pmpaddr1 = 0x3b1,
    csr_pmpaddr2 = 0x3b2,
    csr_pmpaddr3 = 0x3b3,
    csr_pmpaddr4 = 0x3b4,
    csr_pmpaddr5 = 0x3b5,
    csr_pmpaddr6 = 0x3b6,
    csr_pmpaddr7 = 0x3b7,
    csr_pmpaddr8 = 0x3b8,
    csr_pmpaddr9 = 0x3b9,
    csr_pmpaddr10 = 0x3ba,
    csr_pmpaddr11 = 0x3bb,
    csr_pmpaddr12 = 0x3bc,
    csr_pmpaddr13 = 0x3bd,
    csr_pmpaddr14 = 0x3be,
    csr_pmpaddr15 = 0x3bf,
    // Machine Counter/Timers
    csr_mcycle = 0xb00,
    csr_minstret = 0xb02,
    csr_tselect = 0x7a0,
    csr_tdata1 = 0x7a1
} RV_CSR_Addr_enum;

typedef union {
    struct {
        uint64_t cycle: 1;
        uint64_t time: 1;
        uint64_t instr_retire: 1;
        uint64_t blank: 61;
    };
    uint64_t val;
} CSR_CounterEN_t;

typedef union {
    struct {
        uint64_t blank0: 1;
        uint64_t s_s_ip: 1; // 1
        uint64_t blank1: 1;
        uint64_t m_s_ip: 1; // 3
        uint64_t blank2: 1;
        uint64_t s_t_ip: 1; // 5
        uint64_t blank3: 1;
        uint64_t m_t_ip: 1; // 7
        uint64_t blank4: 1;
        uint64_t s_e_ip: 1; // 9
        uint64_t blank5: 1;
        uint64_t m_e_ip: 1; // 11
    };
    uint64_t val;
} CSReg_IntPending_t;

typedef union {
    struct {
        uint64_t blank0: 1;
        uint64_t sie: 1;     // supervisor interrupt enable
        uint64_t blank1: 1;
        uint64_t mie: 1;     // machine interrupt enable
        uint64_t blank2: 1;
        uint64_t spie: 1;     // sie prior to trapping
        uint64_t ube: 1;     // u big-endian, zero
        uint64_t mpie: 1;     // mie prior to trapping
        uint64_t spp: 1;     // supervisor previous privilege mode.
        uint64_t vs: 2;     // without vector, zero
        uint64_t mpp: 2;     // machine previous privilege mode.
        uint64_t fs: 2;     // without float, zero
        uint64_t xs: 2;     // without user ext, zero
        uint64_t mprv: 1;     // Modify PRiVilege (Turn on virtual memory and protection for load/store in M-Mode) when mpp is not M-Mode
        // mprv will be used by OpenSBI.
        uint64_t sum: 1;     // permit Supervisor User Memory access
        uint64_t mxr: 1;     // Make eXecutable Readable
        uint64_t tvm: 1;     // Trap Virtual Memory (raise trap when sfence.vma and sinval.vma executing in S-Mode)
        uint64_t tw: 1;     // Timeout Wait for WFI
        uint64_t tsr: 1;     // Trap SRET
        uint64_t blank3: 9;
        uint64_t uxl: 2;     // user xlen
        uint64_t sxl: 2;     // supervisor xlen
        uint64_t sbe: 1;     // s big-endian
        uint64_t mbe: 1;     // m big-endian
        uint64_t blank4: 25;
        uint64_t sd: 1;     // no vs,fs,xs, zero
    };
    uint64_t val;
} CSReg_MStatus_t;

typedef union {
    struct {
        uint64_t blank0: 1;
        uint64_t sie: 1;     // supervisor interrupt enable
        uint64_t blank1: 3;
        uint64_t spie: 1;     // sie prior to trapping
        uint64_t ube: 1;     // u big-endian, zero
        uint64_t blank2: 1;     // mie prior to trapping
        uint64_t spp: 1;     // supervisor previous privilege mode.
        uint64_t vs: 2;     // without vector, zero
        uint64_t blank3: 2;     // machine previous privilege mode.
        uint64_t fs: 2;     // without float, zero
        uint64_t xs: 2;     // without user ext, zero
        uint64_t blank4: 1;
        uint64_t sum: 1;     // permit Supervisor User Memory access
        uint64_t mxr: 1;     // Make eXecutable Readable
        uint64_t blank5: 12;
        uint64_t uxl: 2;     // user xlen
        uint64_t blank6: 29;
        uint64_t sd: 1;     // no vs,fs,xs, zero
    };
    uint64_t val;
} CSReg_SStatus_t;

typedef union {
    struct {
        uint64_t mode: 2;    // 0 - Direct    1 - Vectored   2 - Reserved
        uint64_t base: 62;
    };
    uint64_t val;
} CSReg_TrapVector_t;

typedef union {
    struct {
        uint64_t cause: 63;
        uint64_t interrupt: 1;    // If the trap is caused by an interrupt
    };
    uint64_t val;
} CSReg_Cause_t;

#define rv_ext(x) (1<<(x-'a'))
typedef union {
    struct {
        uint64_t ext: 26;
        uint64_t blank: 36;
        uint64_t mxl: 2;
    };
    uint64_t val;
} CSR_MISA_t;
}