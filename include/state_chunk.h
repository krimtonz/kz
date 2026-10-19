/*
* state_chunk.h
*
* memory for kz-lite save states.  a state is stored as a list of chunks so it
* can use whatever free blocks the heap has; see state_chunk_t in state.h.
*/

#ifndef _STATE_CHUNK_H
#define _STATE_CHUNK_H

#include <stddef.h>
#include "state.h"
#include "state_lz.h"

#define STATE_CHUNK(hdr)    ((state_chunk_t*)((char*)(hdr) - offsetof(state_chunk_t, data)))
#define STATE_CHUNK_MIN     0x400

/* hands out chunks from the largest free blocks, keeping STATE_MEM_RESERVE free */
typedef struct {
    size_t          limit;
    size_t          allocated;
    state_chunk_t  *first;
    state_chunk_t  *last;
} chunk_alloc_t;

/* returns 0 if there is not enough free memory to start a state */
int             chunk_alloc_init    (chunk_alloc_t *alloc);

/* allocates a chunk holding up to want bytes, or as much as possible when want is 0 */
state_chunk_t  *chunk_new           (chunk_alloc_t *alloc, size_t want);

/* gives the unused end of a chunk back to the heap, the chunk does not move */
void            chunk_trim          (chunk_alloc_t *alloc, state_chunk_t *chunk);

void            chunks_free         (state_chunk_t *chunk);

/* output stream into the allocator's chunks, starting skip bytes into the first chunk */
void            chunk_out_open      (lz_stream_t *stream, chunk_alloc_t *alloc, size_t skip);

/* ends an output stream: trims the last chunk and returns the total size of all chunks */
size_t          chunk_out_close     (lz_stream_t *stream, chunk_alloc_t *alloc);

#endif
