#include <stdlib.h>
#include "menu.h"
#include "kz.h"

static int room_idx;

static int collison_show(struct menu_item *item, void *data){
    kz.collision_view_status = COL_VIEW_SHOW;
    return 1;
}

static int collison_hide(struct menu_item *item, void *data){
    kz.collision_view_status = COL_VIEW_NONE;
    return 1;
}

static int collison_gen(struct menu_item *item, void *data){
    kz.collision_view_status = COL_VIEW_GENERATE;
    return 1;
}

static int collision_reduced(struct menu_item *item, void *data){
    kz.collision_view_settings ^= COL_VIEW_REDUX;
    return 1;
}

static int collision_opaque(struct menu_item *item, void *data){
    kz.collision_view_settings ^= COL_VIEW_OPAQUE;
    return 1;
}

static int unload_room(struct menu_item *item, void *data){
    z2_game.room_ctx.rooms[0].idx=-1;
    z2_game.room_ctx.rooms[0].file = NULL;
    z2_unloadroom(&z2_game,&z2_game.room_ctx);
    z2_game.room_ctx.rooms[0].idx=-1;
    z2_game.room_ctx.rooms[0].file = NULL;
    return 1;
}

static int load_room(struct menu_item *item, void *data){
    z2_game.room_ctx.rooms[0].idx = room_idx;
    z2_game.room_ctx.rooms[0].file = NULL;
    z2_unloadroom(&z2_game,&z2_game.room_ctx);
    z2_game.room_ctx.rooms[0].idx=room_idx;
    z2_game.room_ctx.rooms[0].file = NULL;
    z2_loadroom(&z2_game,&z2_game.room_ctx,room_idx);
    return 1;
}

static int room_dec(struct menu_item *item, void *data){
    room_idx += z2_game.room_cnt - 1;
    room_idx %= z2_game.room_cnt;
    return 1;
}

static int room_inc(struct menu_item *item, void *data){
    room_idx++;
    room_idx %= z2_game.room_cnt;
    return 1;
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
    menu_add_button(&scene,0,8,"load room",load_room,NULL);
    menu_add_button(&scene,10,8,"-",room_dec,NULL);
    static watch_t room_watch;
    room_watch.address = &room_idx;
    room_watch.type = WATCH_TYPE_S32;
    room_watch.floating=0;
    menu_add_watch(&scene,11,8,&room_watch,1);
    menu_add_button(&scene,12,8,"+",room_inc,NULL);
    return &scene;
}