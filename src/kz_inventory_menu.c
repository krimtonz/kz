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

struct item_switch_data {
    uint8_t                 map_idx;
    uint8_t                 map;
};

struct song_data {
    uint32_t        bitmask;
    uint32_t        color;
    char           *tooltip;
};

struct capacity_upgrade_option {
    uint8_t     shift;
    uint16_t    item_tile;
    int         option_cnt;
    int8_t      cap_vals[8];
    int         tiles_cnt;
    char       *tooltip;
};

struct capacity_upgrade_data {
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
    { &z2_file.items[Z2_SLOT_OCARINA],              Z2_ITEM_OCARINA },
    { &z2_file.items[Z2_SLOT_BOW],                  Z2_ITEM_HEROS_BOW },
    { &z2_file.items[Z2_SLOT_FIRE_ARROW],           Z2_ITEM_FIRE_ARROW },
    { &z2_file.items[Z2_SLOT_ICE_ARROW],            Z2_ITEM_ICE_ARROW },
    { &z2_file.items[Z2_SLOT_LIGHT_ARROW],          Z2_ITEM_LIGHT_ARROW },
    { &z2_file.items[Z2_SLOT_BOMB],                 Z2_ITEM_BOMB },
    { &z2_file.items[Z2_SLOT_BOMBCHU],              Z2_ITEM_BOMBCHU },
    { &z2_file.items[Z2_SLOT_STICK],                Z2_ITEM_STICK },
    { &z2_file.items[Z2_SLOT_NUT],                  Z2_ITEM_NUT },
    { &z2_file.items[Z2_SLOT_MAGIC_BEAN],           Z2_ITEM_MAGIC_BEAN },
    { &z2_file.items[Z2_SLOT_POWDER_KEG],           Z2_ITEM_POWDER_KEG },
    { &z2_file.items[Z2_SLOT_PICTOGRAPH_BOX],       Z2_ITEM_PICTOGRAPH_BOX },
    { &z2_file.items[Z2_SLOT_LENS],                 Z2_ITEM_LENS },
    { &z2_file.items[Z2_SLOT_HOOKSHOT],             Z2_ITEM_HOOKSHOT },
    { &z2_file.items[Z2_SLOT_GREAT_FAIRY_SWORD],    Z2_ITEM_GREAT_FAIRY_SWORD },
};

static struct item_map_row mask_map_table[] = {
    { &z2_file.masks[Z2_SLOT_POSTMAN],              Z2_MASK_POSTMAN },
    { &z2_file.masks[Z2_SLOT_ALL_NIGHT],            Z2_MASK_ALL_NIGHT },
    { &z2_file.masks[Z2_SLOT_BLAST],                Z2_MASK_BLAST },
    { &z2_file.masks[Z2_SLOT_STONE],                Z2_MASK_STONE },
    { &z2_file.masks[Z2_SLOT_GREAT_FAIRY],          Z2_MASK_GREAT_FAIRY },
    { &z2_file.masks[Z2_SLOT_DEKU],                 Z2_MASK_DEKU },

    { &z2_file.masks[Z2_SLOT_KEATON],               Z2_MASK_KEATON },
    { &z2_file.masks[Z2_SLOT_BREMEN],               Z2_MASK_BREMEN },
    { &z2_file.masks[Z2_SLOT_BUNNY_HOOD],           Z2_MASK_BUNNY_HOOD },
    { &z2_file.masks[Z2_SLOT_DON_GERO],             Z2_MASK_DON_GERO },
    { &z2_file.masks[Z2_SLOT_SCENTS],               Z2_MASK_SCENTS },
    { &z2_file.masks[Z2_SLOT_GORON],                Z2_MASK_GORON },

