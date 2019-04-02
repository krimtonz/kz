#ifndef _INPUT_H
#define _INPUT_H

#define BUTTON_C_RIGHT        0x0001
#define BUTTON_C_LEFT         0x0002
#define BUTTON_C_DOWN         0x0004
#define BUTTON_C_UP           0x0008
#define BUTTON_R              0x0010
#define BUTTON_L              0x0020
#define BUTTON_D_RIGHT        0x0100
#define BUTTON_D_LEFT         0x0200
#define BUTTON_D_DOWN         0x0400
#define BUTTON_D_UP           0x0800
#define BUTTON_START          0x1000
#define BUTTON_Z              0x2000
#define BUTTON_B              0x4000
#define BUTTON_A              0x8000

uint16_t make_bind(int len, ...);
void input_update();
uint16_t get_pad_pressed();
uint16_t get_pad_released();
uint16_t get_pad_held();
uint16_t get_pad_pressed_unrestricted();
void reserve_buttons(uint16_t button_bitmask);
void free_buttons(uint16_t button_bitmask);

#endif