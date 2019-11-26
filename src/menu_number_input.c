#include <stdlib.h>
#include "menu.h"

struct menu_data_number{
    uint32_t                value; // 00
    void                   *val_ptr; // 04
    char                   *digits; // 08
    void                   *callback_data; // 0c
    menu_number_callback    callback; // 10
    uint8_t                 base; // 14
    uint8_t                 editing; // 15
    uint8_t                 length; // 16
    uint8_t                 edit_idx; // 17
    uint8_t                 val_len; // 18
    _Bool                   _signed; // 19
};

static char int_to_char(int i){
    if(i>=0 && i<=9)
        return i + '0';
    return i + ('a' - 0xA);
}

static int char_to_int(char c){
    if(c>='0' && c<='9')
        return c - '0';
    return c - ('a' - 0xA);
}

static uint32_t get_val(struct menu_data_number *data){
    if(data->val_ptr){
        switch(data->val_len){
            case 1:
                return *(uint8_t*)data->val_ptr;
                break;
            case 2:
                return *(uint16_t*)data->val_ptr;
                break;
            case 4:
            default:
                return *(uint32_t*)data->val_ptr;
                break;
        }
    }else{
        return data->value;
    }
}

static void set_val(struct menu_data_number *data, uint32_t new_val){
    if(data->val_ptr){
        switch(data->val_len){
            case 1:
                *(uint8_t*)data->val_ptr = (uint8_t)new_val;
                break;
            case 2:
                *(uint16_t*)data->val_ptr = (uint16_t)new_val;
                break;
            case 4:
            default:
                *(uint32_t*)data->val_ptr = (uint32_t)new_val;
                break;
        }
    }else{
        data->value = new_val;
    }
}

static void menu_number_activate_callback(struct menu_item *item){
    struct menu_data_number *data = item->data;
    if(data->editing){
        data->editing = 0;
        int mul = 1;
        uint32_t val = 0;
        for(int i=data->length-1;i>=0;i--){
            if(data->_signed && i == 0) {
                break;
            }
            else if(!data->_signed && i == data->length - 1){
                continue;
            }
            val += (char_to_int(data->digits[i]) % data->base) * mul;
            mul *= data->base;
        }
        if(data->_signed && data->digits[0] == '-') val *= -1;
        data->value = val;
        if(data->callback && data->callback(item, MENU_CALLBACK_ACTIVATE, data->callback_data, data->value)){
            return;
        }
        if(data->val_ptr){
            set_val(data,val);
        }
    }else{
        data->editing = 1;
    }
}

static void menu_number_draw(struct menu_item *item){
    struct menu_data_number *data = item->data;
    z2_rgba32_t color = MENU_DEFAULT_COLOR;
    if(item->owner->selected_item == item && !data->editing)
        color = MENU_SELECTED_COLOR;
    int x = get_item_x_pos(item) + (kfont->c_width * (data->length-1));
    if(!data->_signed) x -= kfont->c_width;
    int y = get_item_y_pos(item);
    uint32_t val = get_val(data);
    int sign = (data->_signed && data->digits[0] == '-') ? -1 : 1;
    val *= sign;
    for(int i=data->length - 1;i>=0;i--){
        if(!data->_signed && i == data->length - 1){
            continue;
        }
        if(data->editing){
            if(data->edit_idx == i){
                color.color = 0x00FF00FF;
            }
            else{
                color=MENU_DEFAULT_COLOR;
            }
        }
        else{
            if(i == 0 && data->_signed && sign>0){
                data->digits[i] = '+';
            }else if(i == 0 && data->_signed && sign<0){
                data->digits[i] = '-';
            }
            else if(!data->_signed && i == data->length - 1){
                data->digits[i] = ' ';
                continue;
            }
            else{
                data->digits[i] = int_to_char(val % data->base);
                val /= data->base;
            }
        }
        gfx_printf_color(x,y,color.color,"%c",data->digits[i]);
        x -= kfont->c_width;
    }
}

