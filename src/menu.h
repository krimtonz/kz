#ifndef _MENU_H
#define _MENU_H
#include <list/list.h>
#include "gfx.h"
#include "watches.h"
#include "z2.h"

extern z2_rgba32_t MENU_SELECTED_COLOR;
extern z2_rgba32_t MENU_DEFAULT_COLOR;

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
    MENU_CALLBACK_UPDATE,
    MENU_CALLBACK_RETURN,
};

struct menu_item{
    struct menu        *owner;
    void              (*draw_proc)(struct menu_item *item);
    void              (*activate_proc)(struct menu_item *item);
    int               (*navigate_proc)(struct menu_item *item, enum menu_nav nav);
    void              (*update_proc)(struct menu_item *item);
    void              (*enter_proc)(struct menu_item *item);
    void              (*exit_proc)(struct menu_item *item);
    const char         *text;
    void               *data;
    uint16_t            x;
    uint16_t            y;
    uint8_t             interactive;
    int16_t             x_offset;
    int16_t             y_offset;
    const char         *tooltip;
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

struct item_list_row {
    int8_t     *slot;
    int8_t      option_cnt;
    int8_t     *options;
};

struct tilebg_info{
    gfx_texture    *texture;
    uint16_t        tile;
    z2_rgba32_t     on_color;
    z2_rgba32_t     off_color;
};

typedef struct{
    gfx_texture        *texture;
    int16_t             on_tile;
    int16_t             off_tile;
    float               x_scale;
    float               y_scale;
    uint16_t            width;
    uint16_t            height;
    z2_rgba32_t         enabled_color;
    z2_rgba32_t         disabled_color;
    _Bool               use_selected_color;
    struct tilebg_info *background;
}draw_info_t;

typedef int  (*menu_generic_callback)(struct menu_item *item, enum menu_callback callback, void *data);
typedef int  (*menu_number_callback)(struct menu_item *item, enum menu_callback callback, void *data, uint32_t value);

void menu_init(struct menu *menu, uint16_t x, uint16_t y);
void menu_draw(struct menu *menu);
void menu_update(struct menu *menu);
void menu_set_cell(struct menu *menu, uint16_t width, uint16_t height);
void menu_set_padding(struct menu *menu, uint16_t x, uint16_t y);

struct menu_item *menu_add(struct menu *menu, uint16_t x, uint16_t y, const char *text);
struct menu_item *menu_add_gfx(struct menu *menu, uint16_t x, uint16_t y, draw_info_t *drawinfo, _Bool load_item);
struct menu_item *menu_add_submenu(struct menu *menu, uint16_t x, uint16_t y, struct menu *submenu, const char *name);
struct menu_item *menu_add_button(struct menu *menu, uint16_t x, uint16_t y, const char *text, menu_generic_callback callback, void *data);
struct menu_item *menu_add_gfx_button(struct menu *menu, uint16_t x, uint16_t y, menu_generic_callback callback, void *data, draw_info_t *drawinfo);
struct menu_item *menu_add_watch(struct menu *menu, uint16_t x, uint16_t y, watch_t *watch, _Bool setpos);

struct menu_item *menu_add_number_input(struct menu* menu, uint16_t x, uint16_t y,
                                        menu_number_callback callback, void *callback_data,
                                        uint8_t base, uint8_t length, void *value, uint8_t val_len);
struct menu_item *menu_add_switch(struct menu *menu, uint16_t x, uint16_t y,
                                  void *addr, uint8_t addr_len, uint32_t bitmask, 
                                  menu_generic_callback callback, void *callback_data,
                                  const char *text);

struct menu_item *menu_add_gfx_switch(struct menu *menu, uint16_t x, uint16_t y,
                                  void *addr, uint8_t addr_len,
                                  uint32_t bitmask, menu_generic_callback callback,
                                  void *callback_data, draw_info_t *drawinfo);

struct menu_item *menu_add_list(struct menu *menu, uint16_t x, uint16_t y,
                                const char **text, void *values, uint8_t value_size,
                                uint16_t options, void *list_data, menu_generic_callback callback);

struct menu_item *menu_add_bit_switch(struct menu *menu, uint16_t x, uint16_t y,
                                      void *addr, uint8_t addr_len, uint32_t bitmask,
                                      menu_generic_callback callback,  void *callback_data,
                                      gfx_texture *texture, uint16_t tex_width, uint16_t tex_height,
                                      uint16_t tile, _Bool has_off_tile, const char *tooltip,
                                      uint32_t on_color, uint32_t off_color, _Bool load_item);

struct menu_item *menu_add_item_list(struct menu *menu, uint16_t x, uint16_t y, menu_generic_callback callback,
                                     void *callback_data, uint16_t start_tile, int8_t *options,
                                     uint8_t option_cnt, int8_t *value_ptr, uint8_t *ovl_values, int tiles_cnt,
                                     draw_info_t *drawinfo, struct tilebg_info *null_item, const char *tooltip);

struct menu_item *menu_add_text_input(struct menu *menu, uint16_t x, uint16_t y,
                                      char *default_text, char **value_ptr);

void menu_navigate(struct menu *menu, enum menu_nav nav);
void menu_callback(struct menu *menu, enum menu_callback callback);
int menu_return(struct menu_item *item, enum menu_callback callback, void *data);

int get_item_x_pos(struct menu_item *item);
int get_item_y_pos(struct menu_item *item);
void set_item_offset(struct menu_item *item, int16_t x, int16_t y);

#endif