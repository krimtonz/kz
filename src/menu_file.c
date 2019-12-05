#ifndef LITE
#include <vector/vector.h>
#include <set/set.h>
#include <stdlib.h>
#include "menu.h"
#include "sys.h"
#include "fat.h"
#include "menu_file.h"
#include "kz.h"

#define FILE_ROWS 13

static struct menu file_menu;
static struct menu_item *file_menu_rows[FILE_ROWS];
static struct menu_item *file_menu_location = NULL;
static struct menu_item *file_menu_text_entry = NULL;
static struct menu_item *file_menu_accept_button = NULL;
static struct menu_item *file_menu_clear_button = NULL;
static const char *file_menu_default = NULL;
static const char *file_menu_extension = NULL;
static enum file_mode file_menu_mode;
static int file_menu_extension_len = 0;
static get_file_callback_t file_menu_callback = NULL;
static void *file_menu_callback_data = NULL;
static struct set dir_files;
static _Bool file_menu_ready = 0;
static char *file_menu_text_value;

typedef struct{
    char name[256];
    _Bool isdir;
    char text[32];
} dir_entry_t;

static _Bool stricmp(const char *a, const char *b){
    while(*a && *b){
        char ca = *a++;
        char cb = *b++;
        if(ca>='a' && ca<='z'){
            ca+= 'A' - 'a';
        }
        if(cb>='a' && cb<='z'){
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
    DIR *dir = opendir(".");
    if(!dir){
        return 0;
    }
    getcwd(file_menu_location->text,32);
    file_menu_location->text[31] = 0;
    dirent_t *dirent;
    while((dirent = readdir(dir))){
        if((dirent->dir_name[0] == '.' && strcmp(dirent->dir_name, ".."))!=0 || 
            !(dirent->mode & S_IRUSR)){
            continue;
        }
        _Bool isdir = ((dirent->mode & S_IFMT) == S_IFDIR);
        int name_len = strlen(dirent->dir_name);
        if(!isdir && (name_len < file_menu_extension_len || !stricmp(&dirent->dir_name[name_len - file_menu_extension_len], file_menu_extension))){
            continue;
        }
        dir_entry_t ent;
        strcpy(ent.name,dirent->dir_name);
        ent.isdir = isdir;
        if(strcmp(dirent->dir_name,"..") == 0){
            strcpy(ent.text,"back");
        }
        else{
            memcpy(ent.text,dirent->dir_name,32);
            if(name_len > 31){
                strcpy(&ent.text[28],"...");
            }
        }
        set_insert(&dir_files,&ent);
    }
    closedir(dir);
    return 1;
}

static void update_view(){
    update_file_list();
    int y = 0;
    if(file_menu_mode == FILE_MODE_LOAD){
        y = 3;
        file_menu_accept_button->enabled = 0;
        file_menu_clear_button->enabled = 0;
        file_menu_text_entry->enabled = 0;
    }else{
        file_menu_accept_button->enabled = 1;
        file_menu_clear_button->enabled = 1;
        file_menu_text_entry->enabled = 1;
        y = 5;
    }
    for(int i = 0; i<FILE_ROWS;i++){
        struct menu_item *item = file_menu_rows[i];
        item->y = y++;
        if(i < dir_files.container.size){
            dir_entry_t *ent = set_at(&dir_files,i);
            item->enabled = 1;
            item->text = ent->text;
        }else{
            item->enabled = 0;  
        }
    }
}

static _Bool dir_ent_comp(void *a, void *b){
    dir_entry_t *dir_a = a;
    dir_entry_t *dir_b = b;
    if(strcmp(dir_a->name,"..")==0){
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
        if(ac>='0' && ac<='9' && bc>='0' && bc<='9'){
            char *na = a_name - 1;
            char *nb = b_name - 1;
            while(*na =='0') na++;
            while(*nb == '0') nb++;
            while(*a_name>='0' && *a_name<='9')a_name++;
            while(*b_name>='0' && *b_name<='9')b_name++;
            d = (a_name - na) - (b_name - nb);
            if(d<0){
                return 1;
            }
            if(d>0){
                return 0;
            }
            while(na!=a_name && nb!= b_name){
                d = *na++ - *nb++;
                if(d<0){
                    return 1;
                }
                if(d>0){
                    return 0;
                }
            }
            continue;
        }
        if(ac>='a' && ac<='z'){
            ac += 'A' - 'a';
        }
        if(bc>='a' && bc<='z'){
            bc += 'A' - 'a';
        }
        d = ac - bc;
        if(d<0){
            return 1;
        }
        if(d>0){
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
    if(getcwd(ret,4096)){
        int dl = strlen(ret);
        int nl = strlen(path);
        if(dl+1 + nl + file_menu_extension_len < 4096){
            ret[dl] = '/';
            strcpy(&ret[dl+1],path);
            strcpy(&ret[dl+1+nl],file_menu_extension);
        }
        file_menu_callback(ret,file_menu_callback_data);
        menu_callback(&file_menu,MENU_CALLBACK_RETURN);
    }
    free(ret);
}

static int reset_disk_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        return 1;
    }
    return 0;
}

static int accept_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        return_path(file_menu_text_value);
        return 1;
    }
    return 0;
}

static int clear_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        return 1;
    }
    return 0;
}

static int file_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        dir_entry_t *dirent = (dir_entry_t*)set_at(&dir_files,(int)data);
        if(dirent->isdir){
            chdir(dirent->name);
            update_view();
        }else{
            int len = strlen(dirent->name) - file_menu_extension_len;
            char *path = malloc(len + 1);
            memcpy(path,dirent->name,len);
            path[len] = 0;
            if(file_menu_mode == FILE_MODE_SAVE){
                strncpy(file_menu_text_value,path,31);
                file_menu_text_value[31] = 0;
            }else{
                return_path(path);
            }
            free(path);
        }
        return 1;
    }
    return 0;
}

static void init_file_menu(){
    if(file_menu_ready){
        return;
    }
    set_init(&dir_files,sizeof(dir_entry_t),dir_ent_comp);
    menu_init(&file_menu,kz.main_menu.x,kz.main_menu.y);
    file_menu.selected_item = menu_add_button(&file_menu,0,0,"return",menu_return,NULL);
    menu_add_button(&file_menu,0,1,"reset disk",reset_disk_callback,NULL);
    file_menu_location = menu_add(&file_menu,0,2,"");
    file_menu_text_value = malloc(32);
    file_menu_text_entry = menu_add_text_input(&file_menu,0,3,"untitled",&file_menu_text_value);
    file_menu_accept_button = menu_add_button(&file_menu,0,4,"accept",accept_callback,NULL);
    file_menu_clear_button = menu_add_button(&file_menu,8,4,"clear",clear_callback,NULL);
    int y = 3;
    for(int i=0;i<FILE_ROWS;i++){
        file_menu_rows[i] = menu_add_button(&file_menu,0,y++,NULL,file_callback,(void*)i);
    }
    file_menu_ready = 1;
}

void menu_get_file(enum file_mode mode, const char *default_fn, const char *extension, get_file_callback_t callback, void *callback_data){
    if(!file_menu_ready){
        init_file_menu();
    }
    file_menu_default = default_fn;
    file_menu_extension = extension;
    file_menu_extension_len = strlen(extension);
    file_menu_callback = callback;
    file_menu_callback_data = callback_data;
    file_menu_mode = mode;
    if(default_fn){
        strncpy(file_menu_text_value,default_fn,31);
        file_menu_text_value[31] = 0;
    }
    menu_enter(&kz.main_menu,&file_menu);
    update_view();
}

#endif