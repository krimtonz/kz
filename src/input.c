#include <stdint.h>
#include "input.h"
#include "z2.h"

static uint16_t pad_pressed;
static uint16_t pad_held;
static uint16_t pad_released;
static uint16_t reserved;

void input_update(){
    z2_input_t inp = z2_game.common.input[0];
    pad_pressed = inp.pad_pressed;
    pad_held |= inp.pad_pressed;
    pad_held ^= inp.pad_released;
    pad_released = inp.pad_released;
}

uint16_t get_pad_pressed(){
    return pad_pressed;
}

uint16_t get_pad_released(){
    return pad_released;
}

void reserve_buttons(uint16_t button_bitmask){
    reserved |= button_bitmask;
}

void free_buttons(uint16_t button_bitmask){
    reserved ^= button_bitmask;
}