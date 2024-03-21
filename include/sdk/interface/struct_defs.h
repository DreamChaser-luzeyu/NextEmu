#pragma once


typedef union RegItemVal {
    struct {
        uint8_t byte_0;         // Low significant byte
        uint8_t byte_1;
        uint8_t byte_2;
        uint8_t byte_3;
        uint8_t byte_4;
        uint8_t byte_5;
        uint8_t byte_6;
        uint8_t byte_7;         // High significant byte
    };
    struct {
        uint32_t u32_val_0;
        uint32_t u32_val_1;
    };
    uint64_t u64_val;
} RegItemVal_t;

typedef struct {
    uint16_t reg_id;                ///< ID or index of the register
    char disp_name[8];              ///< name of the register
    uint8_t size;                   ///< size (in byte) of the register
    RegItemVal_t val;               ///< Value of the register
} RegisterItem_t;

