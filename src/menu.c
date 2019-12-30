#include "menu.h"
#include "kz.h"
#include "resource.h"

#define DEFAULT_CELL_SIZE 8

menu_sprite_t   *scroll_up_sprite = NULL;
menu_sprite_t   *scroll_down_sprite = NULL;

static void menu_navigate(menu_t *menu, menu_nav_t nav){
    if(nav == MENU_NAV_NONE) return;

    if(menu->child){
        menu_navigate(menu->child,nav);
        return;
    }

    if(menu->selected_item && menu_item_trigger_event(menu->selected_item, MENU_EVENT_NAVIGATE, (void*)nav))
        return;

    int dir_horiz = nav == MENU_NAV_LEFT ? -1 : (nav == MENU_NAV_RIGHT ? 1 : 0);
    int dir_vert = nav == MENU_NAV_UP ? -1 : (nav == MENU_NAV_DOWN ? 1 : 0);

    int ndist_pa = 0;
    int ndist_pe = 0;
    int fdist_pa = 0;
    int fdist_pe = 0;
    int cur_x_pos = menu_item_x(menu->selected_item);
    int cur_y_pos = menu_item_y(menu->selected_item);

    menu_item_t *near = NULL;
    menu_item_t *far = NULL;

    for(menu_item_t *item = menu->items.first;item;item = list_next(item)){
        if(!item->interactive || !item->enabled) continue;
        int distance_x = menu_item_x(item) - cur_x_pos;
        int distance_y = menu_item_y(item) - cur_y_pos;

        int pa = dir_horiz ? dir_horiz * distance_x : (dir_vert ? dir_vert * distance_y : 0);
        int pe = dir_vert ? distance_x : distance_y;

        if(pe < 0){
            pe = -pe;
        }

        if(pa > 0 && (near == NULL || pe < ndist_pe || (pe == ndist_pe && pa < ndist_pa))){
            ndist_pa = pa;
            ndist_pe = pe;
            near = item;
        }

        if(pa < 0 && (far == NULL || -pa > fdist_pa || (-pa == fdist_pa && pe < fdist_pe))){
            far = item;
            fdist_pa = -pa;
            fdist_pe = pe;
        }
    }

    void *event_data = NULL;
    if((near || far) && menu->selected_item){
        
        menu_item_trigger_event(menu->selected_item, MENU_EVENT_EXIT, &event_data);
    }

    if(near){

        menu_item_trigger_event(near, MENU_EVENT_ENTER, &event_data);
        menu->selected_item = near;
    }else if(far){
        menu_item_trigger_event(far, MENU_EVENT_ENTER, &event_data);
        menu->selected_item = far;
    }
}

void menu_static_sprites_init(){
    static menu_sprite_t up_sprite = {
        NULL,   0,      0,      DEFAULT_COLOR,  DEFAULT_COLOR,
        8,      8,      NULL,   DEFAULT_COLOR,   0,
        0,      NULL,
    };

    static menu_sprite_t down_sprite = {
        NULL,   1,      0,      DEFAULT_COLOR,  DEFAULT_COLOR,
        8,      8,      NULL,   DEFAULT_COLOR,   0,
        0,      NULL,
    };

    up_sprite.texture = resource_get(R_KZ_ARROWS);
    down_sprite.texture = resource_get(R_KZ_ARROWS);

    scroll_up_sprite = &up_sprite;
    scroll_down_sprite = &down_sprite;
}

void menu_init(menu_t *menu, int16_t x_offset, int16_t y_offset){
    list_init(&menu->items,sizeof(menu_item_t));
    list_init(&menu->events,sizeof(event_handler_t));
    list_init(&menu->item_events,sizeof(event_handler_t));
    menu->x_offset = x_offset;
    menu->y_offset = y_offset;
    menu->selected_item = NULL;
    menu->child = NULL;
    menu->parent = NULL;
    menu->x_padding = 0;
    menu->y_padding = 0;
    menu->cell_width = DEFAULT_CELL_SIZE;
    menu->cell_height = DEFAULT_CELL_SIZE;
}

int menu_x(menu_t *menu){
    int x = 0;
    menu_t *menu_p = menu;
    while(menu_p){
        x += menu_p->y_offset;
        menu_p = menu_p->parent;
    }
    return x;
}

int menu_y(menu_t *menu){
    int y = 0;
    menu_t *menu_p = menu;
    while(menu_p){
        y += menu_p->y_offset;
        menu_p = menu_p->parent;
    }
    return y;
}

int menu_item_x(menu_item_t *item){
    return menu_x(item->owner) + (item->x_cell * item->owner->cell_width) + (item->x_cell * item->owner->x_padding) + item->x_offset;
}

int menu_item_y(menu_item_t *item){
    return menu_y(item->owner) + (item->y_cell * item->owner->cell_height) + (item->y_cell * item->owner->y_padding) + item->y_offset;
}

void menu_draw(menu_t *menu){
    if(menu->child){
        menu_draw(menu->child);
        return;
    }
    for(menu_item_t *item = menu->items.first;item;item = list_next(item)){
        if(!item->enabled) continue;
        if(item->draw_proc){
            item->draw_proc(item);
            continue;
        }
        uint32_t color = item->color;
        if(item == menu->selected_item){
            color = SELECTED_COLOR;
        }
        gfx_printf_color(menu_item_x(item), menu_item_y(item), color, "%s", item->text);
    }
}

