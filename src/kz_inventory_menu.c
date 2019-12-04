#include <stdlib.h>
#include "menu.h"
#include "kz.h"
#include "resource.h"

#define ITEM_SCREEN_SIZE 11

static uint8_t dungeon_idx = 0;
static uint8_t dungeon_keys = 0x00;
static uint8_t dungeon_items = 0x00;
static uint8_t stray_fairies = 0x00;

#ifndef LITE
struct item_map_row {
    int8_t     *slot;
    int8_t      item;
};
#else
struct item_map_row {
    char       *tooltip;
    int8_t     *slot;
    int8_t      item;
};
#endif

struct switch_data {
    uint32_t    bitmask;
    char       *tooltip;
};

struct song_data {
    uint32_t        bitmask;
    uint32_t        color;
    char           *tooltip;
};

struct capacity_upgrade_option{
    uint8_t     shift;
    uint16_t    item_tile;
    int         option_cnt;
    uint8_t     cap_vals[8];
    int         tiles_cnt;
    char       *tooltip;
};

struct capacity_upgrade_data{
    int8_t      idx;
    int8_t      value;
};

static struct capacity_upgrade_option capacity_options[] = {
    {
        3, Z2_ITEM_BOMB_BAG_20, 0b111,
        { 0, 2, 3, 4, 0, 0, 0, 0 }, -1,
        "bomb bag"
    },
    {
        0, Z2_ITEM_QUIVER_30, 0b111,
        { 0, 3, 4, 5, 6, 2, 3, 4 }, -1,
        "quiver"
    },
    {
        12, Z2_ITEM_ADULTS_WALLET, 0b11,
        { -1, -1, -1, -1, -1, -1, -1, -1}, -1, "wallet"
    },
};

#ifndef LITE
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
#else
static struct item_map_row item_map_table[] = {
    {"ocarina", &z2_file.items[Z2_SLOT_OCARINA], Z2_ITEM_OCARINA },
    {"bow", &z2_file.items[Z2_SLOT_BOW], Z2_ITEM_HEROS_BOW },
    {"fire arrow", &z2_file.items[Z2_SLOT_FIRE_ARROW], Z2_ITEM_FIRE_ARROW },
    {"ice arrow", &z2_file.items[Z2_SLOT_ICE_ARROW], Z2_ITEM_ICE_ARROW },
    {"light arrow", &z2_file.items[Z2_SLOT_LIGHT_ARROW], Z2_ITEM_LIGHT_ARROW },
    {"bomb", &z2_file.items[Z2_SLOT_BOMB], Z2_ITEM_BOMB },
    {"bombchu", &z2_file.items[Z2_SLOT_BOMBCHU], Z2_ITEM_BOMBCHU },
    {"stick", &z2_file.items[Z2_SLOT_STICK], Z2_ITEM_STICK },
    {"nut", &z2_file.items[Z2_SLOT_NUT], Z2_ITEM_NUT },
    {"magic bean", &z2_file.items[Z2_SLOT_MAGIC_BEAN], Z2_ITEM_MAGIC_BEAN },
    {"powder keg", &z2_file.items[Z2_SLOT_POWDER_KEG], Z2_ITEM_POWDER_KEG },
    {"pictograph box", &z2_file.items[Z2_SLOT_PICTOGRAPH_BOX], Z2_ITEM_PICTOGRAPH_BOX },
    {"lens of truth", &z2_file.items[Z2_SLOT_LENS], Z2_ITEM_LENS },
    {"hookshot", &z2_file.items[Z2_SLOT_HOOKSHOT], Z2_ITEM_HOOKSHOT },
    {"great fairy sword", &z2_file.items[Z2_SLOT_GREAT_FAIRY_SWORD], Z2_ITEM_GREAT_FAIRY_SWORD },
};

static struct item_map_row mask_map_table[] = {
    {"postman", &z2_file.masks[Z2_SLOT_POSTMAN], Z2_MASK_POSTMAN },
    {"all night", &z2_file.masks[Z2_SLOT_ALL_NIGHT], Z2_MASK_ALL_NIGHT },
    {"blast", &z2_file.masks[Z2_SLOT_BLAST], Z2_MASK_BLAST },
    {"stone", &z2_file.masks[Z2_SLOT_STONE], Z2_MASK_STONE },
    {"great fairy", &z2_file.masks[Z2_SLOT_GREAT_FAIRY], Z2_MASK_GREAT_FAIRY },
    {"deku", &z2_file.masks[Z2_SLOT_DEKU], Z2_MASK_DEKU },

