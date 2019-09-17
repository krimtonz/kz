#include <stdlib.h>
#include "resource.h"
#include "menu.h"

struct item_list_data {
    uint8_t                 selected_idx;
    _Bool                   active;
    menu_generic_callback   callback;
    void                   *callback_data;
    int8_t                 *options;
    uint8_t                 option_cnt;
    int8_t                 *value_ptr;
    uint16_t                start_tile;
    uint8_t                *ovl_values;
    uint8_t                 tiles_cnt;
    draw_info_t            *draw_info;
    struct tilebg_info     *null_item;
};

inline int8_t get_option_idx(struct item_list_data *data){
    if(data->options){
        for(int i=0;i<data->option_cnt;i++){
            if(data->options[i]==*(data->value_ptr)) return i;
        }
    }else{
        return *data->value_ptr;
    }
    return 0;
}

static void draw_item_list(struct menu_item *item){
    struct item_list_data *data = item->data;
    draw_info_t *draw = data->draw_info;
    struct tilebg_info *bg = draw->background;
    if(bg){
        if(data->active){
            gfx_push(gsDPSetPrimColor(0,0,bg->on_color.r,bg->on_color.g,bg->on_color.b,bg->on_color.a));
        }
        else if(item->owner->selected_item==item){
            gfx_push(gsDPSetPrimColor(0,0,0x00,0x00,0xFF,0xFF));
        }else{

            gfx_push(gsDPSetPrimColor(0,0,bg->off_color.r,bg->off_color.g,bg->off_color.b,bg->off_color.a));
        }
        gfx_push(gsDPPipeSync());
        gfx_draw_sprite_scale(bg->texture,get_item_x_pos(item),get_item_y_pos(item),bg->tile,16,16,draw->x_scale,draw->y_scale);
    }
    gfx_push(gsDPSetPrimColor(0,0,0xFF,0xFF,0xFF,0xFF));
    gfx_push(gsDPPipeSync());
    int idx = data->active?data->selected_idx:get_option_idx(data);
    int val;
    if(data->options){
        val = data->options[idx];
    }else{
        if(data->active)idx--;
        val = idx;
        idx++;
    }
    if(val!=Z2_ITEM_NULL){
        int tile_idx = idx;
        tile_idx = tile_idx + data->start_tile - 1;
        gfx_texture *texture = get_item_texture(tile_idx);
        gfx_draw_sprite_scale(texture,get_item_x_pos(item),get_item_y_pos(item),0, 16,16,draw->x_scale,draw->y_scale);
        if(data->ovl_values){
            gfx_printf_scale(get_item_x_pos(item)+8,get_item_y_pos(item)+11,0.5f,0.5f,"%d",data->ovl_values[idx]);
        }
    }else if(data->null_item!=NULL){
        struct tilebg_info *null_item = data->null_item;
        if(data->active){
            gfx_push(gsDPSetPrimColor(0,0,null_item->on_color.r,null_item->on_color.g,null_item->on_color.b,null_item->on_color.a));
        }
        else if(item->owner->selected_item==item){
            gfx_push(gsDPSetPrimColor(0,0,0x00,0x00,0xFF,0xFF));
        }else{

            gfx_push(gsDPSetPrimColor(0,0,null_item->off_color.r,null_item->off_color.g,null_item->off_color.b,null_item->off_color.a));
        }
        gfx_draw_sprite_scale(data->null_item->texture,get_item_x_pos(item),get_item_y_pos(item),data->null_item->tile, 16,16, draw->x_scale, draw->y_scale);
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
        int8_t val;
        if(data->options)
            val = data->options[data->selected_idx];
        else
            val = data->selected_idx - 1;
        *(data->value_ptr) = val;
        if(data->callback){
            data->callback(item,MENU_CALLBACK_ACTIVATE,data->callback_data);
        }
    }else{
        int8_t idx = get_option_idx(data) + (data->options?0:1);
        data->selected_idx=idx;
    }
    data->active = !data->active;
}

static void update_item_list(struct menu_item *item){
    struct item_list_data *data = item->data;
    if(!data->active){
        if(data->callback){
            data->callback(item,MENU_CALLBACK_UPDATE,data->callback_data);
        }
    }
}

struct menu_item *menu_add_item_list(struct menu *menu, uint16_t x, uint16_t y, menu_generic_callback callback,
                                     void *callback_data, uint16_t start_tile, int8_t *options,
                                     uint8_t option_cnt, int8_t *value_ptr, uint8_t *ovl_values, uint8_t tiles_cnt,
                                     draw_info_t *drawinfo, struct tilebg_info *null_item){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_list_data *data = malloc(sizeof(*data));
        data->callback = callback;
        data->callback_data = callback_data;
        data->selected_idx = 0;
        data->active = 0;
        data->options = options;
        data->option_cnt = option_cnt;
        data->value_ptr = value_ptr;
        data->start_tile = start_tile;
        data->ovl_values = ovl_values;
        data->tiles_cnt = tiles_cnt;
        data->draw_info = drawinfo;
        data->null_item = null_item;
        item->data = data;
        item->draw_proc = draw_item_list;
        item->interactive = 1;
        item->navigate_proc = navigate_item_list;
        item->activate_proc = activate_item_list;
        item->update_proc = update_item_list;
    }
    return item;
}