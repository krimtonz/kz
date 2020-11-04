#ifndef _HB_HEAP_H
#define _HB_HEAP_H

#include <stddef.h>

void *halloc(size_t size);
void hfree(void *ptr);
void *hrealloc(void *ptr, size_t size);
void *hmemcpy(void *dst, void *src, size_t size);

#define gDisplayListDataHB(pgdl,d)      (*(Gfx**)(pgdl)-=(sizeof(d)+            \
                                       sizeof(Gfx)-1)/sizeof(Gfx),            \
                                       (__typeof__(&(d)))                     \
                                       hmemcpy(*(Gfx**)(pgdl),&(d),sizeof(d)))

#define HB_SEG(addr)                    ((void*)(((uint32_t)addr - 0xA8060000) | 0x0B000000))

#endif