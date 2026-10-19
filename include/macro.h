/*
* macro.h
*
* input macros: record controller 1's input frame by frame and play it back.
* rng reseeds that happen while recording (on every scene load) are recorded
* too, and replayed, so macros stay in sync across scenes.
*/

#ifndef _MACRO_H
#define _MACRO_H

#include <stdint.h>
#include "z2.h"
#include "macro_store.h"

enum macro_mode {
    MACRO_IDLE,
    MACRO_RECORDING,
    MACRO_PLAYING,
};

/* patches the game's rng seeding so seeds can be recorded and replayed */
void            macro_init          (void);

/* called once per game frame with the game's input for controller 1 */
void            macro_update_input  (z2_input_t *input);

void            macro_record        (void);
void            macro_play          (void);
void            macro_stop          (void);
void            macro_rewind        (void);
void            macro_trim          (void);
void            macro_clear         (void);
void            macro_seek          (uint32_t frame);

enum macro_mode macro_mode          (void);
uint32_t        macro_frame         (void);
uint32_t        macro_length        (void);
macro_store_t  *macro_get_store     (void);

#endif
