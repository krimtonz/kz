#include <stdlib.h>
#include <sys/stat.h>
#include "kz.h"
#include "settings.h"
#include "kzresource.h"
#include "commands.h"
#include "menu_file.h"
#include "menu_bind.h"
#include "sys.h"
#include "hb_heap.h"

enum settings_switch_item {
    SW_INPUT_DISPLAY,
    SW_TIMER,
    SW_LAG_COUNTER,
    SW_TURBO
};

static char *memfile_action_text[] = {
    "none", "void", "load position",
};

static int memfile_action_values[] = {
    MEMFILE_NONE,   MEMFILE_VOID,   MEMFILE_POS,
};

struct binding_view_row {
    menu_item_t    *command_button;
    menu_item_t    *command_bind;
};

static struct binding_view_row  command_bindings[13];
static int                      command_pos = 0;

static int profile_dec_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    kz.settings_profile += SETTINGS_MAX - 1;
    kz.settings_profile %= SETTINGS_MAX;
    return 1;
}

static int profile_inc_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    kz.settings_profile++;
    kz.settings_profile %= SETTINGS_MAX;
    return 1;
}

static int memfile_dec_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    command_prev_memfile();
    return 1;
}

static int memfile_inc_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    command_next_memfile();
    return 1;
}

static int position_dec_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    command_prev_position();
    return 1;
}

static int position_inc_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    command_next_position();
    return 1;
}

static int save_profile_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    save_settings_to_flashram(kz.settings_profile);
    return 1;
}

static int load_profile_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    load_settings_from_flashram(kz.settings_profile);
    kz_apply_settings();
    return 1;
}

static int default_settings_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    load_default_settings();
    kz_apply_settings();
    return 1;
}

static int command_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    int cmd = (int)handler->callback_data + command_pos;
    if(kz_commands[cmd].proc){
        kz_commands[cmd].proc();
    }
    return 1;
}

static int settings_switch_event(event_handler_t *handler, menu_event_t event, void **event_data){
    enum settings_switch_item sw = (enum settings_switch_item)handler->callback_data;
    if(event == MENU_EVENT_ACTIVATE){
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
            case SW_TURBO:
            settings->turbo_type = !settings->turbo_type;
            if(settings->turbo_type == TURBO_HOLD){
                kz_commands[KZ_CMD_TURBO].type = COMMAND_HOLD;
            }else{
                kz_commands[KZ_CMD_TURBO].type = COMMAND_PRESS;
            }
            break;
        }
    }else if(event == MENU_EVENT_UPDATE){
        menu_item_t *item = handler->subscriber;
        switch(sw){
            case SW_INPUT_DISPLAY:
            menu_checkbox_set(item, settings->input_display);
            break;
            case SW_TIMER:
            menu_checkbox_set(item, settings->timer);
            break;
            case SW_LAG_COUNTER:
            menu_checkbox_set(item, settings->lag_counter);
            break;
            case SW_TURBO:
            menu_checkbox_set(item, settings->turbo_type);
            break;
        }
    }
    return 1;
}

static void update_commands_table(void){
    for(int i = 0;i < 13;i++){
        command_bindings[i].command_button->text = kz_commands[i + command_pos].text;
        menu_bind_set(command_bindings[i].command_bind, i + command_pos);
    }
}

static int command_dec_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(command_pos == 0){
        command_pos = KZ_CMD_MAX - 13;
    }else{
        command_pos--;
    }
    update_commands_table();
    return 1;
}

static int command_inc_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(command_pos > KZ_CMD_MAX - 14){
        command_pos = 0;
    }else{
        command_pos++;
    }
    update_commands_table();
    return 1;
}

#ifndef LITE
static void do_export_pos(char *path, void *data){
    int file = -1;
    creat(&file, path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(file != -1){
        position_t *pos = kz.position_save[kz.pos_slot];
        int res;
        write(&res, file, pos, sizeof(*pos));
        kz_log("export from position %d", kz.pos_slot);
        close(&res, file);
    }
}

static void do_import_pos(char *path, void *data){
    int file = -1;
    open(&file, path, O_RDONLY);
    if(file != -1){
        position_t *pos = kz.position_save[kz.pos_slot];
        if(pos == NULL){
            pos = malloc(sizeof(*pos));
        }
        int res;
        read(&res, file, pos, sizeof(*pos));
        kz.position_save[kz.pos_slot] = pos;
        kz_log("imported to position %d", kz.pos_slot);
        close(&res, file);
    }
}

static int export_pos_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(kz.position_save[kz.pos_slot] == NULL){
        kz_log("no position save");
        return 1;
    }
    menu_file_get(FILE_MODE_SAVE, "position", ".kzp", do_export_pos, NULL);
    return 1;
}

static int import_pos_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_file_get(FILE_MODE_LOAD, "position", ".kzp", do_import_pos, NULL);
    return 1;
}

static void do_export_memfile(char *path, void *data){
    int file = -1;
    creat(&file, path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(file != -1){
        memfile_t *memfile = kz.memfile[kz.memfile_slot];
        int res;
        write(&res, file, memfile, sizeof(*memfile));
        kz_log("export from memfile %d", kz.memfile_slot);
        close(&res, file);
    }
}

static void do_import_memfile(char *path, void *data){
    int file = -1;
    open(&file, path, O_RDONLY);
    if(file != -1){
        memfile_t *memfile = kz.memfile[kz.memfile_slot];
        if(!memfile){
            memfile = malloc(sizeof(*memfile));
        }
        int res;
        read(&res, file, memfile, sizeof(*memfile));
        kz.memfile[kz.memfile_slot] = memfile;
        kz_log("imported to memfile %d", kz.memfile_slot);
        close(&res, file);
    }
}

static int export_memfile_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(!kz.memfile[kz.memfile_slot]){
        kz_log("no memfile");
        return 1;
    }
    menu_file_get(FILE_MODE_SAVE, "memfile", ".kzm", do_export_memfile, NULL);
    return 1;
}

