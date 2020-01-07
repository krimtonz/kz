/*
* menu.h
*
* definitions for menu elements
*/

#ifndef _MENU_H
#define _MENU_H
#include <list/list.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "watches.h"
#include "gfx.h"

#define menu_item_disable(x)    (((menu_item_t*)x)->enabled = 0)
#define menu_item_enable(x)     (((menu_item_t*)x)->enabled = 1)
#define menu_add(x,y,z)         (menu_label_add(x, y, z, NULL))

#define MENU_EVENT_NUMBER   (0b1 << 8)
#define MENU_EVENT_KEYBOARD (0b1 << 7)
#define MENU_EVENT_LIST     (0b1 << 6)
#define MENU_EVENT_NAVIGATE (0b1 << 5)
#define MENU_EVENT_UPDATE   (0b1 << 4)
#define MENU_EVENT_RETURN   (0b1 << 3)
#define MENU_EVENT_EXIT     (0b1 << 2)
#define MENU_EVENT_ENTER    (0b1 << 1)
#define MENU_EVENT_ACTIVATE (0b1 << 0)

#define SELECTED_COLOR  0x8080FFFF
#define DEFAULT_COLOR   0xFFFFFFFF
#define COLOR_GREEN     0x00FF00FF
#define COLOR_RED       0xFF0000FF
#define COLOR_FADED     0x999999FF
#define COLOR_BLACK     0x000000FF

typedef enum {
    MENU_NAV_NONE = -1,
    MENU_NAV_UP,
    MENU_NAV_DOWN,
    MENU_NAV_LEFT,
    MENU_NAV_RIGHT,
} menu_nav_t;

typedef int                     menu_event_t;
typedef struct menu_s           menu_t;
typedef struct menu_item_s      menu_item_t;
typedef struct menu_sprite_s    menu_sprite_t;
typedef struct event_handler_s  event_handler_t;
typedef void*                   item_data_t;

typedef int  (*menu_event_cb_t)(event_handler_t *handler, menu_event_t event, void **event_data);
typedef int  (*menu_item_event_cb_t)(event_handler_t *handler, menu_event_t event, void **event_data);
typedef void (*menu_item_draw_t)(menu_item_t *item);
typedef void (*menu_item_remove_t)(menu_item_t *item);

struct event_handler_s {
    menu_event_t    event;
    void           *callback;
    void           *callback_data;
    void           *subscriber;
};

struct menu_s {
    struct list         items;              /* 0x0000 */
    menu_item_t        *selected_item;      /* 0x0010 */
    menu_t             *child;              /* 0x0014 */
    menu_t             *parent;             /* 0x0018 */
    struct list         events;             /* 0x001C */
    struct list         item_events;        /* 0x002C */
    int16_t             x_offset;           /* 0x003C */
    int16_t             y_offset;           /* 0x003E */
    uint16_t            x_padding;          /* 0x0040 */
    uint16_t            y_padding;          /* 0x0042 */
    uint16_t            cell_width;         /* 0x0044 */
    uint16_t            cell_height;        /* 0x0046 */
};                                          /* 0x0048 */

struct menu_item_s {
    menu_t             *owner;              /* 0x0000 */
    menu_item_draw_t    draw_proc;          /* 0x0004 */
    menu_item_remove_t  remove_proc;        /* 0x0008 */
    char               *text;               /* 0x000C */
    char               *tooltip;            /* 0x0010 */
    item_data_t         data;               /* 0x0014 */
    uint32_t            color;              /* 0x0018 */
    uint16_t            x_cell;             /* 0x001C */
    uint16_t            y_cell;             /* 0x001E */
    int16_t             x_offset;           /* 0x0020 */
    int16_t             y_offset;           /* 0x0022 */
    bool                interactive;        /* 0x0024 */
    bool                enabled;            /* 0x0025 */
};                                          /* 0x0026 */

struct menu_sprite_s {
    gfx_texture    *texture;
    uint16_t        on_tile;
    uint16_t        off_tile;
    uint32_t        on_color;
    uint32_t        off_color;
    uint16_t        width;
    uint16_t        height;
    gfx_texture    *background;
    uint32_t        background_color;
    int16_t         background_tile;
    int16_t         null_tile;
    gfx_texture    *null_texture;
};

/* event functions */
event_handler_t    *menu_register_event(menu_t *menu, menu_event_t event, menu_event_cb_t callback, void *callback_data);
void                menu_unregister_event(menu_t *menu, menu_event_t event);
int                 menu_trigger_event(menu_t *menu, menu_event_t event, void **event_data);
event_handler_t    *menu_item_register_event(menu_item_t *item, menu_event_t event, menu_item_event_cb_t callback, void *callback_data);
void                menu_item_unregister_event(menu_item_t *item, menu_event_t event);
int                 menu_item_trigger_event(menu_item_t *item, menu_event_t event, void **event_data);

