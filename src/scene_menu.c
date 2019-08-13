#include <stdlib.h>
#include "menu.h"
#include "kz.h"

static void collison_show(struct menu_item *item){
    kz.collision_view_status = COL_VIEW_SHOW;
}

static void collison_hide(struct menu_item *item){
    kz.collision_view_status = COL_VIEW_NONE;
}

static void collison_gen(struct menu_item *item){
    kz.collision_view_status = COL_VIEW_GENERATE;
}

static void collision_reduced(struct menu_item *item){
    kz.collision_view_settings ^= COL_VIEW_REDUX;
}

static void collision_opaque(struct menu_item *item){
    kz.collision_view_settings ^= COL_VIEW_OPAQUE;
}

static void unload_room(struct menu_item *item){
    z2_game.room_ctx.rooms[0].idx=-1;
    z2_game.room_ctx.rooms[0].file = NULL;
    z2_unloadroom(&z2_game,&z2_game.room_ctx);
    z2_game.room_ctx.rooms[0].idx=-1;
    z2_game.room_ctx.rooms[0].file = NULL;
}

struct menu *create_scene_menu(){
    static struct menu scene;
    menu_init(&scene,0,0);
    scene.selected_item = menu_add_button(&scene,0,0,"return",menu_return,NULL);
    menu_add(&scene,0,1,"collision viewer");
    menu_add_button(&scene,0,2,"show", collison_show, NULL);
    menu_add_button(&scene,0,3,"hide", collison_hide, NULL);
    menu_add_button(&scene,0,4,"gen", collison_gen, NULL);
    menu_add_button(&scene,0,5,"reduce", collision_reduced, NULL);
    menu_add_button(&scene,0,6,"opaque",collision_opaque, NULL);
    menu_add_button(&scene,0,7,"unload room",unload_room,NULL);
    return &scene;
}