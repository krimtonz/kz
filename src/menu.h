#ifndef _MENU_H
#define _MENU_H
#include <list/list.h>
#include "gfx.h"
#include "watches.h"

#define MENU_SELECTED_COLOR GPACK_RGBA8888(0x80,0x80,0xFF,0xFF);
#define MENU_DEFAULT_COLOR GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF);

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
    MENU_CALLBACK_ENTER,
    MENU_CALLBACK_EXIT,
};

struct menu_item{
    struct menu        *owner;
    void              (*draw_proc)(struct menu_item *item);
    void              (*activate_proc)(struct menu_item *item);
    int               (*navigate_proc)(struct menu_item *item, enum menu_nav nav);
    const char         *text;
    void               *data;
    uint16_t            x;
    uint16_t            y;
    uint8_t             interactive;
};

struct menu{
    struct list         items;
    struct menu_item   *selected_item;
    struct menu        *child;
    struct menu        *parent;
    void              (*callback_proc)(enum menu_callback callback);
    uint16_t            x;
    uint16_t            y;
    uint16_t            cell_width;
    uint16_t            cell_height;
    uint16_t            x_padding;
    uint16_t            y_padding;
};

typedef void (*menu_button_callback)(struct menu_item *item);
typedef void (*menu_number_callback)(struct menu_item *item, void *data, uint32_t value);

void menu_init(struct menu *menu, uint16_t x, uint16_t y);
void menu_draw(struct menu *menu);
void menu_set_cell(struct menu *menu, uint16_t width, uint16_t height);
void menu_set_padding(struct menu *menu, uint16_t x, uint16_t y);

struct menu_item *menu_add(struct menu *menu, uint16_t x, uint16_t y, const char *text);
struct menu_item *menu_add_submenu(struct menu *menu, uint16_t x, uint16_t y, struct menu *submenu, const char *name);
struct menu_item *menu_add_button(struct menu *menu, uint16_t x, uint16_t y, const char *text, menu_button_callback callback, void *data);
struct menu_item *menu_add_watch(struct menu *menu, uint16_t x, uint16_t y, watch_t *watch);
struct menu_item *menu_add_number_input(struct menu* menu, uint16_t x, uint16_t y, menu_number_callback callback, void *callback_data, uint8_t base, uint8_t length, int initial);

void menu_navigate(struct menu *menu, enum menu_nav nav);
void menu_callback(struct menu *menu, enum menu_callback callback);
void menu_return(struct menu_item *item);

int get_item_x_pos(struct menu_item *item);
int get_item_y_pos(struct menu_item *item);

#endif