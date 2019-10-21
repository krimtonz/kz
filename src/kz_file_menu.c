#include "kz.h"
#include "resource.h"
#include "input.h"

static uint8_t cur_form;

const char *form_options_text[] = {
    "fierce diety","goron","zora","deku","human"
};

static uint8_t form_options_values[] = {
    0,1,2,3,4
};

const char *stored_song_text[] = {
    "none", "sonata of awakening", "goron's lullaby", "new wave bossa nova", "elegy of emptiness",
    "oath to order", "saria's song", "song of time", "song of healing", "epona's song",
    "song of soaring", "song of storms", "sun's song", "inverted song of time", 
    "song of double time", "goron's lullaby intro"
};

static uint16_t stored_song_values[] = {
    255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
};

static int change_selected_form(struct menu_item *item, enum menu_callback callback, void *data){
    int selected_idx = (int)selected_idx;
    z2_file.current_form = form_options_values[selected_idx];
    return 1;
}

static int debug_menu(struct menu_item *item, enum menu_callback callback, void *data){
    if(z2_game.pause_ctx.state == 0){
        z2_game.pause_ctx.state = 1;
            z2_pause_persp(&z2_game);
            z2_game.pause_ctx.unk_0x202 = (z2_game.pause_ctx.screen_idx * 2) + 1;
    }
    z2_game.pause_ctx.debug_menu = 1;
    reserve_buttons(BUTTON_D_DOWN | BUTTON_D_RIGHT | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_L);
    kz.debug_active = 1;
    return 1;
}

struct menu *create_file_menu(){
    static struct menu file;
    menu_init(&file,0,0);
    file.selected_item = menu_add_button(&file,0,0,"return",menu_return,NULL);
    draw_info_t draw_info = {
        resource_get(R_KZ_CHECKBOX), 1,-1, 1.f, 1.f, 8,8,{{0xFF,0xFF,0xFF,0xFF}}, {{0xFF,0xFF,0xFF,0xFF}},0,checkbox_bg
    };
    menu_add_gfx_switch(&file,0,1,&z2_file.intro_flag,1,0x01,NULL,NULL,&draw_info);
    menu_add(&file,2,1,"intro watched");
    menu_add_gfx_switch(&file,0,2,&z2_file.week_event_inf[0x17],1,0x02,NULL,NULL,&draw_info);
    menu_add(&file,2,2,"great spin");
    menu_add_button(&file,0,3,"debug menu",debug_menu,NULL);
    menu_add(&file,0,4,"stored song");
    menu_add_list(&file,12,4,stored_song_text,stored_song_values,2,sizeof(stored_song_values)/sizeof(*stored_song_values),&z2_stored_song,NULL);
    return &file;
}