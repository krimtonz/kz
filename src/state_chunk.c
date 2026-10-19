#ifdef LITE
#include <stdint.h>
#include <string.h>
#include "mem.h"
#include "state.h"
#include "state_chunk.h"

#define CHUNK_ALIGN(s)  (((s) + 0xF) & ~(size_t)0xF)

int chunk_alloc_init(chunk_alloc_t *alloc){
    size_t total;
    mem_largest_free(&total);
    alloc->limit = total > STATE_MEM_RESERVE ? total - STATE_MEM_RESERVE : 0;
    alloc->allocated = 0;
    alloc->first = NULL;
    alloc->last = NULL;
    return alloc->limit >= STATE_MIN_ALLOC;
}

void chunks_free(state_chunk_t *chunk){
    while(chunk != NULL) {
        state_chunk_t *next = chunk->next;
        free(chunk);
        chunk = next;
    }
}

state_chunk_t *chunk_new(chunk_alloc_t *alloc, size_t want){
    size_t avail = mem_largest_free(NULL);
    size_t budget = alloc->limit - alloc->allocated;
    if(avail > budget) {
        avail = budget;
    }
    avail &= ~(size_t)0xF;

    size_t size = avail;
    size_t need = 0;
    if(want != 0) {
        need = CHUNK_ALIGN(sizeof(state_chunk_t) + want);
        if(size > need) {
            size = need;
        }
    }

    /* a small chunk is only worth it when it finishes the data */
    if(size <= sizeof(state_chunk_t) ||
       (size < sizeof(state_chunk_t) + STATE_CHUNK_MIN && size < need)) {
        return NULL;
    }

    state_chunk_t *chunk = malloc(size);
    if(chunk == NULL) {
        return NULL;
    }
    chunk->next = NULL;
    chunk->used = 0;
    chunk->cap = size - sizeof(*chunk);
    alloc->allocated += size;
    if(alloc->last != NULL) {
        alloc->last->next = chunk;
    } else {
        alloc->first = chunk;
    }
    alloc->last = chunk;
    return chunk;
}

void chunk_trim(chunk_alloc_t *alloc, state_chunk_t *chunk){
    size_t unused = chunk->cap - chunk->used;
    mem_shrink(chunk, sizeof(*chunk) + chunk->used);
    chunk->cap = chunk->used;
    alloc->allocated -= unused;
}

static int out_next(lz_stream_t *stream){
    chunk_alloc_t *alloc = stream->data;
    alloc->last->used = alloc->last->cap;
    state_chunk_t *chunk = chunk_new(alloc, 0);
    if(chunk == NULL) {
        return 0;
    }
    stream->p = chunk->data;
    stream->end = chunk->data + chunk->cap;
    return 1;
}

void chunk_out_open(lz_stream_t *stream, chunk_alloc_t *alloc, size_t skip){
    stream->p = alloc->first->data + skip;
    stream->end = alloc->first->data + alloc->first->cap;
    stream->next = out_next;
    stream->data = alloc;
}

size_t chunk_out_close(lz_stream_t *stream, chunk_alloc_t *alloc){
    alloc->last->used = stream->p - alloc->last->data;
    chunk_trim(alloc, alloc->last);

    size_t size = 0;
    for(state_chunk_t *chunk = alloc->first; chunk != NULL; chunk = chunk->next) {
        size += chunk->used;
    }
    return size;
}

static int in_next(lz_stream_t *stream){
    state_chunk_t *chunk = ((state_chunk_t*)stream->data)->next;
    if(chunk == NULL) {
        return 0;
    }
    stream->data = chunk;
    stream->p = chunk->data;
    stream->end = chunk->data + chunk->used;
    return 1;
}

void state_stream_open(lz_stream_t *stream, kz_state_hdr_t *state, void *pos){
    state_chunk_t *chunk = STATE_CHUNK(state);
    stream->p = pos;
    stream->end = chunk->data + chunk->used;
    stream->next = in_next;
    stream->data = chunk;
}

void state_free(kz_state_hdr_t *state){
    if(state != NULL) {
        chunks_free(STATE_CHUNK(state));
    }
}

int state_write_file(kz_state_hdr_t *state, int (*write_fn)(void *ctx, const void *buf, size_t len), void *ctx){
    for(state_chunk_t *chunk = STATE_CHUNK(state); chunk != NULL; chunk = chunk->next) {
        if(!write_fn(ctx, chunk->data, chunk->used)) {
            return 0;
        }
    }
    return 1;
}

kz_state_hdr_t *state_read_file(const kz_state_hdr_t *hdr, int (*read_fn)(void *ctx, void *buf, size_t len), void *ctx, int *error){
    chunk_alloc_t alloc;
    *error = STATE_READ_MEMORY;
    if(hdr->size < sizeof(*hdr) || !chunk_alloc_init(&alloc)) {
        return NULL;
    }

    /* allocate everything first, so nothing has been read if memory runs out */
    size_t left = hdr->size;
    while(left > 0) {
        state_chunk_t *chunk = chunk_new(&alloc, left);
        if(chunk == NULL || (chunk == alloc.first && chunk->cap < sizeof(*hdr))) {
            chunks_free(alloc.first);
            return NULL;
        }
        chunk->used = chunk->cap < left ? chunk->cap : left;
        left -= chunk->used;
    }

    /* the header was already read by the caller */
    state_chunk_t *first = alloc.first;
    memcpy(first->data, hdr, sizeof(*hdr));
    for(state_chunk_t *chunk = first; chunk != NULL; chunk = chunk->next) {
        size_t skip = chunk == first ? sizeof(*hdr) : 0;
        if(!read_fn(ctx, chunk->data + skip, chunk->used - skip)) {
            *error = STATE_READ_IO;
            chunks_free(first);
            return NULL;
        }
    }

    /* chunks are allocated a little larger than needed, give the rest back */
    for(state_chunk_t *chunk = first; chunk != NULL; chunk = chunk->next) {
        chunk_trim(&alloc, chunk);
    }

    *error = 0;
    return (kz_state_hdr_t*)first->data;
}

#endif
