#include <stdlib.h>
#include <stdbool.h>
#include "menu.h"
#include "input.h"
#include "kzresource.h"
#include "kz.h"

#define MAX_STR_LEN         30

#define SHORTCUT_BACKSPACE  (BUTTON_C_LEFT)
#define SHORTCUT_SHIFT      (BUTTON_C_UP)
#define SHORTCUT_SPACE      (BUTTON_C_RIGHT)
#define SHORTCUT_ACCEPT    (BUTTON_C_DOWN)

struct key_data {
    int row;
    int col;
};

menu_t                          menu_keyboard;
static char                   **dest_string = NULL;
static menu_item_t             *keyboard_caller = NULL;
static _Bool                    shifted = 0;
static char                     input_buf[MAX_STR_LEN + 1] = {0};
static char                     input_buf_pre[MAX_STR_LEN + 1] = {0};
static char                     input_buf_post[MAX_STR_LEN + 1] = {0};
static int                      cursor_pos = 0;
static _Bool                    keyboard_ready = false;

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
        memset(&input_buf[buf_len], ' ', cursor_pos - buf_len);
    }
    for(int i = cursor_pos + 1;i < sizeof(input_buf) - 1;i++){
        if(input_buf[i] == ' ' || input_buf[i] == '_'){
            input_buf[i] = 0;
        }
    }
}

static void keyboard_input_draw(menu_item_t *item){
    uint32_t color = DEFAULT_COLOR;
    if(item->owner->selected_item == item){
        color = SELECTED_COLOR;
    }
    int input_len = strlen(input_buf);
    int x = menu_item_x(item);
    int y = menu_item_y(item);

    if(cursor_pos > 0){
        memcpy(input_buf_pre, input_buf, cursor_pos);
        input_buf_pre[cursor_pos] = 0;
        gfx_printf_color(x, y, color, "%s", input_buf_pre);
        x += strlen(input_buf_pre) * 8;
    }
    char c = input_buf[cursor_pos];
    if(c == ' '){
        c = '_';
    }
    gfx_printf_color(x, y, COLOR_GREEN, "%c", c);
    x += 8;

    int post_len = input_len - cursor_pos - 1;
    if(cursor_pos != input_len){
        memcpy(input_buf_post, input_buf + cursor_pos + 1, post_len);
    }
    if(post_len < 0){
        post_len++;
    }
    if(cursor_pos < sizeof(input_buf) - 1){
        memset(input_buf_post + post_len, '_', sizeof(input_buf) - 1 - input_len);
        input_buf_post[sizeof(input_buf) - cursor_pos - 2]  = 0;
        gfx_printf_color(x, y, color, "%s", input_buf_post);
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

static int accept(void **event_data) {
    input_buf[cursor_pos] = '\0';
    if(dest_string){
        if(!*dest_string){
            *dest_string = malloc(sizeof(input_buf));
            if(!*dest_string){
                return 0;
            }
        }
        int buf_len = strlen(input_buf);
        if(buf_len > 0 && input_buf[buf_len - 1] == '_'){
            input_buf[buf_len - 1] = 0;
        }
        memcpy(*dest_string, input_buf, sizeof(input_buf));
    }
    *event_data = &input_buf;
    menu_item_trigger_event(keyboard_caller, MENU_EVENT_KEYBOARD, event_data);
    dest_string = NULL;
    keyboard_caller = NULL;
    menu_trigger_event(&menu_keyboard, MENU_EVENT_RETURN, event_data);
    return 1;
}

static int keyboard_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_UPDATE){
        uint16_t input = input_pressed();
        if((input & SHORTCUT_BACKSPACE) == SHORTCUT_BACKSPACE){
            backspace();
        }
        else if((input & SHORTCUT_SHIFT) == SHORTCUT_SHIFT){
            shift();
        } else if((input & SHORTCUT_ACCEPT) == SHORTCUT_ACCEPT) {
            return accept(event_data);
        }
        else if((input & SHORTCUT_SPACE) == SHORTCUT_SPACE){
            if(cursor_pos < MAX_STR_LEN - 1){
                input_buf[cursor_pos] = ' ';
                cursor_pos++;
            }
        }
    }else if(event == MENU_EVENT_NAVIGATE){
        menu_nav_t nav = (menu_nav_t)event_data;
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
    return 1;
}

static menu_item_t *menu_keyboard_input_add(menu_t *menu, int x, int y){
    menu_item_t *item = menu_add(menu,x,y);
    if(item){
        item->draw_proc = keyboard_input_draw;
        item->interactive = 1;
        menu_item_register_event(item, MENU_EVENT_UPDATE | MENU_EVENT_NAVIGATE, keyboard_event, NULL);
    }
    return item;
}

static void key_draw(menu_item_t *item){
    struct key_data *data = item->data;
    const char **source = shifted ? keyboard_shift_rows : keyboard_rows;
    if(item->owner->selected_item == item){
        gfx_draw_rectangle(menu_item_x(item) - 4, menu_item_y(item) - 4, 16, 16, SELECTED_COLOR);
    }
    char c = source[data->row][data->col];
    if(c == 0x01){
        gfx_draw_sprite(resource_get(R_Z2_ICON), menu_item_x(item), menu_item_y(item), 5, 8, 8);
    }else if(c == 0x08){
        gfx_draw_sprite(resource_get(R_Z2_ICON), menu_item_x(item), menu_item_y(item), 6, 8, 8);
    }else{
        gfx_printf(menu_item_x(item), menu_item_y(item), "%c", source[data->row][data->col]);
    }
}

static int key_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    struct key_data *data = item->data;
    const char **source = shifted ? keyboard_shift_rows : keyboard_rows;
    int c = source[data->row][data->col];
    if(c == 0x01){
        shift();
        return 1;
    }else if(c == 0x08){
        backspace();
        return 1;
    }
    input_buf[cursor_pos] = source[data->row][data->col];
    if(cursor_pos < sizeof(input_buf) - 1){
        cursor_pos++;
    }
    if(input_buf[cursor_pos] == 0){
        input_buf[cursor_pos] = '_';
    }
    return 1;
}

