#include <stdlib.h>
#include "menu.h"

struct item_data{
    void                   *addr;
    uint8_t                 addr_len;
    uint32_t                bitmask;
    menu_switch_callback    callback;
    void                   *callback_data;
    draw_info_t             draw_info;
};

void draw_menu_switch(struct menu_item *item){
    struct item_data *data = item->data;
    uint32_t val = 0;
    switch(data->addr_len){
        case 1:
            val = *(uint8_t*)data->addr & data->bitmask;
            break;
        case 2:
            val = *(uint16_t*)data->addr & data->bitmask;
            break;
        case 4:
        default:
            val = *(uint32_t*)data->addr & data->bitmask;
            break;
    }
    z2_rgba32_t color = data->draw_info.disabled_color;
    int tile = data->draw_info.off_tile;
    if(item->owner->selected_item == item){
        color = MENU_SELECTED_COLOR;
    }
    else if(val){
        color = data->draw_info.enabled_color;
    }
    tile = val?data->draw_info.on_tile:data->draw_info.off_tile;
    if(data->draw_info.background){
        gfx_push(gsDPSetPrimColor(0,0,color.r,color.g,color.b,color.a));
        gfx_draw_sprite_scale(data->draw_info.background->texture,get_item_x_pos(item),get_item_y_pos(item),0,data->draw_info.width,data->draw_info.height,data->draw_info.x_scale,data->draw_info.y_scale);
    }

    if(data->draw_info.texture==NULL){
        gfx_printf_color(get_item_x_pos(item),get_item_y_pos(item),color.color,"%s",item->text);
    }else{
        if(tile<0) return;
        if(!data->draw_info.use_selected_color){
            color = val?data->draw_info.enabled_color : data->draw_info.disabled_color;
        }
        gfx_push(gsDPSetPrimColor(0,0,color.r,color.g,color.b,color.a));
        gfx_draw_sprite_scale(data->draw_info.texture,get_item_x_pos(item),get_item_y_pos(item),tile,data->draw_info.width,data->draw_info.height,data->draw_info.x_scale,data->draw_info.y_scale);
    }

}

void activate_menu_switch(struct menu_item *item){
    struct item_data *data = item->data;
    if(data->callback && data->callback(item,NULL,MENU_CALLBACK_ACTIVATE)){
        return;
    }
    switch(data->addr_len){
        case 1:
            *(uint8_t*)data->addr = *(uint8_t*)data->addr ^ data->bitmask;
            break;
        case 2:
            *(uint16_t*)data->addr = *(uint16_t*)data->addr ^ data->bitmask;
            break;
        case 4:
        default:
            *(uint32_t*)data->addr = *(uint32_t*)data->addr ^ data->bitmask;
            break;
    }
}

static void update_menu_switch(struct menu_item *item){
    struct item_data *data = item->data;
    if(data->callback){
        data->callback(item,data->callback_data,MENU_CALLBACK_UPDATE);
    }
}

struct menu_item *menu_add_switch(struct menu *menu, uint16_t x, uint16_t y,
                                  void *addr, uint8_t addr_len,
                                  uint32_t bitmask, menu_switch_callback callback,
                                  const char *text){
    struct menu_item *item = menu_add(menu,x,y,text);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->addr = addr;
        data->addr_len = addr_len;
        data->bitmask = bitmask;
        data->callback = callback;
        data->draw_info.texture=NULL;
        data->draw_info.enabled_color.color = COLOR_GREEN;
        data->draw_info.disabled_color = MENU_DEFAULT_COLOR;
        item->data = data;
        item->draw_proc = draw_menu_switch;
        item->activate_proc = activate_menu_switch;
        item->update_proc = update_menu_switch;
        item->interactive = 1;
    }
    return item;
}

struct menu_item *menu_add_gfx_switch(struct menu *menu, uint16_t x, uint16_t y,
                                  void *addr, uint8_t addr_len,
                                  uint32_t bitmask, menu_switch_callback callback,
                                  void *callback_data, draw_info_t *drawinfo){

    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->addr = addr;
        data->addr_len = addr_len;
        data->bitmask = bitmask;
        data->callback = callback;
        data->callback_data = callback_data;
        memcpy(&data->draw_info,drawinfo,sizeof(*drawinfo));
        item->data = data;
        item->draw_proc = draw_menu_switch;
        item->activate_proc = activate_menu_switch;
        item->update_proc = update_menu_switch;
        item->interactive = 1;

    }
    return item;
}