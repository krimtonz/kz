#include <stdlib.h>
#include "menu.h"
#include "kz.h"

#define ITEM_SCREEN_SIZE 11

static gfx_texture *items_texture = NULL;
static gfx_texture *bottles_texture = NULL;
static gfx_texture *buttons_texture = NULL;
static gfx_texture *masks_texture = NULL;
static gfx_texture *trade_quest_texture = NULL;

struct item_map_row {
    int8_t     *slot;
    int8_t      item;
};

struct item_list_row {
    int8_t     *slot;
    int8_t      option_cnt;
    int8_t     *options;
};

static struct item_map_row item_map[] = {
    { &z2_file.items[Z2_SLOT_OCARINA], Z2_ITEM_OCARINA },
    { &z2_file.items[Z2_SLOT_BOW], Z2_ITEM_HEROS_BOW },
    { &z2_file.items[Z2_SLOT_FIRE_ARROW], Z2_ITEM_FIRE_ARROW },
    { &z2_file.items[Z2_SLOT_ICE_ARROW], Z2_ITEM_ICE_ARROW },
    { &z2_file.items[Z2_SLOT_LIGHT_ARROW], Z2_ITEM_LIGHT_ARROW },
    { &z2_file.items[Z2_SLOT_BOMB], Z2_ITEM_BOMB },
    { &z2_file.items[Z2_SLOT_BOMBCHU], Z2_ITEM_BOMBCHU },
    { &z2_file.items[Z2_SLOT_STICK], Z2_ITEM_STICK },
    { &z2_file.items[Z2_SLOT_NUT], Z2_ITEM_NUT },
    { &z2_file.items[Z2_SLOT_MAGIC_BEAN], Z2_ITEM_MAGIC_BEAN },
    { &z2_file.items[Z2_SLOT_POWDER_KEG], Z2_ITEM_POWDER_KEG },
    { &z2_file.items[Z2_SLOT_PICTOGRAPH_BOX], Z2_ITEM_PICTOGRAPH_BOX },
    { &z2_file.items[Z2_SLOT_LENS], Z2_ITEM_LENS },
    { &z2_file.items[Z2_SLOT_HOOKSHOT], Z2_ITEM_HOOKSHOT },
    { &z2_file.items[Z2_SLOT_GREAT_FAIRY_SWORD], Z2_ITEM_GREAT_FAIRY_SWORD },
};

static struct item_map_row mask_map[] = {
    { &z2_file.masks[Z2_SLOT_POSTMAN], Z2_MASK_POSTMAN },
    { &z2_file.masks[Z2_SLOT_ALL_NIGHT], Z2_MASK_ALL_NIGHT },
    { &z2_file.masks[Z2_SLOT_BLAST], Z2_MASK_BLAST },
    { &z2_file.masks[Z2_SLOT_STONE], Z2_MASK_STONE },
    { &z2_file.masks[Z2_SLOT_GREAT_FAIRY], Z2_MASK_GREAT_FAIRY },
    { &z2_file.masks[Z2_SLOT_DEKU], Z2_MASK_DEKU },

    { &z2_file.masks[Z2_SLOT_KEATON], Z2_MASK_KEATON },
    { &z2_file.masks[Z2_SLOT_BREMEN], Z2_MASK_BREMEN },
    { &z2_file.masks[Z2_SLOT_BUNNY_HOOD], Z2_MASK_BUNNY_HOOD },
    { &z2_file.masks[Z2_SLOT_DON_GERO], Z2_MASK_DON_GERO },
    { &z2_file.masks[Z2_SLOT_SCENTS], Z2_MASK_SCENTS },
    { &z2_file.masks[Z2_SLOT_GORON], Z2_MASK_GORON },

    { &z2_file.masks[Z2_SLOT_ROMANI], Z2_MASK_ROMANI },
    { &z2_file.masks[Z2_SLOT_CIRCUS_LEADER], Z2_MASK_CIRCUS_LEADER },
    { &z2_file.masks[Z2_SLOT_KAFEI], Z2_MASK_KAFEI },
    { &z2_file.masks[Z2_SLOT_COUPLE], Z2_MASK_COUPLE },
    { &z2_file.masks[Z2_SLOT_MASK_OF_TRUTH], Z2_MASK_MASK_OF_TRUTH },
    { &z2_file.masks[Z2_SLOT_ZORA], Z2_MASK_ZORA },

