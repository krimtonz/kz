#include <grc.h>
#include <stdlib.h>
#include "resource.h"

#if Z2_VERSION!=NZSE
static z2_item_t texture_map[] = {
    Z2_ITEM_OCARINA,
    Z2_ITEM_HEROS_BOW,
    Z2_ITEM_FIRE_ARROW,
    Z2_ITEM_ICE_ARROW,
    Z2_ITEM_LIGHT_ARROW,
    Z2_ITEM_FAIRY_OCARINA,
    Z2_ITEM_BOMB,
    Z2_ITEM_BOMBCHU,
    Z2_ITEM_STICK,
    Z2_ITEM_NUT,
    Z2_ITEM_MAGIC_BEAN,
    Z2_ITEM_SLINGSHOT,
    Z2_ITEM_POWDER_KEG,
    Z2_ITEM_PICTOGRAPH_BOX,
    Z2_ITEM_LENS,
    Z2_ITEM_HOOKSHOT,
    Z2_ITEM_GREAT_FAIRY_SWORD,
    Z2_ITEM_OOT_HOOKSHOT,
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
    Z2_ITEM_BOTTLE,
    Z2_ITEM_BOTTLE,
    Z2_ITEM_SEAHORSE,
    Z2_ITEM_CHATEAU_ROMANI,
    Z2_ITEM_HYLIAN_LOACH,
    Z2_ITEM_BOTTLE,
    Z2_ITEM_BOTTLE,
    Z2_ITEM_BOTTLE,
    Z2_ITEM_BOTTLE,
    Z2_ITEM_BOTTLE,
    Z2_ITEM_BOTTLE,
    Z2_ITEM_BOTTLE,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_LAND_DEED,
    Z2_ITEM_SWAP_DEED,
    Z2_ITEM_MOUNTAIN_DEED,
    Z2_ITEM_OCEAN_DEED,
    Z2_ITEM_POACHERS_SAW,
    Z2_ITEM_BROKEN_GORON_SWORD,
    Z2_ITEM_PRESCRIPTION,
    Z2_ITEM_SPEEDFROG,
    Z2_ITEM_EYEDROPS,
    Z2_ITEM_ROOM_KEY,
    Z2_ITEM_MAMA_LETTER,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_KAFEI_LETTER,
    Z2_ITEM_PENDANT,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_MOONS_TEAR,
    Z2_ITEM_UNK_MAP,
    Z2_MASK_DEKU,
    Z2_MASK_GORON,
    Z2_MASK_ZORA,
    Z2_MASK_FIERCE_DEITY,
    Z2_MASK_MASK_OF_TRUTH,
    Z2_MASK_KAFEI,
    Z2_MASK_ALL_NIGHT,
    Z2_MASK_BUNNY_HOOD,
    Z2_MASK_KEATON,
    Z2_MASK_GARO,
    Z2_MASK_ROMANI,
    Z2_MASK_CIRCUS_LEADER,
    Z2_MASK_POSTMAN,
    Z2_MASK_COUPLE,
    Z2_MASK_GREAT_FAIRY,
    Z2_MASK_GIBDO,
    Z2_MASK_DON_GERO,
    Z2_MASK_KAMARO,
    Z2_MASK_CAPTAINSHAT,
    Z2_MASK_STONE,
    Z2_MASK_BREMEN,
    Z2_MASK_BLAST,
    Z2_MASK_SCENTS,
    Z2_MASK_GIANT,
    Z2_ITEM_BOW_FIRE_ARROW,
    Z2_ITEM_BOW_ICE_ARROW,
    Z2_ITEM_BOW_LIGHT_ARROW,
    Z2_ITEM_DRUM,
    Z2_ITEM_GUITAR,
    Z2_ITEM_PIPES,
    Z2_ITEM_KOKIRI_SWORD,
    Z2_ITEM_RAZOR_SWORD,
    Z2_ITEM_GILDED_SWORD,
    Z2_ITEM_DEITY_SWORD,
    Z2_ITEM_HERO_SHIELD,
    Z2_ITEM_MIRROR_SHIELD,
    Z2_ITEM_OOT_MIRROR,
    Z2_ITEM_QUIVER_30,
    Z2_ITEM_QUIVER_40,
    Z2_ITEM_QUIVER_50,
    Z2_ITEM_BOMB_BAG_20,
    Z2_ITEM_BOMB_BAG_30,
    Z2_ITEM_BOMB_BAG_40,
    Z2_ITEM_MAGIC,
    Z2_ITEM_ADULTS_WALLET,
    Z2_ITEM_GIANTS_WALLET,
    Z2_ITEM_FISHING_ROD,
    Z2_ITEM_ODOLWAS_REMAINS,
    Z2_ITEM_GOHTS_REMAINS,
    Z2_ITEM_GYORGS_REMAINS,
    Z2_ITEM_TWINMOLDS_REMAINS,
    Z2_ITEM_BOMBERS_NOTEBOOK,
};
#endif

