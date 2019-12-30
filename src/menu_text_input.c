#include <string.h>
#include <malloc.h>

#include "menu.h"
#include "kz.h"

struct item_data {
    char   *value;
    char   *empty_string;
    char  **value_ptr;
    int     val_len;
};

static void menu_text_input_draw(menu_item_t *item){
    struct item_data *data = item->data;
    uint32_t color = DEFAULT_COLOR;
    if(item->owner->selected_item == item){
        color = SELECTED_COLOR;
    }
    char *text = data->value;
    if(strlen(text) == 0){
        text = data->empty_string;
    }
    gfx_printf_color(menu_item_x(item), menu_item_y(item), color, "%s", text);
}

static int menu_text_input_event(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    struct item_data *data = item->data;
    if(event == MENU_EVENT_ACTIVATE){
        menu_keyboard_get(item, &data->value);
    }else if(event == MENU_EVENT_UPDATE){
        if(data->value_ptr){
            strncpy(data->value, *data->value_ptr, data->val_len);
            data->value[data->val_len] = 0;
        }
    }else if(event == MENU_EVENT_KEYBOARD){
        if(data->value_ptr && *data->value_ptr){
            strncpy(*data->value_ptr,data->value,data->val_len);
            (*data->value_ptr)[data->val_len] = 0;
        }
    }
    return 1;
}

static void menu_text_remove(menu_item_t *item){
    struct item_data *data = item->data;
    free(data->value);
    data->value = NULL;
    free(data->empty_string);
    data->empty_string = NULL;
    free(data);
    item->data = NULL;
}

menu_item_t *menu_text_input_add(menu_t *menu, uint16_t x, uint16_t y, char *default_text, char **value_ptr, int val_len){
    menu_item_t *item = menu_add(menu,x,y);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->value = malloc(32);
        data->empty_string = malloc(32);
        memset(data->value,0,32);
        memset(data->empty_string,0,32);
        data->value_ptr = value_ptr;
        data->val_len = val_len - 1;
        if(data->val_len > 31) data->val_len = 31;
        if(default_text){
            strncpy(data->empty_string, default_text, 31);
        }else{
            strncpy(data->empty_string, "untitled", 31);
        }
        item->interactive = 1;
        item->data = data;
        item->draw_proc = menu_text_input_draw;
        item->remove_proc = menu_text_remove;
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE | MENU_EVENT_KEYBOARD, menu_text_input_event, NULL);
    }
    return item;
}