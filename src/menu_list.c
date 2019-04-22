#include <stdlib.h>
#include "menu.h"

struct item_data {
    const char        **text;
    void               *values;
    uint8_t             value_size;
    uint16_t            option_cnt;
    uint16_t            selected_idx;
    _Bool               active;
    void               *data;
    menu_list_callback  callback;
};

static int option_nav(struct menu_item *item, enum menu_nav nav){
    struct item_data *data = item->data;
    if(data->active){
        if(nav == MENU_NAV_UP){
            data->selected_idx++;
            data->selected_idx %= data->option_cnt;
        }else if(nav == MENU_NAV_DOWN){
            data->selected_idx = data->option_cnt - 1;
            data->selected_idx %= data->option_cnt;
        }
        return 1;
    }
    return 0;
}

void option_activate(struct menu_item *item){
    struct item_data *data = item->data;
    if(data->active){
        if(data->data){
            switch(data->value_size){
                case 1:
                    *(uint8_t*)data->data = ((uint8_t*)data->values)[data->selected_idx];
                    break;
                case 2:
                    *(uint16_t*)data->data = ((uint16_t*)data->values)[data->selected_idx];
                    break;
                case 4:
                default:
                    *(uint32_t*)data->data = ((uint32_t*)data->values)[data->selected_idx];
                    break;
            }
        }
        if(data->callback){
            data->callback(item, data->selected_idx);
        }
    }
    data->active = !data->active;
}

void option_draw(struct menu_item *item){
    struct item_data *data = item->data;
    uint32_t color = 0xFFFFFFFF;
    if(data->active) color = COLOR_GREEN;
    else if(item->owner->selected_item == item) color = MENU_SELECTED_COLOR;
    gfx_printf_color(get_item_x_pos(item),get_item_y_pos(item),color,"%s",data->text[data->selected_idx]);
}

struct menu_item *menu_add_list(struct menu *menu, uint16_t x, uint16_t y, 
                                const char **text, void *values, 
                                uint8_t value_size, uint16_t options, 
                                void *list_data, menu_list_callback callback){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->text = text;
        data->values = values;
        data->value_size = value_size;
        data->option_cnt = options;
        data->selected_idx = 0;
        data->data = list_data;
        data->active = 0;
        data->callback = callback;
        item->draw_proc = option_draw;
        item->navigate_proc = option_nav;
        item->activate_proc = option_activate;
        item->data = data;
        item->interactive = 1;
    }
    return item;
}
