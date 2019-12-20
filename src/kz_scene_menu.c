#include <stdlib.h>
#include "menu.h"
#include "kz.h"
#include "resource.h"

static int room_idx;

static int collision_gen_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(kz.collision_view_status == COL_VIEW_NONE){
            kz.collision_view_status = COL_VIEW_GENERATE;
        }else{
            kz.collision_view_status = COL_VIEW_KILL;
        }
    }else if (callback == MENU_CALLBACK_UPDATE){
        menu_checkbox_set(item,kz.collision_view_status == COL_VIEW_SHOW);
        return 1;
    }
    return 1;
}

static int collision_switch_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        int d = (int)data;
        if(d == 0){
            settings->col_view_red = !settings->col_view_red;
            if(kz.collision_view_status == COL_VIEW_SHOW){
                kz.collision_view_status = COL_VIEW_REGENERATE;
            }
        }else if(d == 1){
            settings->col_view_opq = !settings->col_view_opq;
        }else{
            settings->col_view_upd = !settings->col_view_upd;
        }
        return 1;
    }else if(callback == MENU_CALLBACK_UPDATE){
        int d = (int)data;
        _Bool set = 0;
        if(d == 0){
            set = settings->col_view_red;
        }else if(d == 1){
            set = settings->col_view_opq;
        }else{
            set = settings->col_view_upd;
        }
        menu_checkbox_set(item,set);
    }
    return 0;
}

static int hitbox_gen_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(kz.hitbox_view_status == COL_VIEW_NONE){
            kz.hitbox_view_status = COL_VIEW_GENERATE;
        }else{
            kz.hitbox_view_status = COL_VIEW_KILL;
        }
        return 1;
    }else if(callback == MENU_CALLBACK_UPDATE){
        menu_checkbox_set(item,kz.hitbox_view_status == COL_VIEW_SHOW);
        return 1;
    }
    return 0;
}

static int hitbox_opaque_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        settings->hit_view_opq = !settings->hit_view_opq;
        return 1;
    }else if(callback == MENU_CALLBACK_UPDATE){
        menu_checkbox_set(item,settings->hit_view_opq);
    }
    return 0;
}

static int unload_room(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_game.room_ctx.rooms[0].idx=-1;
        z2_game.room_ctx.rooms[0].file = NULL;
        z2_UnloadRoom(&z2_game,&z2_game.room_ctx);
        z2_game.room_ctx.rooms[0].idx=-1;
        z2_game.room_ctx.rooms[0].file = NULL;
        return 1;
    }
    return 0;
}

static int load_room(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(room_idx == z2_game.room_ctx.rooms[0].idx) return unload_room(item,callback,data);
        if(room_idx >= z2_game.room_ctx.transition_cnt) return 1;
        z2_game.room_ctx.rooms[0].idx = room_idx;
        z2_game.room_ctx.rooms[0].file = NULL;
        z2_UnloadRoom(&z2_game,&z2_game.room_ctx);
        z2_game.room_ctx.rooms[0].idx=room_idx;
        z2_game.room_ctx.rooms[0].file = NULL;
        z2_LoadRoom(&z2_game,&z2_game.room_ctx,room_idx);
        return 1;
    }
    return 0;
}

static int hide_actors_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        kz.hide_actors = !kz.hide_actors;
        return 1;
    }
    return 0;
}

static int hide_room_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        kz.hide_room = !kz.hide_room;
        return 1;
    }
    return 0;
}

struct menu *create_scene_menu(){
    static struct menu scene;
    static struct menu collision;
    menu_init(&scene);
    menu_set_padding(&scene,0,2);
    scene.selected_item = menu_add_button(&scene,0,0,"return",menu_return,NULL);
    
    menu_add_submenu(&scene,0,1,&collision,"collision");

    // Generate collision menu
    {
        menu_init(&collision);
        menu_set_padding(&collision,0,2);
        collision.selected_item = menu_add_button(&collision,0,0,"return",menu_return,NULL);
        // Collision Viewer
        {
            menu_add_checkbox(&collision,0,1,collision_gen_callback,NULL,NULL);
            menu_add(&collision,2,1,"enable collision viewer");
            menu_add_checkbox(&collision,2,2,collision_switch_callback,(void*)0,NULL);
            menu_add(&collision,4,2,"reduce");
            menu_add_checkbox(&collision,2,3,collision_switch_callback,(void*)1,NULL);
            menu_add(&collision,4,3,"opaque");
            menu_add_checkbox(&collision,2,4,collision_switch_callback,(void*)2,NULL);
            menu_add(&collision,4,4,"update on scene change");
        }
        menu_add_checkbox(&collision,0,5,hide_room_callback,NULL,NULL);
        menu_add(&collision,2,5,"hide room");

        // hitbox viewer
        {
            menu_add_checkbox(&collision,0,6,hitbox_gen_callback,NULL,NULL);
            menu_add(&collision,2,6,"enable hitbox viewer");
            menu_add_checkbox(&collision,2,7,hitbox_opaque_callback,NULL,NULL);
            menu_add(&collision,4,7,"opaque");
        }
        menu_add_checkbox(&collision,0,8,hide_actors_callback,NULL,NULL);
        menu_add(&collision,2,8,"hide actors");
    }

    menu_add(&scene,0,2,"current room");
    static watch_t cur_room;
    cur_room.address = &z2_game.room_ctx.rooms[0].idx;
    cur_room.type = WATCH_TYPE_U8;
    cur_room.floating=0;
    menu_add_watch(&scene,13,2,&cur_room,1);
    menu_add(&scene,0,3,"num rooms");
    static watch_t num_rooms;
    num_rooms.address = &z2_game.room_ctx.transition_cnt;
    num_rooms.type = WATCH_TYPE_U8;
    num_rooms.floating = 0;
    menu_add_watch(&scene,13,3,&num_rooms,1);

    menu_add_button(&scene,0,4,"unload room",unload_room,NULL);
    menu_add_button(&scene,0,5,"load room",load_room,NULL);
    menu_add_number_input(&scene,13,5,NULL,NULL,10,2,&room_idx,4);

    return &scene;
}