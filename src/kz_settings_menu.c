#include "kz.h"
#include "settings.h"
#include "resource.h"
#include "input.h"
#include "commands.h"

enum settings_switch_item {
    SW_INPUT_DISPLAY,
    SW_TIMER,
    SW_LAG_COUNTER,
    SW_MEMFILE_VOID,
    SW_MEMFILE_POS,
};

static int profile_dec(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        kz.settings_profile += SETTINGS_MAX - 1;
        kz.settings_profile %= SETTINGS_MAX;
        return 1;
    }
    return 0;
}

static int profile_inc(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        kz.settings_profile++;
        kz.settings_profile %= SETTINGS_MAX;
        return 1;
    }
    return 0;
}

static int memfile_dec(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        command_prev_memfile();
        return 1;
    }
    return 0;
}

static int memfile_inc(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        command_next_memfile();
        return 1;
    }
    return 0;
}

static int position_dec(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        command_prev_position();
        return 1;
    }
    return 0;
}

static int position_inc(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        command_next_position();
        return 1;
    }
    return 0;
}

static int save_profile(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        save_settings_to_flashram(kz.settings_profile);
        return 1;
    }
    return 0;
}

static int load_profile(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        load_settings_from_flashram(kz.settings_profile);
        kz_apply_settings();
        return 1;
    }
    return 0;
}

static int default_settings(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        load_default_settings();
        kz_apply_settings();
        return 1;
    }
    return 0;
}

static int run_command(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        int cmd = (int)data;
        if(kz_commands[cmd].proc){
            kz_commands[cmd].proc();
        }
        return 1;
    }
    return 0;
}

static int settings_switch_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        enum settings_switch_item sw = (enum settings_switch_item)data;
        switch(sw){
            case SW_INPUT_DISPLAY:
            settings->input_display = !settings->input_display;
            break;
            case SW_TIMER:
            settings->timer = !settings->timer;
            break;
            case SW_LAG_COUNTER:
            settings->lag_counter = !settings->lag_counter;
            break;
            case SW_MEMFILE_VOID:
            if(settings->memfile_action != MEMFILE_NONE){
                settings->memfile_action = MEMFILE_NONE;
            }else{
                settings->memfile_action = MEMFILE_VOID;
            }
            break;
            case SW_MEMFILE_POS:
            if(settings->memfile_action != MEMFILE_NONE){
                settings->memfile_action = MEMFILE_NONE;
            }else{
                settings->memfile_action = MEMFILE_POS;
            }
            break;
        }
        return 1;
    }
    return 0;
}

struct menu *create_settings_menu(){
    static struct menu settingsm;
    menu_init(&settingsm,kz.main_menu.x,kz.main_menu.y);
    menu_set_padding(&settingsm,0,2);
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
    
    struct menu_item *item = menu_add_checkbox(&settingsm,0,5,settings_switch_callback,(void*)SW_INPUT_DISPLAY,NULL);
    menu_checkbox_set(item,settings->input_display);
    menu_add(&settingsm,2,5,"input display");
    menu_add_move_button(&settingsm,16,5,&settings->id_x,&settings->id_y,NULL,NULL);
    
    item = menu_add_checkbox(&settingsm,0,6,settings_switch_callback,(void*)SW_TIMER,NULL);
    menu_checkbox_set(item,settings->timer);
    menu_add(&settingsm,2,6,"timer");
    menu_add_move_button(&settingsm,16,6,&settings->timer_x,&settings->timer_y,NULL,NULL);

    item = menu_add_checkbox(&settingsm,0,7,settings_switch_callback,(void*)SW_LAG_COUNTER,NULL);
    menu_checkbox_set(item,settings->lag_counter);
    menu_add(&settingsm,2,7,"lag counter");
    menu_add_move_button(&settingsm,16,7,&settings->lag_x,&settings->lag_y,NULL,NULL);

    menu_add(&settingsm,0,8,"memfile");
    menu_add_button(&settingsm,15,8,"-",memfile_dec,NULL);
    static watch_t memfile_watch;
    memfile_watch.address = &kz.memfile_slot;
    memfile_watch.type=WATCH_TYPE_U8;
    memfile_watch.floating = 0;
    menu_add_watch(&settingsm,16,8,&memfile_watch,1);
    menu_add_button(&settingsm,17,8,"+",memfile_inc,NULL);
    
    item = menu_add_checkbox(&settingsm,0,9,settings_switch_callback,(void*)SW_MEMFILE_VOID,NULL);
    if(settings->memfile_action == MEMFILE_VOID){
        menu_checkbox_set(item,1);
    }
    menu_add(&settingsm,3,9,"void on load memfile");

    item = menu_add_checkbox(&settingsm,0,10,settings_switch_callback,(void*)SW_MEMFILE_POS,NULL);
    if(settings->memfile_action == MEMFILE_POS){
        menu_checkbox_set(item,1);
    }
    menu_add(&settingsm,3,10,"load pos on load memfile");

    menu_add(&settingsm,0,11,"saved position");
    menu_add_button(&settingsm,15,11,"-",position_dec,NULL);
    static watch_t position_watch;
    position_watch.address = &kz.pos_slot;
    position_watch.type=WATCH_TYPE_U8;
    position_watch.floating = 0;
    menu_add_watch(&settingsm,16,11,&position_watch,1);
    menu_add_button(&settingsm,17,11,"+",position_inc,NULL);

    static struct menu commands;
    menu_init(&commands,0,0);
    menu_set_padding(&commands,0,1);

    commands.selected_item = menu_add_button(&commands,0,0,"return",menu_return,NULL);
    menu_add(&commands,0,1,"command");
    menu_add(&commands,20,1,"bind");
    int y = 2;
    for(int i=0;i<Z2_CMD_MAX;i++){
        menu_add_button(&commands,0,y,kz_commands[i].text,run_command,(void*)i);
        menu_add_bind(&commands,20,y++,i);
    }

    menu_add_submenu(&settingsm,0,12,&commands,"commands");

    return &settingsm;
}