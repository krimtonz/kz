#include <stdlib.h>
#include "input.h"
#include "menu.h"
#include "resource.h"

struct item_data{
    int16_t *x;
    int16_t *y;
    menu_generic_callback callback;
    void *callback_data;
    _Bool active;
};

static void move_activate_proc(struct menu_item *item){
    struct item_data *data = item->data;
    data->active = !data->active;
    if(data->callback){
        data->callback(item,MENU_CALLBACK_ACTIVATE,data->callback_data);
    }
}

static int move_nav_proc(struct menu_item *item, enum menu_nav nav){
    struct item_data *data = item->data;
    if(!data->active) return 0;
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

static void move_draw_proc(struct menu_item *item){
    struct item_data *data = item->data;
    uint32_t color = 0xFFFFFFFF;
    if(data->active){
        color = 0x00FF00FF;
    }else if(item->owner->selected_item == item){
        color = MENU_SELECTED_COLOR.color;
    }
    gfx_draw_sprite_color(resource_get(R_KZ_ICON),get_item_x_pos(item),get_item_y_pos(item),2,8,8,color);
}

struct menu_item *menu_add_move_button(struct menu *menu, uint16_t x, uint16_t y,
                                       int16_t *move_x, int16_t *move_y,
                                       menu_generic_callback callback, void *callback_data){    
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->x = move_x;
        data->y = move_y;
        data->callback = callback;
        data->callback_data = data;
        data->active = 0;
        item->data = data;
        item->interactive = 1;
        item->draw_proc = move_draw_proc;
        item->navigate_proc = move_nav_proc;
        item->activate_proc = move_activate_proc;
    }
    return item;
}