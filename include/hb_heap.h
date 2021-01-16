#ifndef _HB_HEAP_H
#define _HB_HEAP_H

#include <stddef.h>
#include <n64/gbi.h>
#include <mips.h>
#include "z2.h"

#define HB_HEAP_ADDR 0xA8060000

void *halloc(size_t size);
void hfree(void *ptr);
void *hrealloc(void *ptr, size_t size);
void *hmemcpy(void *dst, void *src, size_t size);
size_t hmem_free(void);
void set_hb_seg(Gfx **pgdl);
void restore_hb_seg(Gfx **pgdl);

#define gDisplayListDataHB(pgdl,d)      (*(Gfx**)(pgdl)-=(sizeof(d)+            \
                                       sizeof(Gfx)-1)/sizeof(Gfx),            \
                                       (__typeof__(&(d)))                     \
                                       hmemcpy(*(Gfx**)(pgdl),&(d),sizeof(d)))

#define HB_SEG(addr)                    ((void*)(((uint32_t)addr - HB_HEAP_ADDR) | 0x0B000000))

#endif
