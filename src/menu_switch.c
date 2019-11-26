#include <stdlib.h>
#include "menu.h"
#include "resource.h"

struct item_data {
    menu_generic_callback   callback;
    void                   *callback_data;
    gfx_texture            *on_texture;
    gfx_texture            *off_texture;
    uint32_t                on_color;
    uint32_t                off_color;
    int                     width;
    int                     height;
    _Bool                   status;
    uint8_t                 on_tile;
    uint8_t                 off_tile;
};

static void draw_switch_proc(struct menu_item *item){
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
    int x = get_item_x_pos(item);
    int y = get_item_y_pos(item);
    if(item->owner->selected_item == item){
        gfx_draw_rectangle(x,y,data->width,data->height,MENU_SELECTED_COLOR.color);
    }

    gfx_draw_sprite_color(texture,x,y,
                                tile,data->width,data->height, color);
}

static void activate_switch_proc(struct menu_item *item){
    struct item_data *data = item->data;
    data->status = !data->status;
    if(data->callback){
        data->callback(item,MENU_CALLBACK_ACTIVATE,data->callback_data);
    }
}

static void update_switch_proc(struct menu_item *item){
    struct item_data *data = item->data;
    if(data->callback){
        data->callback(item,MENU_CALLBACK_UPDATE,data->callback_data);
    }
}

void menu_switch_set(struct menu_item *item, _Bool status){
    struct item_data *data = item->data;
    data->status = status;
}

struct menu_item *menu_add_switch(struct menu *menu, uint16_t x, uint16_t y,
                                  menu_generic_callback callback, void *callback_data,
                                  gfx_texture *on_texture, gfx_texture *off_texture,
                                  uint32_t on_color, uint32_t off_color,
                                  uint8_t on_tile, uint8_t off_tile,
                                  int width, int height, char *tooltip){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->callback = callback;
        data->callback_data = callback_data;
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
        item->draw_proc = draw_switch_proc;
        item->activate_proc = activate_switch_proc;
        item->update_proc = update_switch_proc;
    }
    return item;
}