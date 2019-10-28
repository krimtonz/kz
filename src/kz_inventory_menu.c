#include <stdlib.h>
#include "menu.h"
#include "kz.h"
#include "resource.h"

#define ITEM_SCREEN_SIZE 11

static uint8_t dungeon_idx = 0;
static uint8_t dungeon_keys = 0x00;
static uint8_t dungeon_items = 0x00;
static uint8_t stray_fairies = 0x00;

struct item_map_row {
    int8_t     *slot;
    int8_t      item;
};

struct switch_data {
    uint32_t    bitmask;
    const char *tooltip;
};

struct song_data {
    uint32_t        bitmask;
    uint32_t        color;
    const char     *tooltip;
};

struct capacity_upgrade_option{
    uint8_t     shift;
    uint16_t    item_tile;
    _Bool       tile_per_option;
    uint8_t     cap_vals[8];
    int         tiles_cnt;
    const char *tooltip;
};

struct capacity_upgrade_data{
    uint8_t     shift;
    _Bool       tile_per_option;
    int8_t      value;
};

static struct capacity_upgrade_option capacity_options[] = {
    {
        3, Z2_ITEM_BOMB_BAG_20,1,
        { 0, 2, 3, 4, 0, 0, 0, 0 }, -1,
        "bomb bag"
    },
    {
        0, Z2_ITEM_QUIVER_30,1,
        { 0, 3, 4, 5, 6, 2, 3, 4 }, -1,
        "quiver"
    },
    {
        17, Z2_ITEM_STICK,0,
        { 0, 1, 2, 3, 0, 2, 3, 4}, 1,
        "stick upgrade"
    },
    {
        20, Z2_ITEM_NUT,0,
        { 0, 2, 3, 4, 0, 7, 0, 7}, 1,
        "nut upgrade"
    },
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
#if Z2_VERSION!=NZSE
    Z2_ITEM_BOTTLE2,
    Z2_ITEM_BOTTLE3,
#endif
    Z2_ITEM_SEAHORSE,
    Z2_ITEM_CHATEAU_ROMANI,
    Z2_ITEM_HYLIAN_LOACH,
#if Z2_VERSION==NZSE
    Z2_ITEM_BOTTLE2,
#else
    Z2_ITEM_BOTTLE4,
    Z2_ITEM_BOTTLE5,
    Z2_ITEM_BOTTLE6,
    Z2_ITEM_BOTTLE7,
    Z2_ITEM_BOTTLE8,
    Z2_ITEM_BOTTLE9,
    Z2_ITEM_BOTTLE10,
#endif
};

static int8_t trade_quest_contents[] = {
    Z2_ITEM_NULL,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_LAND_DEED,
    Z2_ITEM_SWAP_DEED,
    Z2_ITEM_MOUNTAIN_DEED,
    Z2_ITEM_OCEAN_DEED,
#if Z2_VERSION!=NZSE
    Z2_ITEM_POACHERS_SAW,
    Z2_ITEM_BROKEN_GORON_SWORD,
    Z2_ITEM_PRESCRIPTION,
    Z2_ITEM_SPEEDFROG,
    Z2_ITEM_EYEDROPS,
#endif
    Z2_ITEM_ROOM_KEY,
    Z2_ITEM_MAMA_LETTER,
#if Z2_VERSION!=NZSE
    Z2_ITEM_MOONS_TEAR2,
    Z2_ITEM_MOONS_TEAR3,
    Z2_ITEM_MOONS_TEAR4,
    Z2_ITEM_MOONS_TEAR5,
    Z2_ITEM_MOONS_TEAR6,
    Z2_ITEM_MOONS_TEAR7,
    Z2_ITEM_MOONS_TEAR8,
    Z2_ITEM_MOONS_TEAR9,
#endif
    Z2_ITEM_KAFEI_LETTER,
    Z2_ITEM_PENDANT,
#if Z2_VERSION!=NZSE
    Z2_ITEM_MOONS_TEAR10,
    Z2_ITEM_MOONS_TEAR11,
    Z2_ITEM_MOONS_TEAR12,
    Z2_ITEM_MOONS_TEAR13,
    Z2_ITEM_MOONS_TEAR14,
    Z2_ITEM_MOONS_TEAR15,
    Z2_ITEM_MOONS_TEAR16,
#endif
};

static const char *dungeon_names[] = {
    "woodfall", "snowhead", "great bay",
    "stone tower",
};

static uint8_t dungeon_name_values[] = {
    0, 1, 2,
    3,
};

struct item_switch_data {
    uint8_t                 map_idx;
    uint8_t                 map;
};

