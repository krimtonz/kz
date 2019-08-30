#include "kz.h"

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

struct menu *create_settings_menu(){
    static struct menu settings;
    menu_init(&settings,0,0);
    settings.selected_item = menu_add_button(&settings,0,0,"return",menu_return,NULL);
    menu_add(&settings,0,1,"profile");
    menu_add_button(&settings,12,1,"-",profile_dec,NULL);
    static watch_t profile_watch;
    profile_watch.address = &kz.settings_profile;
    profile_watch.type=WATCH_TYPE_U8;
    profile_watch.floating = 0;
    menu_add_watch(&settings,13,1,&profile_watch);
    menu_add_button(&settings,14,1,"+",profile_inc,NULL);
    menu_add_button(&settings,0,2,"save settings",save_profile,NULL);
    menu_add_button(&settings,0,3,"load settings",load_profile,NULL);
    menu_add_button(&settings,0,4,"load default settings",default_settings,NULL);
    return &settings;
}