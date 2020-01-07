#include "kz.h"
#include "resource.h"
#include "input.h"
#include "zu.h"

static char *stored_song_text[] = {
    "none",                 "sonata of awakening",      "goron's lullaby",      "new wave bossa nova",
    "elegy of emptiness",   "oath to order",            "saria's song",         "song of time",
    "song of healing",      "epona's song",             "song of soaring",      "song of storms",
    "sun's song",           "inverted song of time",    "song of double time",  "goron's lullaby intro",
};

static uint16_t stored_song_values[] = {
    255,    0,  1,  2,
    3,      4,  5,  6,
    7,      8,  9,  10,
    11,     12, 13, 14,
};

static int debug_menu_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(zu_is_ingame() && z2_game.cutscene_state == 0){
        if(z2_game.pause_ctx.state == 0){
            z2_game.pause_ctx.state = 1;
                z2_pause_persp(&z2_game);
                z2_game.pause_ctx.unk_0x202 = (z2_game.pause_ctx.screen_idx * 2) + 1;
        }
        z2_game.pause_ctx.debug_menu = 1;
        reserve_buttons(BUTTON_D_DOWN | BUTTON_D_RIGHT | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_L);
        kz.debug_active = 1;
        kz.menu_active = 0;
        kz.pending_frames = -1;
    }else{
        kz_log("cannot debug menu here");
    }
    return 1;
}

static int time_of_day_event(event_handler_t *handler, menu_event_t event, void **event_data){
    uint16_t value = (uint16_t)((uint32_t)*event_data);
    if(event == MENU_EVENT_NUMBER){
        z2_file.time_of_day = value;
        for(z2_actor_t *actor = z2_game.actor_ctxt.actor_list[0].first;actor;actor = actor->next){
            if(actor->id == 0x15A){
                if(z2_file.daynight == 1 && (value >= 0xC000 || value < 0x4000)){
                    z2_file.daynight = 0;
                }else if(z2_file.daynight == 0 && (value <= 0xC000 || value >= 0x4000)){
                    z2_file.daynight = 1;
                }
                z2_timer_t *timer = (z2_timer_t*)actor;
                timer->daynight = z2_file.daynight;
                timer->timer_boundaries[0] = value;
                if(value < 0x4000){
                    timer->timer_boundaries[1] = 0x4000;
                }else{
                    timer->timer_boundaries[1] = 0xC000;
                }
                timer->timer_boundaries[2] = value;
                break;
            }
        }
    }else{
        menu_number_set(handler->subscriber, z2_file.time_of_day);
    }
    return 1;
}

static int intro_watched_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        z2_file.intro_flag = !z2_file.intro_flag;
    }else if(event == MENU_EVENT_UPDATE){
        menu_checkbox_set(handler->subscriber, z2_file.intro_flag);
    }
    return 1;
}

static int great_spin_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        z2_file.week_event_inf[0x17] ^= 0x2;
    }else if(event == MENU_EVENT_UPDATE){
        menu_checkbox_set(handler->subscriber, (z2_file.week_event_inf[0x17] & 0x2) == 0x2);
    }
    return 0;
}

static int have_tatl_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        z2_file.tatl_flag = !z2_file.tatl_flag;
    }else if(event == MENU_EVENT_UPDATE){
        menu_checkbox_set(handler->subscriber, z2_file.tatl_flag);
    }
    return 1;
}

static int stored_song_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_UPDATE){
        int idx = 0;
        for(int i = 0;i < sizeof(stored_song_values) / sizeof(*stored_song_values);i++){
            if(z2_stored_song == stored_song_values[i]){
                idx = i;
                break;
            }
        }
        menu_list_set(handler->subscriber, idx);
    }else if(event == MENU_EVENT_LIST){
        z2_stored_song = stored_song_values[(int)*event_data];
    }
    return 1;
}

menu_t *create_file_menu(void){
    static menu_t file;
    menu_init(&file, 0, 0);
    menu_padding_set(&file, 0, 2);
    file.selected_item = menu_button_add(&file, 0, 0, "return", menu_return, NULL);

    menu_item_t *item = menu_checkbox_add(&file, 0, 1);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, have_tatl_event, NULL);
    menu_label_add(&file, 2, 1, "have tatl");

    item = menu_checkbox_add(&file, 0, 2);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, intro_watched_event, NULL);
    menu_label_add(&file, 2, 2, "intro watched");

    item = menu_checkbox_add(&file, 0, 3);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, great_spin_event, NULL);
    menu_label_add(&file, 2, 3,"great spin");

    menu_button_add(&file, 0, 4, "debug menu", debug_menu_onactivate ,NULL);

    menu_label_add(&file, 0, 5, "stored song");
    item = menu_list_add(&file, 12, 5, stored_song_text,sizeof(stored_song_values) / sizeof(*stored_song_values));
    menu_item_register_event(item, MENU_EVENT_LIST | MENU_EVENT_UPDATE, stored_song_event, NULL);

    menu_label_add(&file, 0, 6, "current day");
    item = menu_number_input_add(&file, 12, 6, 10, 1);
    menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_word_event, &z2_file.day);

    menu_label_add(&file, 0, 7, "time of day");
    item = menu_number_input_add(&file, 12, 7, 16, 4);
    menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, time_of_day_event, NULL);

    menu_label_add(&file, 0, 8, "time speed");
    item = menu_number_input_add(&file, 12, 8, -10, 2);
    menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_word_event, &z2_file.timespeed);
    return &file;
}