#include "kz.h"
#include "settings.h"
#include "resource.h"

struct item_data{
    menu_button_callback    callback;
    void                   *callback_data;
};


enum moving_element{
    INPUT_DISPLAY,
    TIMER,
    LAG_COUNTER,
    NONE,
};

static enum moving_element moving = NONE;

static int profile_dec(struct menu_item *item, void *data){
    kz.settings_profile += SETTINGS_MAX - 1;
    kz.settings_profile %= SETTINGS_MAX;
    return 1;
}

static int profile_inc(struct menu_item *item, void *data){
    kz.settings_profile++;
    kz.settings_profile %= SETTINGS_MAX;
    return 1;
}

static int save_profile(struct menu_item *item,void *data){
    save_settings_to_flashram(kz.settings_profile);
    return 1;
}

static int load_profile(struct menu_item *item, void *data){
    load_settings_from_flashram(kz.settings_profile);
    kz_apply_settings();
    return 1;
}

static int default_settings(struct menu_item *item, void *data){
    load_default_settings();
    kz_apply_settings();
    return 1;
}

static int move_item(struct menu_item *item, void *data){
    if(moving == NONE){
        moving = (enum moving_element)data;
    }else{
        moving = NONE;
    }
    return 1;
}

static int nav_item(struct menu_item *item, enum menu_nav nav){
    struct item_data *data = item->data;
    int16_t *x = NULL;
    int16_t *y = NULL;
    if((enum moving_element)data->callback_data != moving) return 0;
    switch(moving){
        case INPUT_DISPLAY:
            x = &settings->id_x;
            y = &settings->id_y;
            break;
        case LAG_COUNTER:
            x = &settings->lag_x;
            y = &settings->lag_y;
            break;
        case TIMER:
            x = &settings->timer_x;
            y = &settings->timer_y;
            break;
        case NONE:
        default:
            return 0;
    }
    switch(nav){
        case MENU_NAV_DOWN:
            (*y)++;
            break;
        case MENU_NAV_LEFT:
            (*x)--;
            break;
        case MENU_NAV_RIGHT:
            (*x)++;
            break;
        case MENU_NAV_UP:
            (*y)--;
            break;
        default:
            return 0;
    }
    return 1;
}

struct menu *create_settings_menu(){
    static struct menu settingsm;
    menu_init(&settingsm,0,0);
    settingsm.selected_item = menu_add_button(&settingsm,0,0,"return",menu_return,NULL);
    menu_add(&settingsm,0,1,"profile");
    menu_add_button(&settingsm,12,1,"-",profile_dec,NULL);
    static watch_t profile_watch;
    profile_watch.address = &kz.settings_profile;
    profile_watch.type=WATCH_TYPE_U8;
    profile_watch.floating = 0;
    menu_add_watch(&settingsm,13,1,&profile_watch,1);
    menu_add_button(&settingsm,14,1,"+",profile_inc,NULL);
    menu_add_button(&settingsm,0,2,"save settings",save_profile,NULL);
    menu_add_button(&settingsm,0,3,"load settings",load_profile,NULL);
    menu_add_button(&settingsm,0,4,"load default settings",default_settings,NULL);
    draw_info_t draw_info = {
        resource_get(R_KZ_CHECKBOX), 1,-1, 1.f, 1.f, 8,8,{{0xFF,0xFF,0xFF,0xFF}}, {{0xFF,0xFF,0xFF,0xFF}},0,checkbox_bg
    };
    menu_add_gfx_switch(&settingsm,0,5,&settings->input_display,1,0x01,NULL,NULL,&draw_info);
    menu_add(&settingsm,2,5,"input display");
    draw_info_t draw = {
        resource_get(R_KZ_ICON), 2, -1, 1.f, 1.f, 8, 8, {{0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF}},1,NULL
    };
    struct menu_item *item = menu_add_gfx_button(&settingsm,16,5,move_item,(void*)INPUT_DISPLAY,&draw);
    item->navigate_proc = nav_item;
    menu_add_gfx_switch(&settingsm,0,6,&settings->timer,1,0x01,NULL,NULL,&draw_info);
    menu_add(&settingsm,2,6,"timer");
    item = menu_add_gfx_button(&settingsm,16,6,move_item,(void*)TIMER,&draw);
    item->navigate_proc = nav_item;
    menu_add_gfx_switch(&settingsm,0,7,&settings->lag_counter,1,0x01,NULL,NULL,&draw_info);
    menu_add(&settingsm,2,7,"lag counter");
    item = menu_add_gfx_button(&settingsm,16,7,move_item,(void*)LAG_COUNTER,&draw);
    item->navigate_proc = nav_item;
    return &settingsm;
}