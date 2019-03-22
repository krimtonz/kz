#include "menu.h"
#include "kz.h"

void watch_update_callback(struct menu_item *item, void *data, uint32_t value){
    watch_t *watch = data;
    watch->address = (void*)value;
}

void menu_watch_delete(struct menu_item *item){
    
}

void menu_watch_anchor(struct menu_item *item){
    watch_t *watch = item->data;
    watch->floating = !watch->floating;
}

void menu_watch_move(struct menu_item *item){
    
}

void watches_button_add(struct menu_item *item){
    watch_t *watch = vector_push_back(&kz.watches,1,NULL);
    watch->address = (void*)0x80000000;
    watch->type = WATCH_TYPE_X32;

    /* Add New Watch Row */
    int x = item->x;
    menu_add_button(item->owner,x,item->y,"X",menu_watch_delete,watch);
    menu_add_button(item->owner,x+1,item->y,"V",menu_watch_anchor,watch);
    menu_add_button(item->owner,x+2,item->y,"<>",menu_watch_move,watch);
    menu_add_number_input(item->owner,x+4,item->y,watch_update_callback,watch,16,8,0x80000000);
    menu_add_watch(item->owner,x + 15,item->y,watch);

    kz.watch_cnt++;
    item->y++;

}
struct menu *create_watches_menu(){
    static struct menu watches;
    watches.selected_item = menu_add_button(&watches,0,0,"return",menu_return,NULL);
    menu_add_button(&watches,0,1,"+",watches_button_add,NULL);
    kz.main_menu.selected_item = kz.main_menu.items.first;
    return &watches;
}