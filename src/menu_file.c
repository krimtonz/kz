#ifndef LITE
#include <vector/vector.h>
#include <set/set.h>
#include <stdlib.h>
#include <errno.h>
#include "menu.h"
#include "sys.h"
#include "menu_file.h"
#include "kz.h"
#include "kzresource.h"

#define FILE_ROWS 13

static menu_t               file_menu;
static menu_item_t         *file_menu_rows[FILE_ROWS];
static menu_item_t         *file_menu_location = NULL;
static menu_item_t         *file_menu_text_entry = NULL;
static menu_item_t         *file_menu_accept_button = NULL;
static menu_item_t         *file_menu_clear_button = NULL;
static menu_item_t         *file_menu_down_button = NULL;
static menu_item_t         *file_menu_up_button = NULL;
static menu_item_t         *file_menu_new_folder = NULL;
static int                  file_menu_offset = 0;
static const char          *file_menu_default = NULL;
static const char          *file_menu_extension = NULL;
static file_mode_t          file_menu_mode;
static int                  file_menu_extension_len = 0;
static get_file_callback_t  file_menu_callback = NULL;
static void                *file_menu_callback_data = NULL;
static struct set           dir_files;
static _Bool                file_menu_ready = 0;
static char                *file_menu_text_value;
static char                *new_folder_string = NULL;

typedef struct {
    char    name[256];
    _Bool   isdir;
    char    text[32];
} dir_entry_t;

static _Bool stricmp(const char *a, const char *b){
    while(*a && *b){
        char ca = *a++;
        char cb = *b++;
        if(ca >= 'a' && ca <= 'z'){
            ca += 'A' - 'a';
        }
        if(cb >= 'a' && cb <= 'z'){
            cb += 'A' - 'a';
        }
        if(ca != cb){
            return 0;
        }
    }
    return *a == *b;
}

static _Bool update_file_list(void){
    vector_clear(&dir_files.container);
    DIR *dir = NULL;
    opendir(&dir, ".");
    if(!dir){
        /*
        if(errno == ENODEV){
            strcpy(file_menu_location->text, "no disk");
        }else if(errno == ENOENT){
            strcpy(file_menu_location->text, "no filesystem");
        }else{
            strncpy(file_menu_location->text, strerror(errno), 31);
        }
        */
        return 0;
    }
    getcwd(file_menu_location->text, 32);
    file_menu_location->text[31] = 0;
    dirent_t dirent;
    int ret;
    readdir(&ret, &dirent, dir);
    while(ret != 0){
        if((dirent.dir_name[0] == '.' && strcmp(dirent.dir_name, "..")) != 0 ||
            !(dirent.mode & S_IRUSR)){
            readdir(&ret, &dirent, dir);
            continue;
        }
        _Bool isdir = ((dirent.mode & S_IFMT) == S_IFDIR);
        int name_len = strlen(dirent.dir_name);
        if(!isdir && (name_len < file_menu_extension_len || !stricmp(&dirent.dir_name[name_len - file_menu_extension_len], file_menu_extension))){
            readdir(&ret, &dirent, dir);
            continue;
        }
        dir_entry_t ent;
        strcpy(ent.name, dirent.dir_name);
        ent.isdir = isdir;

        memcpy(ent.text, dirent.dir_name, 32);
        if(name_len > 31){
            strcpy(&ent.text[28],"...");
        }
        set_insert(&dir_files,&ent);
        readdir(&ret, &dirent, dir);
    }
    closedir(&ret, dir);

    return 1;
}

static void update_view(void){
    int y = 0;
    if(update_file_list()){
        if(file_menu_mode == FILE_MODE_LOAD){
            y = 3;
            file_menu_accept_button->enabled = 0;
            file_menu_clear_button->enabled = 0;
            file_menu_text_entry->enabled = 0;
            file_menu_new_folder->enabled = 0;
        }else{
            file_menu_accept_button->enabled = 1;
            file_menu_clear_button->enabled = 1;
            file_menu_text_entry->enabled = 1;
            file_menu_new_folder->enabled = 1;
            y = 6;
        }
        file_menu_up_button->y_cell = y;
        file_menu_down_button->y_cell = y + FILE_ROWS - 1;
        if(dir_files.container.size > FILE_ROWS){
            file_menu_up_button->enabled = 1;
            file_menu_down_button->enabled = 1;
        }else{
            file_menu_up_button->enabled = 0;
            file_menu_down_button->enabled = 0;
        }
        for(int i = 0; i < FILE_ROWS;i++){
            menu_item_t *item = file_menu_rows[i];
            item->y_cell = y++;
            if(i < dir_files.container.size){
                item->enabled = 1;
            }else{
                item->enabled = 0;
            }
        }
    }else{
        file_menu_up_button->enabled = 0;
        file_menu_down_button->enabled = 0;
        file_menu_accept_button->enabled = 0;
        file_menu_clear_button->enabled = 0;
        file_menu_text_entry->enabled = 0;
        for(int i = 0; i < FILE_ROWS;i++){
            file_menu_rows[i]->enabled = 0;
        }
    }
}

