#include <stdlib.h>
#include "keyboard.h"
#include "input.h"
#include "kz.h"
#include "resource.h"

#define MAX_STR_LEN         30

#define SHORTCUT_BACKSPACE  (BUTTON_Z | BUTTON_C_LEFT)
#define SHORTCUT_SHIFT      (BUTTON_Z | BUTTON_C_UP)
#define SHORTCUT_SPACE      (BUTTON_Z | BUTTON_C_RIGHT)

struct key_data {
    int row;
    int col;
};

static _Bool shifted = 0;
static char input_buf[MAX_STR_LEN + 1] = {0};
static char input_buf_pre[MAX_STR_LEN + 1] = {0};
static char input_buf_post[MAX_STR_LEN + 1] = {0};
static int cursor_pos = 0;

struct menu kz_keyboard;

static char **dest_string = NULL;
static menu_generic_callback dest_callback = NULL;
static struct menu_item *keyboard_caller = NULL;

static const char *keyboard_rows[] = {
    "`1234567890-=\x08",
    "qwertyuiop[]\\",
    "asdfghjkl;'",
    "\x01zxcvbnm,./\x01"
};

static const char *keyboard_shift_rows[] = {
    "~!@#$%^&*()_+\x08",
    "QWERTYUIOP{}|",
    "ASDFGHJKL;\"",
    "\x01ZXCVBNM<>?\x01"
};

static void buf_sanity(){
    if(input_buf[cursor_pos] == 0){
        input_buf[cursor_pos] = '_';
    }
    int buf_len = strlen(input_buf);
    if(cursor_pos > buf_len){
        memset(&input_buf[buf_len],' ',cursor_pos - buf_len);
    }
    for(int i = cursor_pos + 1;i < sizeof(input_buf) - 1;i++){
        if(input_buf[i] == ' ' || input_buf[i] == '_'){
            input_buf[i] = 0;
        }
    }
}

static void keyboard_input_draw_proc(struct menu_item *item){
    uint32_t color = 0xFFFFFFFF;
    if(item->owner->selected_item == item){
        color = MENU_SELECTED_COLOR.color;
    }
    int input_len = strlen(input_buf);
    int x = get_item_x_pos(item);
    int y = get_item_y_pos(item);
    
    if(cursor_pos > 0){
        memcpy(input_buf_pre,input_buf,cursor_pos);
        input_buf_pre[cursor_pos] = 0;
        gfx_printf_color(x,y,color,"%s",input_buf_pre);
        x += strlen(input_buf_pre) * 8;
    }
    char c = input_buf[cursor_pos];
    if(c ==' ') c = '_';
    gfx_printf_color(x,y,COLOR_GREEN,"%c",c);
    x += 8;

    int post_len = input_len - cursor_pos - 1;
    if(cursor_pos != input_len){
        memcpy(input_buf_post,input_buf + cursor_pos + 1,post_len);
    }
    if(post_len < 0) post_len++;
    if(cursor_pos < sizeof(input_buf) - 1){
        memset(input_buf_post + post_len,'_',sizeof(input_buf) - 1 - input_len);
        input_buf_post[sizeof(input_buf) - cursor_pos - 2]  = 0;
        gfx_printf_color(x,y,color,"%s",input_buf_post);
    }
}

static int keyboard_input_nav_proc(struct menu_item *item, enum menu_nav nav){
    switch(nav){
        case MENU_NAV_LEFT:
            cursor_pos += sizeof(input_buf) - 2;
            cursor_pos %= sizeof(input_buf) - 1;
            buf_sanity();
            return 1;
        case MENU_NAV_RIGHT:
            cursor_pos++;
            cursor_pos %= sizeof(input_buf) - 1;
            buf_sanity();
            return 1;
        case MENU_NAV_DOWN:
        case MENU_NAV_UP:
        default:
        return 0;
    }
}

static void backspace(){
    if(cursor_pos > 0){
        strcpy(input_buf + cursor_pos - 1, input_buf + cursor_pos);
        cursor_pos--;
        buf_sanity();
    }
}

static inline void shift(){
    shifted = !shifted;
}

static void keyboard_update_proc(struct menu_item *item){
    uint16_t input = input_pressed();
    if((input & SHORTCUT_BACKSPACE) == SHORTCUT_BACKSPACE){
        backspace();
    }
    else if((input & SHORTCUT_SHIFT) == SHORTCUT_SHIFT){
        shift();
    }
    else if((input & SHORTCUT_SPACE) == SHORTCUT_SPACE){
        if(cursor_pos < MAX_STR_LEN - 1){
            input_buf[cursor_pos] = ' ';
            cursor_pos++;
        }
    }
}

