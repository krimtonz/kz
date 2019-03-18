#include <stdlib.h>
#include "watches.h"
#include "menu.h"

struct menu_data_watch{
    watch_t    *watch;
    watch_type  type;
    uint8_t     floating;

};

void menu_draw_watch(struct menu_item *item){
    struct menu_data_watch *watch_data = (struct menu_data_watch*)item->data;
    watch_t *watch = watch_data->watch;
    if(watch->floating) return;
    int x = 0;
    int y = 0;
    x = get_item_x_pos(item);
    y = get_item_y_pos(item);
    char buf[16];
    watch_printf(watch,buf);
    gfx_printf(x,y,"%s",buf);
}

struct menu_item *menu_add_watch(struct menu *menu,uint16_t x, uint16_t y, watch_t *watch){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct menu_data_watch *data = malloc(sizeof(*data));
        data->watch = watch;
        watch->x = get_item_x_pos(item);
        watch->y = get_item_y_pos(item);
        item->data = data;
        item->draw_proc = menu_draw_watch;
        item->interactive = 0;
    }
    return item;
}