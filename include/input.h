/*
* input.h
*
* definitions for input handling
*/

#ifndef _INPUT_H
#define _INPUT_H

#include "menu.h"

#define BUTTON_C_RIGHT          0x0001
#define BUTTON_C_LEFT           0x0002
#define BUTTON_C_DOWN           0x0004
#define BUTTON_C_UP             0x0008
#define BUTTON_R                0x0010
#define BUTTON_L                0x0020
#define BUTTON_D_RIGHT          0x0100
#define BUTTON_D_LEFT           0x0200
#define BUTTON_D_DOWN           0x0400
#define BUTTON_D_UP             0x0800
#define BUTTON_START            0x1000
#define BUTTON_Z                0x2000
#define BUTTON_B                0x4000
#define BUTTON_A                0x8000

#define INPUT_REPEAT            8
#define BIND_END                6

int         bind_get_component      (uint16_t bind, int index);
uint16_t    bind_get_bitmask        (uint16_t bind);
void        bind_override           (int index);
uint16_t    make_bind               (int len, ...);
void        input_update            (void);
_Bool       input_bind_held         (int index);
_Bool       input_bind_pressed      (int index);
_Bool       input_bind_pressed_raw  (int index);
void        reserve_buttons         (uint16_t button_bitmask);
void        free_buttons            (uint16_t button_bitmask);
void        reserve_stick           (void);
void        free_stick              (void);
uint16_t    input_pressed           (void);
uint16_t    input_pressed_raw       (void);
uint16_t    pad_pressed_raw         (void);
int8_t      input_x                 (void);
int8_t      input_y                 (void);
void        input_enable            (void);
void        input_disable           (void);
int         button_time             (int btn);
int         button_get_index        (uint16_t button_mask);
void        input_init              (uint16_t *pad, int8_t *stick_x, int8_t *stick_y, uint16_t *binds, int n_binds);

extern uint32_t button_colors[16];

#endif