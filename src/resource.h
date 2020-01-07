/*
* resource.h
*
* definitions for loading graphic resources
*/

#ifndef _RESOURCE_H
#define _RESOURCE_H
#include "gfx.h"
#include "menu.h"

#define     OWL_OFFSET      0x14668
#define     RUPEE_OFFSET    0xE50

#if Z2_VERSION==NZSE
    #define NOTE_INDEX      98
#else
    #define DUNGEON_OFFSET  0
    #define NOTE_INDEX      130
#endif

enum resource{
    R_Z2_ITEMS,
    R_Z2_BUTTONS,
    R_Z2_OWL,
    R_Z2_NOTE,
    R_Z2_DUNGEON,
    R_KZ_CHECKBOX,
    R_KZ_ICON,
    R_KZ_FONT,
    R_KZ_BUTTONS,
    R_KZ_AMOUNTS,
    R_KZ_FLAGS,
    R_KZ_ARROWS,
    R_KZ_FILES,
    R_Z2_RUPEE,
    R_END
};

struct item_texture{
    gfx_texture *texture;
#ifdef LITE
    int last_access_counter;
    _Bool release;
#endif
};

void           *resource_get(enum resource resource);
gfx_texture    *get_item_texture(uint8_t item_id, _Bool release);

#endif