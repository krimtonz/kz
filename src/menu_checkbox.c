#include <stdlib.h>
#include "menu.h"
#include "kzresource.h"

void menu_checkbox_draw(menu_item_t *item){
    int data = (int)item->data;
    static gfx_texture *checkbox_tex = NULL;
    if(!checkbox_tex){
        checkbox_tex = resource_get(R_Z2_CHECKBOX);
    }
    uint32_t color = DEFAULT_COLOR;
    if(item->owner->selected_item == item){
        color = SELECTED_COLOR;
    }
    int x = menu_item_x(item);
    int y = menu_item_y(item);
    gfx_draw_sprite_color(checkbox_tex,x,y,0,
                          checkbox_tex->tile_width,checkbox_tex->tile_height,
                          color);
    if(data){
        gfx_draw_sprite(checkbox_tex,x,y,1,
                          checkbox_tex->tile_width,checkbox_tex->tile_height);
    }
}

int menu_checkbox_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    item->data = (void*)(!((int)item->data));
    return 0;
}

void menu_checkbox_set(menu_item_t *item, int set){
    if(set > 0){
        set = 1;
    }else{
        set = 0;
    }
    item->data = (void*)set;
}

menu_item_t *menu_checkbox_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        item->data = (void*)0;
        item->draw_proc = menu_checkbox_draw;
        item->interactive = 1;
        menu_item_register_event(item, MENU_EVENT_ACTIVATE, menu_checkbox_onactivate, (void*)false);
    }
    return item;
}