static int import_memfile_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_file_get(FILE_MODE_LOAD, "memfile", ".kzm", do_import_memfile, NULL);
    return 1;
}
#endif

static int memfile_action_onlist(event_handler_t *handler, menu_event_t event, void **event_data){
    settings->memfile_action = memfile_action_values[(int)*event_data];
    return 1;
}

menu_t *create_settings_menu(void){
    static menu_t settingsm;
    static menu_t commands;

    menu_init(&settingsm, 0, 0);
    menu_padding_set(&settingsm, 0, 2);
    settingsm.selected_item = menu_button_add(&settingsm, 0, 0, "return", menu_return, NULL);

    menu_label_add(&settingsm, 0, 1, "profile");
    menu_button_add(&settingsm, 12, 1, "-", profile_dec_onactivate, NULL);
    static watch_t profile_watch;
    profile_watch.address = &kz.settings_profile;
    profile_watch.type = WATCH_TYPE_U8;
    profile_watch.floating = 0;
    menu_watch_add(&settingsm, 13, 1, &profile_watch, 1);
    menu_button_add(&settingsm, 14, 1, "+", profile_inc_onactivate, NULL);
    menu_button_add(&settingsm, 0, 2, "save settings", save_profile_onactivate, NULL);
    menu_button_add(&settingsm, 0, 3, "load settings", load_profile_onactivate, NULL);
    menu_button_add(&settingsm, 0, 4, "load default settings", default_settings_onactivate, NULL);

    menu_item_t *item = menu_checkbox_add(&settingsm, 0, 5);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, settings_switch_event, (void*)SW_INPUT_DISPLAY);
    menu_checkbox_set(item, settings->input_display);
    menu_label_add(&settingsm, 2, 5, "input display");
    menu_move_button_add(&settingsm, 16, 5, &settings->id_x, &settings->id_y);

    item = menu_checkbox_add(&settingsm, 0, 6);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, settings_switch_event, (void*)SW_TIMER);
    menu_checkbox_set(item, settings->timer);
    menu_label_add(&settingsm, 2, 6, "timer");
    menu_move_button_add(&settingsm, 16, 6, &settings->timer_x, &settings->timer_y);

    item = menu_checkbox_add(&settingsm, 0, 7);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, settings_switch_event, (void*)SW_LAG_COUNTER);
    menu_checkbox_set(item, settings->lag_counter);
    menu_label_add(&settingsm, 2, 7, "lag counter");
    menu_move_button_add(&settingsm, 16, 7, &settings->lag_x, &settings->lag_y);

    item = menu_checkbox_add(&settingsm, 0, 8);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, settings_switch_event, (void*)SW_TURBO);
    menu_checkbox_set(item, settings->turbo_type);
    menu_label_add(&settingsm, 2, 8, "turbo hold");

    menu_label_add(&settingsm, 0, 9, "memfile");
    menu_button_add(&settingsm, 15, 9, "-", memfile_dec_onactivate, NULL);
    static watch_t memfile_watch;
    memfile_watch.address = &kz.memfile_slot;
    memfile_watch.type = WATCH_TYPE_U8;
    memfile_watch.floating = 0;
    menu_watch_add(&settingsm, 16, 9, &memfile_watch, 1);
    menu_button_add(&settingsm, 17, 9, "+", memfile_inc_onactivate, NULL);

    int y = 10;
#ifndef LITE
    menu_button_add(&settingsm, 0, y, "export", export_memfile_onactivate, NULL);
    menu_button_add(&settingsm, 8, y++, "import", import_memfile_onactivate, NULL);
#endif

    menu_label_add(&settingsm, 0, y, "memfile action");
    item = menu_list_add(&settingsm, 15, y++, memfile_action_text, sizeof(memfile_action_values) / sizeof(*memfile_action_values));
    menu_item_register_event(item, MENU_EVENT_LIST, memfile_action_onlist, NULL);
    menu_label_add(&settingsm, 0, y, "saved position");
    menu_button_add(&settingsm, 15, y, "-", position_dec_onactivate, NULL);
    static watch_t position_watch;
    position_watch.address = &kz.pos_slot;
    position_watch.type = WATCH_TYPE_U8;
    position_watch.floating = 0;
    menu_watch_add(&settingsm, 16, y, &position_watch, 1);
    menu_button_add(&settingsm, 17, y++, "+", position_inc_onactivate, NULL);

#ifndef LITE
    menu_button_add(&settingsm, 0, y, "export", export_pos_onactivate, NULL);
    menu_button_add(&settingsm, 8, y++, "import", import_pos_onactivate, NULL);
#endif
    y++;

    menu_submenu_add(&settingsm, 0, y, "commands", &commands);

    // Build commands menu
    {
        menu_init(&commands, 0, 0);
        menu_padding_set(&commands, 0, 1);

        commands.selected_item = menu_button_add(&commands, 0, 0, "return", menu_return, NULL);
        menu_label_add(&commands, 2, 1, "command");
        menu_label_add(&commands, 22, 1, "bind");
        y = 2;

        menu_gfx_button_add(&commands, 0, 2, scroll_up_sprite, command_dec_onactivate, NULL);
        menu_gfx_button_add(&commands, 0, 14, scroll_down_sprite,  command_inc_onactivate, NULL);

        for(int i = 0; i < 13; i++){
            command_bindings[i].command_button = menu_button_add(&commands, 2, y, kz_commands[i].text, command_onactivate, (void*)i);
            command_bindings[i].command_bind = menu_bind_add(&commands, 22, y++, i);
        }
    }
    return &settingsm;
}