    {"keaton", &z2_file.masks[Z2_SLOT_KEATON], Z2_MASK_KEATON },
    {"bremen", &z2_file.masks[Z2_SLOT_BREMEN], Z2_MASK_BREMEN },
    {"bunny hood", &z2_file.masks[Z2_SLOT_BUNNY_HOOD], Z2_MASK_BUNNY_HOOD },
    {"don gero", &z2_file.masks[Z2_SLOT_DON_GERO], Z2_MASK_DON_GERO },
    {"scents", &z2_file.masks[Z2_SLOT_SCENTS], Z2_MASK_SCENTS },
    {"goron", &z2_file.masks[Z2_SLOT_GORON], Z2_MASK_GORON },

    {"romani", &z2_file.masks[Z2_SLOT_ROMANI], Z2_MASK_ROMANI },
    {"circus leader", &z2_file.masks[Z2_SLOT_CIRCUS_LEADER], Z2_MASK_CIRCUS_LEADER },
    {"kafei", &z2_file.masks[Z2_SLOT_KAFEI], Z2_MASK_KAFEI },
    {"couples", &z2_file.masks[Z2_SLOT_COUPLE], Z2_MASK_COUPLE },
    {"mask of truth", &z2_file.masks[Z2_SLOT_MASK_OF_TRUTH], Z2_MASK_MASK_OF_TRUTH },
    {"zora", &z2_file.masks[Z2_SLOT_ZORA], Z2_MASK_ZORA },

    {"kamaro", &z2_file.masks[Z2_SLOT_KAMARO], Z2_MASK_KAMARO },
    {"gibdo", &z2_file.masks[Z2_SLOT_GIBDO], Z2_MASK_GIBDO },
    {"garo", &z2_file.masks[Z2_SLOT_GARO], Z2_MASK_GARO },
    {"captain's hat", &z2_file.masks[Z2_SLOT_CAPTAIN], Z2_MASK_CAPTAINSHAT },
    {"giant's", &z2_file.masks[Z2_SLOT_GIANT], Z2_MASK_GIANT },
    {"fierce deity", &z2_file.masks[Z2_SLOT_FIERCE_DEITY], Z2_MASK_FIERCE_DEITY },
};
#endif

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

static struct switch_data quest_status_table[] = {
    { 0x0001,       "odolwa's remains" },
    { 0x0002,       "goht's remains" },
    { 0x0004,       "gyorg's remains" },
    { 0x0008,       "twinmold's remains" },
    { 0x0040000,    "bomber's notebook" }
};

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

static char *dungeon_names[] = {
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

static int max_health_callback(struct menu_item *item, enum menu_callback callback, void *data, uint32_t value){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_file.max_health = value;
        if(z2_file.has_double_defense) z2_file.defense_hearts = value/16;
        return 1;
    }
    return 0;
}

static int double_defense_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback==MENU_CALLBACK_ACTIVATE){
        z2_file.has_double_defense = !z2_file.has_double_defense;
        if(z2_file.has_double_defense){
            z2_file.defense_hearts = z2_file.max_health / 16;
        }else{
            z2_file.defense_hearts = 0;
        }
        return 1;
    }else if (callback == MENU_CALLBACK_UPDATE){
        menu_checkbox_set(item,z2_file.has_double_defense);
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
    }else if (callback == MENU_CALLBACK_UPDATE){
        if((int)data == 0){
            menu_checkbox_set(item,z2_file.has_magic);
        }else if((int)data == 1){
            menu_checkbox_set(item,z2_file.has_double_magic);
        }
        return 1;
    }
    return 0;
}

static int change_selected_dungeon(struct menu_item *item, enum menu_callback callback, void *data, int idx){
    if(callback == MENU_CALLBACK_ACTIVATE){
        dungeon_keys = z2_file.dungeon_keys[idx];
        dungeon_items = z2_file.dungeon_items[idx].dungeon_item;
        stray_fairies = z2_file.stray_fairies[idx];
        return 1;
    }
    return 0;
}

static int update_dungeon_keys(struct menu_item *item, enum menu_callback callback, void *data, uint32_t val){
    if(callback == MENU_CALLBACK_UPDATE){
        z2_file.dungeon_keys[dungeon_idx] = val;
    }
    return 0;
}

static int update_stray_fairies(struct menu_item *item, enum menu_callback callback, void *data, uint32_t val){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_file.stray_fairies[dungeon_idx] = val;
    }
    return 0;
}

static int dungeon_item_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        dungeon_items = dungeon_items ^ (uint8_t)((int)data);
        z2_file.dungeon_items[dungeon_idx].dungeon_item = dungeon_items;
        return 1;
    }else if(callback == MENU_CALLBACK_UPDATE){
        menu_switch_set(item,(dungeon_items & (uint8_t)((int)data)) > 0);
        return 1;
    }
    return 0;
}

