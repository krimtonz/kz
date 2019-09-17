#include <stdlib.h>
#include "menu.h"
#include "gfx.h"
#include "resource.h"

struct item_data{
    gfx_texture    *texture;
    uint16_t        width;
    uint16_t        height;
    uint16_t        tile;
    _Bool           load_item;
};

static void draw_menu_gfx(struct menu_item *item){
    struct item_data *data = item->data;
    gfx_texture *texture = data->texture;
    uint16_t tile = data->tile;
    if(data->load_item){
        texture = get_item_texture(tile);
        tile = 0;
    }
    gfx_push(gsDPSetCombine(G_CC_MODE(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM)));
    gfx_push(gsDPSetPrimColor(0,0,0xFF,0xFF,0xFF,0xFF));
    gfx_push(gsDPPipeSync());
    gfx_draw_sprite(texture,get_item_x_pos(item),get_item_y_pos(item),tile,data->width,data->height);
}

struct menu_item *menu_add_gfx(struct menu *menu, uint16_t x, uint16_t y, draw_info_t *drawinfo, _Bool load_item){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        if(data){
            data->texture = drawinfo->texture;
            data->width = drawinfo->width;
            data->height = drawinfo->height;
            data->tile = drawinfo->on_tile;
            data->load_item = load_item;
            item->data = data;
        }
        item->draw_proc = draw_menu_gfx;
        item->interactive = 0;
    }
    return item;
}