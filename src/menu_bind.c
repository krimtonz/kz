#include <stdint.h>
#include "menu.h"
#include "menu_bind.h"
#include "kzresource.h"
#include "settings.h"
#include "input.h"

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

static void bind_draw(menu_item_t *item){
    struct bind_item_data *data = item->data;
    uint16_t bind = settings->binds[data->cmd];
    uint32_t color = DEFAULT_COLOR;
    int x = menu_item_x(item);
    int y = menu_item_y(item);
    if(data->state != BIND_STATE_NONE){
        color = COLOR_GREEN;
    }else if(item->owner->selected_item == item){
        color = SELECTED_COLOR;
    }
    if(bind == BIND_END){
        gfx_printf_color(x, y, color, "none");
        return;
    }
    int i = 0;
    _Bool do_color = (color == DEFAULT_COLOR);
    int b;
    gfx_texture *button_tex = resource_get(R_KZ_BUTTONS);
    while((b = bind_get_component(bind, i)) != BIND_END){
        if(do_color){
            color = button_colors[b];
        }
        gfx_draw_sprite_color(button_tex, x + (i++ * 10), y, b, 8, 8, color);
        if(i == 4) break;
    }
}

static int bind_event(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    struct bind_item_data *data = item->data;
    if(event == MENU_EVENT_UPDATE){
        if(data->state == BIND_STATE_NONE){
            return 0;
        }
        uint16_t *bind = &settings->binds[data->cmd];
        if(data->state == BIND_STATE_BEGIN){
            if(!input_pressed_raw()){
                data->state = BIND_STATE_ACTION;
            }
            else if(button_time(button_get_index(BUTTON_L)) >= INPUT_REPEAT){
                input_enable();
                *bind = BIND_END;
                data->state = BIND_STATE_NONE;
            }
        }
        if (data->state == BIND_STATE_ACTION){
            if(input_pressed_raw()){
                *bind = BIND_END;
                data->state = BIND_STATE_LISTENING;
            }
        }
        if(data->state == BIND_STATE_LISTENING){
            uint16_t p = bind_get_bitmask(*bind);
            if(input_pressed_raw() == 0){
                data->state = 0;
                input_enable();
            }else{
                uint16_t pressed = pad_pressed_raw() & ~p;
                for(int i = 0;pressed && i < 4;++i){
                    int btn = bind_get_component(*bind, i);
                    if(btn != BIND_END){
                        continue;
                    }
                    btn = button_get_index(pressed);
                    *bind = (*bind & ~(0x000F << (i * 4))) | (btn << (i * 4));
                    if(i < 3){
                        *bind = (*bind & ~(0x000F << ((i + 1) * 4))) | (BIND_END << ((i + 1) * 4));
                    }
                    pressed &= ~(1 << btn);
                }
            }
        }
    }else if(event == MENU_EVENT_ACTIVATE){
        if(data->state == BIND_STATE_NONE){
            data->state = BIND_STATE_BEGIN;
            input_disable();
        }
    }
    return 1;
}

void menu_bind_set(menu_item_t *item, int cmd){
    struct bind_item_data *data = item->data;
    data->cmd = cmd;
    data->state = BIND_STATE_NONE;
}

menu_item_t *menu_bind_add(menu_t *menu, int x, int y, int cmd){
    menu_item_t *item = menu_add(menu, x, y);
    if(item){
        struct bind_item_data *data = malloc(sizeof(*data));
        data->cmd = cmd;
        data->state = BIND_STATE_NONE;
        item->data = data;
        item->interactive = 1;
        item->draw_proc = bind_draw;
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, bind_event, NULL);
    }
    return item;
}
