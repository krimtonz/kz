#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "macro_store.h"

static const char macro_magic[4] = {'k', 'z', 'm', 'c'};

void macro_store_init(macro_store_t *store){
    memset(store, 0, sizeof(*store));
}

void macro_store_clear(macro_store_t *store){
    for(uint32_t i = 0; i < store->page_cnt; i++) {
        free(store->pages[i]);
    }
    free(store->pages);
    free(store->seeds);
    macro_store_init(store);
}

macro_input_t *macro_store_get(macro_store_t *store, uint32_t frame){
    if(frame >= store->length) {
        return NULL;
    }
    return &store->pages[frame / MACRO_PAGE_FRAMES][frame % MACRO_PAGE_FRAMES];
}

int macro_store_set(macro_store_t *store, uint32_t frame, const macro_input_t *input){
    if(frame > store->length) {
        return 0;
    }

    uint32_t page = frame / MACRO_PAGE_FRAMES;
    if(page >= store->page_cnt) {
        macro_input_t **pages = realloc(store->pages, sizeof(*pages) * (page + 1));
        if(pages == NULL) {
            return 0;
        }
        store->pages = pages;
        pages[page] = malloc(sizeof(macro_input_t) * MACRO_PAGE_FRAMES);
        if(pages[page] == NULL) {
            return 0;
        }
        store->page_cnt = page + 1;
    }

    store->pages[page][frame % MACRO_PAGE_FRAMES] = *input;
    if(frame == store->length) {
        store->length++;
    }
    return 1;
}

void macro_store_trim(macro_store_t *store, uint32_t length){
    if(length >= store->length) {
        return;
    }
    store->length = length;

    uint32_t pages = (length + MACRO_PAGE_FRAMES - 1) / MACRO_PAGE_FRAMES;
    for(uint32_t i = pages; i < store->page_cnt; i++) {
        free(store->pages[i]);
    }
    store->page_cnt = pages;
    if(pages == 0) {
        free(store->pages);
        store->pages = NULL;
    }

    uint32_t keep = 0;
    while(keep < store->seed_cnt && store->seeds[keep].frame < length) {
        keep++;
    }
    store->seed_cnt = keep;
}

/* seeds are kept sorted by frame */
static uint32_t seed_find(macro_store_t *store, uint32_t frame){
    uint32_t i = 0;
    while(i < store->seed_cnt && store->seeds[i].frame < frame) {
        i++;
    }
    return i;
}

int macro_store_set_seed(macro_store_t *store, uint32_t frame, uint32_t seed){
    uint32_t i = seed_find(store, frame);
    if(i < store->seed_cnt && store->seeds[i].frame == frame) {
        store->seeds[i].seed = seed;
        return 1;
    }

    if(store->seed_cnt == store->seed_cap) {
        uint32_t cap = store->seed_cap ? store->seed_cap * 2 : 8;
        macro_seed_t *seeds = realloc(store->seeds, sizeof(*seeds) * cap);
        if(seeds == NULL) {
            return 0;
        }
        store->seeds = seeds;
        store->seed_cap = cap;
    }

    memmove(&store->seeds[i + 1], &store->seeds[i], sizeof(*store->seeds) * (store->seed_cnt - i));
    store->seeds[i].frame = frame;
    store->seeds[i].seed = seed;
    store->seed_cnt++;
    return 1;
}

int macro_store_get_seed(macro_store_t *store, uint32_t frame, uint32_t *seed){
    uint32_t i = seed_find(store, frame);
    if(i < store->seed_cnt && store->seeds[i].frame == frame) {
        *seed = store->seeds[i].seed;
        return 1;
    }
    return 0;
}

size_t macro_store_size(macro_store_t *store){
    return sizeof(macro_file_hdr_t) + sizeof(macro_input_t) * store->length +
           sizeof(macro_seed_t) * store->seed_cnt;
}

