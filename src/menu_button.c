#include "menu.h"

struct menu_item *menu_add_button(struct menu *menu, uint16_t x, uint16_t y, const char *name, menu_button_callback callback, void *data){
    struct menu_item *item = menu_add(menu,x,y,name);
    if(item){
        item->activate_proc = callback;
        item->data = data;
        item->interactive = 1;
    }
    return item;
}