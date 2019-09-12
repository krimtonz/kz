#include <stdlib.h>

#include "menu.h"

struct item_data{
    menu_generic_callback   callback;
    void                   *callback_data;
};

struct gfx_item_data{
    menu_generic_callback   callback;
    void                   *callback_data;
    draw_info_t             draw_info;
};

static void button_activate(struct menu_item *item){
    struct item_data *data = item->data;
    if(data->callback){
        data->callback(item,MENU_CALLBACK_ACTIVATE,data->callback_data);
    }
}

static void draw_gfx_button(struct menu_item *item){
    struct gfx_item_data *data = item->data;
    z2_rgba32_t color = data->draw_info.enabled_color;
    if(item->owner->selected_item == item){
        color = MENU_SELECTED_COLOR;
    }
    gfx_push(gsDPSetPrimColor(0,0,color.r,color.g,color.b,color.a));
    gfx_draw_sprite(data->draw_info.texture,get_item_x_pos(item),get_item_y_pos(item),data->draw_info.on_tile,data->draw_info.width,data->draw_info.height);
}

struct menu_item *menu_add_button(struct menu *menu, uint16_t x, uint16_t y, const char *name, menu_generic_callback callback, void *data){
    struct menu_item *item = menu_add(menu,x,y,name);
    if(item){
        struct item_data *idata = malloc(sizeof(*idata));
        idata->callback = callback;
        idata->callback_data = data;
        item->activate_proc = button_activate;
        item->data = idata;
        item->interactive = 1;
    }
    return item;
}
struct menu_item *menu_add_gfx_button(struct menu *menu, uint16_t x, uint16_t y, menu_generic_callback callback, void *data, draw_info_t *drawinfo){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct gfx_item_data *idata = malloc(sizeof(*idata));
        idata->callback = callback;
        idata->callback_data = data;
        memcpy(&idata->draw_info,drawinfo,sizeof(*drawinfo));
        item->data = data;
        item->draw_proc = draw_gfx_button;
        item->activate_proc = button_activate;
        item->interactive = 1;
        item->data = idata;

    }
    return item;
}