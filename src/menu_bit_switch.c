#include <stdlib.h>
#include "menu.h"

struct bit_switch_data{
    void                   *addr;
    uint8_t                 addr_len;
    uint32_t                bitmask;
    gfx_texture            *texture;
    uint16_t                tex_width;
    uint16_t                tex_height;
    uint8_t                 tile;
    _Bool                   has_off_tile;
    uint32_t                color;
    uint32_t                off_color;
    const char             *tooltip;
    menu_button_callback    callback;
};

static void draw_bit_switch_proc(struct menu_item *item){
    struct bit_switch_data *data = item->data;
    gfx_texture *texture = data->texture;
    if(item->owner->selected_item == item){
        gfx_draw_rectangle(get_item_x_pos(item),get_item_y_pos(item),data->tex_width,data->tex_height,GPACK_RGBA8888(0x80,0x80,0xFF,0x80));
    }
    gfx_push(gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM));
    uint32_t color = data->color;
    gfx_push(gsDPSetPrimColor(0,0,(color >> 24) & 0xFF,(color >> 16) & 0xFF,(color >> 8) & 0xFF,color & 0xFF));
    gfx_push(gsDPPipeSync());
    uint8_t tile = data->tile;
    uint32_t val = 0;
    switch(data->addr_len){
        case 1:
            val = *(uint8_t*)data->addr;
            break;
        case 2:
            val = *(uint16_t*)data->addr;
            break;
        case 4:
        default:
            val = *(uint32_t*)data->addr;
            break;
    }
    if(!(val & data->bitmask)){
        if(data->has_off_tile){
            size_t tiles_cnt = texture->x_tiles * texture->y_tiles;
            if(tiles_cnt==1) tile++;
            else{
                tile = tile + (texture->x_tiles * texture->y_tiles)/2;
            }
        }else{
            uint32_t off_color = data->off_color;
            gfx_push(gsDPSetPrimColor(0,0,(off_color >> 24) & 0xFF,(off_color >> 16) & 0xFF,(off_color >> 8) & 0xFF,off_color & 0xFF));
        }
    }
    gfx_draw_sprite(texture,get_item_x_pos(item),get_item_y_pos(item),tile,data->tex_width,data->tex_height);
}

static void activate_bit_switch_proc(struct menu_item *item){
    struct bit_switch_data *data = item->data;
    switch(data->addr_len){
        case 1:
            *(uint8_t*)(data->addr) = *(uint8_t*)(data->addr) ^ data->bitmask;
            break;
        case 2:
            *(uint16_t*)(data->addr) = *(uint16_t*)(data->addr) ^ data->bitmask;
            break;
        case 4:
        default:
            *(uint32_t*)(data->addr) = *(uint32_t*)(data->addr) ^ data->bitmask;
            break;
    }
    if(data->callback){
        data->callback(item);
    }
}

struct menu_item *menu_add_bit_switch(struct menu *menu, uint16_t x, uint16_t y, 
                                            void *addr, uint8_t addr_len, uint32_t bitmask,  
                                            menu_button_callback callback, gfx_texture *texture, 
                                            uint16_t tex_width, uint16_t tex_height, int8_t tile, 
                                            _Bool has_off_tile, const char *tooltip,
                                            uint32_t on_color, uint32_t off_color){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct bit_switch_data *data = malloc(sizeof(*data));
        data->addr = addr;
        data->addr_len = addr_len;
        data->bitmask = bitmask;
        data->texture = texture;
        data->tile = tile;
        data->tex_width = tex_width;
        data->tex_height = tex_height;
        data->has_off_tile = has_off_tile;
        data->color = on_color;
        data->off_color = off_color;
        data->tooltip = tooltip;
        data->callback = callback;
        item->draw_proc = draw_bit_switch_proc;
        item->activate_proc = activate_bit_switch_proc;
        item->interactive = 1;
        item->data = data;
    }
    return item;
}