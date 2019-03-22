#include <stdlib.h>
#include "menu.h"

#define ITEM_SCREEN_SIZE 11

static gfx_texture *items_texture = NULL;
static gfx_texture *bottles_texture = NULL;
static gfx_texture *buttons_texture = NULL;

struct item_map {
    void   *slot;
    int8_t  item;
};

static struct item_map items[] = {
    { &z2_file.items[Z2_SLOT_OCARINA], Z2_ITEM_OCARINA },
    { &z2_file.items[Z2_SLOT_BOW], Z2_ITEM_HEROS_BOW },
    { &z2_file.items[Z2_SLOT_FIRE_ARROW], Z2_ITEM_FIRE_ARROW },
    { &z2_file.items[Z2_SLOT_ICE_ARROW], Z2_ITEM_ICE_ARROW },
    { &z2_file.items[Z2_SLOT_LIGHT_ARROW], Z2_ITEM_LIGHT_ARROW },
    { &z2_file.items[Z2_SLOT_QUEST_1], Z2_ITEM_NULL },
    { &z2_file.items[Z2_SLOT_BOMB], Z2_ITEM_BOMB },
    { &z2_file.items[Z2_SLOT_STICK], Z2_ITEM_STICK },
    { &z2_file.items[Z2_SLOT_NUT], Z2_ITEM_NUT },
    { &z2_file.items[Z2_SLOT_MAGIC_BEAN], Z2_ITEM_MAGIC_BEAN },
    { &z2_file.items[Z2_SLOT_QUEST_2], Z2_ITEM_NULL },
};

struct switch_data {
    uint8_t map_idx;
    uint8_t map; 
};

void draw_item_switch(struct menu_item *item){
}

void activate_item_switch(struct menu_item *item){

}

struct menu_item *menu_add_item_switch(struct menu *menu, uint16_t x, uint16_t y, uint8_t map_idx, uint8_t map){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct switch_data *data = malloc(sizeof(*data));
        data->map = map;
        data->map_idx = map_idx;
        item->draw_proc = draw_item_switch;
        item->activate_proc = activate_item_switch;
        item->interactive = 1;
        item->data = data;
    }
    return item;
}

void callback_proc(enum menu_callback callback){
    if(callback == MENU_CALLBACK_ENTER){
        items_texture = gfx_load_icon_item_static();
        bottles_texture = gfx_load_bottle_icons();
        buttons_texture = gfx_load_game_texture(G_IM_FMT_IA,G_IM_SIZ_8b,32,32,1,5,1126,0xF60);
    }else{
        gfx_destroy_texture(items_texture);
        gfx_destroy_texture(bottles_texture);
        gfx_destroy_texture(bottles_texture);
    }
}

struct menu *create_inventory_menu(){
    static struct menu menu;
    menu_init(&menu,0,0);
    menu.callback_proc = callback_proc;
    
    menu.selected_item = menu_add_button(&menu,0,0,"return",menu_return,NULL);
    
    return &menu;
}