static menu_item_t *menu_key_add(menu_t *menu, int x_cell, int y_cell, int row, int col){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        struct key_data *data = malloc(sizeof(*data));
        data->col = col;
        data->row = row;
        item->data = data;
        item->draw_proc = key_draw;
        item->interactive = 1;
        menu_item_register_event(item, MENU_EVENT_ACTIVATE, key_onactivate, NULL);
    }
    return item;
}

static void legend_draw(menu_item_t *item) {
    int x = menu_item_x(item);
    int y = menu_item_y(item);

    gfx_draw_sprite_color(resource_get(R_KZ_BUTTONS), x, y, 1, 8, 8, 0xFFF000FF);
    gfx_printf(x + 10, y, "backspace");

    gfx_draw_sprite_color(resource_get(R_KZ_BUTTONS), x, y + 10, 0, 8, 8, 0xFFF000FF);
    gfx_printf(x + 10, y + 10, "space");

    gfx_draw_sprite_color(resource_get(R_KZ_BUTTONS), x, y + 20, 3, 8, 8, 0xFFF000FF);
    gfx_printf(x + 10, y + 20, "caps lock");

    gfx_draw_sprite_color(resource_get(R_KZ_BUTTONS), x, y + 30, 2, 8, 8, 0xFFF000FF);
    gfx_printf(x + 10, y + 30, "accept");
    
}

static int accept_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    return accept(event_data);
}

static int keyboard_eventhandler(event_handler_t *handler, menu_event_t event, void **event_data) {
    switch(event) {
        case MENU_EVENT_HIDE:
        case MENU_EVENT_EXIT:
            input_mask_clear(BUTTON_C_BUTTONS, 0x00, 0x00);
            break;
        case MENU_EVENT_SHOW:
            input_mask_set(BUTTON_C_BUTTONS, 0x00, 0x00);
            break;
    }
    return 0;
}

static void init_keyboard(void){
    menu_init(&menu_keyboard, 0, 0);
    menu_padding_set(&menu_keyboard, 8, 8);
    menu_register_event(&menu_keyboard, MENU_EVENT_EXIT | MENU_EVENT_HIDE | MENU_EVENT_SHOW, keyboard_eventhandler, NULL);

    menu_keyboard.selected_item = menu_button_add(&menu_keyboard, 0, 0, "return", menu_return, NULL);
    menu_button_add(&menu_keyboard, 0, 2, "accept", accept_onactivate, NULL);
    menu_keyboard_input_add(&menu_keyboard, 0, 1);
    for(int i = 0;i < sizeof(keyboard_rows) / sizeof(*keyboard_rows);i++){
        for(int j = 0;j < strlen(keyboard_rows[i]);j++){
            menu_key_add(&menu_keyboard, j, i + 3, i, j);
        }
    }
    menu_item_t *legend = menu_label_add(&menu_keyboard, 0, sizeof(keyboard_rows) / sizeof(*keyboard_rows) + 4, NULL);
    legend->draw_proc = legend_draw;
    keyboard_ready = true;
}

void menu_keyboard_get(menu_item_t *item, char **dest){
    if(!keyboard_ready){
        init_keyboard();
    }
    memset(input_buf, 0, sizeof(input_buf));
    dest_string = dest;
    keyboard_caller = item;
    if(dest && *dest){
        strcpy(input_buf, *dest);
        cursor_pos = strlen(input_buf);
        buf_sanity();
    }
    menu_keyboard.selected_item = menu_keyboard.items.first;
    void *keyboard_data = &menu_keyboard;
    input_mask_set(BUTTON_Z | BUTTON_C_BUTTONS, 0x00, 0x00);
    menu_trigger_event(&kz.main_menu, MENU_EVENT_ENTER, &keyboard_data);
}