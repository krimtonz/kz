#include <stdlib.h>
#include "menu.h"

struct menu_data_watch{
    uint32_t address;
    watch_type type;
};

void menu_init(struct menu *menu){
    list_init(&menu->items, sizeof(struct menu_item));
    menu->child=NULL;
    menu->parent=NULL;
    menu->selected_item=NULL;
}

void menu_draw_watch(struct menu_item *item){
    struct menu_data_watch *watch = (struct menu_data_watch*)item->data;
    int x = item->x;
    int y = item->y;
    switch(watch->type){
        case WATCH_TYPE_U8:
            gfx_printf(x,y,"%u",*(uint8_t*)watch->address);
            break;
        case WATCH_TYPE_S8:
            gfx_printf(x,y,"%d",*(int8_t*)watch->address);
            break;
        case WATCH_TYPE_X8:
            gfx_printf(x,y,"%1X",*(uint8_t*)watch->address);
            break;
        case WATCH_TYPE_U16:
            gfx_printf(x,y,"%u",*(uint16_t*)watch->address);
            break;
        case WATCH_TYPE_S16:
            gfx_printf(x,y,"%d",*(int16_t*)watch->address);
            break;
        case WATCH_TYPE_X16:
            gfx_printf(x,y,"%2X",*(uint16_t*)watch->address);
            break;
        case WATCH_TYPE_U32:
            gfx_printf(x,y,"%u",*(uint32_t*)watch->address);
            break;
        case WATCH_TYPE_S32:
            gfx_printf(x,y,"%d",*(int32_t*)watch->address);
            break;
        case WATCH_TYPE_X32:
            gfx_printf(x,y,"%4X",*(uint32_t*)watch->address);
            break;
        case WATCH_TYPE_FLOAT:
            gfx_printf(x,y,"%f",*(float*)watch->address);
            break;
    }
}

void menu_draw(struct menu *menu){
    if(menu->child){
        menu_draw(menu->child);
        return;
    }
    int x = 0;
    int y = 0;
    for(struct menu_item *item = menu->items.first; item; item = list_next(item)){
        y+=10;
        if(item->draw_proc){
            item->y = y;
            item->x = x;
            item->draw_proc(item);
            continue;
        }
        uint32_t color = GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF);
        if(item == menu->selected_item){
            color = GPACK_RGBA8888(0x80,0x80,0xFF,0xFF);
        }
        gfx_printf_color(x,y,color,"%s",item->text);
    }
}

struct menu_item *menu_add(struct menu *menu, const char *text){
    struct menu_item *item = list_push_back(&menu->items,NULL);
    if(item){
        item->text = text;
        item->owner = menu;
    }
    return item;
}

void menu_submenu_activate(struct menu_item *item){
    item->owner->child = (struct menu*)item->data;
    ((struct menu*)item->data)->parent = item->owner;
}

struct menu_item *menu_add_submenu(struct menu *menu, struct menu *submenu, const char *name){
    struct menu_item *item = menu_add(menu,name);
    if(item){
        item->activate_proc = menu_submenu_activate;
        item->data = submenu;
    }
    return item;
}

struct menu_item *menu_add_button(struct menu *menu, const char *name, menu_button_callback callback, void *data){
    struct menu_item *item = menu_add(menu,name);
    if(item){
        item->activate_proc = callback;
        item->data = data;
    }
    return item;
}

struct menu_item *menu_add_watch(struct menu *menu, uint32_t address, watch_type type){
    struct menu_item *item = menu_add(menu,NULL);
    if(item){
        struct menu_data_watch *data = malloc(sizeof(*data));
        data->address = address;
        data->type = type;
        item->data = data;
        item->draw_proc = menu_draw_watch;
    }
    return item;
}

void menu_navigate(struct menu *menu, enum menu_nav nav){
    if(nav==MENU_NAV_NONE) return;
    if(menu->child){
        menu_navigate(menu->child,nav);
        return;
    }
    if(menu->selected_item && menu->selected_item->navigate_proc && menu->selected_item->navigate_proc(menu->selected_item,nav))
        return;
    if(nav==MENU_NAV_DOWN){
        menu->selected_item = list_next(menu->selected_item);
        if(!menu->selected_item) menu->selected_item = menu->items.first;
    }else if(nav==MENU_NAV_UP){
        menu->selected_item = list_prev(menu->selected_item);
        if(!menu->selected_item) menu->selected_item = menu->items.last;
    }
}

void menu_callback(struct menu *menu, enum menu_callback callback){
    if(callback==MENU_CALLBACK_NONE) return;
    if(menu->child){
        menu_callback(menu->child,callback);
        return;
    }
    if(menu->selected_item->activate_proc)
        menu->selected_item->activate_proc(menu->selected_item);
}

void menu_return(struct menu_item *item){
    if(item->owner->parent){
        item->owner->parent->child = NULL;
    }
}