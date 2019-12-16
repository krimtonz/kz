#include "menu.h"

static void menu_submenu_activate(struct menu_item *item){
    menu_enter(item->owner,(struct menu*)item->data);
}

struct menu_item *menu_add_submenu(struct menu *menu, uint16_t x, uint16_t y, struct menu *submenu, char *name){
    struct menu_item *item = menu_add(menu,x,y,name);
    if(item){
        item->activate_proc = menu_submenu_activate;
        item->data = submenu;
        item->interactive = 1;
        item->x = x;
        item->y = y;
    }
    return item;
}