static int menu_number_nav(struct menu_item *item, enum menu_nav nav){
    struct menu_data_number *data = item->data;
    if(!data->editing) return 0;
    int v = char_to_int(data->digits[data->edit_idx]);
    switch(nav){
        case MENU_NAV_LEFT:
            if(data->edit_idx == 0){
                if(data->_signed){
                    data->edit_idx = data->length - 1;
                }
                else{
                    data->edit_idx = data->length - 2;
                }
            }
            else{
                data->edit_idx--;
            }
            break;
        case MENU_NAV_RIGHT:
            data->edit_idx++;
            if(!data->_signed && data->edit_idx == data->length-1){
                data->edit_idx = 0;
            }
            else if(data->edit_idx>=data->length){
                data->edit_idx = 0;
            }
            break;
        case MENU_NAV_UP:
            if(data->_signed && data->edit_idx == 0){
                if(data->digits[0] == '+'){
                    data->digits[0] = '-';
                }else{
                    data->digits[0] = '+';
                }
            }else{
                v++;
                v = (v + data->base) % data->base;
                data->digits[data->edit_idx] = int_to_char(v);
            }
            break;
        case MENU_NAV_DOWN:
        if(data->_signed && data->edit_idx == 0){
                if(data->digits[0] == '+'){
                    data->digits[0] = '-';
                }else{
                    data->digits[0] = '+';
                }
            }else{
                v--;
                v = (v + data->base) % data->base;
                data->digits[data->edit_idx] = int_to_char(v);
            }
            break;
        default:
            break;
    }

    return 1;
}

void menu_number_set(struct menu_item *item, uint32_t val){
    struct menu_data_number *data = item->data;
    data->value = val;
    for(int i=data->length - 1;i>=0;i--){
        if(i == 0 && data->_signed){
            data->digits[i] = '+';
        }
        else if(!data->_signed && i == data->length - 1){
            data->digits[i] = ' ';
            continue;
        }
        else{
            data->digits[i] = int_to_char(val % data->base);
            val /= data->base;
        }
    }
}

static void menu_number_update_proc(struct menu_item *item){
    struct menu_data_number *data = item->data;
    if(!data->editing && data->callback){
        data->callback(item, MENU_CALLBACK_UPDATE, data->callback_data, data->value);
    }
}

struct menu_item *menu_add_number_input(struct menu* menu, uint16_t x, uint16_t y,
                                        menu_number_callback callback, void *callback_data,
                                        int8_t base, uint8_t length, void *value,
                                        uint8_t val_len){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct menu_data_number *data = malloc(sizeof(*data));
        item->activate_proc = menu_number_activate_callback;
        item->update_proc = menu_number_update_proc;
        item->draw_proc = menu_number_draw;
        item->navigate_proc = menu_number_nav;
        item->data = data;
        item->interactive = 1;
        if(base<0){
            base = -base;
            data->_signed = 1;
        }else{
            data->_signed = 0;
        }
        length++;
        data->base = base;
        data->length = length;
        data->editing = 0;
        data->val_ptr = value;
        data->val_len = val_len;
        if(value){
            data->value = get_val(data);
        }else{
            data->value = 0;
        }
        data->digits = malloc(length + 1);
        data->digits[length] = 0;
        data->callback_data = callback_data;
        data->callback = callback;
        int val = data->value;
        for(int i=data->length - 1;i>=0;i--){
            if(i == 0 && data->_signed){
                data->digits[i] = '+';
            }
            else if(!data->_signed && i == data->length - 1){
                data->digits[i] = ' ';
                continue;
            }
            else{
                data->digits[i] = int_to_char(val % data->base);
                val /= data->base;
            }
        }
        if(data->_signed){
            data->edit_idx = length - 1;
        }
        else{
            data->edit_idx = length - 2;
        }
    }
    return item;
}