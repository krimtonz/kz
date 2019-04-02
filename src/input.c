#include <stdint.h>
#include <stdarg.h>
#include "input.h"
#include "z2.h"
#include "commands.h"

#define BIND_END 6

static uint16_t pad_pressed;
static uint16_t pad_held;
static uint16_t pad_released;
static uint16_t reserved;

static uint16_t bind_commands[COMMAND_CNT];
static uint16_t bind_held[COMMAND_CNT];

uint16_t make_bind(int len, ...){
    va_list list;
    va_start(list,len);
    uint16_t ret = 0;
    uint16_t val = 0;
    for(int i=0;i<len;i++){
        val = va_arg(list,int);
        for(int j=0;j<16;j++){
            if(val & (1 << j)){
                ret |= j << (i * 4);
            }
        }
    }
    va_end(list);
    if(len<4){
        ret |= 6 << (len*4);
    }
    return ret;
}

uint16_t get_bind_componenet(uint16_t bind, int pos){
    return 1 << ((bind >> (pos*4)) & 0x000F);
}

void input_update(){
    z2_input_t inp = z2_game.common.input[0];
    pad_pressed = inp.pad_pressed;
    pad_held |= inp.pad_pressed;
    pad_held ^= inp.pad_released;
    pad_released = inp.pad_released;

    for(int i=0;i<COMMAND_CNT;i++){
        for(int j=0;j<4;j++){
            
        }
    }
}

uint16_t get_pad_pressed(){
    return (pad_pressed ^ reserved) & pad_pressed;
}

uint16_t get_pad_pressed_unrestricted(){
    return pad_pressed;
}

uint16_t get_pad_released(){
    return pad_released;
}

uint16_t get_pad_held(){
    return (pad_held ^ reserved) & pad_held;
}

void reserve_buttons(uint16_t button_bitmask){
    reserved |= button_bitmask;
}

void free_buttons(uint16_t button_bitmask){
    reserved ^= button_bitmask;
}