    { &z2_file.masks[Z2_SLOT_ROMANI],               Z2_MASK_ROMANI },
    { &z2_file.masks[Z2_SLOT_CIRCUS_LEADER],        Z2_MASK_CIRCUS_LEADER },
    { &z2_file.masks[Z2_SLOT_KAFEI],                Z2_MASK_KAFEI },
    { &z2_file.masks[Z2_SLOT_COUPLE],               Z2_MASK_COUPLE },
    { &z2_file.masks[Z2_SLOT_MASK_OF_TRUTH],        Z2_MASK_MASK_OF_TRUTH },
    { &z2_file.masks[Z2_SLOT_ZORA],                 Z2_MASK_ZORA },

    { &z2_file.masks[Z2_SLOT_KAMARO],               Z2_MASK_KAMARO },
    { &z2_file.masks[Z2_SLOT_GIBDO],                Z2_MASK_GIBDO },
    { &z2_file.masks[Z2_SLOT_GARO],                 Z2_MASK_GARO },
    { &z2_file.masks[Z2_SLOT_CAPTAIN],              Z2_MASK_CAPTAINSHAT },
    { &z2_file.masks[Z2_SLOT_GIANT],                Z2_MASK_GIANT },
    { &z2_file.masks[Z2_SLOT_FIERCE_DEITY],         Z2_MASK_FIERCE_DEITY },
};
#else
static struct item_map_row item_map_table[] = {
    { "ocarina",            &z2_file.items[Z2_SLOT_OCARINA],            Z2_ITEM_OCARINA },
    { "bow",                &z2_file.items[Z2_SLOT_BOW],                Z2_ITEM_HEROS_BOW },
    { "fire arrow",         &z2_file.items[Z2_SLOT_FIRE_ARROW],         Z2_ITEM_FIRE_ARROW },
    { "ice arrow",          &z2_file.items[Z2_SLOT_ICE_ARROW],          Z2_ITEM_ICE_ARROW },
    { "light arrow",        &z2_file.items[Z2_SLOT_LIGHT_ARROW],        Z2_ITEM_LIGHT_ARROW },
    { "bomb",               &z2_file.items[Z2_SLOT_BOMB],               Z2_ITEM_BOMB },
    { "bombchu",            &z2_file.items[Z2_SLOT_BOMBCHU],            Z2_ITEM_BOMBCHU },
    { "stick",              &z2_file.items[Z2_SLOT_STICK],              Z2_ITEM_STICK },
    { "nut",                &z2_file.items[Z2_SLOT_NUT],                Z2_ITEM_NUT },
    { "magic bean",         &z2_file.items[Z2_SLOT_MAGIC_BEAN],         Z2_ITEM_MAGIC_BEAN },
    { "powder keg",         &z2_file.items[Z2_SLOT_POWDER_KEG],         Z2_ITEM_POWDER_KEG },
    { "pictograph box",     &z2_file.items[Z2_SLOT_PICTOGRAPH_BOX],     Z2_ITEM_PICTOGRAPH_BOX },
    { "lens of truth",      &z2_file.items[Z2_SLOT_LENS],               Z2_ITEM_LENS },
    { "hookshot",           &z2_file.items[Z2_SLOT_HOOKSHOT],           Z2_ITEM_HOOKSHOT },
    { "great fairy sword",  &z2_file.items[Z2_SLOT_GREAT_FAIRY_SWORD],  Z2_ITEM_GREAT_FAIRY_SWORD },
};

static struct item_map_row mask_map_table[] = {
    { "postman",            &z2_file.masks[Z2_SLOT_POSTMAN],            Z2_MASK_POSTMAN },
    { "all night",          &z2_file.masks[Z2_SLOT_ALL_NIGHT],          Z2_MASK_ALL_NIGHT },
    { "blast",              &z2_file.masks[Z2_SLOT_BLAST],              Z2_MASK_BLAST },
    { "stone",              &z2_file.masks[Z2_SLOT_STONE],              Z2_MASK_STONE },
    { "great fairy",        &z2_file.masks[Z2_SLOT_GREAT_FAIRY],        Z2_MASK_GREAT_FAIRY },
    { "deku",               &z2_file.masks[Z2_SLOT_DEKU],               Z2_MASK_DEKU },

