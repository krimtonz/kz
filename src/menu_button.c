#include <stdlib.h>

#include "menu.h"

struct item_data{
    menu_button_callback    callback;
    void                   *callback_data;
};

static void button_activate(struct menu_item *item){
    struct item_data *data = item->data;
    if(data->callback){
        data->callback(item,data->callback_data);
    }
}

struct menu_item *menu_add_button(struct menu *menu, uint16_t x, uint16_t y, const char *name, menu_button_callback callback, void *data){
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