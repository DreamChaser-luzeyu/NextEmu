#include <stdint.h>

#define NR_CTX               (4ul)
#define NR_CHN               (16ul)
#define CURR_CTX_REG_ADDR    (0x66ccf8ul)
#define MEM_CTRL_REG_BASE    (0x200000000ul)
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

uint64_t get_curr_ctx_id() {
     uint64_t *ptr = (uint64_t*)CURR_CTX_REG_ADDR;
     return *ptr;
}

void switch2ctx(uint64_t ctx_id) {
    uint64_t *ctx_reg_ptr = (uint64_t*)(CURR_CTX_REG_ADDR);
    *ctx_reg_ptr = ctx_id;
}

void append_curr_ctx_waiting_chn(int64_t chn_id) {
    int64_t curr_ctx = get_curr_ctx_id();
    curr_ctx %= NR_CTX;
    ctx_rd_waiting[curr_ctx] |= (1ul << (uint64_t)chn_id);
}

int app_main();

void init() {
    // ----- Only scheduler ctx do the initialization
    if (get_curr_ctx_id() == 0ul) {
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

    return 0;
}
