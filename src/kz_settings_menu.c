#include <stdlib.h>
#include "kz.h"
#include "settings.h"
#include "resource.h"
#include "input.h"
#include "commands.h"
#include "menu_file.h"
#include "sys.h"

enum settings_switch_item {
    SW_INPUT_DISPLAY,
    SW_TIMER,
    SW_LAG_COUNTER,
    SW_MEMFILE_VOID,
    SW_MEMFILE_POS,
    SW_TURBO
};

static char *memfile_action_text[] = {
    "none", "void", "load position",
};

static int memfile_action_values[] = {
    MEMFILE_NONE, MEMFILE_VOID, MEMFILE_POS,
};

struct binding_view_row{
    struct menu_item *command_button;
    struct menu_item *command_bind;
};

static struct binding_view_row command_bindings[13];
static int command_pos = 0;

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
            case SW_TURBO:
            settings->turbo_type = !settings->turbo_type;
            if(settings->turbo_type == TURBO_HOLD){
                kz_commands[KZ_CMD_TURBO].type = COMMAND_HOLD;
            }else{
                kz_commands[KZ_CMD_TURBO].type = COMMAND_PRESS;
            }
            break;
        }
        return 1;
    }
    return 0;
}

static void update_commands_table(){
    for(int i=0;i<13;i++){
        command_bindings[i].command_button->text = kz_commands[i + command_pos].text;
        menu_button_cb_data_set(command_bindings[i].command_button,(void*)(i + command_pos));
        menu_set_bind(command_bindings[i].command_bind,i + command_pos);
    }
}

static int command_dec_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(command_pos==0){
            command_pos = KZ_CMD_MAX - 13;
        }else{
            command_pos--;
        }
        update_commands_table();
        return 1;
    }
    return 0;
}

static int command_inc_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(command_pos>KZ_CMD_MAX - 14){
            command_pos = 0;
        }else{
            command_pos++;
        }
        update_commands_table();
        return 1;
    }
    return 0;
}

#ifndef LITE
static void do_export_memfile(char *path, void *data){
    int file = creat(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(file!=-1){
        memfile_t *memfile = kz.memfile[kz.memfile_slot];
        write(file,memfile,sizeof(*memfile));
        kz_log("export from memfile %d",kz.memfile_slot);
        close(file);
    }
}

static void do_import_memfile(char *path, void *data){
    int file = open(path, O_RDONLY);
    if(file!=-1){
        memfile_t *memfile = kz.memfile[kz.memfile_slot];
        if(!memfile){
            memfile = malloc(sizeof(*memfile));
        }
        read(file,memfile,sizeof(*memfile));
        kz.memfile[kz.memfile_slot] = memfile;
        kz_log("imported to memfile %d",kz.memfile_slot);
        close(file);
    }
}

static int export_memfile_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(!kz.memfile[kz.memfile_slot]){
            kz_log("no memfile");
            return 1;
        }
        menu_get_file(FILE_MODE_SAVE,"memfile",".kzm",do_export_memfile,NULL);
        return 1;
    }
    return 0;
}

static int import_memfile_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        menu_get_file(FILE_MODE_LOAD,"memfile",".kzm",do_import_memfile,NULL);
        return 1;
    }
    return 0;
}
#endif

static int memfile_action_callback(struct menu_item *item, enum menu_callback callback, void *data, int idx){
    if(callback == MENU_CALLBACK_ACTIVATE){
        settings->memfile_action = memfile_action_values[idx];
        return 1;
    }
    return 0;
}

struct menu *create_settings_menu(){
    static struct menu settingsm;
    static struct menu commands;

    menu_init(&settingsm);
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

    menu_add_checkbox(&settingsm,0,8,settings_switch_callback,(void*)SW_TURBO,NULL);
    menu_checkbox_set(item,settings->turbo_type);
    menu_add(&settingsm,2,8,"turbo hold");
    int y = 9;
    menu_add(&settingsm,0,y,"memfile");
    menu_add_button(&settingsm,15,y,"-",memfile_dec,NULL);
    static watch_t memfile_watch;
    memfile_watch.address = &kz.memfile_slot;
    memfile_watch.type=WATCH_TYPE_U8;
    memfile_watch.floating = 0;
    menu_add_watch(&settingsm,16,y,&memfile_watch,1);
    menu_add_button(&settingsm,17,y++,"+",memfile_inc,NULL);

#ifndef LITE
    menu_add_button(&settingsm,0,y,"export",export_memfile_callback,NULL);
    menu_add_button(&settingsm,8,y++,"import",import_memfile_callback,NULL);
#endif
    menu_add(&settingsm,0,y,"memfile action");
    item = menu_add_list(&settingsm,15,y++,memfile_action_text,memfile_action_values,4,sizeof(memfile_action_values)/sizeof(*memfile_action_values),NULL,
                         memfile_action_callback,NULL);
    
    menu_add(&settingsm,0,y,"saved position");
    menu_add_button(&settingsm,15,y,"-",position_dec,NULL);
    static watch_t position_watch;
    position_watch.address = &kz.pos_slot;
    position_watch.type=WATCH_TYPE_U8;
    position_watch.floating = 0;
    menu_add_watch(&settingsm,16,y,&position_watch,1);
    menu_add_button(&settingsm,17,y++,"+",position_inc,NULL);
    menu_add_submenu(&settingsm,0,y,&commands,"commands");

    // Build commands menu
    {
        menu_init(&commands);
        menu_set_padding(&commands,0,1);

        commands.selected_item = menu_add_button(&commands,0,0,"return",menu_return,NULL);
        menu_add(&commands,2,1,"command");
        menu_add(&commands,22,1,"bind");
        y = 2;

        static draw_info_t dec_scroll_draw = {
            NULL, 0,0,1.0f, 1.0f, 8,8,{{0xFF,0xFF,0xFF,0xFF}}, {{0xFF,0xFF,0xFF,0xFF}}, 1, NULL
        };

        dec_scroll_draw.texture = resource_get(R_KZ_ARROWS);
        static draw_info_t inc_scroll_draw;
        memcpy(&inc_scroll_draw,&dec_scroll_draw,sizeof(inc_scroll_draw));
        inc_scroll_draw.on_tile = 1;

        menu_add_gfx_button(&commands,0,2,command_dec_callback,NULL,&dec_scroll_draw);
        menu_add_gfx_button(&commands,0,14,command_inc_callback,NULL,&inc_scroll_draw);

        for(int i=0;i<13;i++){
            command_bindings[i].command_button = menu_add_button(&commands,2,y,kz_commands[i].text,run_command,(void*)i);
            command_bindings[i].command_bind = menu_add_bind(&commands,22,y++,i);
        }
    }
    return &settingsm;
}