    { "keaton",             &z2_file.masks[Z2_SLOT_KEATON],             Z2_MASK_KEATON },
    { "bremen",             &z2_file.masks[Z2_SLOT_BREMEN],             Z2_MASK_BREMEN },
    { "bunny hood",         &z2_file.masks[Z2_SLOT_BUNNY_HOOD],         Z2_MASK_BUNNY_HOOD },
    { "don gero",           &z2_file.masks[Z2_SLOT_DON_GERO],           Z2_MASK_DON_GERO },
    { "scents",             &z2_file.masks[Z2_SLOT_SCENTS],             Z2_MASK_SCENTS },
    { "goron",              &z2_file.masks[Z2_SLOT_GORON],              Z2_MASK_GORON },

    { "romani",             &z2_file.masks[Z2_SLOT_ROMANI],             Z2_MASK_ROMANI },
    { "circus leader",      &z2_file.masks[Z2_SLOT_CIRCUS_LEADER],      Z2_MASK_CIRCUS_LEADER },
    { "kafei",              &z2_file.masks[Z2_SLOT_KAFEI],              Z2_MASK_KAFEI },
    { "couples",            &z2_file.masks[Z2_SLOT_COUPLE],             Z2_MASK_COUPLE },
    { "mask of truth",      &z2_file.masks[Z2_SLOT_MASK_OF_TRUTH],      Z2_MASK_MASK_OF_TRUTH },
    { "zora",               &z2_file.masks[Z2_SLOT_ZORA],               Z2_MASK_ZORA },

    { "kamaro",             &z2_file.masks[Z2_SLOT_KAMARO],             Z2_MASK_KAMARO },
    { "gibdo",              &z2_file.masks[Z2_SLOT_GIBDO],              Z2_MASK_GIBDO },
    { "garo",               &z2_file.masks[Z2_SLOT_GARO],               Z2_MASK_GARO },
    { "captain's hat",      &z2_file.masks[Z2_SLOT_CAPTAIN],            Z2_MASK_CAPTAINSHAT },
    { "giant's",            &z2_file.masks[Z2_SLOT_GIANT],              Z2_MASK_GIANT },
    { "fierce deity",       &z2_file.masks[Z2_SLOT_FIERCE_DEITY],       Z2_MASK_FIERCE_DEITY },
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
    { 0x0001000,    0xFFFFFFFF, "song of time" },
    { 0x0002000,    0xFFFFFFFF, "song of healing" },
    { 0x0004000,    0xFFFFFFFF, "epona's song" },
    { 0x0008000,    0xFFFFFFFF, "song of soaring" },
    { 0x0010000,    0xFFFFFFFF, "song of storms" },
    { 0x0000040,    0x96FF64FF, "sonata of awakening" },
    { 0x0000080,    0xFF5028FF, "goron lullaby" },
    { 0x0000100,    0x6496FFFF, "new wave bossa nova" },
    { 0x0000200,    0xFFA000FF, "elegy of emptiness" },
    { 0x0000400,    0xFF64FFFF, "oath to order" },
    { 0x1000000,    0xFF5028FF, "lullaby intro" },
    { 0x0020000,    0xFFFFFFFF, "suns song" },
    { 0x0000800,    0xFFFFFFFF, "sarias song" },
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
    "woodfall",     "snowhead",
    "great bay",    "stone tower",
};

static int max_health_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_NUMBER){
        uint16_t value = (uint16_t)((uint32_t)*event_data);
        z2_file.max_health = value;
        if(z2_file.has_double_defense){
            z2_file.defense_hearts = value / 16;
        }
    }else if(event == MENU_EVENT_UPDATE){
        menu_number_set(handler->subscriber, z2_file.max_health);
    }
    return 1;
}

