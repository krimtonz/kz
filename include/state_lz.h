/*
* state_lz.h
*
* tiny streaming lz77 compressor used by kz-lite save states.
*
* kz-lite does not have enough free ram to hold an uncompressed save state,
* so every chunk the state code writes is compressed as it is written, and
* decompressed directly into game memory when it is read back.  Matches only
* reference data within the same chunk, so no history buffer is required on
* either side.
*
* the compressed stream does not need to be contiguous: the output and input
* streams call back to move to the next block of memory when one runs out.
*/

#ifndef _STATE_LZ_H
#define _STATE_LZ_H

#include <stdint.h>
#include <stddef.h>

#define LZ_HASH_BITS    12
#define LZ_HASH_SIZE    (1 << LZ_HASH_BITS)

/* hash table used while compressing, LZ_HASH_SIZE entries */
typedef const uint8_t *lz_hash_t;

typedef struct lz_stream_s lz_stream_t;

struct lz_stream_s {
    uint8_t    *p;
    uint8_t    *end;
    /* called when p reaches end, must set p and end to the next block. returns 0 on failure */
    int       (*next)(lz_stream_t *stream);
    void       *data;
};

/**
 * Compress len bytes from src into the output stream.
 * Returns 0 if the output stream could not provide more space.
 */
int     lz_compress     (lz_stream_t *out, const void *src, size_t len, lz_hash_t *hash);

/**
 * Decompress exactly len bytes from the input stream into dst.
 */
void    lz_decompress   (lz_stream_t *in, void *dst, size_t len);

#endif
