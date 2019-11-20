#include <stdlib.h>
#include "menu.h"

struct item_data {
    const char            **text;
    void                   *values;
    uint8_t                 value_size;
    uint16_t                option_cnt;
    int                     selected_idx;
    _Bool                   active;
    void                   *data;
    menu_list_callback      callback;
    void                   *callback_data;
};

int get_option_index(struct menu_item *item, uint32_t value){
    struct item_data *data = item->data;
    for(int i=0;i<data->option_cnt;i++){
        uint32_t val;
        switch(data->value_size){
            case 1:
                val = ((uint8_t*)(data->values))[i];
                break;
            case 2:
                val = ((uint16_t*)(data->values))[i];
                break;
            case 4:
            default:
                val = ((uint32_t*)(data->values))[i];
        }
        if(value == val){
            return i;
        }
    }
    return -1;
}

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

static void option_activate(struct menu_item *item){
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
            data->callback(item,MENU_CALLBACK_ACTIVATE,data->callback_data,data->selected_idx);
        }
    }
    data->active = !data->active;
}

static void option_update(struct menu_item *item){
    struct item_data *data = item->data;
    if(data->data){
        if(!data->active){
            uint32_t d; 
            switch(data->value_size){
                case 1:
                    d = *(uint8_t*)data->data;
                    break;
                case 2:
                    d = *(uint16_t*)data->data;
                    break;
                case 4:
                default:
                    d = *(uint32_t*)data->data;
                    break;
            }
            int idx = get_option_index(item,d);
            if(idx>=0){
                data->selected_idx = idx;
            }
        }
    }
    if(data->callback){
        data->callback(item, MENU_CALLBACK_UPDATE, NULL, -1);
    }
}

static void option_draw(struct menu_item *item){
    struct item_data *data = item->data;
    z2_rgba32_t color = MENU_DEFAULT_COLOR;
    if(data->active) color.color = COLOR_GREEN;
    else if(item->owner->selected_item == item) color = MENU_SELECTED_COLOR;
    gfx_printf_color(get_item_x_pos(item),get_item_y_pos(item),color.color,"%s",data->text[data->selected_idx]);
}

struct menu_item *menu_add_list(struct menu *menu, uint16_t x, uint16_t y,
                                const char **text, void *values,
                                uint8_t value_size, uint16_t options, void *list_data,
                                menu_list_callback callback, void *callback_data){
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
        data->callback_data = callback_data;
        item->draw_proc = option_draw;
        item->navigate_proc = option_nav;
        item->activate_proc = option_activate;
        item->update_proc = option_update;
        item->data = data;
        item->interactive = 1;
    }
    return item;
}
