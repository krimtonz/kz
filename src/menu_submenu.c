#include "menu.h"

void menu_submenu_activate(struct menu_item *item){
    item->owner->child = (struct menu*)item->data;
    ((struct menu*)item->data)->parent = item->owner;
    if(item->owner->child->callback_proc){
        item->owner->child->callback_proc(MENU_CALLBACK_ENTER);
    }
}

struct menu_item *menu_add_submenu(struct menu *menu, uint16_t x, uint16_t y, struct menu *submenu, const char *name){
    struct menu_item *item = menu_add(menu,x,y,name);
    if(item){
        item->activate_proc = menu_submenu_activate;
        item->data = submenu;
        item->interactive = 1;
        item->x = x;
        item->y = y;
    }
    submenu->x = menu->x;
    submenu->y = menu->y;
    return item;
}