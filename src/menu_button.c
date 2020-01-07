#include <stdlib.h>
#include "menu.h"
#include "resource.h"

static void menu_gfx_button_draw(menu_item_t *item){
    menu_sprite_t *sprite = item->data;
    uint32_t color = sprite->on_color;
    if(item->owner->selected_item == item){
        color = SELECTED_COLOR;
    }
    gfx_draw_sprite_color(sprite->texture, menu_item_x(item), menu_item_y(item), sprite->on_tile, sprite->width, sprite->height, color);
}

menu_item_t *menu_button_add(menu_t *menu, uint16_t cell_x, uint16_t cell_y, char *text, menu_item_event_cb_t on_activate, void *activate_data){
    menu_item_t *item = menu_add(menu, cell_x, cell_y);
    if(item){
        item->text = text;
        item->interactive = 1;
        if(on_activate){
            menu_item_register_event(item, MENU_EVENT_ACTIVATE, on_activate, activate_data);
        }
    }
    return item;
}

menu_sprite_t *menu_button_sprite_get(menu_item_t *item){
    return (menu_sprite_t*)item->data;
}

menu_item_t *menu_gfx_button_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell, menu_sprite_t *sprite, menu_item_event_cb_t on_activate, void *activate_data){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        item->interactive = 1;
        item->data = sprite;
        item->draw_proc = menu_gfx_button_draw;
        if(on_activate){
            menu_item_register_event(item, MENU_EVENT_ACTIVATE, on_activate, activate_data);
        }
    }
    return item;
}