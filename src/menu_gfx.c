#include <stdlib.h>
#include "menu.h"
#include "gfx.h"
#include "resource.h"

struct item_data {
    gfx_texture    *texture;
    uint16_t        width;
    uint16_t        height;
    uint16_t        tile;
};

static void menu_gfx_draw(menu_item_t *item){
    struct item_data *data = item->data;
    gfx_draw_sprite_color(data->texture, menu_item_x(item), menu_item_y(item), data->tile, data->width, data->height, item->color);
}

menu_item_t *menu_gfx_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell, gfx_texture *texture, int tile, int width, int height){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        if(data){
            data->texture = texture;
            data->width = width;
            data->height = height;
            data->tile = tile;
            item->data = data;
        }
        item->draw_proc = menu_gfx_draw;
    }
    return item;
}