    { &z2_file.masks[Z2_SLOT_KAMARO], Z2_MASK_KAMARO },
    { &z2_file.masks[Z2_SLOT_GIBDO], Z2_MASK_GIBDO },
    { &z2_file.masks[Z2_SLOT_GARO], Z2_MASK_GARO },
    { &z2_file.masks[Z2_SLOT_CAPTAIN], Z2_MASK_CAPTAINSHAT },
    { &z2_file.masks[Z2_SLOT_GIANT], Z2_MASK_GIANT },
    { &z2_file.masks[Z2_SLOT_FIERCE_DEITY], Z2_MASK_FIERCE_DEITY },
};

static int8_t bottle_contents[] = {
    Z2_ITEM_NULL,
    Z2_ITEM_BOTTLE,
    Z2_ITEM_RED_POTION,
    Z2_ITEM_GREEN_POTION,
    Z2_ITEM_BLUE_POTION,
    Z2_ITEM_FAIRY,
    Z2_ITEM_DEKU_PRINCESS,
    Z2_ITEM_MILK,
    Z2_ITEM_HALF_MILK,
    Z2_ITEM_FISH,
    Z2_ITEM_BUGS,
    Z2_ITEM_BLUE_FIRE,
    Z2_ITEM_POE,
    Z2_ITEM_BIG_POE,
    Z2_ITEM_SPRING_WATER,
    Z2_ITEM_HOT_SPRING_WATER,
    Z2_ITEM_ZORA_EGG,
    Z2_ITEM_GOLD_DUST,
    Z2_ITEM_MUSHROOM,
    Z2_ITEM_SEAHORSE,
    Z2_ITEM_CHATEAU_ROMANI,
    Z2_ITEM_HYLIAN_LOACH,
};

static int8_t trade_quest_contents[] = {
    Z2_ITEM_NULL,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_LAND_DEED,
    Z2_ITEM_SWAP_DEED,
    Z2_ITEM_MOUNTAIN_DEED,
    Z2_ITEM_OCEAN_DEED,
    Z2_ITEM_ROOM_KEY,
    Z2_ITEM_MAMA_LETTER,
    Z2_ITEM_KAFEI_LETTER,
    Z2_ITEM_PENDANT,
};

static struct item_list_row bottle_options[] = {
    { &z2_file.items[Z2_SLOT_BOTTLE_1], 22, bottle_contents },
    { &z2_file.items[Z2_SLOT_BOTTLE_2], 22, bottle_contents },
    { &z2_file.items[Z2_SLOT_BOTTLE_3], 22, bottle_contents },
    { &z2_file.items[Z2_SLOT_BOTTLE_4], 22, bottle_contents },
    { &z2_file.items[Z2_SLOT_BOTTLE_5], 22, bottle_contents },
    { &z2_file.items[Z2_SLOT_BOTTLE_6], 22, bottle_contents },
};

static struct item_list_row trade_quest_options[] = {
    { &z2_file.items[Z2_SLOT_QUEST_1], 10, trade_quest_contents },
    { &z2_file.items[Z2_SLOT_QUEST_2], 10, trade_quest_contents },
    { &z2_file.items[Z2_SLOT_QUEST_3], 10, trade_quest_contents },
};

struct switch_data {
    uint8_t map_idx;
    uint8_t map; 
};

struct list_data {
    struct item_list_row *row;
    uint8_t selected_idx;
    gfx_texture **texture;
    _Bool active;
};

inline uint8_t get_row_idx(struct item_list_row *row){
    for(int i=0;i<row->option_cnt;i++){
        if(row->options[i]==*(row->slot)) return i;
    }
    return 0;
}