static void draw_item_switch(struct menu_item *item){
    struct item_switch_data *data = item->data;
    struct item_map_row map = data->map==0?item_map_table[data->map_idx]:mask_map_table[data->map_idx];
    gfx_texture *texture = get_item_texture(map.item);
    int tile_idx = *map.slot==Z2_ITEM_NULL ? 1 : 0;
    if(item->owner->selected_item == item){
        gfx_draw_rectangle(get_item_x_pos(item),get_item_y_pos(item),16,16,GPACK_RGBA8888(0x80,0x80,0xFF,0x80));
    }
    gfx_draw_sprite(texture,get_item_x_pos(item),get_item_y_pos(item),tile_idx,16,16);
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

static int max_health_callback(struct menu_item *item, enum menu_callback callback, void *data, uint32_t value){
    z2_file.max_health = value;
    if(z2_file.defense_hearts>0) z2_file.defense_hearts = value;
    return 1;
}

static int double_defense_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback==MENU_CALLBACK_ACTIVATE){
        if(z2_file.defense_hearts==0){
            z2_file.defense_hearts = z2_file.max_health;
        }else{
            z2_file.defense_hearts = 0;
        }
        return 1;
    }
    return 0;
}

static int magic_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback==MENU_CALLBACK_ACTIVATE){
        z2_file.magic_level = 0;
        if((int)data == 0){
            z2_file.has_magic = !z2_file.has_magic;
            if(z2_file.has_magic){
                z2_file.current_magic = 0x30;
            }else{
                z2_file.has_double_magic = 0;
            }
        }
        else if((int)data == 1){
            z2_file.has_double_magic = !z2_file.has_double_magic;
            if(z2_file.has_double_magic){
                z2_file.has_magic = 1;
                z2_file.current_magic = 0x60;
            }else if(z2_file.has_magic && z2_file.current_magic>0x30){
                z2_file.current_magic=0x30;
            }
        }
        return 1;
    }
    return 0;
}

static int change_selected_dungeon(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        int selected_idx = (int)data;
        dungeon_keys = z2_file.dungeon_keys[selected_idx];
        dungeon_items = z2_file.dungeon_items[selected_idx].dungeon_item;
        stray_fairies = z2_file.stray_fairies[selected_idx];
        return 1;
    }
    return 0;
}

static int update_dungeon_keys(struct menu_item *item, enum menu_callback callback, void *data, uint32_t val){
    z2_file.dungeon_keys[dungeon_idx] = val;
    return 1;
}

static int update_stray_fairies(struct menu_item *item, enum menu_callback callback, void *data, uint32_t val){
    z2_file.stray_fairies[dungeon_idx] = val;
    return 1;
}

static int set_dungeon_item(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback==MENU_CALLBACK_ACTIVATE){
        dungeon_items = dungeon_items ^ (uint8_t)((int)data);
        z2_file.dungeon_items[dungeon_idx].dungeon_item = dungeon_items;
        return 1;
    }
    return 0;
}

static int cap_callback(struct menu_item *item, enum menu_callback callback, void *data){
    struct capacity_upgrade_data *cdata = data;
    if(callback == MENU_CALLBACK_ACTIVATE){
        int v = cdata->value;
        if(cdata->tile_per_option) v++;
        z2_file.equipment_upgrades = (z2_file.equipment_upgrades & ~(0b111 << cdata->shift)) | (v << cdata->shift);
        return 1;
    }else{
        int v = (z2_file.equipment_upgrades >> cdata->shift) & 0b111;
        if(cdata->tile_per_option) v--;
        cdata->value = v;
        return 1;
    }
    return 0;
}

struct menu *create_inventory_menu(){
    static struct menu menu;
    static struct menu items;
    static struct menu masks;
    static struct menu quest_status;
    static struct menu amounts;

    static struct tilebg_info list_bg;
    list_bg.texture = resource_get(R_Z2_BUTTONS);
    list_bg.off_color.color=0xFFFFFFFF;
    list_bg.on_color.color = 0x00FF00FF;
    list_bg.tile = 1;

    menu_init(&menu,kz.main_menu.x,kz.main_menu.y);
    menu_init(&items,0,0);
    menu_init(&masks,0,0);
    menu_init(&quest_status,0,0);
    menu_init(&amounts,0,0);

    menu.selected_item = menu_add_button(&menu,0,0,"return",menu_return,NULL);
    menu_add_submenu(&menu,0,1,&items,"items");
    menu_add_submenu(&menu,0,2,&masks,"masks");
    menu_add_submenu(&menu,0,3,&quest_status,"quest status");
    menu_add_submenu(&menu,0,4,&amounts,"amounts");

