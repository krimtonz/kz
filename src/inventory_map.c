#include "z2.h"
#include "inventory_map.h"

#ifndef LITE
struct item_map_row item_map_table[] = {
    { &z2_file.items[Z2_SLOT_OCARINA],              Z2_ITEM_OCARINA },
    { &z2_file.items[Z2_SLOT_BOW],                  Z2_ITEM_HEROS_BOW },
    { &z2_file.items[Z2_SLOT_FIRE_ARROW],           Z2_ITEM_FIRE_ARROW },
    { &z2_file.items[Z2_SLOT_ICE_ARROW],            Z2_ITEM_ICE_ARROW },
    { &z2_file.items[Z2_SLOT_LIGHT_ARROW],          Z2_ITEM_LIGHT_ARROW },
    { &z2_file.items[Z2_SLOT_QUEST_1],              Z2_ITEM_LAND_DEED },
    
    { &z2_file.items[Z2_SLOT_BOMB],                 Z2_ITEM_BOMB },
    { &z2_file.items[Z2_SLOT_BOMBCHU],              Z2_ITEM_BOMBCHU },
    { &z2_file.items[Z2_SLOT_STICK],                Z2_ITEM_STICK },
    { &z2_file.items[Z2_SLOT_NUT],                  Z2_ITEM_NUT },
    { &z2_file.items[Z2_SLOT_MAGIC_BEAN],           Z2_ITEM_MAGIC_BEAN },
    { &z2_file.items[Z2_SLOT_QUEST_2],              Z2_ITEM_LAND_DEED },

    { &z2_file.items[Z2_SLOT_POWDER_KEG],           Z2_ITEM_POWDER_KEG },
    { &z2_file.items[Z2_SLOT_PICTOGRAPH_BOX],       Z2_ITEM_PICTOGRAPH_BOX },
    { &z2_file.items[Z2_SLOT_LENS],                 Z2_ITEM_LENS },
    { &z2_file.items[Z2_SLOT_HOOKSHOT],             Z2_ITEM_HOOKSHOT },
    { &z2_file.items[Z2_SLOT_GREAT_FAIRY_SWORD],    Z2_ITEM_GREAT_FAIRY_SWORD },
    { &z2_file.items[Z2_SLOT_QUEST_3],              Z2_ITEM_LAND_DEED },

    { &z2_file.items[Z2_SLOT_BOTTLE_1],             Z2_ITEM_BOTTLE },
    { &z2_file.items[Z2_SLOT_BOTTLE_2],             Z2_ITEM_BOTTLE },
    { &z2_file.items[Z2_SLOT_BOTTLE_3],             Z2_ITEM_BOTTLE },
    { &z2_file.items[Z2_SLOT_BOTTLE_4],             Z2_ITEM_BOTTLE },
    { &z2_file.items[Z2_SLOT_BOTTLE_5],             Z2_ITEM_BOTTLE },
    { &z2_file.items[Z2_SLOT_BOTTLE_6],             Z2_ITEM_BOTTLE },
};

struct item_map_row mask_map_table[] = {
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
struct item_map_row item_map_table[] = {
    { "ocarina",            &z2_file.items[Z2_SLOT_OCARINA],            Z2_ITEM_OCARINA },
    { "bow",                &z2_file.items[Z2_SLOT_BOW],                Z2_ITEM_HEROS_BOW },
    { "fire arrow",         &z2_file.items[Z2_SLOT_FIRE_ARROW],         Z2_ITEM_FIRE_ARROW },
    { "ice arrow",          &z2_file.items[Z2_SLOT_ICE_ARROW],          Z2_ITEM_ICE_ARROW },
    { "light arrow",        &z2_file.items[Z2_SLOT_LIGHT_ARROW],        Z2_ITEM_LIGHT_ARROW },
    { "trade quest 1",      &z2_file.items[Z2_SLOT_QUEST_1],              Z2_ITEM_LAND_DEED },
    { "bomb",               &z2_file.items[Z2_SLOT_BOMB],               Z2_ITEM_BOMB },
    { "bombchu",            &z2_file.items[Z2_SLOT_BOMBCHU],            Z2_ITEM_BOMBCHU },
    { "stick",              &z2_file.items[Z2_SLOT_STICK],              Z2_ITEM_STICK },
    { "nut",                &z2_file.items[Z2_SLOT_NUT],                Z2_ITEM_NUT },
    { "magic bean",         &z2_file.items[Z2_SLOT_MAGIC_BEAN],         Z2_ITEM_MAGIC_BEAN },
    { "trade quest 2",      &z2_file.items[Z2_SLOT_QUEST_2],              Z2_ITEM_LAND_DEED },
    { "powder keg",         &z2_file.items[Z2_SLOT_POWDER_KEG],         Z2_ITEM_POWDER_KEG },
    { "pictograph box",     &z2_file.items[Z2_SLOT_PICTOGRAPH_BOX],     Z2_ITEM_PICTOGRAPH_BOX },
    { "lens of truth",      &z2_file.items[Z2_SLOT_LENS],               Z2_ITEM_LENS },
    { "hookshot",           &z2_file.items[Z2_SLOT_HOOKSHOT],           Z2_ITEM_HOOKSHOT },
    { "great fairy sword",  &z2_file.items[Z2_SLOT_GREAT_FAIRY_SWORD],  Z2_ITEM_GREAT_FAIRY_SWORD },
    { "trade quest 3",      &z2_file.items[Z2_SLOT_QUEST_3],              Z2_ITEM_LAND_DEED },
};

struct item_map_row mask_map_table[] = {
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