void                menu_static_sprites_init    (void);
void                menu_init                   (menu_t *menu, int16_t x_offset, int16_t y_offset);
void                menu_draw                   (menu_t *menu);
int                 menu_return                 (event_handler_t *handler, menu_event_t event, void **event_data);
void                menu_item_remove            (menu_item_t *item);

int                 menu_x(menu_t *menu);
int                 menu_y(menu_t *menu);
int                 menu_item_x(menu_item_t *item);
int                 menu_item_y(menu_item_t *item);
void                menu_padding_set(menu_t *menu, uint16_t padding_x, uint16_t padding_y);
void                menu_cell_set(menu_t *menu, uint16_t cell_width, uint16_t cell_height);
void                menu_item_offset_set(menu_item_t *item, int16_t offset_x, int16_t offset_y);

menu_item_t        *menu_label_add          (menu_t *menu, uint16_t cell_x, uint16_t cell_y, void *text);
menu_item_t        *menu_submenu_add        (menu_t *menu, uint16_t cell_x, uint16_t cell_y, char *text, menu_t *submenu);
menu_item_t        *menu_checkbox_add       (menu_t *menu, uint16_t cell_x, uint16_t cell_y);
menu_item_t        *menu_button_add         (menu_t *menu, uint16_t cell_x, uint16_t cell_y, char *text, menu_item_event_cb_t on_activate, void *activate_data);
menu_item_t        *menu_gfx_button_add     (menu_t *menu, uint16_t cell_x, uint16_t cell_y, menu_sprite_t *sprite, menu_item_event_cb_t on_activate, void *activate_data);
menu_item_t        *menu_gfx_add            (menu_t *menu, uint16_t cell_x, uint16_t cell_y, gfx_texture *texture, int tile, int width, int height);
menu_item_t        *menu_list_add           (menu_t *menu, uint16_t cell_x, uint16_t cell_y, char **text, int option_cnt);
menu_item_t        *menu_watch_add          (menu_t *menu, uint16_t cell_x, uint16_t cell_y, watch_t *watch, _Bool setpos);
menu_item_t        *menu_text_input_add     (menu_t *menu, uint16_t cell_x, uint16_t cell_y, char *default_text, char **value_ptr, int val_len);
menu_item_t        *menu_watch_add          (menu_t *menu, uint16_t cell_x, uint16_t cell_y, watch_t *watch, _Bool setpos);
menu_item_t        *menu_text_input_add     (menu_t *menu, uint16_t cell_x, uint16_t cell_y, char *default_text, char **value_ptr, int val_len);
menu_item_t        *menu_bind_add           (menu_t *menu, int x, int y, int cmd);
menu_item_t        *menu_item_list_add      (menu_t *menu, uint16_t cell_x, uint16_t cell_y, uint16_t start_tile, int8_t *options,
                                             uint8_t option_cnt, int8_t *value_ptr, uint8_t *ovl_values, int tiles_cnt,
                                             menu_sprite_t *sprite, char *tooltip);
menu_item_t        *menu_move_button_add    (menu_t *menu, uint16_t cell_x, uint16_t cell_y,
                                             int16_t *move_x, int16_t *move_y);
menu_item_t        *menu_number_input_add   (menu_t *menu, uint16_t cell_x, uint16_t cell_y,
                                             int8_t base, uint8_t length);
menu_item_t        *menu_switch_add         (menu_t *menu, uint16_t cell_x, uint16_t cell_y,
                                             gfx_texture *on_texture, gfx_texture *off_texture,
                                             uint32_t on_color, uint32_t off_color,
                                             uint8_t on_tile, uint8_t off_tile,
                                             int width, int height, char *tooltip);

void                menu_checkbox_set           (menu_item_t *item, int set);
void                menu_switch_set             (menu_item_t *item, int set);
void                menu_number_set             (menu_item_t *item, uint32_t value);
void                menu_list_set               (menu_item_t *item, int idx);
void                menu_bind_set               (menu_item_t *item, int cmd);
menu_sprite_t      *menu_button_sprite_get      (menu_item_t *item);
void                menu_keyboard_get           (menu_item_t *item, char **dest);

int                 menu_number_word_event      (event_handler_t *handler, menu_event_t event, void **event_data);
int                 menu_number_halfword_event  (event_handler_t *handler, menu_event_t event, void **event_data);
int                 menu_number_byte_event      (event_handler_t *handler, menu_event_t event, void **event_data);

extern menu_sprite_t           *scroll_up_sprite;
extern menu_sprite_t           *scroll_down_sprite;

#endif