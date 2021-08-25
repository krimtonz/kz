#include <stdint.h>
#include <mips.h>
#include "hb_heap.h"

#define HB_HEAP_SIZE 0x00800000
#define HB_ALIGN(s) (((s) + _Alignof(max_align_t) - 1) & ~(_Alignof(max_align_t) - 1))
#define HDR_SIZE (HB_ALIGN(sizeof(struct hb_heap_hdr_s)))
#define NOINLINE __attribute__ ((noinline))

typedef struct hb_heap_hdr_s{
    int free;
    size_t size;
    struct hb_heap_hdr_s *prev;
    struct hb_heap_hdr_s *next;
} hb_heap_hdr_t;

static char *hb_heap = (char*)HB_HEAP_ADDR;
static hb_heap_hdr_t *first_free_block;
static _Bool hb_heap_initalized = 0;

static void hb_heap_init(){
    first_free_block = (hb_heap_hdr_t*)hb_heap;
    first_free_block->free = 1;
    first_free_block->size = HB_HEAP_SIZE - HDR_SIZE;
    first_free_block->prev = NULL;
    first_free_block->next = NULL;
    hb_heap_initalized = 1;
}

NOINLINE void *halloc(size_t size) {
    if(!hb_heap_initalized) {
        hb_heap_init();
    }

    if(size == 0) {
        return NULL;
    }

    size = HB_ALIGN(size);

    hb_heap_hdr_t *blk, *next_free;

    for(blk = first_free_block; blk != NULL; blk = blk->next) {
        if(blk->free && blk->size >= size) {
            // Enough space to allocate at least 1 aligned unit.
            if(blk->size - size >= HDR_SIZE + HB_ALIGN(1)) {
                next_free = (hb_heap_hdr_t*)((char*)blk + size + HDR_SIZE);
                next_free->free = 1;
                next_free->size = blk->size - size - HDR_SIZE;
                next_free->next = blk->next;
                next_free->prev = blk;
                if(blk->next != NULL) {
                    blk->next->prev = next_free;
                }
                blk->next = next_free;
                blk->size = size;
            }

            blk->free = 0;

            if(blk == first_free_block) {
                for(next_free = blk->next; next_free != NULL; next_free = next_free->next) {
                    if(next_free->free) {
                        break;
                    }
                }
                first_free_block = next_free;
            }

            return (void*)((char*)blk + HDR_SIZE);
        }
    }

    // couldn't allocate a block.
    return NULL;
}

NOINLINE void hfree(void *ptr) {
    if(ptr == NULL) {
        return;
    }

    hb_heap_hdr_t *blk = (hb_heap_hdr_t*)((char*)ptr - HDR_SIZE);
    if(blk->next != NULL && blk->next->free) {
        // combine the blocks into a single larger block.
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

    if(blk < first_free_block) {
        first_free_block = blk;
    }
}

NOINLINE void *hrealloc(void *ptr, size_t size) {
    if(ptr == NULL) {
        return halloc(size);
    }

    if(size == 0) {
        hfree(ptr);
        return NULL;
    }

    size = HB_ALIGN(size);

    hb_heap_hdr_t *blk, *next_free;

    blk = (hb_heap_hdr_t*)((char*)ptr - HDR_SIZE);
    if(blk->size > size && blk->size - size >= HDR_SIZE + HB_ALIGN(1)) {
        next_free = (hb_heap_hdr_t*)((char*)blk + size + HDR_SIZE);
        next_free->free = 1;
        next_free->size = blk->size - size - HDR_SIZE;
        next_free->prev = blk;
        next_free->next = blk->next;
        if(blk->next != NULL) {
            blk->next->prev = next_free;
        }
        blk->next = next_free;
        blk->size = size;
        if(first_free_block == blk) {
            for(next_free = blk->next; next_free != NULL; next_free = next_free->next) {
                if(next_free->free) {
                    break;
                }
            }
            first_free_block = next_free;
        }

        return ptr;
    }

    void *new_blk = halloc(size);
    if(new_blk == NULL) {
        return NULL;
    }

    hmemcpy(new_blk, ptr, blk->size);
    hfree(ptr);

    return new_blk;
}

NOINLINE size_t hmem_free(void) {
    size_t free = 0;
    hb_heap_hdr_t *hdr = first_free_block;
    while(hdr != NULL) {
        if(hdr->free) {
            free += hdr->size;
        }
        hdr = hdr->next;
    }
    return free;
}

NOINLINE void *hmemcpy(void *dst, void *src, size_t size)
{
    char *d = (char*)MIPS_KSEG0_TO_KSEG1(dst);
    const char *p = (const char*)MIPS_KSEG0_TO_KSEG1(src);

    for(int i = 0; i < size / 4; i++, p += 4, d += 4, size -= 4){
        *((uint32_t*)d) = *((uint32_t*)p);
    }

    while(size--){
        *d++ = *p++;
    }

    return dst;
}

inline void set_hb_seg(Gfx **pgdl) {
    gSPSegment((*pgdl)++, 0xB, HB_HEAP_ADDR);
}

inline void restore_hb_seg(Gfx **pgdl) {
    gSPSegment((*pgdl)++, 0xB, MIPS_PHYS_TO_KSEG0(z2_segment.segments[0xB]));
}