static int double_defense_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        z2_file.has_double_defense = !z2_file.has_double_defense;
        if(z2_file.has_double_defense){
            z2_file.defense_hearts = z2_file.max_health / 16;
        }else{
            z2_file.defense_hearts = 0;
        }
    }else if (event == MENU_EVENT_UPDATE){
        menu_checkbox_set(handler->subscriber, z2_file.has_double_defense);
    }
    return 1;
}

static int magic_event(event_handler_t *handler, menu_event_t event, void **event_data){
    int data = (int)handler->callback_data;
    if(event == MENU_EVENT_ACTIVATE){
        z2_file.magic_level = 0;
        if((int)handler->callback_data == 0){
            z2_file.has_magic = !z2_file.has_magic;
            if(z2_file.has_magic){
                z2_file.current_magic = 0x30;
            }else{
                z2_file.has_double_magic = 0;
            }
        }
        else if((int)handler->callback_data == 1){
            z2_file.has_double_magic = !z2_file.has_double_magic;
            if(z2_file.has_double_magic){
                z2_file.has_magic = 1;
                z2_file.current_magic = 0x60;
            }else if(z2_file.has_magic && z2_file.current_magic > 0x30){
                z2_file.current_magic = 0x30;
            }
        }
    }else if (event == MENU_EVENT_UPDATE){
        if(data == 0){
            menu_checkbox_set(handler->subscriber, z2_file.has_magic);
        }else if(data == 1){
            menu_checkbox_set(handler->subscriber, z2_file.has_double_magic);
        }
    }
    return 1;
}

static int selected_dungeon_onlist(event_handler_t *handler, menu_event_t event, void **event_data){
    int idx = (int)*event_data;
    dungeon_idx = idx;
    dungeon_keys = z2_file.dungeon_keys[idx];
    dungeon_items = z2_file.dungeon_items[idx].dungeon_item;
    stray_fairies = z2_file.stray_fairies[idx];

    return 1;
}

static int dungeon_keys_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_NUMBER){
        z2_file.dungeon_keys[dungeon_idx] = (uint8_t)((uint32_t)*event_data);
    }else if (event == MENU_EVENT_UPDATE){
        dungeon_keys = z2_file.dungeon_keys[dungeon_idx];
    }
    return 1;
}

static int stray_fairies_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_NUMBER){
        z2_file.stray_fairies[dungeon_idx] = (uint8_t)((uint32_t)*event_data);
    }else if(event == MENU_EVENT_UPDATE){
        stray_fairies = z2_file.stray_fairies[dungeon_idx];
    }
    return 1;
}

static int dungeon_item_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        dungeon_items = dungeon_items ^ (uint8_t)((int)handler->callback_data);
        z2_file.dungeon_items[dungeon_idx].dungeon_item = dungeon_items;
    }else if(event == MENU_EVENT_UPDATE){
        menu_switch_set(handler->subscriber, (dungeon_items & (uint8_t)((int)handler->callback_data)) > 0);
    }
    return 1;
}

static int quest_status_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        z2_file.quest_status = z2_file.quest_status ^ (int)handler->callback_data;
    }else if(event == MENU_EVENT_UPDATE){
        menu_switch_set(handler->subscriber, (z2_file.quest_status & (int)handler->callback_data) > 0);
    }
    return 1;
}

static int owl_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        z2_file.owls_hit = z2_file.owls_hit ^ (int)handler->callback_data;
    }else if(event == MENU_EVENT_UPDATE){
        menu_switch_set(handler->subscriber, (z2_file.owls_hit & (int)handler->callback_data) > 0);
    }
    return 1;
}

static int capacity_event(event_handler_t *handler, menu_event_t event, void **event_data){
    struct capacity_upgrade_data *cdata = handler->callback_data;
    struct capacity_upgrade_option *row = &capacity_options[cdata->idx];
    if(event == MENU_EVENT_ACTIVATE){
        int v = ++cdata->value;
        z2_file.equipment_upgrades = (z2_file.equipment_upgrades & ~(row->option_cnt << row->shift)) | (v << row->shift);
    }else if(event == MENU_EVENT_UPDATE){
        int v = (z2_file.equipment_upgrades >> row->shift) & row->option_cnt;
        cdata->value = v - 1;
    }
    return 1;
}

