#include <stddef.h>
#include "vc.h"

typedef struct {
    char unk_00[4];
    char *ram;
    uint32_t ram_size;
} gClassRAM_t;

char **hb_mem = (char**)0x9000e6ac;

bool kz_ramGetBuffer(gClassRAM_t *ram, char **buf, uint32_t addr, uint32_t *len) {
    addr &= ~0xF0000000;
    if(addr >= 0x8060000){
        // kz hb_mem
        *buf = *hb_mem + (addr & ~0xA8060000);
        return true;
    }

    addr &= ~0xFC000000;

    if (ram->ram_size == 0x0) {
        return false;
    }

    if ((len != NULL) && (ram->ram_size <= addr + *len)){
        if((ram->ram_size - addr) < 0x0) {
            *len = 0x0;
        } else {
            *len = ram->ram_size - addr;
        }
    }

    *buf = ram->ram + (addr & (ram->ram_size - 0x1));
    return true;
}