#include <grc.h>
#include <stdlib.h>
#include <startup.h>
#include "menu.h"
#include "kzresource.h"
#include "gfx.h"
#include "z2.h"
#include "hb.h"

static struct list  resource_list;
static int          next_resource_id = 0;

static resource_ent_t *resource_find(int resource_id){
    for(resource_ent_t *ent = resource_list.first;ent;ent = list_next(ent)){
        if(ent->resource_id == resource_id){
            return ent;
        }
    }
    return NULL;
}

void *grc_ctor(int resource_id){
    resource_ent_t *ent = resource_find(resource_id);
    gfx_texture *kztext = malloc(sizeof(*kztext));
    if(!kztext){
        return kztext;
    }
    memset(kztext, 0, sizeof(*kztext));
    void *grctexture;
    if(!grc_resource_get(ent->resource_name, &grctexture, NULL)){
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

void *resource_get(int resource_id){
    resource_ent_t *ent = resource_find(resource_id);
    if(ent){
        if(!ent->data){
            ent->data = ent->ctor(resource_id);
        }
        return ent->data;
    }
    return NULL;
}

void resource_free(int resource_id){
    resource_ent_t *ent = resource_find(resource_id);
    if(ent){
        if(ent->dtor){
            ent->dtor(resource_id);
        }
        list_erase(&resource_list, ent);
    }
}

resource_ent_t *resource_create(const char *name, resource_ctor ctor, resource_dtor dtor){
    resource_ent_t *new_ent = list_push_back(&resource_list, NULL);
    if(new_ent){
        new_ent->resource_name = name;
        new_ent->resource_id = next_resource_id++;
        new_ent->data = NULL;
        new_ent->ctor = ctor;
        new_ent->dtor = dtor;
    }
    return new_ent;
}

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

menu_sprite_t   *scroll_up_sprite = NULL;
menu_sprite_t   *scroll_down_sprite = NULL;

int resource_handles[R_KZ_END] = { 0 };

static const char *resource_names[R_KZ_END] = {
    "checkbox",
    "icon",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "font",
    "buttons",
    "amounts",
    "flags",
    "arrows",
    "files",
    NULL,
};

static void *resource_load_items(int resource_id){
    struct item_texture *textures = malloc(sizeof(*textures) * Z2_ITEM_END);
    memset(textures, 0, sizeof(*textures) * Z2_ITEM_END);
#ifndef LITE
    for(int i = 0;i < Z2_ITEM_END;i++){
        textures[i].texture = gfx_load_item_texture(i);
    }
#endif
    return textures;
}

static void *resource_buttons_ctor(int resource_id){
    texture_loader loader = {
        0xF60,          32,             32, z2_parameter_static,
        G_IM_FMT_IA,    G_IM_SIZ_8b,    0,  0,
        1,              5,              SOURCE_FILE,
    };
    return gfx_load(&loader);
}

static void *resource_note_ctor(int resource_id){
    texture_loader loader = {
        0,              16,             24,         z2_item_icon_archive,
        G_IM_FMT_IA,    G_IM_SIZ_8b,    NOTE_INDEX, NOTE_INDEX,
        0,              0,              SOURCE_ARCHIVE,
    };
    return gfx_load(&loader);
}

static void *resource_owl_ctor(int resource_id){
    texture_loader loader = {
        OWL_OFFSET,     24,             12,         z2_icon_item_field_static,
        G_IM_FMT_RGBA,  G_IM_SIZ_32b,   0,          0,
        1,              1,              SOURCE_FILE,
    };
    return gfx_load(&loader);
}

static void *resource_dungeon_items_ctor(int resource_id){
    texture_loader loader = {
#if Z2_VERSION==NZSE
        0,              24,             24,         z2_icon_item_24_static,
        G_IM_FMT_RGBA,  G_IM_SIZ_32b,   0,          13,
        1,              0,              SOURCE_ARCHIVE,
#else
        0,              24,             24,         z2_icon_item_24_static,
        G_IM_FMT_RGBA,  G_IM_SIZ_32b,   0,          0,
        1,              13,             SOURCE_FILE,
#endif
    };
    return gfx_load(&loader);
}

static void *resource_rupee_ctor(int resource_id){
    texture_loader loader = {
        RUPEE_OFFSET,   16,             16,         z2_parameter_static,
        G_IM_FMT_IA,    G_IM_SIZ_8b,    0,          0,
        0,              1,              SOURCE_FILE
    };
    return gfx_load(&loader);
}

static resource_ctor resource_ctors[R_KZ_END] = {
    grc_ctor,
    grc_ctor,
    resource_load_items,
    resource_buttons_ctor,
    resource_owl_ctor,
    resource_note_ctor,
    resource_dungeon_items_ctor,
    grc_ctor,
    grc_ctor,
    grc_ctor,
    grc_ctor,
    grc_ctor,
    grc_ctor,
    resource_rupee_ctor,
};

gfx_texture *get_item_texture(uint8_t item_id, _Bool release){
#if Z2_VERSION!=NZSE
    item_id = texture_map[item_id];
#endif
    struct item_texture *item = (struct item_texture*)resource_get(resource_handles[R_Z2_ITEMS]) + item_id;
#ifdef LITE
    item->last_access_counter = 0;
    item->release = release;
#endif
    if(item->texture){
        return item->texture;
    }
    item->texture = gfx_load_item_texture(item_id);
    return item->texture;
}

void kz_resource_init(){
    list_init(&resource_list, sizeof(resource_ent_t));
    for(int i = 0; i < R_KZ_END; i++) {
        resource_ent_t *res = resource_create(resource_names[i], resource_ctors[i], NULL);
        if(res){
            resource_handles[i] = res->resource_id;
        }
    }

    static menu_sprite_t up_sprite = {
        NULL,   0,      0,      DEFAULT_COLOR,  DEFAULT_COLOR,
        8,      8,      NULL,   DEFAULT_COLOR,   0,
        0,      NULL,
    };

    static menu_sprite_t down_sprite = {
        NULL,   1,      0,      DEFAULT_COLOR,  DEFAULT_COLOR,
        8,      8,      NULL,   DEFAULT_COLOR,   0,
        0,      NULL,
    };

    up_sprite.texture = resource_get(resource_handles[R_KZ_ARROWS]);
    down_sprite.texture = resource_get(resource_handles[R_KZ_ARROWS]);

    scroll_up_sprite = &up_sprite;
    scroll_down_sprite = &down_sprite;
}