static const char *resource_names[R_END] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "checkbox",
    "icons",
    "font",
    "buttons",
    "amounts",
    "flags",
    "arrows",
    "files",
    NULL,
};

static void *resource_load_kz_texture(enum resource resource){
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
        kztext->tile_size = (kztext->tile_width * kztext->tile_height * G_SIZ_BITS(kztext->img_size) + 63) / 64 * 8;
        return kztext;
    }
    return NULL;
}

static void *resource_load_items(enum resource resource){
    struct item_texture *textures = malloc(sizeof(*textures) * Z2_ITEM_END);
    memset(textures,0,sizeof(*textures) * Z2_ITEM_END);
#ifndef LITE
    for(int i = 0;i < Z2_ITEM_END;i++){
        textures[i].texture = gfx_load_item_texture(i);
    }
#endif
    return textures;
}

static void *resource_load_buttons(enum resource resource){
    texture_loader loader = {
        0xF60,          32,             32, BUTTONS_FILE,
        G_IM_FMT_IA,    G_IM_SIZ_8b,    0,  0,
        1,              5,              SOURCE_FILE,
    };
    return gfx_load(&loader);
}

static void *resource_load_note(enum resource resource){
    texture_loader loader = {
        0,              16,             24,         ICON_ITEM_STATIC,
        G_IM_FMT_IA,    G_IM_SIZ_8b,    NOTE_INDEX, NOTE_INDEX,
        0,              0,              SOURCE_ARCHIVE,
    };
    return gfx_load(&loader);
}

static void *resource_load_owl(enum resource resource){
    texture_loader loader = {
        OWL_OFFSET,     24,             12,         OWL_FILE,
        G_IM_FMT_RGBA,  G_IM_SIZ_32b,   0,          0,
        1,              1,              SOURCE_FILE,
    };
    return gfx_load(&loader);
}

static void *resource_load_dungeon_items(enum resource resource){
    texture_loader loader = {
#if Z2_VERSION==NZSE
        0,              24,             24,         DUNGEON_FILE,
        G_IM_FMT_RGBA,  G_IM_SIZ_32b,   0,          13,
        1,              0,              SOURCE_ARCHIVE,
#else
        DUNGEON_OFFSET, 24,             24,         DUNGEON_FILE,
        G_IM_FMT_RGBA,  G_IM_SIZ_32b,   0,          0,
        1,              13,             SOURCE_FILE,
#endif
    };
    return gfx_load(&loader);
}

static void *resource_load_rupee_texture(enum resource resource){
    texture_loader loader = {
        RUPEE_OFFSET,   16,             16,         RUPEE_FILE,
        G_IM_FMT_IA,    G_IM_SIZ_8b,    0,          0,
        0,              1,              SOURCE_FILE
    };
    return gfx_load(&loader);
}

static void *resource_table[R_END] = {NULL};
static void *(*resource_ctors[R_END])(enum resource) = {
    resource_load_items,
    resource_load_buttons,
    resource_load_owl,
    resource_load_note,
    resource_load_dungeon_items,
    resource_load_kz_texture,
    resource_load_kz_texture,
    resource_load_kz_texture,
    resource_load_kz_texture,
    resource_load_kz_texture,
    resource_load_kz_texture,
    resource_load_kz_texture,
    resource_load_kz_texture,
    resource_load_rupee_texture,
};

void *resource_get(enum resource resource){
    if(!resource_table[resource]){
        if(resource_ctors[resource]){
            resource_table[resource] = resource_ctors[resource](resource);
        }
    }
    return resource_table[resource];
}

gfx_texture *get_item_texture(uint8_t item_id, _Bool release){
#if Z2_VERSION!=NZSE 
    item_id = texture_map[item_id];
#endif
    struct item_texture *item = (struct item_texture*)resource_get(R_Z2_ITEMS) + item_id;
#ifdef LITE
    item->last_access_counter = 0;
    item->release = release;
#endif
    if(item->texture) return item->texture;
    item->texture = gfx_load_item_texture(item_id);
    return item->texture;
}

#include <grc.c>