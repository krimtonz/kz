#include <stdlib.h>
#include "kz.h"
#include "kzresource.h"
#include "input.h"

static int room_idx;

static int collision_gen_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        if(kz.collision_view_status == COL_VIEW_INACTIVE){
            kz.collision_view_status = COL_VIEW_START;
        }else{
            kz.collision_view_status = COL_VIEW_BEGIN_STOP;
        }
    }else if (event == MENU_EVENT_UPDATE){
        menu_checkbox_set(handler->subscriber, kz.collision_view_status == COL_VIEW_ACTIVE);
    }
    return 1;
}

enum collison_switch_type {
    COL_SWITCH_REDUCE,
    COL_SWITCH_OPAQUE,
    COL_SWITCH_UPDATE,
    COL_SWITCH_WIREFRAME,
    COL_SWITCH_MAX
};

static int collision_switch_event(event_handler_t *handler, menu_event_t event, void **event_data) {
    if(event == MENU_EVENT_ACTIVATE) {
        int d = (int)handler->callback_data;
        switch(d) {
            case COL_SWITCH_REDUCE:
                settings->col_view_red = !settings->col_view_red;
                if(kz.collision_view_status == COL_VIEW_ACTIVE) {
                    kz.collision_view_status = COL_VIEW_BEGIN_RESTART;
                }
                break;
            case COL_SWITCH_OPAQUE:
                settings->col_view_opq = !settings->col_view_opq;
                break;
            case COL_SWITCH_UPDATE:
                settings->col_view_upd = !settings->col_view_upd;
                break;
            case COL_SWITCH_WIREFRAME:
                settings->col_view_line = !settings->col_view_line;
                break;

        }
    }else if(event == MENU_EVENT_UPDATE) {
        int d = (int)handler->callback_data;
        int set = 0;
        switch(d) {
            case COL_SWITCH_REDUCE:
                set = settings->col_view_red;
                break;
            case COL_SWITCH_OPAQUE:
                set = settings->col_view_opq;
                break;
            case COL_SWITCH_UPDATE:
                set = settings->col_view_upd;
                break;
            case COL_SWITCH_WIREFRAME:
                set = settings->col_view_line;
                break;
        }

        menu_checkbox_set(handler->subscriber, set);
    }
    return 1;
}

static int hitbox_gen_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        if(kz.hitbox_view_status == COL_VIEW_INACTIVE){
            kz.hitbox_view_status = COL_VIEW_START;
        }else{
            kz.hitbox_view_status = COL_VIEW_BEGIN_STOP;
        }
    }else if(event == MENU_EVENT_UPDATE){
        menu_checkbox_set(handler->subscriber,kz.hitbox_view_status == COL_VIEW_ACTIVE);
    }
    return 1;
}

static int hitbox_opaque_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        settings->hit_view_opq = !settings->hit_view_opq;
    }else if(event == MENU_EVENT_UPDATE){
        menu_checkbox_set(handler->subscriber,settings->hit_view_opq);
    }
    return 1;
}

static int unload_room_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    z2_game.room_ctx.rooms[0].idx = -1;
    z2_game.room_ctx.rooms[0].file = NULL;
    z2_UnloadRoom(&z2_game, &z2_game.room_ctx);
    z2_game.room_ctx.rooms[0].idx = -1;
    z2_game.room_ctx.rooms[0].file = NULL;
    return 1;
}

static int load_room_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(room_idx == z2_game.room_ctx.rooms[0].idx){
        return unload_room_onactivate(handler, event, event_data);
    }
    if(room_idx >= z2_game.room_ctx.transition_cnt){
        return 1;
    }
    z2_game.room_ctx.rooms[0].idx = room_idx;
    z2_game.room_ctx.rooms[0].file = NULL;
    z2_UnloadRoom(&z2_game, &z2_game.room_ctx);
    z2_game.room_ctx.rooms[0].idx = room_idx;
    z2_game.room_ctx.rooms[0].file = NULL;
    z2_LoadRoom(&z2_game, &z2_game.room_ctx, room_idx);
    return 1;
}

static int hide_actors_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    kz.hide_actors = !kz.hide_actors;
    return 1;
}

static int hide_room_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    kz.hide_room = !kz.hide_room;
    return 1;
}

static int free_cam_event(event_handler_t *handler, menu_event_t event, void **event_data) {
    if(event == MENU_EVENT_ACTIVATE) {
        kz.free_cam_active = !kz.free_cam_active;
        if(kz.free_cam_active) {
            input_mask_set(BUTTON_C_BUTTONS, 0xFF, 0xFF);
        } else {
            input_mask_clear(BUTTON_C_BUTTONS, 0xFF, 0xFF);
        }
    } else if(event == MENU_EVENT_UPDATE) {
        menu_checkbox_set(handler->subscriber, kz.free_cam_active);
    }
    return 1;
}

static int lock_cam_event(event_handler_t *handler, menu_event_t event, void **event_data) {
    if(event == MENU_EVENT_ACTIVATE) {
        kz.free_cam_locked = !kz.free_cam_locked;
        if(!kz.free_cam_locked && kz.free_cam_active) {
            input_mask_set(BUTTON_C_BUTTONS | BUTTON_DPAD, 0xFF, 0xFF);
        } else {
            input_mask_clear(BUTTON_C_BUTTONS | BUTTON_DPAD, 0xFF, 0xFF);
        }
    } else if(event == MENU_EVENT_UPDATE) {
        menu_checkbox_set(handler->subscriber, kz.free_cam_locked);
    }
    return 1;
}

