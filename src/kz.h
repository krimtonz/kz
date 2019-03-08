#ifndef _KZ_H
#define _KZ_H

#include <stdint.h>
#include <vector/vector.h>
#include "z2.h"
#include "menu.h"

typedef struct  {
    _Bool ready;    
    struct vector watches;
    size_t watchescnt;
    union{
        struct{
            uint16_t cheat_infinite_arrows      : 1;
            uint16_t cheat_infinite_magic       : 1;
            uint16_t cheat_infinite_health      : 1;
            uint16_t cheat_infinite_bombchu     : 1;
            uint16_t cheat_infinite_bombs       : 1;
            uint16_t cheat_infinite_powder_keg  : 1;
            uint16_t cheat_blast_mask           : 1;
            uint16_t cheat_isg                  : 1;
        };
        uint16_t cheats;
    };
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
} kz_ctxt_t;

extern kz_ctxt_t kz;

#endif