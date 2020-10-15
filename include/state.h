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

typedef struct {
    uint16_t    z2_version;
    uint16_t    settings_version;
    size_t      size;
} kz_state_hdr_t;

size_t  save_state  (void *state);
void    load_state  (void *state);

#endif
#endif