static int set_cam_pos_event(event_handler_t *handler, menu_event_t event, void **event_data) {
    sph_coord_t sph;
    z2_xyzf_t at_dir;

    sph.pitch = 0.0f;
    sph.yaw = 0.0f;

    geo_to_vec3f(&sph, &at_dir);

    kz.kz_at = z2_link.common.pos_2;
    kz.cam_sph = sph;

    vec3f_add(&kz.kz_at, &at_dir, &kz.kz_eye);
    return 1;
}

#if 0
static int cam_draw_event(event_handler_t *handler, menu_event_t event, void **event_data) {
    if(event == MENU_EVENT_ACTIVATE) {
        kz.draw_camera = !kz.draw_camera;
    } else if(event == MENU_EVENT_UPDATE) {
        menu_checkbox_set(handler->subscriber, kz.draw_camera);
    }
    return 1;
}
#endif

menu_t *create_scene_menu(void){
    static menu_t scene;
    static menu_t collision;
    static menu_t camera;

    menu_init(&scene, 0, 0);
    menu_padding_set(&scene, 0, 2);
    scene.selected_item = menu_button_add(&scene, 0, 0, "return", menu_return, NULL);

    menu_submenu_add(&scene, 0, 1, "collision", &collision);

    menu_item_t *item;
    // Generate collision menu
    {
        menu_init(&collision, 0, 0);
        menu_padding_set(&collision, 0, 2);
        collision.selected_item = menu_button_add(&collision, 0, 0, "return", menu_return, NULL);
        // Collision Viewer
        {
            item = menu_checkbox_add(&collision, 0, 1);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, collision_gen_event, NULL);
            menu_label_add(&collision, 2, 1, "enable collision viewer");

            item = menu_checkbox_add(&collision, 2, 2);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, collision_switch_event, (void*)COL_SWITCH_REDUCE);
            menu_label_add(&collision, 4, 2, "reduce");

            item = menu_checkbox_add(&collision, 2, 3);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, collision_switch_event, (void*)COL_SWITCH_OPAQUE);
            menu_label_add(&collision, 4, 3, "opaque");

            item = menu_checkbox_add(&collision, 2, 4);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, collision_switch_event, (void*)COL_SWITCH_WIREFRAME);
            menu_label_add(&collision, 4, 4, "wireframe");

            item = menu_checkbox_add(&collision, 2, 5);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, collision_switch_event, (void*)COL_SWITCH_UPDATE);
            menu_label_add(&collision, 4, 5, "update on scene change");
        }

        item = menu_checkbox_add(&collision, 0, 6);
        menu_item_register_event(item, MENU_EVENT_ACTIVATE, hide_room_onactivate, NULL);
        menu_label_add(&collision, 2, 6, "hide room");

        // hitbox viewer
        {
            item = menu_checkbox_add(&collision, 0, 7);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, hitbox_gen_event, NULL);
            menu_label_add(&collision, 2, 7, "enable hitbox viewer");
            item = menu_checkbox_add(&collision, 2, 8);
            menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, hitbox_opaque_event, NULL);
            menu_label_add(&collision, 4, 8, "opaque");
        }
        item = menu_checkbox_add(&collision, 0, 9);
        menu_item_register_event(item, MENU_EVENT_ACTIVATE, hide_actors_onactivate, NULL);
        menu_label_add(&collision, 2, 9, "hide actors");
    }

    menu_submenu_add(&scene, 0, 2, "camera", &camera);
    // Generate Camera Menu
    {
        menu_init(&camera, 0, 0);
        menu_padding_set(&camera, 0, 2);
        camera.selected_item = menu_button_add(&camera, 0, 0, "return", menu_return, NULL);
        item = menu_checkbox_add(&camera, 0, 1);
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, free_cam_event, NULL);
        menu_label_add(&camera, 2, 1, "enable free cam");
        item = menu_checkbox_add(&camera, 0, 2);
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, lock_cam_event, NULL);
        menu_label_add(&camera, 2, 2, "lock free cam");
        menu_button_add(&camera, 0, 3, "set cam pos to link pos", set_cam_pos_event, NULL);
        /*
        item = menu_checkbox_add(&camera, 0, 3);
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, cam_draw_event, NULL);
        menu_label_add(&camera, 2, 3, "draw camera");
        */
    }

    item = menu_label_add(&scene, 0, 3, "current room");
    item->color = COLOR_FADED;
    static watch_t cur_room;
    cur_room.address = &z2_game.room_ctx.rooms[0].idx;
    cur_room.type = WATCH_TYPE_U8;
    cur_room.floating = 0;
    item = menu_watch_add(&scene, 13, 3, &cur_room, 1);
    item->color = COLOR_FADED;
    item = menu_label_add(&scene, 0, 4, "num rooms");
    item->color = COLOR_FADED;
    static watch_t num_rooms;
    num_rooms.address = &z2_game.room_ctx.transition_cnt;
    num_rooms.type = WATCH_TYPE_U8;
    num_rooms.floating = 0;
    item = menu_watch_add(&scene, 13, 4, &num_rooms, 1);
    item->color = COLOR_FADED;

    menu_button_add(&scene, 0, 5, "unload room", unload_room_onactivate, NULL);
    menu_button_add(&scene, 0, 6, "load room", load_room_onactivate, NULL);
    item = menu_number_input_add(&scene, 13, 6, 10, 2);
    menu_item_register_event(item, MENU_EVENT_NUMBER, menu_number_word_event, &room_idx);

    return &scene;
}
