#include <stddef.h>
#include <stdint.h>

#define NR_CTX               (4ul)
#define SCHEDULER_CTX_ID     (0ul)
#define NR_CHN               (16ul)
#define CURR_CTX_REG_ADDR    (0x66ccf8ul)
#define MEM_CTRL_REG_BASE    (0x200000000ul)
#define PUTC_MMIO_ADDR       (0x66cd00ul)
#pragma pack(8)
typedef struct mem_controller_reg {
    uint64_t base_reserved;
    uint64_t reserved_0;
    uint64_t rd_pending_chn_bits;
    uint64_t wr_pending_chn_bits;
    uint8_t  reserved_1[224];
    struct {
        uint64_t src_addr;
        uint64_t dst_addr;
    } rd_chn_descs[NR_CHN];  // RAM -> MiniCPU
    struct {
        uint64_t src_addr;
        uint64_t dst_addr;
    } wr_chn_descs[NR_CHN];  // MiniCPU -> RAM
} mem_controller_reg_t;
#pragma pack()
#define REG_PTR              ((mem_controller_reg_t*)(MEM_CTRL_REG_BASE))

uint64_t ctx_rd_waiting[4];         // bit mask of each context's read waiting chn
uint64_t ctx_wr_waiting[4];         // bit mask of each context's write waiting chn

// ----- SDK API

int putchar(int ch) {
    int32_t *ptr = (int32_t*)PUTC_MMIO_ADDR;
    *ptr = ch;
    return *ptr;
}

void print_str(const char *str) {
    const char *ptr = str;
    while (*ptr != '\0') {
        putchar(*ptr);
        ptr++;
    }
}

void simple_assert(bool cond) {
    if (!cond) {
        print_str("ASSERT FALSE! \n\n");
        while (true);
    }
}

uint64_t get_curr_ctx_id() {
     uint64_t *ptr = (uint64_t*)CURR_CTX_REG_ADDR;
     return *ptr;
}

void switch2ctx(uint64_t ctx_id) {
    uint64_t *ctx_reg_ptr = (uint64_t*)(CURR_CTX_REG_ADDR);
    *ctx_reg_ptr = ctx_id;
}

void yield() {
    // if (get_curr_ctx_id() != 0) {
        switch2ctx(SCHEDULER_CTX_ID);
    // }
}

void append_curr_ctx_rd_waiting_chn(int64_t chn_id) {
    uint64_t curr_ctx = get_curr_ctx_id();
    curr_ctx %= NR_CTX;
    ctx_rd_waiting[curr_ctx] |= (1ul << (uint64_t)chn_id);
}

void append_curr_ctx_wr_waiting_chn(int64_t chn_id) {
    uint64_t curr_ctx = get_curr_ctx_id();
    curr_ctx %= NR_CTX;
    ctx_wr_waiting[curr_ctx] |= (1ul << (uint64_t)chn_id);
}

/**
 * RAM --> MiniCPU in-chip
 * @param src_addr
 * @param dst_addr
 */
int async_rd_dword(uint64_t src_addr, uint64_t dst_addr) {
    // 1-- Find free channel
    int free_chn = 0;
    for (free_chn = 0; free_chn < NR_CHN; free_chn++) {
        uint64_t mask = 1ul << free_chn;
        if ((mask & (REG_PTR->rd_pending_chn_bits)) == 0ul) {
            break;
        }
    }
    if (free_chn >= NR_CHN) {
        return -1;  // No free channel now...
    }
    // 2-- Set current ctx waiting bit
    append_curr_ctx_rd_waiting_chn(free_chn);
    // 3-- Set src and dst addr
    REG_PTR->rd_chn_descs[free_chn].src_addr = src_addr;
    REG_PTR->rd_chn_descs[free_chn].dst_addr = dst_addr;
    // 4-- Set chn bit
    (REG_PTR->rd_pending_chn_bits) |= (1ul << free_chn);
    return free_chn;
}

/**
 * MiniCPU in-chip --> RAM
 * @param src_addr
 * @param dst_addr
 */
int async_wr_dword(uint64_t src_addr, uint64_t dst_addr) {
    // 1-- Find free channel
    int free_chn = 0;
    for (free_chn = 0; free_chn < NR_CHN; free_chn++) {
        uint64_t mask = 1ul << free_chn;
        if ((mask & (REG_PTR->wr_pending_chn_bits)) == 0ul) {
            break;
        }
    }
    if (free_chn >= NR_CHN) {
        return -1;  // No free channel now...
    }
    // 2-- Set current ctx waiting bit
    append_curr_ctx_rd_waiting_chn(free_chn);
    // 3-- Set src and dst addr
    REG_PTR->rd_chn_descs[free_chn].src_addr = src_addr;
    REG_PTR->rd_chn_descs[free_chn].dst_addr = dst_addr;
    // 4-- Set chn bit
    (REG_PTR->rd_pending_chn_bits) |= (1ul << free_chn);
    return free_chn;
}

int app_main();

void init() {
    // ----- Only scheduler ctx do the initialization
    if (get_curr_ctx_id() == SCHEDULER_CTX_ID) {
        // --- Clear .bss section
        extern uint32_t _sbss;
        extern uint32_t _ebss;
        uint32_t *bss_ptr = &_sbss;
        while(bss_ptr < &_ebss) {
            *(bss_ptr++) = 0u;
        }
        // --- Do the schedule here
        uint64_t chk_ctx = 1;
        while (true) {
            if (((ctx_rd_waiting[chk_ctx] & (REG_PTR->rd_pending_chn_bits)) == 0ul)
             && ((ctx_wr_waiting[chk_ctx] & (REG_PTR->wr_pending_chn_bits)) == 0ul)) {
                switch2ctx(chk_ctx);
            }
            chk_ctx += 1;
            if(chk_ctx == NR_CTX) {
                chk_ctx = 1;
            }
        }
    }

    // ----- only app ctx can reach here
    app_main();
}

int app_main() {
    print_str("app_main: Hello, world! \n");

    uint64_t val;
    int chn;
    do { chn = async_rd_dword(0x80000000, (uint64_t)&val); } while (chn < 0);
    simple_assert(((1ul << chn) & (REG_PTR->rd_pending_chn_bits)));
    yield();

    print_str("app_main: async read success! \n");

    yield();

    return 0;
}
