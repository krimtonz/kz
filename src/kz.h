#ifndef _KZ_H
#define _KZ_H

#include <stdint.h>
#include <vector/vector.h>
#include "z2.h"
#include "menu.h"

enum cheats {
    CHEAT_STICKS,
    CHEAT_NUTS,
    CHEAT_ARROWS,
    CHEAT_BOMBS,
    CHEAT_BOMBCHUS,
    CHEAT_POWDER_KEG,
    CHEAT_HEALTH,
    CHEAT_MAGIC,
    CHEAT_RAZOR_SWORD,
    CHEAT_BLAST_MASK,
    CHEAT_RESTRICTION,
    CHEAT_ISG,
    CHEAT_RUPEES,
};

typedef struct  {
    _Bool ready;    
    struct vector watches;
    size_t watch_cnt;
    uint16_t cheats;
    union{
        struct{
            uint8_t col_enable : 1;
            uint8_t col_gen : 1 ;
            uint8_t col_opaque : 1;
            uint8_t col_redux : 1;
        };
        uint8_t collision_view_settings;
    };
    struct menu main_menu;
    _Bool menu_active;
} kz_ctxt_t;

extern kz_ctxt_t kz;

struct menu *create_warps_menu();
struct menu *create_inventory_menu();
struct menu *create_scene_menu();
struct menu *create_watches_menu();
struct menu *create_cheats_menu();

#endif