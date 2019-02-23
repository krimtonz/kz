#ifndef _MENU_H
#define _MENU_H
#include <list/list.h>
#include "gfx.h"

enum menu_nav{
    MENU_NAV_NONE = -1,
    MENU_NAV_UP,
    MENU_NAV_DOWN,
    MENU_NAV_LEFT,
    MENU_NAV_RIGHT,
};

enum menu_callback{
    MENU_CALLBACK_NONE = -1,
    MENU_CALLBACK_ACTIVATE,
};

typedef void (*menu_button_callback)(struct menu_item *item);

struct menu_item{
    struct menu *owner;
    void (*draw_proc)(struct menu_item *item);
    void (*activate_proc)(struct menu_item *item);
    const char *text;
    void *data;
    int x;
    int y;
};

struct menu{
    struct list items;
    struct menu_item *selected_item;
    struct menu *child;
    struct menu *parent;
};

void menu_init(struct menu *menu);
void menu_draw(struct menu *menu);

struct menu_item *menu_add(struct menu *menu, const char *text);
struct menu_item *menu_add_submenu(struct menu *menu, struct menu *submenu, const char *name);
struct menu_item *menu_add_button(struct menu *menu, const char *text, menu_button_callback callback, void *data);
void menu_navigate(struct menu *menu, enum menu_nav nav);
void menu_callback(struct menu *menu, enum menu_callback callback);
void menu_return(struct menu_item *item);
#endif