    // Populate items menu
    {
        items.selected_item = menu_add_button(&items,0,0,"return",menu_return,NULL);
        menu_set_cell(&items,16,16);
        menu_set_padding(&items,5,5);
        int x = 1;
        int y = 1;
        for(int i=0;i<15;i++){
            menu_add_item_switch(&items,x++,y,i,0);
            if(x==6){
                x=1;
                y++;
            }
        }
        
        static draw_info_t list_draw_info;
        list_draw_info.texture = NULL;
        list_draw_info.enabled_color = MENU_DEFAULT_COLOR;
        list_draw_info.x_scale=1.0f;
        list_draw_info.y_scale=1.0f;
        list_draw_info.background = &list_bg;

        for(int i=0;i<6;i++){
            menu_add_item_list(&items,i+1,4,NULL,NULL,
                               Z2_ITEM_BOTTLE,bottle_contents,
                               sizeof(bottle_contents),
                               &z2_file.items[Z2_SLOT_BOTTLE_1 + i],NULL,-1,&list_draw_info, NULL, "bottle");

        }

        for(int i=0;i<3;i++){
            menu_add_item_list(&items,6,i+1,NULL,NULL,
                               Z2_ITEM_MOONS_TEAR,trade_quest_contents,
                               sizeof(trade_quest_contents),
                               &z2_file.items[Z2_SLOT_QUEST_1 + i*6],NULL,-1,&list_draw_info, NULL, "trade quest");
        }

        int nopt = sizeof(capacity_options)/sizeof(*capacity_options);
        y = 1;
        static int8_t options[] = {0,1,2,3,4,5,6,7};
        static int8_t options_pertile[] = {Z2_ITEM_NULL,0,1,2,3,4,5,6};
        for(int i=0;i<nopt;i++){
            struct capacity_upgrade_data *capdata = malloc(sizeof(*capdata));
            _Bool peropt = capacity_options[i].tile_per_option;
            capdata->shift=capacity_options[i].shift;  
            capdata->tile_per_option = capacity_options[i].tile_per_option;
            menu_add_item_list(&items,0,y++,cap_callback,capdata,capacity_options[i].item_tile,
                                peropt?options_pertile:options,7,&capdata->value,capacity_options[i].cap_vals,
                                capacity_options[i].tiles_cnt,&list_draw_info,NULL,capacity_options[i].tooltip);
        }
    }
    
    // Populate masks menu
    {
        masks.selected_item = menu_add_button(&masks,0,0,"return",menu_return,NULL);
        menu_set_cell(&masks,16,16);
        menu_set_padding(&masks,5,5);
        int x = 0;
        int y = 1;
        for(int i=0;i<24;i++){
            menu_add_item_switch(&masks,x++,y,i,1);
            if(x==6){
                x=0;
                y++;
            }
        }
    }
    
