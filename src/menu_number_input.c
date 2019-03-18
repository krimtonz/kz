#include <stdlib.h>
#include "menu.h"

struct menu_data_number{
    uint8_t                 base;
    uint8_t                 editing;
    uint8_t                 length;
    uint8_t                 edit_idx;
    int                     value;
    char                   *digits;
    void                   *callback_data;
    menu_number_callback    callback;
};

int int_to_char(int i){
    if(i>=0 && i<=9)
        return i + '0';
    return i + ('a' - 0xA);
}

int char_to_int(char c){
    if(c>='0' && c<='9')
        return c - '0';
    return c - ('a' - 0xA);
}

void menu_number_activate_callback(struct menu_item *item){
    struct menu_data_number *data = item->data;
    if(data->editing){
        data->editing = 0;
        int mul = 1;
        uint32_t val = 0;
        for(int i=data->length-1;i>=0;i--){
            val += (char_to_int(data->digits[i]) % data->base) * mul;
            mul *= data->base;
        }
        data->value = val;
        if(data->callback){
            data->callback(item, data->callback_data, data->value);
        }
    }else{
        data->editing = 1;
    }
}

void menu_number_draw(struct menu_item *item){
    struct menu_data_number *data = item->data;
    uint32_t color = MENU_DEFAULT_COLOR;
    if(item->owner->selected_item == item && !data->editing)
        color = MENU_SELECTED_COLOR;
    int x = get_item_x_pos(item) + (kfont->c_width * (data->length-1));
    int y = get_item_y_pos(item);
    for(int i=data->length-1;i>=0;i--){
        if(data->editing){
            if(data->edit_idx == i){ 
                color=MENU_SELECTED_COLOR;
            }
            else{
                color=MENU_DEFAULT_COLOR;
            }
        }
        gfx_printf_color(x,y,color,"%c",data->digits[i]);
        x -= kfont->c_width;
    }
}

int menu_number_nav(struct menu_item *item, enum menu_nav nav){
    struct menu_data_number *data = item->data;
    if(!data->editing) return 0;
    switch(nav){
        case MENU_NAV_LEFT:
            if(data->edit_idx==00) data->edit_idx = data->length - 1;
            else data->edit_idx--;
            break;
        case MENU_NAV_RIGHT:
            data->edit_idx++;
            if(data->edit_idx>=data->length)data->edit_idx = 0;
            break;
        case MENU_NAV_UP:
            data->digits[data->edit_idx]++;
            if(data->digits[data->edit_idx]>'f') data->digits[data->edit_idx] = '0';
            else if(data->digits[data->edit_idx]>'9' && data->digits[data->edit_idx]<'a') data->digits[data->edit_idx] = 'a';
            break;
        case MENU_NAV_DOWN:
            data->digits[data->edit_idx]--;
            if(data->digits[data->edit_idx]<'0') data->digits[data->edit_idx] = 'f';
            else if(data->digits[data->edit_idx]<'a' && data->digits[data->edit_idx]>'9') data->digits[data->edit_idx] = '9';
            break;
        default:
            break;
    }
    
    return 1;
}

struct menu_item *menu_add_number_input(struct menu* menu, uint16_t x, uint16_t y, menu_number_callback callback, void *callback_data, uint8_t base, uint8_t length, int initial){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct menu_data_number *data = malloc(sizeof(*data));
        item->activate_proc = menu_number_activate_callback;
        item->draw_proc = menu_number_draw;
        item->navigate_proc = menu_number_nav;
        item->data = data;
        item->interactive = 1;
        data->base = base;
        data->edit_idx = length-1;
        data->length = length;
        data->editing = 0;
        data->value = initial;
        data->digits = malloc(length+1);
        data->digits[length] = 0;
        data->callback_data = callback_data;
        data->callback = callback;
        uint32_t val = initial;
        for(int i=length-1;i>=0;i--){
            data->digits[i] = int_to_char(val % base);
            val /= base;
        }
    }
    return item;
}