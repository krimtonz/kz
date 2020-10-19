#ifndef _HB_HEAP_H
#define _HB_HEAP_H

#include <stddef.h>

void *halloc(size_t size);
void hfree(void *ptr);
void *hmemcpy(void *dst, void *src, size_t size);

#endif