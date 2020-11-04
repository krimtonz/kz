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

static heap_hdr_t *find_chunk(size_t size) {
    size += HDR_SIZE;
    for(int i = 0; i < HEAP_CNT; i++) {
        heap_hdr_t *hdr = heap_first_free[i];
        while(hdr != NULL) {
            if(hdr->free && hdr->size >= size) {
                hdr->heap = i;
                return hdr;
            }
            hdr = hdr->next;
        }
    }
    return NULL;
}

void *malloc(size_t size) {
    if(!heap_initialized) {
        heap_init();
    }

    size = HEAP_ALIGN(size);
    heap_hdr_t *blk = find_chunk(size);

    if(blk == NULL) {
        return NULL;
    }

    // sbrk enough memory for the block, header, and the next header.
    if(blk->heap == (HEAP_CNT - 1) && (void*)blk >= sbrk(0)) {
        sbrk(size + HDR_SIZE + HDR_SIZE);
    }

    heap_hdr_t *next_free;
    if((int)(blk->size - size) >= (int)(HDR_SIZE + HEAP_ALIGN(1))) {
        next_free = (heap_hdr_t*)((char*)blk + HDR_SIZE + size);
        next_free->free = 1;
        next_free->heap = blk->heap;
        next_free->size = blk->size - size - HDR_SIZE;
        next_free->prev = blk;
        if(blk->next != NULL) {
            blk->next->prev = next_free;
        }
        next_free->next = blk->next;
        blk->next = next_free;
        blk->size = size;
    }

    blk->free = 0;
    if(blk == heap_first_free[blk->heap]) {
        next_free = blk->next;
        while(next_free != NULL) {
            if(next_free->free) {
                heap_first_free[blk->heap] = next_free;
                break;
            }
            next_free = next_free->next;
        }
    }

    return (char*)blk + HDR_SIZE;
}

void free(void *ptr) {
    if(ptr == NULL) {
        return;
    }

    heap_hdr_t *hdr = (heap_hdr_t*)((char*)ptr - HDR_SIZE);
    if(hdr->next != NULL && hdr->next->free) {
        hdr->size = ((char*)hdr->next + hdr->next->size) - (char*)hdr;
        if(hdr->next->next != NULL) {
            hdr->next->next->prev = hdr;
        }
        hdr->next = hdr->next->next;
    }

    if(hdr->prev != NULL && hdr->prev->free) {
        hdr->prev->size = ((char*)hdr + hdr->size) - (char*)hdr->prev;
        hdr->prev->next = hdr->next;
        if(hdr->next != NULL) {
            hdr->next->prev = hdr->prev;
        }
        hdr = hdr->prev;
    } else {
        hdr->free = 1;
    }

    if(heap_first_free[hdr->heap] > hdr) {
        heap_first_free[hdr->heap] = hdr;
    }
}

void *realloc(void *ptr, size_t size) {
    if(size == 0) {
        free(ptr);
        return NULL;
    }

    if(ptr == NULL) {
        return malloc(size);
    }

    heap_hdr_t *hdr = (heap_hdr_t*)((char*)ptr - HDR_SIZE);
    if(hdr->free) {
        return NULL;
    }

    size = HEAP_ALIGN(size);

    // reallocing the same size.
    if(hdr->size == size) {
        return ptr;
    }

    // requesting a smaller size, and we can fragment the existing block.
    if((int)(hdr->size - size) >= (int)(HDR_SIZE + HEAP_ALIGN(1))) {
        heap_hdr_t *free = (heap_hdr_t*)((char*)hdr + HDR_SIZE + size);
        free->free = 1;
        free->size = hdr->size - size - HDR_SIZE;
        free->next = hdr->next;
        free->next->prev = free;
        free->prev = hdr;
        hdr->next = free;
        if(heap_first_free[free->heap] > free) {
            heap_first_free[free->heap] = free;
        }
        return ptr;
    }

    void *new_blk = malloc(size);
    if(new_blk == NULL) {
        return NULL;
    }

    memcpy(new_blk, ptr, hdr->size);

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