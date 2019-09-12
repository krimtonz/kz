#include <grc.h>
#include <stdlib.h>
#include "resource.h"

#define OWL_OFFSET 0x14668
#define OWL_FILE 10

#if Z2_VERSION==NZSE
    #define DUNGEON_FILE 20
    #define BUTTONS_FILE 1126
    #define NOTE_INDEX 98
#else
    #define DUNGEON_OFFSET 0x0
    #define DUNGEON_FILE 9
    #define BUTTONS_FILE 1125
    #define NOTE_INDEX 130
#endif

struct tilebg_info *checkbox_bg;

static const char *resource_names[R_END] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "checkbox",
    "icons"
};

static gfx_texture *resource_load_kz_texture(enum resource resource){
    if(resource_names[resource]){
        gfx_texture *kztext = malloc(sizeof(*kztext));
        if(!kztext) return kztext;
        memset(kztext,0,sizeof(*kztext));
        void *grctexture;
        if(!grc_resource_get(resource_names[resource],&grctexture,NULL)){
            return kztext;
        }
        struct grc_texture *gtext = grctexture;
        kztext->data = gtext->texture_data;
        kztext->img_fmt = gtext->im_fmt;
        kztext->img_size = gtext->im_siz;
        kztext->tile_width = gtext->tile_width;
        kztext->tile_height = gtext->tile_height;
        kztext->x_tiles = gtext->tiles_x;
        kztext->y_tiles = gtext->tiles_y;
        kztext->tile_size = ((kztext->tile_width * kztext->tile_height * G_SIZ_BITS(kztext->img_size) + 7) / 8 + 63) / 64 * 64;
        return kztext;
    }
    return NULL;
}

gfx_texture *resource_load_items(enum resource resource){
    texture_loader loader = {
        32, 32, ICON_ITEM_STATIC, G_IM_FMT_RGBA, G_IM_SIZ_32b,
        Z2_ITEM_OCARINA, Z2_ITEM_BOMBERS_NOTEBOOK, 1, SOURCE_ARCHIVE,
        0, 0
    };
    return gfx_load(&loader);
}

static gfx_texture *resource_load_buttons(enum resource resource){
    texture_loader loader = {
        32, 32, BUTTONS_FILE, G_IM_FMT_IA, G_IM_SIZ_8b,
        0, 0, 1, SOURCE_FILE,
        0xF60, 5
    };
    return gfx_load(&loader);
}

static gfx_texture *resource_load_note(enum resource resource){
    texture_loader loader = {
        16, 24, ICON_ITEM_STATIC, G_IM_FMT_IA, G_IM_SIZ_8b,
        NOTE_INDEX, NOTE_INDEX, 0, SOURCE_ARCHIVE,
        0, 0
    };
    return gfx_load(&loader);
}

static gfx_texture *resource_load_owl(enum resource resource){
    texture_loader loader = {
        24, 12, OWL_FILE, G_IM_FMT_RGBA, G_IM_SIZ_32b,
        0, 0, 1, SOURCE_FILE,
        OWL_OFFSET, 1
    };
    return gfx_load(&loader);
}

static gfx_texture *resource_load_dungeon_items(enum resource resource){
    texture_loader loader = {
        24, 24, DUNGEON_FILE, G_IM_FMT_RGBA, G_IM_SIZ_32b,
#if Z2_VERSION==NZSE
        0, 13, 1, SOURCE_ARCHIVE,
        0, 0
#else
        0, 0, 1, SOURCE_FILE,
        DUNGEON_OFFSET, 13
#endif
    };
    return gfx_load(&loader);
}

static gfx_texture *resource_table[R_END] = {NULL};
static gfx_texture *(*resource_ctors[R_END])(enum resource) = {
    resource_load_items,
    resource_load_buttons,
    resource_load_owl,
    resource_load_note,
    resource_load_dungeon_items,
    resource_load_kz_texture,
    resource_load_kz_texture
};

gfx_texture *resource_get(enum resource resource){
    if(!resource_table[resource]){
        if(resource_ctors[resource]){
            resource_table[resource] = resource_ctors[resource](resource);
        }
    }
    return resource_table[resource];
}

void init_resources(){
    checkbox_bg = malloc(sizeof(*checkbox_bg));
    checkbox_bg->texture = resource_get(R_KZ_CHECKBOX);
    checkbox_bg->tile = 0;
    checkbox_bg->off_color.color = 0xFFFFFFFF;
    checkbox_bg->on_color.color=0xFFFFFFFF;
}
#include <grc.c>