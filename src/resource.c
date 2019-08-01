#include "resource.h"

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
    masks_texture = gfx_load_icon_item_static(19,Z2_MASK_DEKU,Z2_MASK_GIANT,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
    items_texture = gfx_load_icon_item_static(19,Z2_ITEM_OCARINA,Z2_ITEM_GREAT_FAIRY_SWORD,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
    bottles_texture = gfx_load_icon_item_static(19,Z2_ITEM_BOTTLE,Z2_ITEM_BOTTLE2,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,0);
    trade_quest_texture = gfx_load_icon_item_static(19,Z2_ITEM_MOONS_TEAR,Z2_ITEM_PENDANT,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,0);
    buttons_texture = gfx_load_game_texture(G_IM_FMT_IA,G_IM_SIZ_8b,32,32,1,5,1126,0xF60,0);
    remains_texture = gfx_load_icon_item_static(19,Z2_ITEM_ODOLWAS_REMAINS,Z2_ITEM_TWINMOLDS_REMAINS,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
    note_texture = gfx_load_icon_item_static(19,98,98,G_IM_FMT_IA,G_IM_SIZ_8b,16,24,0);
    notebook_texture = gfx_load_icon_item_static(19,Z2_ITEM_BOMBERS_NOTEBOOK,Z2_ITEM_BOMBERS_NOTEBOOK,G_IM_FMT_RGBA,G_IM_SIZ_32b,32,32,1);
    owl_icon_texture = gfx_load_game_texture(G_IM_FMT_RGBA, G_IM_SIZ_32b,24,12,1,1,10,0x14668,1);
    dungeon_items_tex = gfx_load_icon_item_static(20,6,8,G_IM_FMT_RGBA,G_IM_SIZ_32b,24,24,1);
}