    // Populate quest status menu
    {
        quest_status.selected_item = menu_add_button(&quest_status,0,0,"return",menu_return,NULL);

        int x = 0;
        int y = 1;
        int oy = 0;
        int ox = 0;

        struct menu_item *item;
        menu_add(&quest_status,x,y,"max health");
        menu_add_number_input(&quest_status,15,y++,max_health_callback,NULL,16,4,&z2_file.max_health,sizeof(z2_file.max_health));
        menu_add_switch(&quest_status,0,y++,&z2_file.has_double_defense,2,0x01,double_defense_callback,NULL,"double defense");
        menu_add_switch(&quest_status,0,y++,&z2_file.has_magic,1,0x01,magic_callback,(void*)0,"magic");
        menu_add_switch(&quest_status,0,y++,&z2_file.has_double_magic,1,0x01,magic_callback,(void*)1,"double magic");
        menu_add(&quest_status,0,y++,"dungeon");
        menu_add_list(&quest_status,15,y++,dungeon_names,dungeon_name_values,1,4,&dungeon_idx,change_selected_dungeon);
        menu_add(&quest_status,0,y,"small keys");
        menu_add_number_input(&quest_status,15,y++,update_dungeon_keys,NULL,16,2,&dungeon_keys,1);
        menu_add(&quest_status,0,y,"stray fairies");
        menu_add_number_input(&quest_status,15,y++,update_stray_fairies,NULL,16,2,&stray_fairies,1);
        oy = 5;

        gfx_texture *dungeon_items_tex = resource_get(R_Z2_DUNGEON);

        item = menu_add_bit_switch(&quest_status,0,y,&dungeon_items,1,
                                   0x01,set_dungeon_item,(void*)1,dungeon_items_tex,
                                   16,16,6,1,"boss key",0xFFFFFFFF,0xFFFFFFFF,0);
        set_item_offset(item,0,oy);
        item = menu_add_bit_switch(&quest_status,1,y,&dungeon_items,1,
                                   0x02,set_dungeon_item,(void*)2,dungeon_items_tex,
                                   16,16,7,1,"compass",0xFFFFFFFF,0xFFFFFFFF,0);
        set_item_offset(item,10,oy);
        item = menu_add_bit_switch(&quest_status,2,y++,&dungeon_items,1,0x04,
                                   set_dungeon_item,(void*)4,dungeon_items_tex,
                                   16,16,8,1,"map",0xFFFFFFFF,0xFFFFFFFF,0);
        set_item_offset(item,20,oy);
        oy+=10;
        for(int i=0;i<sizeof(remains_data_table)/sizeof(*remains_data_table);i++){
            item = menu_add_bit_switch(&quest_status,x,y,&z2_file.quest_status,4,
                                        remains_data_table[i].bitmask,NULL,NULL,NULL,
                                        16,16,Z2_ITEM_ODOLWAS_REMAINS + i,1, remains_data_table[i].tooltip,
                                        0xFFFFFFFF,0xFFFFFFFF,1);
            set_item_offset(item,x++ * 10,oy);
        }
        item = menu_add_bit_switch(&quest_status,x,y++,&z2_file.quest_status,4,bombers_notebook_table.bitmask,
                                    NULL,NULL,NULL,16,16,Z2_ITEM_BOMBERS_NOTEBOOK,1,
                                    bombers_notebook_table.tooltip,0xFFFFFFFF,0xFFFFFFFF,1);
        set_item_offset(item,x*10,oy);
        x=0;
        int start_y = y;
        int start_oy = oy;
        int start_ox = ox;
        gfx_texture *owl_icon_texture = resource_get(R_Z2_OWL);
        for(int i=0;i<sizeof(owl_data_table)/sizeof(*owl_data_table);i++){
            x = i % 5;
            ox = x * 6;
            if(i % 5 == 0){
                y++;
                oy+=9;
            }
            struct menu_item *item = menu_add_bit_switch(&quest_status,x,y,&z2_file.owls_hit,2,
                                                         owl_data_table[i].bitmask,NULL,NULL,owl_icon_texture,
                                                         16,8,0,1,owl_data_table[i].tooltip,0xFFFFFFFF,0xFFFFFFFF,0);
            set_item_offset(item,ox,oy);
        }
        y = start_y;
        oy = start_oy;
        ox = start_ox;
        gfx_texture *note_texture = resource_get(R_Z2_NOTE);
        for(int i=0;i<sizeof(song_data_table)/sizeof(*song_data_table);i++){
            x = i % 5 + 6;
            ox = x * 6;
            if(i % 5 == 0){
                y++;
                oy+=8;
            }

            struct menu_item *item = menu_add_bit_switch(&quest_status,x,y,&z2_file.quest_status,4,
                                                         song_data_table[i].bitmask,NULL,NULL,
                                                         note_texture,10,16,0,0,song_data_table[i].tooltip,
                                                         song_data_table[i].color,0x808080FF,0);
            set_item_offset(item,ox,oy);
        }
    }

    // Populate amounts menu
    {
        menu_set_cell(&amounts,16,16);
        amounts.selected_item = menu_add_button(&amounts,0,0,"return",menu_return,NULL);
        z2_rgba32_t rgba = {{0xFF,0xFF,0xFF,0xFF}};
        draw_info_t draw = {
            NULL, 0, -1, 1.f, 1.f, 12, 12, rgba, rgba,
            0, NULL
        };

        uint8_t items[] = {
            Z2_ITEM_HEROS_BOW, Z2_ITEM_BOMB, Z2_ITEM_BOMBCHU,
            Z2_ITEM_STICK, Z2_ITEM_NUT, Z2_ITEM_MAGIC_BEAN,
            Z2_ITEM_POWDER_KEG
        };
        struct menu_item *item;
        int y = 1, x = 0;
        for(int i=0;i<sizeof(items)/sizeof(*items);i++){
            draw.texture = NULL;
            draw.on_tile = items[i];
            menu_add_gfx(&amounts,x,y,&draw,1);
            item = menu_add_number_input(&amounts,++x,y,NULL,NULL,10,3,&z2_file.ammo[items[i]],1);
            set_item_offset(item,0,2);
            x+=2;
            x%=9;
            if(x==0) y++;
        }
        draw.texture = resource_get(R_Z2_DUNGEON);
        draw.on_tile = 12;
        menu_add_gfx(&amounts,3,3,&draw,0);
        item = menu_add_number_input(&amounts,4,3,NULL,NULL,10,3,&z2_file.current_magic,1);
        set_item_offset(item,0,2);
        draw.on_tile = 1;
        menu_add_gfx(&amounts,6,3,&draw,0);
        item = menu_add_number_input(&amounts,7,3,NULL,NULL,10,5,&z2_file.current_health,2);
        set_item_offset(item,0,2);
        
    }
    
    return &menu;
}
