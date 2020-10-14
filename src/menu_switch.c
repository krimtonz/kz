#include <stdlib.h>
#include "menu.h"

struct item_data {
    gfx_texture            *on_texture;
    gfx_texture            *off_texture;
    uint32_t                on_color;
    uint32_t                off_color;
    int                     width;
    int                     height;
    int                     status;
    uint8_t                 on_tile;
    uint8_t                 off_tile;
};

static void menu_switch_draw(menu_item_t *item){
    struct item_data *data = item->data;
    gfx_texture *texture = data->on_texture;
    int tile = data->on_tile;
    uint32_t color = data->on_color;
    if(!data->status){
        if(data->off_texture){
            texture = data->off_texture;
       }
       tile = data->off_tile;
       color = data->off_color;
    }
    int x = menu_item_x(item);
    int y = menu_item_y(item);
    if(item->owner->selected_item == item){
        gfx_draw_rectangle(x, y, data->width, data->height, SELECTED_COLOR);
    }

    gfx_draw_sprite_color(texture, x, y, tile,data->width, data->height, color);
}

static int menu_switch_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    struct item_data *data = item->data;
    data->status = !data->status;
    return 0;
}

void menu_switch_set(menu_item_t *item, int status){
    struct item_data *data = item->data;
    data->status = status;
}

menu_item_t *menu_switch_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell,
                                  gfx_texture *on_texture, gfx_texture *off_texture,
                                  uint32_t on_color, uint32_t off_color,
                                  uint8_t on_tile, uint8_t off_tile,
                                  int width, int height, char *tooltip){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->on_texture = on_texture;
        data->off_texture = off_texture;
        data->on_color = on_color;
        data->off_color = off_color;
        data->on_tile = on_tile;
        data->off_tile = off_tile;
        data->width = width;
        data->height = height;
        data->status = 0;
        item->data = data;
        item->interactive = 1;
        item->tooltip = tooltip;
        item->draw_proc = menu_switch_draw;
        menu_item_register_event(item, MENU_EVENT_ACTIVATE, menu_switch_onactivate, NULL);
    }
    return item;
}