/*
* state.h
*
* defintions for save state functionality
*/

#ifndef LITE
#ifndef _STATE_H
#define _STATE_H

#include <stdint.h>
#include <stdlib.h>

#if 0
typedef struct {
    uint16_t    z2_version;
    uint16_t    settings_version;
    size_t      size;
} kz_state_hdr_t;
#endif
typedef struct {
    uint16_t    z2_version;
    uint16_t    settings_version;
    size_t      size;
    _Bool       saved;
} kz_state_hdr_t;

size_t  save_state  (kz_state_hdr_t *state);
void    load_state  (kz_state_hdr_t *state);

#endif
#endif