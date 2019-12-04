#include <stdlib.h>
#include "menu.h"
#include "resource.h"

struct item_data {
    menu_generic_callback   callback;
    void                   *callback_data;
    _Bool                   status;
};

static void draw_checkbox_proc(struct menu_item *item){
    struct item_data *data = item->data;
    gfx_texture *checkbox_tex = resource_get(R_KZ_CHECKBOX);
    int x = get_item_x_pos(item);
    int y = get_item_y_pos(item);
    uint32_t color = 0xFFFFFFFF;
    if(item->owner->selected_item == item){
        color = MENU_SELECTED_COLOR.color;
    }
    gfx_draw_sprite_color(checkbox_tex,x,y,0,checkbox_tex->tile_width,checkbox_tex->tile_height - 1,color);
    if(data->status){
        gfx_draw_sprite(checkbox_tex,x,y,1,checkbox_tex->tile_width,checkbox_tex->tile_height);
    }
}

static void activate_checkbox_proc(struct menu_item *item){
    struct item_data *data = item->data;
    data->status = !data->status;
    if(data->callback){
        data->callback(item,MENU_CALLBACK_ACTIVATE,data->callback_data);
    }
}

static void update_checkbox_proc(struct menu_item *item){
    struct item_data *data = item->data;
    if(data->callback){
        data->callback(item,MENU_CALLBACK_UPDATE,data->callback_data);
    }
}

void menu_checkbox_set(struct menu_item *item, _Bool status){
    struct item_data *data = item->data;
    data->status = status;
}

struct menu_item *menu_add_checkbox(struct menu *menu, uint16_t x, uint16_t y,
                                    menu_generic_callback callback, void *callback_data, char *tooltip){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->callback = callback;
        data->callback_data = callback_data;
        data->status = 0;
        item->data = data;
        item->interactive = 1;
        item->tooltip = tooltip;
        item->draw_proc = draw_checkbox_proc;
        item->activate_proc = activate_checkbox_proc;
        item->update_proc = update_checkbox_proc;
    }
    return item;
}