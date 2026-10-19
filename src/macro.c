#include <stdint.h>
#include "kz.h"
#include "cache.h"
#include "macro.h"

static macro_store_t    store;
static enum macro_mode  mode;
static uint32_t         frame;
static z2_controller_t  last_raw;

/*
* the game only reseeds its rng in Play_Init, through Rand_Seed:
*
*   lui at, %hi(sRandInt); jr ra; sw a0, %lo(sRandInt)(at)
*
* macro_init replaces the first instruction with a jump to macro_rand_seed,
* after checking that the function looks exactly like that.
*/
static void macro_rand_seed(uint32_t seed){
    if(mode == MACRO_RECORDING) {
        if(!macro_store_set_seed(&store, frame, seed)) {
            kz_log("macro out of memory");
        }
    } else if(mode == MACRO_PLAYING) {
        if(!macro_store_get_seed(&store, frame, &seed)) {
            kz_log("macro rng seed not recorded");
        }
    }
    z2_random = seed;
}

void macro_init(void){
    macro_store_init(&store);

    uint32_t *code = (uint32_t*)z2_Rand_Seed;
    uint32_t rng = (uint32_t)&z2_random;
    if(code[0] == (0x3C010000 | ((rng + 0x8000) >> 16)) &&
       code[1] == 0x03E00008 &&
       code[2] == (0xAC240000 | (rng & 0xFFFF))) {
        code[0] = 0x08000000 | (((uint32_t)macro_rand_seed >> 2) & 0x03FFFFFF);
        code[1] = 0x00000000;
        osWritebackDCache(code, 8);
        osInvalICache(code, 8);
    }
}

void macro_update_input(z2_input_t *input){
    if(mode == MACRO_RECORDING) {
        macro_input_t in = {
            input->raw.pad, input->pad_pressed, input->pad_released,
            input->raw.x, input->raw.y, input->adjusted_x, input->adjusted_y,
        };
        if(!macro_store_set(&store, frame, &in)) {
            mode = MACRO_IDLE;
            kz_log("macro out of memory");
            return;
        }
        frame++;
    } else if(mode == MACRO_PLAYING) {
        macro_input_t *in = macro_store_get(&store, frame);
        if(in == NULL) {
            mode = MACRO_IDLE;
            kz_log("macro finished");
            return;
        }
        input->raw_prev = last_raw;
        input->raw.pad = in->pad;
        input->raw.x = in->x;
        input->raw.y = in->y;
        input->pad_pressed = in->pressed;
        input->pad_released = in->released;
        input->adjusted_x = in->adjusted_x;
        input->adjusted_y = in->adjusted_y;
        frame++;
    }
    last_raw = input->raw;
}

void macro_record(void){
    if(mode == MACRO_RECORDING) {
        macro_stop();
        return;
    }
    if(frame > store.length) {
        frame = store.length;
    }
    mode = MACRO_RECORDING;
    kz_log("recording macro from frame %d", frame);
}

void macro_play(void){
    if(mode == MACRO_PLAYING) {
        macro_stop();
        return;
    }
    if(store.length == 0) {
        kz_log("no macro");
        return;
    }
    if(frame >= store.length) {
        frame = 0;
    }
    mode = MACRO_PLAYING;
    kz_log("playing macro from frame %d", frame);
}

void macro_stop(void){
    if(mode != MACRO_IDLE) {
        kz_log("macro stopped at frame %d", frame);
    }
    mode = MACRO_IDLE;
}

void macro_rewind(void){
    frame = 0;
}

void macro_trim(void){
    macro_store_trim(&store, frame);
}

void macro_clear(void){
    mode = MACRO_IDLE;
    frame = 0;
    macro_store_clear(&store);
}

void macro_seek(uint32_t new_frame){
    frame = new_frame > store.length ? store.length : new_frame;
}

enum macro_mode macro_mode(void){
    return mode;
}

uint32_t macro_frame(void){
    return frame;
}

uint32_t macro_length(void){
    return store.length;
}

macro_store_t *macro_get_store(void){
    return &store;
}
