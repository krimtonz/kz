#include <stdlib.h>
#include "input.h"
#include "mem.h"
#include "kz.h"

uint32_t button_colors[16] = {
    0xFFF000FF,
    0xFFF000FF,
    0xFFF000FF,
    0xFFF000FF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0,
    0,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFF823CFF,
    0xFFFFFFFF,
    0x64FF78FF,
    0x64C8FFFF,
};

 enum bind_state {
    BIND_STATE_NONE,
    BIND_STATE_ACTION,
    BIND_STATE_BEGIN,
    BIND_STATE_LISTENING,
};

struct bind_item_data {
    int             cmd;
    enum bind_state state;
};

static int          __button_time[16];
static uint16_t     __pad_pressed_raw;
static uint16_t     __pad_pressed;
static uint16_t     __pad_released;
static uint16_t     __pad;
static uint16_t     __pad_reserved;
static int8_t       __stick_x;
static int8_t       __stick_y;
static int         *__bind_component_state;
static int         *__bind_time;
static _Bool       *__bind_pressed_raw;
static _Bool       *__bind_pressed;
static _Bool       *__bind_override;
static _Bool        __input_enabled = 1;
static int          __bind_cnt;
static uint16_t    *__binds_p;
static uint16_t    *__pad_p;
static int8_t      *__x_p;
static int8_t      *__y_p;


int bind_get_component(uint16_t bind, int index){
    return (bind >> (4 * index)) & 0xF;
}

uint16_t bind_get_bitmask(uint16_t bind){
    uint16_t p = 0;
    for(int i = 0;i < 4;i++){
        int c = bind_get_component(bind, i);
        if(c == BIND_END){
            break;
        }
        p |= 1 << c;
    }
    return p;
}

int button_get_index(uint16_t button_mask){
    for(int i = 0;i < 16;i++){
        if(button_mask & (1 << i)){
            return i;
        }
    }
    return -1;
}

void bind_override(int index){
    __bind_override[index] = 1;
}

uint16_t make_bind(int len, ...){
    va_list list;
    va_start(list, len);
    uint16_t ret = 0;
    uint16_t val = 0;
    for(int i = 0;i < len;i++){
        val = va_arg(list, int);
        for(int j = 0;j < 16;j++){
            if(val & (1 << j)){
                ret |= j << (i * 4);
            }
        }
    }
    va_end(list);
    if(len < 4){
        ret |= 6 << (len * 4);
    }
    return ret;
}

void input_init(uint16_t *pad, int8_t *stick_x, int8_t *stick_y, uint16_t *binds, int n_binds){
    __bind_cnt = n_binds;
    __bind_component_state = calloc(__bind_cnt, sizeof(*__bind_component_state));
    __bind_time = calloc(__bind_cnt, sizeof(*__bind_time));
    __bind_pressed_raw = calloc(__bind_cnt, sizeof(*__bind_pressed_raw));
    __bind_pressed = calloc(__bind_cnt, sizeof(*__bind_pressed));
    __bind_override = calloc(__bind_cnt, sizeof(*__bind_override));
    __binds_p = binds;
    __pad_p = pad;
    __x_p = stick_x;
    __y_p = stick_y;
}

void input_update(void){
    uint16_t z_pad = *__pad_p;
    __stick_x = *__x_p;
    __stick_y = *__y_p;
    __pad_pressed_raw = (__pad ^ z_pad) & z_pad;
    __pad_released = (__pad ^ z_pad) & ~z_pad;
    __pad = z_pad;
    __pad_pressed = 0;
    for(int i = 0;i < 16;i++){
        uint16_t p = 1 << i;
        if(__pad & p){
            __button_time[i]++;
        }else{
            __button_time[i] = 0;
        }
        if((__pad_pressed_raw & p) || __button_time[i] >= INPUT_REPEAT){
            __pad_pressed |= p;
        }
    }
    static uint16_t *bind_pad = NULL;
    static _Bool *bind_state = NULL;
    if(bind_pad == NULL){
        bind_pad = calloc(__bind_cnt, sizeof(*bind_pad));
    }
    if(bind_state == NULL){
        bind_state = calloc(__bind_cnt, sizeof(*bind_state));
    }

    for(int i = 0;i < __bind_cnt;i++){
        uint16_t *b =  &__binds_p[i];
        bind_pad[i] = bind_get_bitmask(*b);
        int *cs = &__bind_component_state[i];
        int j = 0;
        uint16_t c = 0;
        if(((__pad_reserved & bind_pad[i]) && !__bind_override[i]) || !__input_enabled){
            *cs = 0;
        }else{
            int css = *cs;
            for(j = 0;j < 4;j++){
                c = bind_get_component(*b, j);
                if(c == BIND_END){
                    break;
                }
                uint8_t csm = 1 << j;
                if(*cs & csm){
                    if(__pad & (1 << c)){
                        continue;
                    }
                    else{
                        if(*cs & ~((1 << (j + 1)) - 1)){
                            *cs = 0;
                        }else{
                            *cs &= ~csm;
                        }
                        break;
                    }
                }
                if((__pad_released & (1 << c)) || (css != 0 && (__pad_pressed_raw & ~bind_pad[i]))){
                    *cs = 0;
                    break;
                }else if(__pad_pressed_raw & (1 << c)){
                    *cs |= csm;
                }else{
                    break;
                }
            }
        }
        bind_state[i] = (*cs && (j == 4 || c == BIND_END));
    }
    for(int i = 0;i < __bind_cnt;i++){
        uint16_t pi = bind_pad[i];
        for(int j = 0;bind_state[i] && j < __bind_cnt;j++){
            if(!bind_state[j]){
                continue;
            }
            uint16_t pj = bind_pad[j];
            if(pi != pj && (pi & pj) == pi){
                __bind_component_state[i] = 0;
                bind_state[i] = 0;
            }
        }
        __bind_pressed_raw[i] = (__bind_time[i] == 0 && bind_state[i]);
        if(!bind_state[i])
            __bind_time[i] = 0;
        else{
            __bind_time[i]++;
        }
        __bind_pressed[i] = (__bind_pressed_raw[i] || __bind_time[i] >= INPUT_REPEAT);
    }
}

_Bool input_bind_held(int index){
    return __bind_time[index] > 0;
}

_Bool input_bind_pressed(int index){
    return __bind_pressed[index];
}

_Bool input_bind_pressed_raw(int index){
    return __bind_pressed_raw[index];
}

void reserve_buttons(uint16_t button_bitmask){
    __pad_reserved |= button_bitmask;
}

void free_buttons(uint16_t button_bitmask){
    __pad_reserved ^= button_bitmask;
}

uint16_t input_pressed(void){
    if(!__input_enabled){
        return 0;
    }
    return __pad_pressed;
}

uint16_t input_pressed_raw(void){
    return __pad;
}

uint16_t pad_pressed_raw(void){
    return __pad_pressed_raw;
}

int8_t input_x(void){
    return __stick_x;
}

int8_t input_y(void){
    return __stick_y;
}

int button_time(int btn){
    return __button_time[btn];
}

void input_enable(void){
    __input_enabled = 1;
}

void input_disable(void){
    __input_enabled = 0;
}

void input_mask_set(uint16_t button, uint8_t x, uint8_t y) {
    kz.input_mask |= button;
    kz.stick_x_mask |= x;
    kz.stick_y_mask |= y;
}

void input_mask_clear(uint16_t button, uint8_t x, uint8_t y) {
    kz.input_mask &= ~button;
    kz.stick_x_mask &= ~x;
    kz.stick_y_mask &= ~y;
}