static int item_switch_event(event_handler_t *handler, menu_event_t event, void **event_data){
    struct item_switch_data *swdata = handler->callback_data;
    struct item_map_row *row = NULL;
    menu_item_t *item = handler->subscriber;
    if(swdata->map == 0){
        row = &item_map_table[swdata->map_idx];
    }else{
        row = &mask_map_table[swdata->map_idx];
    }
    if(event == MENU_EVENT_ACTIVATE){
        if(*row->slot == row->item){
            *row->slot = Z2_ITEM_NULL;
        }else{
            *row->slot = row->item;
        }
    }else if(event == MENU_EVENT_UPDATE){
        if(*row->slot == row->item){

#ifndef LITE
            menu_switch_set(item, 1);
#else
            menu_checkbox_set(item, 1);
#endif
        }else{
#ifndef LITE
            menu_switch_set(item, 0);
#else
            menu_checkbox_set(item, 0);
#endif
        }
    }
    return 1;
}

menu_t *create_inventory_menu(void){
    static menu_t menu;
    static menu_t items;
    static menu_t masks;
    static menu_t quest_status;
    static menu_t amounts;

    menu_init(&menu, 0, 0);
    menu_init(&items, 0, 0);
    menu_init(&masks, 0, 0);
    menu_init(&quest_status, 0, 0);
    menu_init(&amounts, 0, 0);

    menu.selected_item = menu_button_add(&menu, 0, 0, "return", menu_return, NULL);
    menu_submenu_add(&menu, 0, 1, "items", &items);
    menu_submenu_add(&menu, 0, 2, "masks", &masks);
    menu_submenu_add(&menu, 0, 3, "quest status", &quest_status);
    menu_submenu_add(&menu, 0, 4, "amounts", &amounts);

    menu_item_t *item;

    // Populate items menu
    {
        items.selected_item = menu_button_add(&items, 0, 0, "return", menu_return, NULL);
        menu_cell_set(&items, 16, 16);
        menu_padding_set(&items, 5, 5);

        int item_cnt = sizeof(item_map_table) / sizeof(*item_map_table);
        for(int i = 0;i < item_cnt;i++){
            struct item_switch_data *data = malloc(sizeof(*data));
            data->map = 0;
            data->map_idx = i;
#ifndef LITE
            item = menu_switch_add(&items, i % 5 + 1, i / 5 + 1, get_item_texture(item_map_table[i].item, 0), NULL, DEFAULT_COLOR, DEFAULT_COLOR,
                            0, 1, 16, 16, NULL);
#else
            item = menu_checkbox_add(&items, i % 5 + 1, i / 5 + 1);
            item->tooltip = item_map_table[i].tooltip;
#endif
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, item_switch_event, data);
        }

        static menu_sprite_t list_sprite = {
            NULL,   0,      0,      COLOR_GREEN,    DEFAULT_COLOR,
            16,     16,     NULL,   DEFAULT_COLOR,  1,
            0,      NULL,
        };

        list_sprite.background = resource_get(R_Z2_BUTTONS);

        for(int i = 0;i < 6;i++){
            menu_item_list_add(&items, i + 1, 4, Z2_ITEM_BOTTLE, bottle_contents, sizeof(bottle_contents),
                               &z2_file.items[Z2_SLOT_BOTTLE_1 + i], NULL, -1, &list_sprite, "bottle");
        }

        for(int i = 0;i < 3;i++){
            menu_item_list_add(&items, 6, i + 1, Z2_ITEM_MOONS_TEAR, trade_quest_contents, sizeof(trade_quest_contents),
                               &z2_file.items[Z2_SLOT_QUEST_1 + i * 6], NULL, -1, &list_sprite, "trade quest");
        }

        int opt_cnt = sizeof(capacity_options) / sizeof(*capacity_options);
        static int8_t threebit_options[] = { Z2_ITEM_NULL, 0, 1, 2, 3, 4, 5, 6 };
        static int8_t twobit_options[] = { Z2_ITEM_NULL, 0, 1, 2, 3 };
        for(int i = 0;i < opt_cnt;i++){
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
            item = menu_item_list_add(&items, 0, i + 1, capacity_options[i].item_tile,
                               options, option_cnt, &capdata->value, capacity_options[i].cap_vals,
                               capacity_options[i].tiles_cnt, &list_sprite, capacity_options[i].tooltip);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, capacity_event, capdata);
        }
    }

    // Populate masks menu
    {
        masks.selected_item = menu_button_add(&masks, 0, 0, "return", menu_return, NULL);
        menu_cell_set(&masks, 16, 16);
        menu_padding_set(&masks, 5, 5);
        int mask_cnt = sizeof(mask_map_table) / sizeof(*mask_map_table);
        for(int i = 0;i < mask_cnt;i++){
            struct item_switch_data *data = malloc(sizeof(*data));
            data->map = 1;
            data->map_idx = i;
            #ifndef LITE
            item = menu_switch_add(&masks, i % 6, i / 6 + 1, get_item_texture(mask_map_table[i].item, 0), NULL,
                            DEFAULT_COLOR, DEFAULT_COLOR, 0, 1, 16, 16, NULL);
            #else
            item = menu_checkbox_add(&masks, i % 6, i / 6 + 1);
            item->tooltip = mask_map_table[i].tooltip;
            #endif
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, item_switch_event, data);
        }
    }

    // Populate quest status menu
    {
        menu_padding_set(&quest_status, 0, 1);
        quest_status.selected_item = menu_button_add(&quest_status, 0, 0, "return", menu_return, NULL);

        menu_label_add(&quest_status, 0, 1, "max health");
        item = menu_number_input_add(&quest_status, 15, 1, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, max_health_event, NULL);

        item = menu_checkbox_add(&quest_status, 0, 2);
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, double_defense_event, NULL);
        menu_label_add(&quest_status, 2, 2, "double defense");

        item = menu_checkbox_add(&quest_status, 0, 3);
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, magic_event, (void*)0);
        menu_label_add(&quest_status, 2, 3, "magic");

        item = menu_checkbox_add(&quest_status, 0, 4);
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, magic_event, (void*)1);
        menu_label_add(&quest_status, 2, 4, "double magic");

        menu_label_add(&quest_status, 0, 5, "dungeon");
        item = menu_list_add(&quest_status, 15, 5, dungeon_names, 4);
        menu_item_register_event(item, MENU_EVENT_LIST, selected_dungeon_onlist, NULL);

        menu_label_add(&quest_status, 0, 6, "small keys");
        item = menu_number_input_add(&quest_status, 15, 6, 16, 2);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, dungeon_keys_event, (void*)0);

        menu_label_add(&quest_status, 0, 7, "stray fairies");
        item = menu_number_input_add(&quest_status, 15, 7, 16, 2);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, stray_fairies_event, (void*)1);

        gfx_texture *dungeon_items_tex = resource_get(R_Z2_DUNGEON);

        item = menu_switch_add(&quest_status, 0, 8, dungeon_items_tex, NULL, DEFAULT_COLOR, DEFAULT_COLOR,
                               6, dungeon_items_tex->y_tiles / 2 + 6, 16, 16, "boss key");
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, dungeon_item_event, (void*)1);
        menu_item_offset_set(item, 0, 5);

        item = menu_switch_add(&quest_status, 1, 8, dungeon_items_tex, NULL, DEFAULT_COLOR, DEFAULT_COLOR,
                               7, dungeon_items_tex->y_tiles / 2 + 7, 16, 16, "compass");
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, dungeon_item_event, (void*)2);
        menu_item_offset_set(item, 10, 5);

        item = menu_switch_add(&quest_status, 2, 8, dungeon_items_tex, NULL, DEFAULT_COLOR, DEFAULT_COLOR,
                               8, dungeon_items_tex->y_tiles / 2 + 8, 16, 16, "map");
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, dungeon_item_event, (void*)4);
        menu_item_offset_set(item, 20, 5);

        for(int i = 0;i < sizeof(quest_status_table) / sizeof(*quest_status_table);i++){
            item = menu_switch_add(&quest_status, i, 9, get_item_texture(Z2_ITEM_ODOLWAS_REMAINS + i, 0), NULL, DEFAULT_COLOR, DEFAULT_COLOR,
                            0, 1, 16, 16, quest_status_table[i].tooltip);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, quest_status_event, (void*)quest_status_table[i].bitmask);
            menu_item_offset_set(item, i * 10, 12);
        }

        gfx_texture *owl_icon_texture = resource_get(R_Z2_OWL);

        for(int i = 0;i < sizeof(owl_data_table) / sizeof(*owl_data_table);i++){
            item = menu_switch_add(&quest_status, i % 5, i / 5 + 11, owl_icon_texture, NULL, DEFAULT_COLOR, DEFAULT_COLOR,
                                   0, 1, 16, 8, owl_data_table[i].tooltip);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, owl_event, (void*)owl_data_table[i].bitmask);
            menu_item_offset_set(item, i % 5 * 6, ((i / 5) * 9) + 21);
        }

        gfx_texture *note_texture = resource_get(R_Z2_NOTE);

        for(int i = 0;i < sizeof(song_data_table) / sizeof(*song_data_table);i++){
            item = menu_switch_add(&quest_status, i % 5 + 6, i / 5 + 11, note_texture, NULL, song_data_table[i].color, 0x808080FF,
                                   0, 0, 10, 16, song_data_table[i].tooltip);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, quest_status_event, (void*)song_data_table[i].bitmask);
            menu_item_offset_set(item, ((i % 5) + 6) * 6, ((i / 5) * 9) + 21);
        }
    }

    // Populate amounts menu
    {
        menu_cell_set(&amounts, 16, 16);
        amounts.selected_item = menu_button_add(&amounts, 0, 0, "return", menu_return, NULL);

        uint8_t items[] = {
            Z2_ITEM_HEROS_BOW,  Z2_ITEM_BOMB,   Z2_ITEM_BOMBCHU,
            Z2_ITEM_STICK,      Z2_ITEM_NUT,    Z2_ITEM_MAGIC_BEAN,
            Z2_ITEM_POWDER_KEG
        };

        for(int i = 0;i < sizeof(items) / sizeof(*items);i++){
            menu_gfx_add(&amounts, i % 3 * 3, i / 3 + 1, get_item_texture(items[i], 0), 0, 12, 12);
            item = menu_number_input_add(&amounts, i % 3 * 3 + 1, i / 3 + 1, 10, 3);
            menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_byte_event, &z2_file.ammo[items[i]]);
            menu_item_offset_set(item, 5, 2);
        }

        gfx_texture *dungeon_tex = resource_get(R_Z2_DUNGEON);

        menu_gfx_add(&amounts, 3, 3, dungeon_tex, 12, 12, 12);
        item = menu_number_input_add(&amounts, 4, 3, 10, 3);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_byte_event, &z2_file.current_magic);
        menu_item_offset_set(item, 5, 2);

        menu_gfx_add(&amounts, 6, 3, dungeon_tex, 1, 12, 12);
        item = menu_number_input_add(&amounts, 7, 3, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_halfword_event, &z2_file.current_health);
        menu_item_offset_set(item, 5, 2);

        item = menu_gfx_add(&amounts, 0, 4, resource_get(R_Z2_RUPEE), 0, 12, 12);
        item->color = 0xC8FF64FF;
        item = menu_number_input_add(&amounts, 1, 4, 10, 3);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_halfword_event, &z2_file.rupees);
        menu_item_offset_set(item, 5, 2);
    }

    return &menu;
}
