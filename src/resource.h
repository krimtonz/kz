#ifndef _RESOURCE_H
#define _RESOURCE_H
#include "gfx.h"
#include "menu.h"

enum resource{
    R_Z2_ITEMS,
    R_Z2_BUTTONS,
    R_Z2_OWL,
    R_Z2_NOTE,
    R_Z2_DUNGEON,
    R_KZ_CHECKBOX,
    R_KZ_ICON,
    R_END
};

extern struct tilebg_info *checkbox_bg;

void init_resources();
gfx_texture *resource_get(enum resource resource);
void resource_destroy(enum resource resource);
#endif