#pragma once

#include <stdint.h>

static bool is_little_endian() {
    uint16_t test_val = 0xff00;
    return (((uint8_t*)(&test_val))[0] == 0x00) && (((uint8_t*)(&test_val))[1] == 0xff);
}