static _Bool dir_ent_comp(void *a, void *b){
    dir_entry_t *dir_a = a;
    dir_entry_t *dir_b = b;
    if(strcmp(dir_a->name, "..") == 0){
        return 1;
    }
    if(dir_a->isdir && !dir_b->isdir){
        return 1;
    }
    if(!dir_a->isdir && dir_b->isdir){
        return 0;
    }
    char *a_name = dir_a->name;
    char *b_name = dir_b->name;
    int d;
    while(*a_name && *b_name){
        char ac = *a_name++;
        char bc = *b_name++;
        if(ac >= '0' && ac <= '9' && bc >= '0' && bc <= '9'){
            char *na = a_name - 1;
            char *nb = b_name - 1;
            while(*na =='0'){
                na++;
            }
            while(*nb == '0'){
                nb++;
            }
            while(*a_name>='0' && *a_name<='9'){
                a_name++;
            }
            while(*b_name>='0' && *b_name<='9'){
                b_name++;
            }
            d = (a_name - na) - (b_name - nb);
            if(d < 0){
                return 1;
            }
            if(d > 0){
                return 0;
            }
            while(na != a_name && nb != b_name){
                d = *na++ - *nb++;
                if(d < 0){
                    return 1;
                }
                if(d > 0){
                    return 0;
                }
            }
            continue;
        }
        if(ac >= 'a' && ac <= 'z'){
            ac += 'A' - 'a';
        }
        if(bc >= 'a' && bc <= 'z'){
            bc += 'A' - 'a';
        }
        d = ac - bc;
        if(d < 0){
            return 1;
        }
        if(d > 0){
            return 0;
        }
    }
    d = (a_name - dir_a->name) - (b_name - dir_b->name);
    if(d < 0){
        return 1;
    }
    else{
        return 0;
    }
}

static void return_path(char *path){
    char *ret = malloc(4096);
    getcwd(ret, 4096); // todo: error checking.
    int dl = strlen(ret);
    int nl = strlen(path);
    if(dl + 1 + nl + file_menu_extension_len < 4096){
        ret[dl] = '/';
        strcpy(&ret[dl + 1], path);
        strcpy(&ret[dl + 1 + nl], file_menu_extension);
    }
    file_menu_callback(ret, file_menu_callback_data);
    menu_trigger_event(&file_menu, MENU_EVENT_RETURN, NULL);
    free(ret);
}

static int reset_disk_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    //reset_disk();
    update_view();
    file_menu_offset = 0;
    return 1;
}

static int accept_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    return_path(file_menu_text_value);
    return 1;
}

static int clear_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    memset(file_menu_text_value, 0, 32);
    return 1;
}

static int menu_file_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    int data = (int)item->data;
    dir_entry_t *dirent = set_at(&dir_files, data + file_menu_offset);
    if(dirent->isdir){
        int ret;
        chdir(&ret, dirent->name);
        update_view();
    }else{
        int len = strlen(dirent->name) - file_menu_extension_len;
        char *path = malloc(len + 1);
        memcpy(path, dirent->name, len);
        path[len] = 0;
        if(file_menu_mode == FILE_MODE_SAVE){
            strncpy(file_menu_text_value, path, 31);
            file_menu_text_value[31] = 0;
            file_menu.selected_item = file_menu_accept_button;
        }else{
            return_path(path);
        }
        free(path);
    }
    return 1;
}

static void menu_file_row_draw(menu_item_t *item){
    dir_entry_t *ent = set_at(&dir_files, (int)item->data + file_menu_offset);
    int x = menu_item_x(item);
    int y = menu_item_y(item);
    int tile = 0;
    char *text = ent->text;
    if(strcmp(ent->text, "..") == 0){
        tile = 2;
        text = "back";
    }
    else if(ent->isdir){
        tile = 1;
    }
    uint32_t color = DEFAULT_COLOR;
    if(item->owner->selected_item == item){
        color = SELECTED_COLOR;
    }
    gfx_draw_sprite_color(resource_get(resource_handles[R_KZ_FILES]), x, y, tile, 8, 8, color);
    gfx_printf_color(x + 10, y,color,"%s",text);
}