static void draw_item_list(struct menu_item *item){
    struct list_data *data = item->data;
    struct item_list_row *row = data->row;
    if(data->active){
        gfx_push(gsDPSetPrimColor(0,0,0x00,0xFF,0x00,0xFF));
        gfx_push(gsDPPipeSync());
    }
    else if(item->owner->selected_item==item){
        gfx_push(gsDPSetPrimColor(0,0,0x00,0x00,0xFF,0xFF));
        gfx_push(gsDPPipeSync());
    }
    gfx_draw_sprite(buttons_texture,get_item_x_pos(item),get_item_y_pos(item),1,16,16);
    gfx_push(gsDPSetPrimColor(0,0,0xFF,0xFF,0xFF,0xFF));
    gfx_push(gsDPPipeSync());
    if(data->selected_idx!=0){
        if(data->active){
            gfx_draw_sprite(*(data->texture),get_item_x_pos(item),get_item_y_pos(item),data->selected_idx - 1, 16,16);
        }else{
            uint8_t tile_idx = get_row_idx(row);
            if(tile_idx!=0){
                gfx_draw_sprite(*(data->texture),get_item_x_pos(item),get_item_y_pos(item),tile_idx - 1,16,16);
            }
        }
    }
}

static int navigate_item_list(struct menu_item *item, enum menu_nav nav){
    struct list_data *data = item->data;
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
    struct list_data *data = item->data;
    struct item_list_row *row = data->row;
    if(data->active){
        *(row->slot) = row->options[data->selected_idx];
    }else{
        data->selected_idx = get_row_idx(row);
    }
    data->active = !data->active;

}

static void draw_item_switch(struct menu_item *item){
    struct switch_data *data = item->data;
    gfx_texture *texture = data->map==0?items_texture:masks_texture;
    struct item_map_row map = data->map==0?item_map[data->map_idx]:mask_map[data->map_idx];
    int tile_idx = map.item - (data->map==0?0:Z2_MASK_DEKU);
    if(*map.slot == Z2_ITEM_NULL) tile_idx += (texture->y_tiles/2);
    if(item->owner->selected_item == item){
        gfx_draw_rectangle(get_item_x_pos(item),get_item_y_pos(item),16,16,GPACK_RGBA8888(0x80,0x80,0xFF,0x80));
    }
    gfx_push(gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM));
    gfx_push(gsDPSetPrimColor(0,0,0xFF,0xFF,0xFF,0xFF));
    gfx_push(gsDPPipeSync());
    gfx_draw_sprite(texture,get_item_x_pos(item),get_item_y_pos(item),tile_idx,16,16);
}

static void activate_item_switch(struct menu_item *item){
    struct switch_data *data = item->data;
    struct item_map_row map = data->map==0?item_map[data->map_idx]:mask_map[data->map_idx];
    if(*map.slot == Z2_ITEM_NULL) *map.slot = map.item;
    else *map.slot = Z2_ITEM_NULL;
}

static struct menu_item *menu_add_item_switch(struct menu *menu, uint16_t x, uint16_t y, uint8_t map_idx, uint8_t map){
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

static struct menu_item *menu_add_item_list(struct menu *menu, uint16_t x, uint16_t y, struct item_list_row *row, gfx_texture **texture){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct list_data *data = malloc(sizeof(*data));
        data->row = row;
        data->selected_idx = 2;
        data->active = 0;
        data->texture = texture;
        item->data = data;
        item->draw_proc = draw_item_list;
        item->interactive= 1;
        item->navigate_proc = navigate_item_list;
        item->activate_proc = activate_item_list;
    }
    return item;
}

static void masks_callback_proc(enum menu_callback callback){
    if(callback == MENU_CALLBACK_ENTER){
        masks_texture = gfx_load_icon_item_static(Z2_MASK_DEKU,Z2_MASK_GIANT,1);
    }else{
        gfx_destroy_texture(masks_texture);
    }
}

static void items_callback_proc(enum menu_callback callback){
    if(callback == MENU_CALLBACK_ENTER){
        items_texture = gfx_load_icon_item_static(Z2_ITEM_OCARINA,Z2_ITEM_GREAT_FAIRY_SWORD,1);
        bottles_texture = gfx_load_icon_item_static(Z2_ITEM_BOTTLE,Z2_ITEM_BOTTLE2,0);
        trade_quest_texture = gfx_load_icon_item_static(Z2_ITEM_MOONS_TEAR,Z2_ITEM_PENDANT,0);
        buttons_texture = gfx_load_game_texture(G_IM_FMT_IA,G_IM_SIZ_8b,32,32,1,5,1126,0xF60);
    }else{
        gfx_destroy_texture(items_texture);
        gfx_destroy_texture(bottles_texture);
        gfx_destroy_texture(buttons_texture);
        gfx_destroy_texture(trade_quest_texture);
    }
}

