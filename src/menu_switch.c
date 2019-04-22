#include <stdlib.h>
#include "menu.h"

struct item_data{
    void                   *addr;
    uint8_t                 addr_len;
    uint32_t                bitmask;
    menu_button_callback    callback;
};

static void draw_menu_switch(struct menu_item *item){
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
    uint32_t color = 0xFFFFFFFF;
    if(item->owner->selected_item == item){
        color = MENU_SELECTED_COLOR;
    }
    else if(val) color = COLOR_GREEN;
    gfx_printf_color(get_item_x_pos(item),get_item_y_pos(item),color,"%s",item->text);

}

void activate_menu_switch(struct menu_item *item){
    struct item_data *data = item->data;
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
    if(data->callback){
        data->callback(item);
    }
}

struct menu_item *menu_add_switch(struct menu *menu, uint16_t x, uint16_t y, 
                                  void *addr, uint8_t addr_len, 
                                  uint32_t bitmask, menu_button_callback callback,
                                  const char *text){
    struct menu_item *item = menu_add(menu,x,y,text);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->addr = addr;
        data->addr_len = addr_len;
        data->bitmask = bitmask;
        data->callback = callback;
        item->data = data;
        item->draw_proc = draw_menu_switch;
        item->activate_proc = activate_menu_switch;
        item->interactive = 1;
    }
    return item;
}