#include <stdlib.h>
#include "menu.h"
#include "input.h"
#include "kzresource.h"

struct item_data {
    int16_t *x;
    int16_t *y;
    _Bool active;
};

static int menu_move_event(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    struct item_data *data = item->data;
    if(event == MENU_EVENT_ACTIVATE){
        data->active = !data->active;
        return 1;
    }else if(event == MENU_EVENT_NAVIGATE){
        if(!data->active){
            return 0;
        }
        menu_nav_t nav = (menu_nav_t)event_data;
        int dist = (input_pressed_raw() & BUTTON_Z) ? 3 : 1;
        switch(nav){
            case MENU_NAV_DOWN:
            if(data->y){
                *data->y += dist;
            }
            break;
            case MENU_NAV_UP:
            if(data->y){
                *data->y -= dist;
            }
            break;
            case MENU_NAV_LEFT:
            if(data->x){
                *data->x -= dist;
            }
            break;
            case MENU_NAV_RIGHT:
            if(data->x){
                *data->x += dist;
            }
            default:
            break;
        }
        return 1;
    }
    return 0;
}

static void menu_move_draw(menu_item_t *item){
    struct item_data *data = item->data;
    uint32_t color = DEFAULT_COLOR;
    if(data->active){
        color = COLOR_GREEN;
    }else if(item->owner->selected_item == item){
        color = SELECTED_COLOR;
    }
    gfx_draw_sprite_color(resource_get(R_Z2_ICON), menu_item_x(item), menu_item_y(item),
                          2, 8, 8, color);
}

static void menu_move_remove(menu_item_t *item){
    free(item->data);
    item->data = NULL;
}

menu_item_t *menu_move_button_add(menu_t *menu, uint16_t x, uint16_t y, int16_t *move_x, int16_t *move_y){
    menu_item_t *item = menu_add(menu, x, y);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->x = move_x;
        data->y = move_y;
        data->active = 0;
        item->data = data;
        item->interactive = 1;
        item->draw_proc = menu_move_draw;
        item->remove_proc = menu_move_remove;
        menu_item_register_event(item, MENU_EVENT_NAVIGATE | MENU_EVENT_ACTIVATE, menu_move_event, NULL);
    }
    return item;
}