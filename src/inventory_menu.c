#include <stdlib.h>
#include "menu.h"
#include "kz.h"

#define ITEM_SCREEN_SIZE 11

static gfx_texture *items_texture = NULL;
static gfx_texture *bottles_texture = NULL;
static gfx_texture *buttons_texture = NULL;
static gfx_texture *masks_texture = NULL;
static gfx_texture *trade_quest_texture = NULL;
static gfx_texture *owl_icon_texture = NULL;
static gfx_texture *remains_texture = NULL;
static gfx_texture *note_texture = NULL;
static gfx_texture *notebook_texture = NULL;

static struct menu_item *tooltip = NULL;

struct item_map_row {
    int8_t     *slot;
    int8_t      item;
};

struct item_list_row {
    int8_t     *slot;
    int8_t      option_cnt;
    int8_t     *options;
};

struct switch_data {
    uint32_t bitmask;
    const char *tooltip;
};

struct song_data {
    uint32_t bitmask;
    uint32_t color;
    const char *tooltip;
};

static struct item_map_row item_map_table[] = {
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

static struct item_map_row mask_map_table[] = {
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

static struct switch_data owl_data_table[] = {
    { 0x0200,   "stone tower" },
    { 0x0100,   "ikana canyon" },
    { 0x0080,   "southern swamp" },
    { 0x0040,   "woodfall" },
    { 0x0020,   "milk road" },
    { 0x0010,   "clock town" },
    { 0x0008,   "mountain village" },
    { 0x0004,   "snowhead" },
    { 0x0002,   "zora cape" },
    { 0x0001,   "great bay" },
    { 0x8000,   "hidden owl" },
};

static struct switch_data remains_data_table[] = {
    { 0x0001,   "odolwa's remains" },
    { 0x0002,   "goht's remains" },
    { 0x0004,   "gyorg's remains" },
    { 0x0008,   "twinmold's remains" },
};

static struct switch_data bombers_notebook_table = 
    { 0x0040000,    "bomber's notebook" }
;

static struct song_data song_data_table[] = {
    { 0x0001000, 0xFFFFFFFF, "song of time" },
    { 0x0002000, 0xFFFFFFFF, "song of healing" },
    { 0x0004000, 0xFFFFFFFF, "epona's song" },
    { 0x0008000, 0xFFFFFFFF, "song of soaring" },
    { 0x0010000, 0xFFFFFFFF, "song of storms" },
    { 0x0000040, 0x96FF64FF, "sonata of awakening" },
    { 0x0000080, 0xFF5028FF, "goron lullaby" },
    { 0x0000100, 0x6496FFFF, "new wave bossa nova" },
    { 0x0000200, 0xFFA000FF, "elegy of emptiness" },
    { 0x0000400, 0xFF64FFFF, "oath to order" },
    { 0x1000000, 0xFF5028FF, "lullaby intro" },
    { 0x0020000, 0xFFFFFFFF, "suns song" },
    { 0x0000800, 0xFFFFFFFF, "sarias song" },
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

struct item_switch_data {
    uint8_t                 map_idx;
    uint8_t                 map; 
};

struct item_list_data {
    struct item_list_row   *row;
    uint8_t                 selected_idx;
    gfx_texture           **texture;
    _Bool                   active;
};

struct bit_switch_data{
    void                   *addr;
    uint8_t                 addr_len;
    uint32_t                bitmask;
    gfx_texture           **texture;
    uint16_t                tex_width;
    uint16_t                tex_height;
    uint8_t                 tile;
    _Bool                   has_off_tile;
    uint32_t                color;
    uint32_t                off_color;
    const char             *tooltip;
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

static void draw_item_switch(struct menu_item *item){
    struct item_switch_data *data = item->data;
    gfx_texture *texture = data->map==0?items_texture:masks_texture;
    struct item_map_row map = data->map==0?item_map_table[data->map_idx]:mask_map_table[data->map_idx];
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

static void draw_bit_switch_proc(struct menu_item *item){
    struct bit_switch_data *data = item->data;
    if(item->owner->selected_item == item){
        gfx_draw_rectangle(get_item_x_pos(item),get_item_y_pos(item),16,16,GPACK_RGBA8888(0x80,0x80,0xFF,0x80));
        if(tooltip){
            tooltip->text = data->tooltip;
        }
    }
    gfx_push(gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM));
    uint32_t color = data->color;
    gfx_push(gsDPSetPrimColor(0,0,(color >> 24) & 0xFF,(color >> 16) & 0xFF,(color >> 8) & 0xFF,color & 0xFF));
    gfx_push(gsDPPipeSync());
    uint8_t tile = data->tile;
    uint32_t val = 0;
    gfx_texture *texture = *(data->texture);
    switch(data->addr_len){
        case 1:
            val = *(uint8_t*)data->addr;
            break;
        case 2:
            val = *(uint16_t*)data->addr;
            break;
        case 4:
        default:
            val = *(uint32_t*)data->addr;
            break;
    }
    if(!(val & data->bitmask)){
        if(data->has_off_tile){
            size_t tiles_cnt = texture->x_tiles * texture->y_tiles;
            if(tiles_cnt==1) tile++;
            else{
                tile = tile + (texture->x_tiles * texture->y_tiles)/2;
            }
        }else{
            uint32_t off_color = data->off_color;
            gfx_push(gsDPSetPrimColor(0,0,(off_color >> 24) & 0xFF,(off_color >> 16) & 0xFF,(off_color >> 8) & 0xFF,off_color & 0xFF));
        }
    }
    gfx_draw_sprite(texture,get_item_x_pos(item),get_item_y_pos(item),tile,data->tex_width,data->tex_height);
}

static void activate_bit_switch_proc(struct menu_item *item){
    struct bit_switch_data *data = item->data;
    switch(data->addr_len){
        case 1:
            *(uint8_t*)(data->addr) = *(uint8_t*)(data->addr) ^ data->bitmask;
            break;
        case 2:
            *(uint16_t*)(data->addr) = *(uint16_t*)(data->addr) ^ data->bitmask;
            break;
        case 4:
        default:
            *(uint32_t*)(data->addr) = *(uint32_t*)(data->addr) ^ data->bitmask;
            break;
    }
    
}

static void activate_item_switch(struct menu_item *item){
    struct item_switch_data *data = item->data;
    struct item_map_row map = data->map==0?item_map_table[data->map_idx]:mask_map_table[data->map_idx];
    if(*map.slot == Z2_ITEM_NULL) *map.slot = map.item;
    else *map.slot = Z2_ITEM_NULL;
}

static struct menu_item *menu_add_item_switch(struct menu *menu, uint16_t x, uint16_t y, uint8_t map_idx, uint8_t map){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_switch_data *data = malloc(sizeof(*data));
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

static struct menu_item *menu_add_bit_switch(struct menu *menu, uint16_t x, uint16_t y, 
                                            void *addr, uint8_t addr_len, uint32_t bitmask,  
                                            gfx_texture **texture, uint16_t tex_width, uint16_t tex_height,
                                            int8_t tile, _Bool has_off_tile, const char *tooltip){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct bit_switch_data *data = malloc(sizeof(*data));
        data->addr = addr;
        data->addr_len = addr_len;
        data->bitmask = bitmask;
        data->texture = texture;
        data->tile = tile;
        data->tex_width = tex_width;
        data->tex_height = tex_height;
        data->has_off_tile = has_off_tile;
        data->color = 0xFFFFFFFF;
        data->tooltip = tooltip;
        item->draw_proc = draw_bit_switch_proc;
        item->activate_proc = activate_bit_switch_proc;
        item->interactive = 1;
        item->data = data;
    }
    return item;
}

static void masks_callback_proc(enum menu_callback callback){
    if(callback == MENU_CALLBACK_ENTER){
        masks_texture = gfx_load_icon_item_static(Z2_MASK_DEKU,Z2_MASK_GIANT,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
    }else{
        gfx_destroy_texture(masks_texture);
    }
}

static void items_callback_proc(enum menu_callback callback){
    if(callback == MENU_CALLBACK_ENTER){
        items_texture = gfx_load_icon_item_static(Z2_ITEM_OCARINA,Z2_ITEM_GREAT_FAIRY_SWORD,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
        bottles_texture = gfx_load_icon_item_static(Z2_ITEM_BOTTLE,Z2_ITEM_BOTTLE2,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,0);
        trade_quest_texture = gfx_load_icon_item_static(Z2_ITEM_MOONS_TEAR,Z2_ITEM_PENDANT,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,0);
        buttons_texture = gfx_load_game_texture(G_IM_FMT_IA,G_IM_SIZ_8b,32,32,1,5,1126,0xF60,0);
    }else{
        gfx_destroy_texture(items_texture);
        gfx_destroy_texture(bottles_texture);
        gfx_destroy_texture(buttons_texture);
        gfx_destroy_texture(trade_quest_texture);
    }
}

static void quest_status_callback_proc(enum menu_callback callback){
    if(callback == MENU_CALLBACK_ENTER){
        remains_texture = gfx_load_icon_item_static(Z2_ITEM_ODOLWAS_REMAINS,Z2_ITEM_TWINMOLDS_REMAINS,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
        note_texture = gfx_load_icon_item_static(98,98,G_IM_FMT_IA,G_IM_SIZ_8b,16,24,0);
        notebook_texture = gfx_load_icon_item_static(0x6C,0x6C,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
        owl_icon_texture = gfx_load_game_texture(G_IM_FMT_RGBA, G_IM_SIZ_32b,24,12,1,1,10,0x14668,1);
    }else{
        gfx_destroy_texture(remains_texture);
        gfx_destroy_texture(owl_icon_texture);
        gfx_destroy_texture(note_texture);
        gfx_destroy_texture(notebook_texture);
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

    // Populate items menu
    {
        items.selected_item = menu_add_button(&items,0,0,"return",menu_return,NULL);
        items.callback_proc = items_callback_proc;
        menu_set_cell(&items,16,16);
        menu_set_padding(&items,5,5);
        int x = 0;
        int y = 1;
        for(int i=0;i<15;i++){
            menu_add_item_switch(&items,x,y,i,0);
            x++;
            if(x==5){
                x=0;
                y++;
            }
        }
        
        for(int i=0;i<6;i++){
            menu_add_item_list(&items,i,4,&bottle_options[i],&bottles_texture);
        }
        
        for(int i=0;i<3;i++){
            menu_add_item_list(&items,5,i+1,&trade_quest_options[i],&trade_quest_texture);
        }
    }

    // Populate masks menu
    {
        masks.selected_item = menu_add_button(&masks,0,0,"return",menu_return,NULL);
        masks.callback_proc = masks_callback_proc;
        menu_set_cell(&masks,16,16);
        menu_set_padding(&masks,5,5);
        int x = 0;
        int y = 1;
        for(int i=0;i<24;i++){
            menu_add_item_switch(&masks,x,y,i,1);
            x++;
            if(x==6){
                x=0;
                y++;
            }
        }
    }

    // Populate quest status menu
    {
        quest_status.selected_item = menu_add_button(&quest_status,0,0,"return",menu_return,NULL);
        quest_status.callback_proc = quest_status_callback_proc;
        menu_set_cell(&quest_status,16,16);
        int x = 0;
        int y = 0;
        for(int i=0;i<sizeof(owl_data_table)/sizeof(*owl_data_table);i++){
            x = i % 5;
            if(i % 5 == 0) y++;
            menu_add_bit_switch(&quest_status,x,y,&z2_file.owls_hit,2,owl_data_table[i].bitmask,&owl_icon_texture,16,8,0,1,owl_data_table[i].tooltip);
        }
        y++;
        for(int i=0;i<sizeof(remains_data_table)/sizeof(*remains_data_table);i++){
            menu_add_bit_switch(&quest_status,x,y,&z2_file.quest_status,4,remains_data_table[i].bitmask,&remains_texture,16,16,i,1,remains_data_table[i].tooltip);
            x++;
        }
        menu_add_bit_switch(&quest_status,x,y,&z2_file.quest_status,4,bombers_notebook_table.bitmask,&notebook_texture,16,16,0,1,bombers_notebook_table.tooltip);
        x=0;
        y++;
        for(int i=0;i<sizeof(song_data_table)/sizeof(*song_data_table);i++){
            x = i % 5;
            if(i % 5 == 0) y++;

            struct menu_item *item = menu_add_bit_switch(&quest_status,x,y,&z2_file.quest_status,4,song_data_table[i].bitmask,&note_texture,10,16,0,0,song_data_table[i].tooltip);
            struct bit_switch_data *data = item->data;
            data->color = song_data_table[i].color;
            data->off_color = 0x808080FF;
        }
        tooltip = menu_add(&quest_status,0,y+1,"");
    }
    return &menu;
}
