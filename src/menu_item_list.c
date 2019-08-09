#include <stdlib.h>
#include "resource.h"
#include "menu.h"

struct item_list_data {
    struct item_list_row   *row;
    uint8_t                 selected_idx;
    gfx_texture            *texture;
    _Bool                   active;
};

inline uint8_t get_row_idx(struct item_list_row *row){
    for(int i=0;i<row->option_cnt;i++){
        if(row->options[i]==*(row->slot)) return i;
    }
    return 0;
}

static void draw_item_list(struct menu_item *item){
    struct item_list_data *data = item->data;
    struct item_list_row *row = data->row;
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
    if(data->selected_idx!=0){
        if(data->active){
            gfx_draw_sprite(data->texture,get_item_x_pos(item),get_item_y_pos(item),data->selected_idx - 1, 16,16);
        }else{
            uint8_t tile_idx = get_row_idx(row);
            if(tile_idx!=0){
                gfx_draw_sprite(data->texture,get_item_x_pos(item),get_item_y_pos(item),tile_idx - 1,16,16);
            }
        }
    }
}

static int navigate_item_list(struct menu_item *item, enum menu_nav nav){
    struct item_list_data *data = item->data;
    if(!data->active) return 0;
    if(nav==MENU_NAV_DOWN){
        data->selected_idx += data->row->option_cnt - 1;
        data->selected_idx %= data->row->option_cnt;
    }else if(nav==MENU_NAV_UP){
        data->selected_idx++;
        data->selected_idx %= data->row->option_cnt;
    }
    return 1;
}

static void activate_item_list(struct menu_item *item){
    struct item_list_data *data = item->data;
    struct item_list_row *row = data->row;
    if(data->active){
        *(row->slot) = row->options[data->selected_idx];
    }else{
        data->selected_idx = get_row_idx(row);
    }
    data->active = !data->active;

}

struct menu_item *menu_add_item_list(struct menu *menu, uint16_t x, uint16_t y, struct item_list_row *row, gfx_texture *texture){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_list_data *data = malloc(sizeof(*data));
        data->row = row;
        data->selected_idx = 2;
        data->active = 0;
        data->texture = texture;
        item->data = data;
        item->draw_proc = draw_item_list;
        item->interactive = 1;
        item->navigate_proc = navigate_item_list;
        item->activate_proc = activate_item_list;
    }
    return item;
}