static struct menu_item *menu_add_keyboard_input(struct menu *menu, int x, int y){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        item->data = NULL;
        item->draw_proc = keyboard_input_draw_proc;
        item->navigate_proc = keyboard_input_nav_proc;
        item->update_proc = keyboard_update_proc;
        item->interactive = 1;
    }
    return item;
}

static void key_draw_proc(struct menu_item *item){
    struct key_data *data = item->data;
    const char **source = shifted ? keyboard_shift_rows : keyboard_rows;
    if(item->owner->selected_item == item){
        gfx_draw_rectangle(get_item_x_pos(item) - 4,get_item_y_pos(item) - 4,16,16,MENU_SELECTED_COLOR.color);
    }
    char c = source[data->row][data->col];
    if(c == 0x01){
        gfx_draw_sprite(resource_get(R_KZ_ICON),get_item_x_pos(item),get_item_y_pos(item),5,8,8);
    }else if(c == 0x08){
        gfx_draw_sprite(resource_get(R_KZ_ICON),get_item_x_pos(item),get_item_y_pos(item),6,8,8);
    }else{
        gfx_printf(get_item_x_pos(item),get_item_y_pos(item),"%c",source[data->row][data->col]);
    }
}

static void key_activate_proc(struct menu_item *item){
    struct key_data *data = item->data;
    const char **source = shifted ? keyboard_shift_rows : keyboard_rows;
    int c = source[data->row][data->col];
    if(c == 0x01){
        shift();
        return;
    }else if(c == 0x08){
        backspace();
        return;
    }
    input_buf[cursor_pos] = source[data->row][data->col];
    if(cursor_pos < sizeof(input_buf) - 1){
        cursor_pos++;
    } 
    if(input_buf[cursor_pos] == 0){
        input_buf[cursor_pos] = '_';
    }
}

static struct menu_item *menu_add_key(struct menu *menu, int x, int y, int row, int col){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct key_data *data = malloc(sizeof(*data));
        data->col = col;
        data->row = row;
        item->data = data;
        item->draw_proc = key_draw_proc;
        item->activate_proc = key_activate_proc;
        item->interactive = 1;
    }
    return item;
}

static int accept_proc(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(dest_string){
            if(!*dest_string){
                *dest_string = malloc(sizeof(input_buf));
                if(!*dest_string) return 0;
            }
            int buf_len = strlen(input_buf);
            if(buf_len > 0 && input_buf[buf_len - 1] == '_') input_buf[buf_len - 1] = 0;
            memcpy(*dest_string,input_buf,sizeof(input_buf));
        }
        if(dest_callback){
            dest_callback(keyboard_caller,MENU_CALLBACK_ACTIVATE,NULL);
        }
        dest_string = NULL;
        dest_callback = NULL;
        keyboard_caller = NULL;
        return menu_return(item,MENU_CALLBACK_ACTIVATE,NULL);
    }
    return 0;
}

void show_keyboard(struct menu_item *item, char **dest, menu_generic_callback callback){
    memset(input_buf,0,sizeof(input_buf));
    dest_string = dest;
    dest_callback = callback;
    keyboard_caller = item;
    if(dest && *dest){
        strcpy(input_buf,*dest);
        cursor_pos = strlen(input_buf);
        buf_sanity();
    }
    item->owner->child = &kz_keyboard;
    kz_keyboard.parent = item->owner;
    if(item->owner->child->callback_proc){
        item->owner->child->callback_proc(MENU_CALLBACK_ENTER);
    }
    menu_set_pos(&kz_keyboard,kz.main_menu.x,kz.main_menu.y);
}

void init_kz_keyboard(){

    menu_init(&kz_keyboard,0,0);
    menu_set_padding(&kz_keyboard,8,8);

    kz_keyboard.selected_item = menu_add_button(&kz_keyboard,0,0,"return",menu_return,NULL);
    menu_add_button(&kz_keyboard,0,2,"accept",accept_proc,NULL);
    menu_add_keyboard_input(&kz_keyboard,0,1);
    for(int i = 0;i < sizeof(keyboard_rows) / sizeof(*keyboard_rows);i++){
        for(int j = 0;j < strlen(keyboard_rows[i]);j++){
            menu_add_key(&kz_keyboard,j,i+3,i,j);
        }
    }
}