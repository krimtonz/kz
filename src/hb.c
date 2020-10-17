#ifndef LITE
#include <mips.h>
#include "hb.h"

int hb_available(void){
    return hb_sd_regs.key == 0x1234;
}

int hb_sd_init(void){
    if(!hb_available()){
        return -1;
    }
    hb_sd_regs.initialize = 1;
    while(hb_sd_regs.busy) ;
    if(hb_sd_regs.inserted && hb_sd_regs.ready){
        return 0;
    }
    return -1;
}

int hb_sd_read(void *buf, uint32_t lba, uint32_t block_cnt){
    if(!hb_available()){
        return -1;
    }
    hb_sd_regs.dram_addr = MIPS_KSEG0_TO_PHYS(buf);
    hb_sd_regs.block_cnt = block_cnt;
    hb_sd_regs.read_lba = lba;
    while(hb_sd_regs.busy) ;
    if(hb_sd_regs.error){
        return -1;
    }
    return 0;
}

int hb_sd_write(void *buf, uint32_t lba, uint32_t block_cnt){
    if(!hb_available()){
        return -1;
    }
    hb_sd_regs.dram_addr = MIPS_KSEG0_TO_PHYS(buf);
    hb_sd_regs.block_cnt = block_cnt;
    hb_sd_regs.write_lba = lba;
    while(hb_sd_regs.busy) ;
    if(hb_sd_regs.error){
        return -1;
    }
    return 0;
}

int hb_mem_write(void *buf, uint32_t hb_addr, uint32_t len){
    if(!hb_available()){
        return -1;
    }
    hb_sd_regs.hb_addr = hb_addr;
    hb_sd_regs.n64_addr = MIPS_KSEG0_TO_PHYS(buf);
    hb_sd_regs.hb_wr_len = len;
}

int hb_mem_read(void *buf, uint32_t hb_addr, uint32_t len){
    if(!hb_available()){
        return -1;
    }
    hb_sd_regs.hb_addr = hb_addr;
    hb_sd_regs.n64_addr = MIPS_KSEG0_TO_PHYS(buf);
    hb_sd_regs.hb_rd_len = len;
}

int hb_reset(void){
    if(!hb_available()){
        return -1;
    }
    hb_sd_regs.reset = 1;
    while(hb_sd_regs.busy);
    if(hb_sd_regs.error){
        return -1;
    }
    return 0;
}

static char *hb_heap = (char*)0xA8060000;

void *hb_alloc(int size) {
    void *ret = hb_heap;
    hb_heap += size;
    return ret;
}

#endif