struct menu *create_inventory_menu(){
    static struct menu menu;
    static struct menu items;
    static struct menu masks;
    static struct menu quest_status;

    menu_init(&menu,kz.main_menu.x,kz.main_menu.y);
    menu_init(&items,0,0);
    menu_init(&masks,0,0);
    menu_init(&quest_status,0,0);

    menu.selected_item = menu_add_button(&menu,0,0,"return",menu_return,NULL);
    menu_add_submenu(&menu,0,1,&items,"items");
    menu_add_submenu(&menu,0,2,&masks,"masks");
    menu_add_submenu(&menu,0,3,&quest_status,"quest status");

    items.selected_item = menu_add_button(&items,0,0,"return",menu_return,NULL);
    items.callback_proc = items_callback_proc;
    menu_set_cell(&items,16,16);
    menu_set_padding(&items,5,5);
    menu_add_item_switch(&items,0,1,0,0);
    menu_add_item_switch(&items,1,1,1,0);
    menu_add_item_switch(&items,2,1,2,0);
    menu_add_item_switch(&items,3,1,3,0);
    menu_add_item_switch(&items,4,1,4,0);

    menu_add_item_switch(&items,0,2,5,0);
    menu_add_item_switch(&items,1,2,6,0);
    menu_add_item_switch(&items,2,2,7,0);
    menu_add_item_switch(&items,3,2,8,0);
    menu_add_item_switch(&items,4,2,9,0);

    menu_add_item_switch(&items,0,3,10,0);
    menu_add_item_switch(&items,1,3,11,0);
    menu_add_item_switch(&items,2,3,12,0);
    menu_add_item_switch(&items,3,3,13,0);
    menu_add_item_switch(&items,4,3,14,0);
    
    menu_add_item_list(&items,0,4,&bottle_options[0], &bottles_texture);
    menu_add_item_list(&items,1,4,&bottle_options[1], &bottles_texture);
    menu_add_item_list(&items,2,4,&bottle_options[2], &bottles_texture);
    menu_add_item_list(&items,3,4,&bottle_options[3], &bottles_texture);
    menu_add_item_list(&items,4,4,&bottle_options[4], &bottles_texture);
    menu_add_item_list(&items,5,4,&bottle_options[5], &bottles_texture);

    menu_add_item_list(&items,5,1,&trade_quest_options[0], &trade_quest_texture);
    menu_add_item_list(&items,5,2,&trade_quest_options[1], &trade_quest_texture);
    menu_add_item_list(&items,5,3,&trade_quest_options[2], &trade_quest_texture);

    masks.selected_item = menu_add_button(&masks,0,0,"return",menu_return,NULL);
    masks.callback_proc = masks_callback_proc;
    menu_set_cell(&masks,16,16);
    menu_set_padding(&masks,5,5);
    menu_add_item_switch(&masks,0,1,0,1);
    menu_add_item_switch(&masks,1,1,1,1);
    menu_add_item_switch(&masks,2,1,2,1);
    menu_add_item_switch(&masks,3,1,3,1);
    menu_add_item_switch(&masks,4,1,4,1);
    menu_add_item_switch(&masks,5,1,5,1);

    menu_add_item_switch(&masks,0,2,6,1);
    menu_add_item_switch(&masks,1,2,7,1);
    menu_add_item_switch(&masks,2,2,8,1);
    menu_add_item_switch(&masks,3,2,9,1);
    menu_add_item_switch(&masks,4,2,10,1);
    menu_add_item_switch(&masks,5,2,11,1);

    menu_add_item_switch(&masks,0,3,12,1);
    menu_add_item_switch(&masks,1,3,13,1);
    menu_add_item_switch(&masks,2,3,14,1);
    menu_add_item_switch(&masks,3,3,15,1);
    menu_add_item_switch(&masks,4,3,16,1);
    menu_add_item_switch(&masks,5,3,17,1);

    menu_add_item_switch(&masks,0,4,18,1);
    menu_add_item_switch(&masks,1,4,19,1);
    menu_add_item_switch(&masks,2,4,20,1);
    menu_add_item_switch(&masks,3,4,21,1);
    menu_add_item_switch(&masks,4,4,22,1);
    menu_add_item_switch(&masks,5,4,23,1);

    return &menu;
}