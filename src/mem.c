/**
 * mem.c provides a basic and tiny dynamic memory allocation.
 * For KZ-VC, memory is allocated from each of the frame buffers, and then via sbrk.
 * For KZ-lite, memory is allocated from the 256k at the end of the actor heap that is reserved for kz, and then via sbrk.
 * For KZ-Full, these functions are not used, and newlib's memory managment is used.
*/
#if defined(WIIVC) || defined(LITE)

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HEAP_ALIGN(s) (((s) + _Alignof(max_align_t) - 1) & ~(_Alignof(max_align_t) - 1))
#define HDR_SIZE (HEAP_ALIGN(sizeof(struct heap_hdr_s)))
#define HEAP_CNT (sizeof(heap_heads) / sizeof(*heap_heads))
#define NOINLINE __attribute__ ((noinline)) 

extern char end;
void *sbrk(int size);

typedef struct heap_hdr_s heap_hdr_t;

struct heap_hdr_s{
    short       free;
    short       heap;
    size_t      size;
    heap_hdr_t *prev;
    heap_hdr_t *next;
};

typedef struct {
    void   *start;
    size_t  size;
} heap_def_t;

static heap_def_t heap_heads[] = {
#ifdef WIIVC
    { (void*)0x80000500, 0x25800 }, // frame buf 1
    { (void*)0x807DA800, 0x25800 }, // frame buf 2
    { (void*)&end, 0x80080000, },
#elif defined(LITE)
    { (void*)0x80700000, 0x80000 }, // actor heap end
    { (void*)&end, 0x80080000 },
#else
    { (void*)&end, 0x80C00000 },
#endif
};

static heap_hdr_t *heap_first_free[HEAP_CNT];
static int heap_initialized = 0;

static void heap_init(void) {
    for(int i = 0; i < HEAP_CNT - 1; i++) {
        heap_hdr_t *hdr = (heap_hdr_t*)heap_heads[i].start;
        hdr->free = 1;
        hdr->size = heap_heads[i].size - HDR_SIZE;
        hdr->prev = NULL;
        hdr->next = NULL;
        hdr->heap = i;
        heap_first_free[i] = hdr;
    }

    heap_hdr_t *hdr = (heap_hdr_t*)sbrk(HDR_SIZE);
    hdr->free = 1;
    hdr->size = heap_heads[HEAP_CNT - 1].size - (uint32_t)hdr - HDR_SIZE;
    hdr->prev = NULL;
    hdr->next = NULL;
    hdr->heap = HEAP_CNT - 1;
    heap_first_free[HEAP_CNT - 1] = hdr;

    heap_initialized = 1;
}

NOINLINE void *malloc(size_t size) {
    if(!heap_initialized) {
        heap_init();
    }

    if(size == 0) {
        return NULL;
    }

    size = HEAP_ALIGN(size);

    heap_hdr_t *blk, *next_free;

    for(int i = 0; i < HEAP_CNT; i++) {
        for(blk = heap_first_free[i]; blk != NULL; blk = blk->next) {
            if(blk->free && blk->size >= size) {
                // using sbrk
                if(blk->heap == HEAP_CNT - 1) {
                    if((void*)blk >= sbrk(0)) {
                        sbrk(size + HDR_SIZE + HDR_SIZE);
                    }
                }

                if(blk->size - size >= HDR_SIZE + HEAP_ALIGN(1)) {
                    next_free = (heap_hdr_t*)((char*)blk + size + HDR_SIZE);
                    next_free->free = 1;
                    next_free->size = blk->size - size - HDR_SIZE;
                    next_free->next = blk->next;
                    next_free->prev = blk;
                    next_free->heap = blk->heap;
                    if(blk->next != NULL) {
                        blk->next->prev = next_free;
                    }
                    blk->next = next_free;
                    blk->size = size;
                }

                blk->free = 0;

                if(blk == heap_first_free[i]) {
                    for(next_free = blk->next; next_free != NULL; next_free = next_free->next) {
                        if(next_free->free) {
                            break;
                        }
                    }
                    heap_first_free[i] = next_free;
                }
                return (char*)blk + HDR_SIZE;
            }
        }
    }

    return NULL;
}

NOINLINE void free(void *ptr) {
    if(ptr == NULL) {
        return;
    }

    heap_hdr_t *blk = (heap_hdr_t*)((char*)ptr - HDR_SIZE);
    if(blk->next != NULL && blk->next->free) {
        blk->size += blk->next->size + HDR_SIZE;
        if(blk->next->next != NULL) {
            blk->next->next->prev = blk;
        }
        blk->next = blk->next->next;
    }

    if(blk->prev != NULL && blk->prev->free) {
        blk->prev->size += blk->size + HDR_SIZE;
        if(blk->next != NULL) {
            blk->prev->next = blk->next;
            blk->next->prev = blk->prev;
        }
        blk->prev->next = blk->next;
        blk = blk->prev;
    }

    blk->free = 1;

    if(blk < heap_first_free[blk->heap]) {
        heap_first_free[blk->heap] = blk;
    }
}

NOINLINE void *realloc(void *ptr, size_t size) {
    if(ptr == NULL) {
        return malloc(size);
    }

    if(size == 0) {
        free(ptr);
        return NULL;
    }

    size = HEAP_ALIGN(size);

    heap_hdr_t *blk, *next_free;

    blk = (heap_hdr_t*)((char*)ptr - HDR_SIZE);
    if(blk->size > size && blk->size - size >= HDR_SIZE + HEAP_ALIGN(1)) {
        next_free = (heap_hdr_t*)((char*)blk + size);
        next_free->free = 1;
        next_free->size = blk->size - size - HDR_SIZE;
        next_free->prev = blk;
        next_free->next = blk->next;
        next_free->heap = blk->heap;
        if(blk->next != NULL) {
            blk->next->prev = next_free;
        }
        blk->next = next_free;
        blk->size = size;
        if(heap_first_free[blk->heap] == blk) {
            for(next_free = blk->next; next_free != NULL; next_free = next_free->next) {
                if(next_free->free) {
                    break;
                }
            }
            heap_first_free[blk->heap] = next_free;
        }

        return ptr;
    }

    void *new_blk = malloc(size);
    if(new_blk == NULL) {
        return NULL;
    }

    memcpy(new_blk, ptr, blk->size);
    free(ptr);

    return new_blk;
}

void *calloc(size_t item_cnt, size_t item_size) {
    void *mem = malloc(item_cnt * item_size);
    if(mem == NULL) {
        return NULL;
    }

    memset(mem, 0, item_cnt * item_size);

    return mem;
}

void *memalign(size_t align, size_t size) {
    // todo: actually memalign
    return malloc(size);
}

void get_free_mem(size_t *sizes) {
    for(int i = 0; i < HEAP_CNT; i++) {
        sizes[i] = 0;
        heap_hdr_t *hdr = (heap_hdr_t*)heap_heads[i].start;
        while(hdr != NULL) {
            if(hdr->free) {
                sizes[i] += hdr->size;
            }
            hdr = hdr->next;
        }
    }
}

#endif
