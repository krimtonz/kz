#ifndef LITE
#ifndef _HB_H
#define _HB_H

#include <stdint.h>

typedef struct{
    uint32_t key;
    uint32_t dram_addr;
    uint32_t write_lba;
    uint32_t read_lba;
    uint32_t block_cnt;
    union{
        struct{
            uint32_t            : 22;
            uint32_t reset      : 1;
            uint32_t error      : 4;
            uint32_t initialize : 1;
            uint32_t sdhc       : 1;
            uint32_t inserted   : 1;
            uint32_t busy       : 1;
            uint32_t ready      : 1;
        };
        uint32_t status;
    };
    uint32_t dram_save;
    uint32_t dram_save_len;
    uint32_t dram_restore_key;
    uint32_t timebase_hi;
    uint32_t timebase_lo;
} hb_sd_regs_t;

#define hb_sd_regs  (*(volatile hb_sd_regs_t*) 0xA8050000)

int hb_sd_write(void *buf, uint32_t lba, uint32_t block_cnt);
int hb_sd_read(void *buf, uint32_t lba, uint32_t block_cnt);
int hb_sd_init(void);
int hb_available(void);

#endif
#endif