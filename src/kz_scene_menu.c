#include <stdlib.h>
#include "menu.h"
#include "kz.h"
#include "resource.h"

static int room_idx;

static int collision_gen(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(kz.collision_view_status == COL_VIEW_NONE){
            kz.collision_view_status = COL_VIEW_GENERATE;
        }else{
            kz.collision_view_status = COL_VIEW_DESTROY;
        }
    }
    return 1;
}

static int collision_view_switch(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        kz.collision_view_settings = kz.collision_view_settings & (1 << (int)data);
        return 1;
    }
    return 0;
}

static int unload_room(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_game.room_ctx.rooms[0].idx=-1;
        z2_game.room_ctx.rooms[0].file = NULL;
        z2_unloadroom(&z2_game,&z2_game.room_ctx);
        z2_game.room_ctx.rooms[0].idx=-1;
        z2_game.room_ctx.rooms[0].file = NULL;
        return 1;
    }
    return 0;
}

static int load_room(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_game.room_ctx.rooms[0].idx = room_idx;
        z2_game.room_ctx.rooms[0].file = NULL;
        z2_unloadroom(&z2_game,&z2_game.room_ctx);
        z2_game.room_ctx.rooms[0].idx=room_idx;
        z2_game.room_ctx.rooms[0].file = NULL;
        z2_loadroom(&z2_game,&z2_game.room_ctx,room_idx);
        return 1;
    }
    return 0;
}

static int room_dec(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        room_idx += z2_game.room_cnt - 1;
        room_idx %= z2_game.room_cnt;
        return 1;
    }
    return 0;
}

static int room_inc(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        room_idx++;
        room_idx %= z2_game.room_cnt;
        return 1;
    }
    return 0;
}

struct menu *create_scene_menu(){
    static struct menu scene;
    menu_init(&scene,0,0);
    menu_set_padding(&scene,0,2);
    scene.selected_item = menu_add_button(&scene,0,0,"return",menu_return,NULL);
    menu_add(&scene,0,1,"collision viewer");
    menu_add_checkbox(&scene,17,1,collision_gen,NULL,NULL);
    menu_add(&scene,2,2,"reduce");
    menu_add_checkbox(&scene,10,2,collision_view_switch,(void*)COL_VIEW_REDUX,NULL);
    menu_add(&scene,2,3,"opaque");
    menu_add_checkbox(&scene,10,3,collision_view_switch,(void*)COL_VIEW_OPAQUE,NULL);
    
    menu_add(&scene,0,4,"current room");
    static watch_t cur_room;
    cur_room.address = &z2_game.room_ctx.rooms[0].idx;
    cur_room.type = WATCH_TYPE_U8;
    cur_room.floating=0;
    menu_add_watch(&scene,13,4,&cur_room,1);
    menu_add(&scene,0,5,"num rooms");
    static watch_t num_rooms;
    num_rooms.address = &z2_game.room_cnt;
    num_rooms.type = WATCH_TYPE_U8;
    num_rooms.floating = 0;
    menu_add_watch(&scene,13,5,&num_rooms,1);

    menu_add_button(&scene,0,6,"unload room",unload_room,NULL);
    menu_add_button(&scene,0,7,"load room",load_room,NULL);
    static watch_t room_watch;
    room_watch.address = &room_idx;
    room_watch.type = WATCH_TYPE_S32;
    room_watch.floating=0;
    menu_add_button(&scene,10,7,"-",room_dec,NULL);
    menu_add_watch(&scene,11,7,&room_watch,1);
    menu_add_button(&scene,12,7,"+",room_inc,NULL);
    return &scene;
}