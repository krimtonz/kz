#ifndef _MEM_H
#define _MEM_H

#include <stdlib.h>
#include <vector/vector.h>

void *sbrk(size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *memcpy(void *dst, const void *src, size_t size);
void *memmove(void *dst, const void *src, size_t size);
void *memset(void *dst, int val, size_t size);
void *calloc(size_t item_cnt, size_t item_size);
void *memalign(size_t align, size_t size);
void get_free_mem(size_t *sizes);

#endif