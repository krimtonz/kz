#include <stddef.h>
#include "vc.h"

typedef struct {
    char unk_00[4];
    char *ram;
    uint32_t ram_size;
} gClassRAM_t;

typedef struct {
    char *heap_ptr;
    size_t heap_size;
} class_hb_heap_t;

extern class_hb_heap_t *hb_heap_obj;

bool kz_ramGetBuffer(gClassRAM_t *ram, void **buffer, int addr, int *len) {
    size_t dram_size = ram->ram_size;
    addr &= ~0xF0000000;

    if(addr >= 0x8060000 && addr <= 0x8C60000) {
        *buffer = hb_heap_obj->heap_ptr + (addr - 0x8060000);
        return true;
    }

    addr &= 0x3FFFFFF;

    if(dram_size == 0) {
        return false;
    }

    if(len != NULL && addr + *len >= dram_size) {
        *len = dram_size - addr;
        if(*len < 0) {
            *len = 0;
        }
    }

    *buffer = ram->ram + addr;
    return true;
}
