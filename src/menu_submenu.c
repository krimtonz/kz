#include "menu.h"

int menu_submenu_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    *event_data = handler->callback_data;
    menu_trigger_event(item->owner, MENU_EVENT_ENTER, event_data);
    return 1;
}

menu_item_t *menu_submenu_add(menu_t *menu, uint16_t cell_x, uint16_t cell_y, char *text, menu_t *submenu){
    menu_item_t *item = menu_add(menu, cell_x, cell_y);
    if(item){
        item->text = text;
        item->interactive = 1;
        menu_item_register_event(item, MENU_EVENT_ACTIVATE, menu_submenu_onactivate, submenu);
    }
    return item;
}