void menu_item_remove(menu_item_t *item){
    menu_t *menu = item->owner;
    event_handler_t *handler = menu->item_events.first;
    if(handler){
        event_handler_t *next = list_next(handler);
        while(next){
            if(handler->subscriber == item){
                list_erase(&menu->item_events, handler);
            }
            handler = next;
            next = list_next(handler);
        }
    }
    if(item->remove_proc){
        item->remove_proc(item);
    }
    list_erase(&menu->items, item);
}

/* event functions */
event_handler_t *menu_register_event(menu_t *menu, menu_event_t event, menu_event_cb_t callback, void *callback_data){
    event_handler_t *handler = list_push_back(&menu->events, NULL);
    if(handler){
        handler->event = event;
        handler->callback = callback;
        handler->callback_data = callback_data;
        handler->subscriber = menu;
    }
    return handler;
}

event_handler_t *menu_item_register_event(menu_item_t *item, menu_event_t event, menu_item_event_cb_t callback, void *callback_data){
    event_handler_t *handler = list_push_back(&item->owner->item_events, NULL);
    if(handler){
        handler->event = event;
        handler->callback = callback;
        handler->callback_data = callback_data;
        handler->subscriber = item;
    }
    return handler;
}

int menu_item_trigger_event(menu_item_t *item, menu_event_t event, void **event_data){
    // run all handlers for this event, and return if one of them considers it handled
    for(event_handler_t *handler = item->owner->item_events.first;handler;handler = list_next(handler)){
        if(handler->subscriber == item && (handler->event & event) == event && ((menu_item_event_cb_t)handler->callback)(handler, event, event_data)){
            return true;
        }
    }

    // default event handlers
    switch(event){
        case MENU_EVENT_ENTER:
        kz.tooltip = item->tooltip;
        return true;
        case MENU_EVENT_EXIT:
        kz.tooltip = NULL;
        return true;
        default:
        return false;
    }
}

int menu_trigger_event(menu_t *menu, menu_event_t event, void **event_data){
    // traverse menu tree and trigger events for children
    if(menu->child && menu_trigger_event(menu->child, event, event_data)){
        return true;
    }
    // run all handlers for this event, and return if the event is considered handled
    for(event_handler_t *handler = menu->events.first;handler;handler = list_next(handler)){
        if(handler->subscriber == menu && (handler->event & event) == event && ((menu_event_cb_t)handler->callback)(handler, event, event_data)){
            return true;
        }
    }

    // default event handlers
    switch(event){
        case MENU_EVENT_ACTIVATE:
        menu_item_trigger_event(menu->selected_item, MENU_EVENT_ACTIVATE, event_data);
        break;
        case MENU_EVENT_ENTER: {
            if(event_data){
                menu_t *submenu = *event_data;
                menu->child = submenu;
                submenu->parent = menu;
                menu_item_trigger_event(menu->selected_item, MENU_EVENT_EXIT, event_data);
                menu_item_trigger_event(submenu->selected_item, MENU_EVENT_ENTER, event_data);
                menu_trigger_event(menu, MENU_EVENT_EXIT, NULL);
            }
        }
        break;
        case MENU_EVENT_UPDATE:{
            for(menu_item_t *item = menu->items.first;item;item = list_next(item)){
                menu_item_trigger_event(item, MENU_EVENT_UPDATE, event_data);
            }
        }
        break;
        case MENU_EVENT_NAVIGATE:
        if(menu_item_trigger_event(menu->selected_item, MENU_EVENT_NAVIGATE, event_data)){
            return true;
        }
        menu_navigate(menu, (menu_nav_t)event_data);
        break;
        case MENU_EVENT_RETURN:{
            if(menu->parent){
                menu_trigger_event(menu, MENU_EVENT_EXIT, NULL);
                menu_item_trigger_event(menu->selected_item, MENU_EVENT_EXIT, event_data);
                menu_trigger_event(menu->parent, MENU_EVENT_ENTER, NULL);
                menu_item_trigger_event(menu->parent->selected_item, MENU_EVENT_ENTER, event_data);
                menu->parent->child = NULL;
            }
        }
        break;
        default:
        return false;
    }
    return true;
}

void menu_item_offset_set(menu_item_t *item, int16_t offset_x, int16_t offset_y){
    item->x_offset = offset_x;
    item->y_offset = offset_y;
}

int menu_return(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_trigger_event(&kz.main_menu, MENU_EVENT_RETURN, event_data);
    return 1;
}

void menu_cell_set(menu_t *menu, uint16_t cell_width, uint16_t cell_height){
    menu->cell_width = cell_width;
    menu->cell_height = cell_height;
}

void menu_padding_set(menu_t *menu, uint16_t x_padding, uint16_t y_padding){
    menu->x_padding = x_padding;
    menu->y_padding = y_padding;
}

menu_item_t *menu_label_add(menu_t *menu, uint16_t cell_x, uint16_t cell_y, void *text){
    menu_item_t *item = list_push_back(&menu->items,NULL);
    if(item){
        item->owner = menu;
        item->x_cell = cell_x;
        item->y_cell = cell_y;
        item->x_offset = 0;
        item->y_offset = 0;
        item->draw_proc = NULL;
        item->remove_proc = NULL;
        item->text = text;
        item->tooltip = NULL;
        item->data = NULL;
        item->interactive = false;
        item->enabled = true;
        item->text = text;
        item->color = DEFAULT_COLOR;
    }
    return item;
}