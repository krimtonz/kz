/*
* state.h
*
* defintions for save state functionality
*/

#ifndef _STATE_H
#define _STATE_H

#include <stdint.h>
#include <stdlib.h>

#define STATE_VERSION 8

#ifdef LITE
/* lite states are compressed, so they are versioned apart from full/vc states */
#define STATE_LITE          0x8000
#define STATE_HDR_VERSION   (STATE_VERSION | STATE_LITE)
#else
#define STATE_HDR_VERSION   STATE_VERSION
#endif

#ifdef LITE
/*
 * memory left free when allocating a state, for kz's own allocations such as
 * menu textures.  menu icons that do not fit are simply not drawn.
 */
#define STATE_MEM_RESERVE   0x10000
/* smallest output buffer worth attempting to compress a state into */
#define STATE_MIN_ALLOC     0x8000
#endif

typedef struct {
    uint16_t    z2_version;
    uint16_t    state_version;
    size_t      size;
    char        name[64];
} kz_state_hdr_t;

size_t  save_state      (void *state);
void    load_state      (void *state);
int     state_is_valid  (kz_state_hdr_t *state);

#ifdef LITE
#include "state_lz.h"

/*
 * a lite state is a list of chunks, so it can use whatever free blocks the
 * heap has.  the first chunk's data starts with the kz_state_hdr_t, and
 * kz.states[] points at that header.  hdr->size is the total of all chunks'
 * used bytes, which is also the size of an exported state file.
 */
typedef struct state_chunk_s state_chunk_t;
struct state_chunk_s {
    state_chunk_t  *next;
    size_t          used;
    size_t          cap;
    uint8_t         data[];
};

#define STATE_READ_MEMORY   1
#define STATE_READ_IO       2

/**
 * Saves a compressed state into newly allocated memory.
 * Returns NULL if there is not enough free memory to hold the state.
 */
kz_state_hdr_t *save_state_alloc(void);

/**
 * Frees all memory used by a state.
 */
void            state_free      (kz_state_hdr_t *state);

/**
 * Writes the state, header included, through write_fn.  Returns 0 if write_fn failed.
 */
int             state_write_file(kz_state_hdr_t *state, int (*write_fn)(void *ctx, const void *buf, size_t len), void *ctx);

/**
 * Allocates a state described by hdr and fills it through read_fn with the
 * rest of the state after the header.  Returns NULL and sets error to
 * STATE_READ_MEMORY or STATE_READ_IO on failure.
 */
kz_state_hdr_t *state_read_file (const kz_state_hdr_t *hdr, int (*read_fn)(void *ctx, void *buf, size_t len), void *ctx, int *error);

void            state_stream_open(lz_stream_t *stream, kz_state_hdr_t *state, void *pos);
#endif

#endif
