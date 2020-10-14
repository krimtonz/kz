#include <stdlib.h>
#include "menu.h"
#include "watches.h"

struct item_data {
    watch_t         *watch;
    enum watch_type  type;
    uint8_t          floating;
};

static void menu_watch_draw(menu_item_t *item){
    struct item_data *data = (struct item_data*)item->data;
    watch_t *watch = data->watch;
    // floating watches is handled in kz_main
    if(watch->floating){
        return;
    }
    watch->x = menu_item_x(item);
    watch->y = menu_item_y(item);
    watch_printf(watch, item->color);
}

static void menu_watch_remove(menu_item_t *item){
    free(item->data);
}

menu_item_t *menu_watch_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell, watch_t *watch, _Bool setpos){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->watch = watch;
        data->floating = 0;
        data->type = watch->type;
        if(setpos){
            watch->x = menu_item_x(item);
            watch->y = menu_item_y(item);
        }
        item->data = data;
        item->draw_proc = menu_watch_draw;
        item->remove_proc = menu_watch_remove;
    }
    return item;
}