static int quest_status_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_file.quest_status = z2_file.quest_status ^ (int)data;
        return 1;
    }else if(callback == MENU_CALLBACK_UPDATE){
        menu_switch_set(item,(z2_file.quest_status & (int)data) > 0);
        return 1;
    }
    return 0;
}

static int owl_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_file.owls_hit = z2_file.owls_hit ^ (int)data;
        return 1;
    }else if(callback == MENU_CALLBACK_UPDATE){
        menu_switch_set(item,(z2_file.owls_hit & (int)data) > 0);
        return 1;
    }
    return 0;
}

static int cap_callback(struct menu_item *item, enum menu_callback callback, void *data){
    struct capacity_upgrade_data *cdata = data;
    struct capacity_upgrade_option *row = &capacity_options[cdata->idx];
    if(callback == MENU_CALLBACK_ACTIVATE){
        int v = ++cdata->value;
        z2_file.equipment_upgrades = (z2_file.equipment_upgrades & ~(row->option_cnt << row->shift)) | (v << row->shift);
        return 1;
    }else{
        int v = (z2_file.equipment_upgrades >> row->shift) & row->option_cnt;
        cdata->value = v - 1;
        return 1;
    }
    return 0;
}

static int item_switch_callback(struct menu_item *item, enum menu_callback callback, void *data){
    struct item_switch_data *swdata = data;
    struct item_map_row *row = NULL;
    if(swdata->map == 0){
        row = &item_map_table[swdata->map_idx];
    }else{
        row = &mask_map_table[swdata->map_idx];
    }
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(*row->slot == row->item){
            *row->slot = Z2_ITEM_NULL;
        }else{
            *row->slot = row->item;
        }
        return 1;
    }else if(callback == MENU_CALLBACK_UPDATE){
        if(*row->slot == row->item){
            
#ifndef LITE
            menu_switch_set(item,1);
#else
            menu_checkbox_set(item,1);
#endif
        }else{
#ifndef LITE
            menu_switch_set(item,0);
#else
            menu_checkbox_set(item,0);
#endif
        }
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
            struct item_switch_data *data = malloc(sizeof(*data));
            data->map = 0;
            data->map_idx = i;
#ifndef LITE
            menu_add_switch(&items,x++,y,item_switch_callback,data,get_item_texture(item_map_table[i].item,0),NULL,
                            0xFFFFFFFF, 0xFFFFFFFF,0,1,
                            16,16,NULL);
#else
            menu_add_checkbox(&items,x++,y,item_switch_callback,data,item_map_table[i].tooltip);
#endif
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
        static int8_t threebit_options[] = {Z2_ITEM_NULL,0,1,2,3,4,5,6};
        static int8_t twobit_options[] = {Z2_ITEM_NULL,0,1,2,3};
        for(int i=0;i<nopt;i++){
            struct capacity_upgrade_data *capdata = malloc(sizeof(*capdata));
            
            capdata->value = 0;
            capdata->idx = i;

            int8_t *options = NULL;
            int8_t option_cnt = 0;
            if(capacity_options[i].option_cnt == 7){
                options = threebit_options;
                option_cnt = 8;
            }else{
                options = twobit_options;
                option_cnt = 4;
            }
            menu_add_item_list(&items,0,y++,cap_callback,capdata,capacity_options[i].item_tile,
                                options,option_cnt,&capdata->value,capacity_options[i].cap_vals,
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
            struct item_switch_data *data = malloc(sizeof(*data));
            data->map = 1;
            data->map_idx = i;
            #ifndef LITE
            menu_add_switch(&masks,x++,y,item_switch_callback,data,get_item_texture(mask_map_table[i].item,0), NULL,
                            0xFFFFFFFF,0xFFFFFFFF,0,1,
                            16,16,NULL);
            #else
            menu_add_checkbox(&masks,x++,y,item_switch_callback,data,mask_map_table[i].tooltip);
            #endif
            if(x==6){
                x=0;
                y++;
            }
        }
    }
    
    // Populate quest status menu
    {
        menu_set_padding(&quest_status,0,1);
        quest_status.selected_item = menu_add_button(&quest_status,0,0,"return",menu_return,NULL);

        int x = 0;
        int y = 1;
        int oy = 0;
        int ox = 0;

        struct menu_item *item;
        menu_add(&quest_status,x,y,"max health");
        menu_add_number_input(&quest_status,15,y++,max_health_callback,NULL,16,4,&z2_file.max_health,sizeof(z2_file.max_health));
        menu_add_checkbox(&quest_status,0,y,double_defense_callback,NULL,NULL);
        menu_add(&quest_status,2,y++,"double defense");
        menu_add_checkbox(&quest_status,0,y,magic_callback,(void*)0,NULL);
        menu_add(&quest_status,2,y++,"magic");
        menu_add_checkbox(&quest_status,0,y,magic_callback,(void*)1,NULL);
        menu_add(&quest_status,2,y++,"double magic");
        menu_add(&quest_status,0,y,"dungeon");
        menu_add_list(&quest_status,15,y++,dungeon_names,dungeon_name_values,1,4,&dungeon_idx,change_selected_dungeon,NULL);
        menu_add(&quest_status,0,y,"small keys");
        menu_add_number_input(&quest_status,15,y++,update_dungeon_keys,NULL,16,2,&dungeon_keys,1);
        menu_add(&quest_status,0,y,"stray fairies");
        menu_add_number_input(&quest_status,15,y++,update_stray_fairies,NULL,16,2,&stray_fairies,1);
        oy = 5;

        gfx_texture *dungeon_items_tex = resource_get(R_Z2_DUNGEON);

        item = menu_add_switch(&quest_status,0,y,dungeon_item_callback,(void*)1,
                               dungeon_items_tex,NULL,0xFFFFFFFF,0xFFFFFFFF,
                               6,dungeon_items_tex->y_tiles/2 + 6,16,16,"boss key");
        set_item_offset(item,0,oy);

        item = menu_add_switch(&quest_status,1,y,dungeon_item_callback,(void*)2,
                               dungeon_items_tex,NULL,0xFFFFFFFF,0xFFFFFFFF,
                               7,dungeon_items_tex->y_tiles/2 + 7,16,16,"compass");
        set_item_offset(item,10,oy);

        item = menu_add_switch(&quest_status,2,y,dungeon_item_callback,(void*)4,
                               dungeon_items_tex,NULL,0xFFFFFFFF,0xFFFFFFFF,
                               8,dungeon_items_tex->y_tiles/2 + 8,16,16,"map");

        set_item_offset(item,20,oy);
        oy+=16;
        for(int i=0;i<sizeof(quest_status_table)/sizeof(*quest_status_table);i++){
            item = menu_add_switch(&quest_status,x,y,quest_status_callback,(void*)quest_status_table[i].bitmask,
                                    get_item_texture(Z2_ITEM_ODOLWAS_REMAINS + i,0),NULL,0xFFFFFFFF, 0xFFFFFFFF,
                                    0, 1, 16,16,quest_status_table[i].tooltip);

            set_item_offset(item,x++ * 10,oy);
        }
        x=0;
        y++;
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

            item = menu_add_switch(&quest_status,x,y,owl_callback,(void*)owl_data_table[i].bitmask,
                                    owl_icon_texture, NULL, 0xFFFFFFFF, 0xFFFFFFFF, 0, 1,
                                    16, 8, owl_data_table[i].tooltip);

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
            item = menu_add_switch(&quest_status,x,y,quest_status_callback,(void*)song_data_table[i].bitmask,
                                    note_texture, NULL,song_data_table[i].color,0x808080FF,0,0
                                    ,10,16,song_data_table[i].tooltip);

            set_item_offset(item,ox,oy);
        }
    }

    // Populate amounts menu
    {
        menu_set_cell(&amounts,16,16);
        amounts.selected_item = menu_add_button(&amounts,0,0,"return",menu_return,NULL);

        uint8_t items[] = {
            Z2_ITEM_HEROS_BOW, Z2_ITEM_BOMB, Z2_ITEM_BOMBCHU,
            Z2_ITEM_STICK, Z2_ITEM_NUT, Z2_ITEM_MAGIC_BEAN,
            Z2_ITEM_POWDER_KEG
        };
        struct menu_item *item;
        int y = 1, x = 0;
        for(int i=0;i<sizeof(items)/sizeof(*items);i++){
            menu_add_gfx(&amounts,x,y,get_item_texture(items[i],0),0,12,12);
            item = menu_add_number_input(&amounts,x+1,y,NULL,NULL,10,3,&z2_file.ammo[items[i]],1);
            set_item_offset(item,5,2);
            x+=3;
            x%=9;
            if(x==0) y++;
        }
        gfx_texture *dungeon_tex = resource_get(R_Z2_DUNGEON);
        menu_add_gfx(&amounts,3,3,dungeon_tex,12,12,12);
        item = menu_add_number_input(&amounts,4,3,NULL,NULL,10,3,&z2_file.current_magic,1);
        set_item_offset(item,5,2);
        menu_add_gfx(&amounts,6,3,dungeon_tex,1,12,12);
        item = menu_add_number_input(&amounts,7,3,NULL,NULL,10,5,&z2_file.current_health,2);
        set_item_offset(item,5,2);
        
    }
    
    return &menu;
}
