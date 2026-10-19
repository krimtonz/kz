/*
* macro_store.h
*
* storage for input macros: one recorded input per game frame, plus the rng
* seeds the game was reseeded with while recording.  inputs are kept in fixed
* size pages so a long macro does not need one large block of memory.
*/

#ifndef _MACRO_STORE_H
#define _MACRO_STORE_H

#include <stdint.h>
#include <stddef.h>

#define MACRO_PAGE_FRAMES   512
#define MACRO_FILE_VERSION  1

/* controller 1 input for one game frame, as the game saw it */
typedef struct {
    uint16_t    pad;
    uint16_t    pressed;
    uint16_t    released;
    int8_t      x;
    int8_t      y;
    int8_t      adjusted_x;
    int8_t      adjusted_y;
} macro_input_t;

typedef struct {
    uint32_t    frame;
    uint32_t    seed;
} macro_seed_t;

typedef struct {
    macro_input_t **pages;
    uint32_t        page_cnt;
    uint32_t        length;
    macro_seed_t   *seeds;
    uint32_t        seed_cnt;
    uint32_t        seed_cap;
} macro_store_t;

/* file header, followed by length inputs and seed_cnt seeds */
typedef struct {
    char        magic[4];
    uint16_t    version;
    uint16_t    z2_version;
    uint32_t    length;
    uint32_t    seed_cnt;
} macro_file_hdr_t;

#define MACRO_READ_MEMORY   1
#define MACRO_READ_IO       2
#define MACRO_READ_FORMAT   3
#define MACRO_READ_VERSION  4

void            macro_store_init    (macro_store_t *store);
void            macro_store_clear   (macro_store_t *store);

/* the input recorded for a frame, NULL past the end of the macro */
macro_input_t  *macro_store_get     (macro_store_t *store, uint32_t frame);

/* records the input for a frame, at most one past the end.  returns 0 when out of memory */
int             macro_store_set     (macro_store_t *store, uint32_t frame, const macro_input_t *input);

/* drops everything from frame length on, including seeds */
void            macro_store_trim    (macro_store_t *store, uint32_t length);

/* rng seeds recorded at a frame.  set returns 0 when out of memory, get returns 0 when there is none */
int             macro_store_set_seed(macro_store_t *store, uint32_t frame, uint32_t seed);
int             macro_store_get_seed(macro_store_t *store, uint32_t frame, uint32_t *seed);

/* bytes in the macro's file */
size_t          macro_store_size    (macro_store_t *store);

int             macro_store_write   (macro_store_t *store, uint16_t z2_version,
                                     int (*write_fn)(void *ctx, const void *buf, size_t len), void *ctx);

/*
 * replaces the store with a macro read through read_fn, which must provide
 * size bytes.  on failure the store is left empty and a MACRO_READ_* error
 * is returned.
 */
int             macro_store_read    (macro_store_t *store, uint16_t z2_version, size_t size,
                                     int (*read_fn)(void *ctx, void *buf, size_t len), void *ctx);

#endif
