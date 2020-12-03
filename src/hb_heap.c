#include <stdint.h>
#include <mips.h>
#include "hb_heap.h"

#define HB_HEAP_ADDR 0xA8060000
#define HB_HEAP_SIZE 0x00C00000 // 12 MB
#define HB_ALIGN(s) (((s) + _Alignof(max_align_t) - 1) & ~(_Alignof(max_align_t) - 1))
#define HDR_SIZE (HB_ALIGN(sizeof(struct __hb_heap_hdr_s)))

typedef struct __hb_heap_hdr_s{
    int free;
    int size;
    struct __hb_heap_hdr_s *prev;
    struct __hb_heap_hdr_s *next;
} __hb_heap_hdr_t;

static char *hb_heap = (char*)HB_HEAP_ADDR;
static __hb_heap_hdr_t *first_free_block;
static _Bool hb_heap_initalized = 0;

static void hb_heap_init(){
    first_free_block = (__hb_heap_hdr_t*)hb_heap;
    first_free_block->free = 1;
    first_free_block->size = HB_HEAP_SIZE - HDR_SIZE;
    first_free_block->prev = NULL;
    first_free_block->next = NULL;
    hb_heap_initalized = 1;
}

void *halloc(size_t size){
    if(!hb_heap_initalized){
        hb_heap_init();
    }
    __hb_heap_hdr_t *blk_ptr = first_free_block;
    __hb_heap_hdr_t *next_free;

    size = HB_ALIGN(size);

    while(blk_ptr != NULL) {
        if(blk_ptr->free && blk_ptr->size >= size) {
            // Can this block be fragmented? 
            if(blk_ptr->size - size >= HDR_SIZE + HB_ALIGN(1)) {
                next_free = (__hb_heap_hdr_t*)((char*)blk_ptr + HDR_SIZE + size);
                next_free->free = 1;
                next_free->size = blk_ptr->size - size - HDR_SIZE;
                next_free->prev = blk_ptr;
                
                if(blk_ptr->next != NULL) {
                    blk_ptr->next->prev = next_free;
                }

                next_free->next = blk_ptr->next;
                blk_ptr->next = next_free;
                blk_ptr->size = size;
            }

            blk_ptr->free = 0;

            if(blk_ptr == first_free_block) {
                next_free = blk_ptr->next;

                while(next_free != NULL) {
                    if(next_free->free){
                        first_free_block = next_free;
                        break;
                    }
                    next_free = next_free->next;
                }
            }

            return (char*)blk_ptr + HDR_SIZE;
        }

        blk_ptr = blk_ptr->next;
    }

    return NULL;
}

void hfree(void *ptr)
{
    __hb_heap_hdr_t *blk = (__hb_heap_hdr_t*)((char*)ptr - HDR_SIZE);
    if(blk->next != NULL && blk->next->free){
        blk->size += blk->next->size + HDR_SIZE;
        if(blk->next->next != NULL){
            blk->next->next->prev = blk;
        }
        blk->next = blk->next->next;
    }

    if(blk->prev != NULL && blk->prev->free){
        blk->prev->size += blk->size + HDR_SIZE;
        blk->prev->next = blk->next;
        if(blk->next != NULL){
            blk->next->prev = blk->prev;
        }
        blk = blk->prev;
    } else {
        blk->free = 1;
    }

    if(first_free_block > blk){
        first_free_block = blk;
    }
}

__attribute__ ((noinline)) 
void *hrealloc(void *ptr, size_t size)
{
    __hb_heap_hdr_t *blk = (__hb_heap_hdr_t*)((char*)ptr - HDR_SIZE);

    if(blk->free)
    {
        return NULL;
    }

    size = HB_ALIGN(size);

    if(blk->size == size)
    {
        return ptr;
    }

    if(blk->size - size >= HDR_SIZE + HB_ALIGN(1)) // we have enough room to fragment this block.
    {
        __hb_heap_hdr_t *free = (__hb_heap_hdr_t*)((char*)blk + HDR_SIZE + size);
        free->free = 1;
        free->size = blk->size - size - HDR_SIZE;
        free->prev = blk;

        if(blk->next != NULL) {
            blk->next->prev = free;
        }

        free->next = blk->next;
        free->next->prev = free;
        blk->next = free;
        blk->size = size;

        if(first_free_block > free) 
        {
            first_free_block = free;
        }
        return ptr;
    }

    // We're expanding this block, so find a new block and free the old block.
    void *new_blk = halloc(size);
    if(new_blk == NULL)
    {
        return NULL;
    }

    hmemcpy(new_blk, ptr, blk->size);

    hfree(ptr);

    return new_blk;
}

__attribute__ ((noinline)) 
size_t hmem_free(void) {
    size_t free = 0;
    __hb_heap_hdr_t *hdr = first_free_block;
    while(hdr != NULL) {
        if(hdr->free) {
            free += hdr->size;
        }
        hdr = hdr->next;
    }
    return free;
}

void *hmemcpy(void *dst, void *src, size_t size)
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
