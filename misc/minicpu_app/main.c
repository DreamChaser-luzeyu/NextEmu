#include <stdint.h>

int app_main();

void init() {
    // --- Clear .bss section
    extern uint32_t _sbss;
    extern uint32_t _ebss;
    uint32_t *bss_ptr = &_sbss;
    while(bss_ptr < &_ebss) {
        *(bss_ptr++) = 0u;
    }

    app_main();
}

int app_main() {

    return 0;
}
