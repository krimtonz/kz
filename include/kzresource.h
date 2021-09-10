/*
* resource.h
*
* definitions for loading graphic resources
*/

#ifndef _KZRESOURCE_H
#define _KZRESOURCE_H
#include "z2.h"
#include "menu.h"

#define     OWL_OFFSET      0x14668
#define     RUPEE_OFFSET    0xE50

#if Z2_VERSION == NZSE
    #define NOTE_INDEX      98
#else
    #define DUNGEON_OFFSET  0
    #define NOTE_INDEX      130
#endif

typedef void   *(*resource_ctor)    (int resource_id);

enum resource {
    R_Z2_CHECKBOX,
    R_Z2_ICON,
    R_Z2_ITEMS,
    R_Z2_BUTTONS,
    R_Z2_OWL,
    R_Z2_NOTE,
    R_Z2_DUNGEON,
    R_KZ_FONT,
    R_KZ_BUTTONS,
    R_KZ_AMOUNTS,
    R_KZ_FLAGS,
    R_KZ_ARROWS,
    R_KZ_FILES,
    R_Z2_RUPEE,
    R_KZ_KZFLEX,
    R_KZ_SD,
    R_KZ_END
};

struct item_texture {
    gfx_texture *texture;
#ifdef LITE
    int last_access_counter;
    _Bool release;
#endif
};

gfx_texture    *get_item_texture    (uint8_t item_id, _Bool release);
void            kz_resource_init    (void);
void           *resource_get        (int resource);

extern menu_sprite_t           *scroll_up_sprite;
extern menu_sprite_t           *scroll_down_sprite;
extern menu_sprite_t           *pexport_sprite;
extern menu_sprite_t           *pimport_sprite;

#endif