int macro_store_write(macro_store_t *store, uint16_t z2_version,
                      int (*write_fn)(void *ctx, const void *buf, size_t len), void *ctx){
    macro_file_hdr_t hdr;
    memcpy(hdr.magic, macro_magic, sizeof(hdr.magic));
    hdr.version = MACRO_FILE_VERSION;
    hdr.z2_version = z2_version;
    hdr.length = store->length;
    hdr.seed_cnt = store->seed_cnt;
    if(!write_fn(ctx, &hdr, sizeof(hdr))) {
        return 0;
    }

    for(uint32_t frame = 0; frame < store->length; frame += MACRO_PAGE_FRAMES) {
        uint32_t cnt = store->length - frame;
        if(cnt > MACRO_PAGE_FRAMES) {
            cnt = MACRO_PAGE_FRAMES;
        }
        if(!write_fn(ctx, store->pages[frame / MACRO_PAGE_FRAMES], sizeof(macro_input_t) * cnt)) {
            return 0;
        }
    }

    return store->seed_cnt == 0 || write_fn(ctx, store->seeds, sizeof(macro_seed_t) * store->seed_cnt);
}

int macro_store_read(macro_store_t *store, uint16_t z2_version, size_t size,
                     int (*read_fn)(void *ctx, void *buf, size_t len), void *ctx){
    macro_store_clear(store);

    macro_file_hdr_t hdr;
    if(size < sizeof(hdr) || !read_fn(ctx, &hdr, sizeof(hdr))) {
        return MACRO_READ_IO;
    }
    /* the limits keep the size calculation from overflowing on a corrupt file */
    if(memcmp(hdr.magic, macro_magic, sizeof(hdr.magic)) != 0 ||
       hdr.length > 0x1000000 || hdr.seed_cnt > 0x100000 ||
       size != sizeof(hdr) + sizeof(macro_input_t) * (size_t)hdr.length + sizeof(macro_seed_t) * (size_t)hdr.seed_cnt) {
        return MACRO_READ_FORMAT;
    }
    if(hdr.version != MACRO_FILE_VERSION || hdr.z2_version != z2_version) {
        return MACRO_READ_VERSION;
    }

    int error = MACRO_READ_MEMORY;
    uint32_t pages = (hdr.length + MACRO_PAGE_FRAMES - 1) / MACRO_PAGE_FRAMES;
    if(pages > 0) {
        store->pages = calloc(pages, sizeof(*store->pages));
        if(store->pages == NULL) {
            goto fail;
        }
        for(uint32_t i = 0; i < pages; i++) {
            store->pages[i] = malloc(sizeof(macro_input_t) * MACRO_PAGE_FRAMES);
            store->page_cnt = i + (store->pages[i] != NULL);
            if(store->pages[i] == NULL) {
                goto fail;
            }
        }
    }
    if(hdr.seed_cnt > 0) {
        store->seeds = malloc(sizeof(macro_seed_t) * hdr.seed_cnt);
        if(store->seeds == NULL) {
            goto fail;
        }
        store->seed_cap = hdr.seed_cnt;
    }

    error = MACRO_READ_IO;
    for(uint32_t frame = 0; frame < hdr.length; frame += MACRO_PAGE_FRAMES) {
        uint32_t cnt = hdr.length - frame;
        if(cnt > MACRO_PAGE_FRAMES) {
            cnt = MACRO_PAGE_FRAMES;
        }
        if(!read_fn(ctx, store->pages[frame / MACRO_PAGE_FRAMES], sizeof(macro_input_t) * cnt)) {
            goto fail;
        }
    }
    if(hdr.seed_cnt > 0 && !read_fn(ctx, store->seeds, sizeof(macro_seed_t) * hdr.seed_cnt)) {
        goto fail;
    }

    store->length = hdr.length;
    store->seed_cnt = hdr.seed_cnt;
    return 0;

fail:
    macro_store_clear(store);
    return error;
}