static void menu_new_folder_draw(menu_item_t *item) {
    int x = menu_item_x(item);
    int y = menu_item_y(item);
    uint32_t color = DEFAULT_COLOR;
    if(item->owner->selected_item == item) {
        color = SELECTED_COLOR;
    }
    gfx_draw_sprite_color(resource_get(resource_handles[R_KZ_FILES]), x, y, 3, 8, 8, color);
    gfx_printf_color(x + 10, y, color, "create new folder");
}

static int new_folder_onactivate(event_handler_t *handler, menu_event_t event, void **event_data) {
    if(event == MENU_EVENT_ACTIVATE) {
        menu_keyboard_get(handler->subscriber, NULL);
    } else if (event == MENU_EVENT_KEYBOARD) {
        int ret;
        mkdir(&ret, (char*)*event_data, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    }
    return;
}

static int file_menu_up_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(file_menu_offset == 0){
        file_menu_offset = dir_files.container.size - FILE_ROWS;
    }else{
        file_menu_offset--;
    }
    return 1;
}

static int file_menu_down_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(file_menu_offset >= dir_files.container.size - FILE_ROWS){
        file_menu_offset = 0;
    }else{
        file_menu_offset++;
    }
    return 1;
}

static void menu_file_init(void){
    if(file_menu_ready){
        return;
    }
    set_init(&dir_files, sizeof(dir_entry_t), dir_ent_comp);
    menu_init(&file_menu, 0, 0);
    file_menu.selected_item = menu_button_add(&file_menu, 0, 0, "return", menu_return, NULL);
    menu_button_add(&file_menu, 0, 1, "reset disk", reset_disk_onactivate, NULL);
    file_menu_location = menu_label_add(&file_menu, 0, 2, "");
    file_menu_text_value = malloc(32);
    file_menu_text_entry = menu_text_input_add(&file_menu, 0, 3, "untitled", &file_menu_text_value, 32);
    file_menu_accept_button = menu_button_add(&file_menu, 0, 4, "accept", accept_onactivate, NULL);
    file_menu_clear_button = menu_button_add(&file_menu, 8, 4, "clear", clear_onactivate, NULL);
    file_menu_new_folder = menu_add(&file_menu, 0, 5);
    file_menu_new_folder->draw_proc = menu_new_folder_draw;
    menu_item_register_event(file_menu_new_folder, MENU_EVENT_ACTIVATE | MENU_EVENT_KEYBOARD, new_folder_onactivate, NULL);
    file_menu_new_folder->interactive = 1;
    new_folder_string = malloc(256);
    int y = 3;

    file_menu_up_button = menu_gfx_button_add(&file_menu, 0, y, scroll_up_sprite, file_menu_up_onactivate, NULL);
    file_menu_down_button = menu_gfx_button_add(&file_menu, 0, y + FILE_ROWS - 1, scroll_down_sprite, file_menu_down_onactivate, NULL);

    for(int i = 0;i < FILE_ROWS;i++){
        menu_item_t *item = menu_add(&file_menu, 1, y++);
        item->data = (void*)i;
        item->draw_proc = menu_file_row_draw;
        item->interactive = 1;
        menu_item_register_event(item, MENU_EVENT_ACTIVATE, menu_file_onactivate, NULL);
        file_menu_rows[i] = item;
    }
    file_menu_ready = 1;
}

void menu_file_get(file_mode_t mode, const char *default_fn, const char *extension, get_file_callback_t callback, void *callback_data){
    if(!file_menu_ready){
        menu_file_init();
    }
    file_menu.selected_item = file_menu.items.first;
    file_menu_default = default_fn;
    file_menu_extension = extension;
    file_menu_extension_len = strlen(extension);
    file_menu_callback = callback;
    file_menu_callback_data = callback_data;
    file_menu_mode = mode;
    file_menu_offset = 0;
    if(default_fn){
        strncpy(file_menu_text_value, default_fn, 31);
        file_menu_text_value[31] = 0;
    }
    void *menu = &file_menu;
    menu_trigger_event(&kz.main_menu, MENU_EVENT_ENTER, &menu);
    update_view();
}

#endif