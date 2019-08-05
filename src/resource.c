#include "resource.h"

#define OWL_OFFSET 0x14668
#define OWL_FILE 10

#if Z2_VERSION==NZSE
    #define DUNGEON_FILE 20
#else
    #define DUNGEON_OFFSET 0x3600
    #define DUNGEON_FILE 9
#endif

gfx_texture *items_texture;
gfx_texture *bottles_texture;
gfx_texture *buttons_texture;
gfx_texture *masks_texture;
gfx_texture *trade_quest_texture;
gfx_texture *owl_icon_texture;
gfx_texture *remains_texture;
gfx_texture *note_texture;
gfx_texture *notebook_texture;
gfx_texture *dungeon_items_tex;

void init_textures(){
    masks_texture = gfx_load_icon_item_static(ICON_ITEM_STATIC,Z2_MASK_DEKU,Z2_MASK_GIANT,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
    items_texture = gfx_load_icon_item_static(ICON_ITEM_STATIC,Z2_ITEM_OCARINA,Z2_ITEM_GREAT_FAIRY_SWORD,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
    bottles_texture = gfx_load_icon_item_static(ICON_ITEM_STATIC,Z2_ITEM_BOTTLE,Z2_END_BOTTLES - 1,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,0);
    trade_quest_texture = gfx_load_icon_item_static(ICON_ITEM_STATIC,Z2_ITEM_MOONS_TEAR,Z2_END_TRADE-1,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,0);
    buttons_texture = gfx_load_game_texture(G_IM_FMT_IA,G_IM_SIZ_8b,32,32,1,5,1125,0xF60,0);
    remains_texture = gfx_load_icon_item_static(ICON_ITEM_STATIC,Z2_ITEM_ODOLWAS_REMAINS,Z2_ITEM_TWINMOLDS_REMAINS,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
    note_texture = gfx_load_icon_item_static(ICON_ITEM_STATIC,130,130,G_IM_FMT_IA,G_IM_SIZ_8b,16,24,0);
    notebook_texture = gfx_load_icon_item_static(ICON_ITEM_STATIC,Z2_ITEM_BOMBERS_NOTEBOOK,Z2_ITEM_BOMBERS_NOTEBOOK,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
    owl_icon_texture = gfx_load_game_texture(G_IM_FMT_RGBA, G_IM_SIZ_32b,24,12,1,1,OWL_FILE,OWL_OFFSET,1);
#if Z2_VERSION==NZSE
    dungeon_items_tex = gfx_load_icon_item_static(DUNGEON_FILE,6,8,G_IM_FMT_RGBA,G_IM_SIZ_32b,24,24,1);
#else
    dungeon_items_tex = gfx_load_game_texture(G_IM_FMT_RGBA, G_IM_SIZ_32b,24,24,1,3,DUNGEON_FILE,DUNGEON_OFFSET,1);
#endif
}