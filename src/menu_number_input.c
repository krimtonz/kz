#include <stdlib.h>
#include "menu.h"

struct item_data {
    uint32_t                value;
    char                   *digits;
    uint8_t                 base;
    int                     editing;
    uint8_t                 length;
    uint8_t                 edit_idx;
    _Bool                   _signed;
};

static char int_to_char(int i){
    if(i >= 0 && i <= 9){
        return i + '0';
    }
    return i + ('a' - 0xA);
}

static int char_to_int(char c){
    if(c >= '0' && c <= '9'){
        return c - '0';
    }
    return c - ('a' - 0xA);
}

static uint32_t get_val(struct item_data *data){
    return data->value;
}

static void menu_number_draw(menu_item_t *item){
    struct item_data *data = item->data;
    uint32_t color = DEFAULT_COLOR;
    if(item->owner->selected_item == item && !data->editing){
        color = SELECTED_COLOR;
    }
    int x = menu_item_x(item) + (kfont->c_width * (data->length-1));
    if(!data->_signed){
        x -= kfont->c_width;
    }
    int y = menu_item_y(item);
    uint32_t val = get_val(data);
    int sign = (data->_signed && data->digits[0] == '-') ? -1 : 1;
    val *= sign;
    for(int i = data->length - 1;i >= 0;i--){
        if(!data->_signed && i == data->length - 1){
            continue;
        }
        if(data->editing){
            if(data->edit_idx == i){
                color = COLOR_GREEN;
            }
            else{
                color = DEFAULT_COLOR;
            }
        }
        else{
            if(i == 0 && data->_signed && sign > 0){
                data->digits[i] = '+';
            }else if(i == 0 && data->_signed && sign < 0){
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
        gfx_printf_color(x, y, color, "%c", data->digits[i]);
        x -= kfont->c_width;
    }
}

static int menu_number_event(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    struct item_data *data = item->data;
    if(event == MENU_EVENT_ACTIVATE){
        if(data->editing){
            data->editing = 0;
            int mul = 1;
            uint32_t val = 0;
            for(int i = data->length - 1;i >= 0;i--){
                if(data->_signed && i == 0) {
                    break;
                }
                else if(!data->_signed && i == data->length - 1){
                    continue;
                }
                val += (char_to_int(data->digits[i]) % data->base) * mul;
                mul *= data->base;
            }
            if(data->_signed && data->digits[0] == '-'){
                val *= -1;
            }
            data->value = val;
            *event_data = (void*)data->value;
            menu_item_trigger_event(item, MENU_EVENT_NUMBER, event_data);
        }else{
            data->editing = 1;
        }
    }else if(event == MENU_EVENT_NAVIGATE){
        if(!data->editing){
            return 0;
        }
        menu_nav_t nav = (menu_nav_t)event_data;
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
                else if(data->edit_idx >= data->length){
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
    }else if(event == MENU_EVENT_UPDATE && data->editing){
        return 1;
    }
    return 0;
}

void menu_number_set(menu_item_t *item, uint32_t val){
    struct item_data *data = item->data;
    data->value = val;
    for(int i = data->length - 1;i >= 0;i--){
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

int menu_number_word_event(event_handler_t *handler, menu_event_t event, void **event_data){
    uint32_t *callback_data = handler->callback_data;
    if(callback_data){
        if(event == MENU_EVENT_NUMBER){
            *callback_data = (uint32_t)*event_data;
        }else if(event == MENU_EVENT_UPDATE){
            menu_item_t *item = handler->subscriber;
            struct item_data *data = item->data;
            data->value = *callback_data;
        }
    }
    return 1;
}

int menu_number_halfword_event(event_handler_t *handler, menu_event_t event, void **event_data){
    uint16_t *callback_data = handler->callback_data;
    if(callback_data){
        if(event == MENU_EVENT_NUMBER){
            *callback_data = ((uint32_t)*event_data) & 0xFFFF;
        }else if(event == MENU_EVENT_UPDATE){
            menu_item_t *item = handler->subscriber;
            struct item_data *data = item->data;
            data->value = *callback_data;
        }
    }
    return 1;
}

int menu_number_byte_event(event_handler_t *handler, menu_event_t event, void **event_data){
    uint8_t *callback_data = handler->callback_data;
    if(callback_data){
        if(event == MENU_EVENT_NUMBER){
            *callback_data = ((uint32_t)*event_data) & 0xFF;
        }else if(event == MENU_EVENT_UPDATE){
            menu_item_t *item = handler->subscriber;
            struct item_data *data = item->data;
            data->value = *callback_data;
        }
    }
    return 1;
}

static void menu_number_remove(menu_item_t *item){
    struct item_data *data = item->data;
    free(data->digits);
    data->digits = NULL;
    free(data);
    item->data = NULL;
}

menu_item_t *menu_number_input_add(menu_t* menu, uint16_t x, uint16_t y, int8_t base, uint8_t length){
    menu_item_t *item = menu_add(menu, x, y);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        item->draw_proc = menu_number_draw;
        item->remove_proc = menu_number_remove;
        item->data = data;
        item->interactive = 1;
        if(base < 0){
            base = -base;
            data->_signed = 1;
        }else{
            data->_signed = 0;
        }
        length++;
        data->base = base;
        data->length = length;
        data->editing = 0;
        data->value = 0;
        data->digits = malloc(length + 1);
        data->digits[length] = 0;
        int val = data->value;
        for(int i = data->length - 1;i >= 0;i--){
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
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_NAVIGATE | MENU_EVENT_UPDATE, menu_number_event, NULL);
    }
    return item;
}