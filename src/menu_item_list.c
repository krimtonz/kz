#include <stdlib.h>
#include "resource.h"
#include "menu.h"

struct item_list_data {
    struct item_list_row   *row;
    uint8_t                 selected_idx;
    gfx_texture            *texture;
    _Bool                   active;
    item_list_callback      callback;
    void                   *callback_data;
    int8_t                 *options;
    uint8_t                 option_cnt;
    int8_t                 *value_ptr;
    uint16_t                start_tile;
    uint8_t                *ovl_values;
    uint8_t                 tiles_cnt;
};

inline uint8_t get_option_idx(struct item_list_data *data){
    for(int i=0;i<data->option_cnt;i++){
        if(data->options[i]==*(data->value_ptr)) return i;
    }
    return 0;
}

static void draw_item_list(struct menu_item *item){
    struct item_list_data *data = item->data;
    if(data->active){
        gfx_push(gsDPSetPrimColor(0,0,0x00,0xFF,0x00,0xFF));
    }
    else if(item->owner->selected_item==item){
        gfx_push(gsDPSetPrimColor(0,0,0x00,0x00,0xFF,0xFF));
    }
    gfx_push(gsDPPipeSync());
    gfx_draw_sprite(buttons_texture,get_item_x_pos(item),get_item_y_pos(item),1,16,16);
    gfx_push(gsDPSetPrimColor(0,0,0xFF,0xFF,0xFF,0xFF));
    gfx_push(gsDPPipeSync());
    int idx = data->active?data->selected_idx:get_option_idx(data);
    if(data->options[idx]!=Z2_ITEM_NULL){
        int tile_idx = idx;
        if(idx>=data->tiles_cnt-1) tile_idx=1;
        tile_idx = tile_idx - 1 + data->start_tile;
        gfx_draw_sprite(data->texture,get_item_x_pos(item),get_item_y_pos(item),tile_idx, 16,16);
        if(data->ovl_values){
            gfx_printf_scale(get_item_x_pos(item)+8,get_item_y_pos(item)+11,0.5f,0.5f,"%d",data->ovl_values[idx]);
        }
    }
}

static int navigate_item_list(struct menu_item *item, enum menu_nav nav){
    struct item_list_data *data = item->data;
    if(!data->active) return 0;
    if(nav==MENU_NAV_DOWN){
        data->selected_idx += data->option_cnt - 1;
        data->selected_idx %= data->option_cnt;
    }else if(nav==MENU_NAV_UP){
        data->selected_idx++;
        data->selected_idx %= data->option_cnt;
    }
    return 1;
}

static void activate_item_list(struct menu_item *item){
    struct item_list_data *data = item->data;   
    if(data->active){
        *(data->value_ptr) = data->options[data->selected_idx];
        if(data->callback){
            data->callback(item,data->callback_data,MENU_CALLBACK_ACTIVATE);
        }
    }else{
        data->selected_idx=get_option_idx(data);
    }
    data->active = !data->active;
}

static void update_item_list(struct menu_item *item){
    struct item_list_data *data = item->data;
    if(!data->active){
        if(data->callback){
            data->callback(item,data->callback_data,MENU_CALLBACK_UPDATE);
        }
    }
}

struct menu_item *menu_add_item_list(struct menu *menu, uint16_t x, uint16_t y, item_list_callback callback, 
                                     void *callback_data, gfx_texture *texture, uint16_t start_tile, int8_t *options,
                                     uint8_t option_cnt, int8_t *value_ptr, uint8_t *ovl_values, uint8_t tiles_cnt){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_list_data *data = malloc(sizeof(*data));
        data->callback = callback;
        data->callback_data = callback_data;
        data->selected_idx = 0;
        data->active = 0;
        data->texture = texture;
        data->options = options;
        data->option_cnt = option_cnt;
        data->value_ptr = value_ptr;
        data->start_tile = start_tile;
        data->ovl_values = ovl_values;
        data->tiles_cnt = tiles_cnt;
        item->data = data;
        item->draw_proc = draw_item_list;
        item->interactive = 1;
        item->navigate_proc = navigate_item_list;
        item->activate_proc = activate_item_list;
        item->update_proc = update_item_list;
    }
    return item;
}