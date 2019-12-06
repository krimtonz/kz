#include <string.h>
#include <malloc.h>

#include "menu.h"
#include "keyboard.h"

struct item_data {
    char   *value;
    char   *empty_string;
    char  **value_ptr;
    
};

static int keyboard_accepted(struct menu_item *item, enum menu_callback callback, void *data){
    struct item_data *idata = item->data;
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(idata->value_ptr && *idata->value_ptr){
            strcpy(*idata->value_ptr,idata->value);
        }
    }
    return 0;
}

static void draw_text_input(struct menu_item *item){
    struct item_data *data = item->data;
    uint32_t color = 0xFFFFFFFF;
    if(item->owner->selected_item == item){
        color = MENU_SELECTED_COLOR.color;
    }
    char *text = data->value;
    if(strlen(text) == 0){
        text = data->empty_string;
    }
    gfx_printf_color(get_item_x_pos(item),get_item_y_pos(item),color,"%s",text);
}

static void activate_text_input(struct menu_item *item){
    struct item_data *data = item->data;
    show_keyboard(item,&data->value,keyboard_accepted);
}

static void update_text_input(struct menu_item *item){
    struct item_data *data = item->data;
    if(data->value_ptr){
        strncpy(data->value,*data->value_ptr,31);
        data->value[31] = 0;
    }
}

struct menu_item *menu_add_text_input(struct menu *menu, uint16_t x, uint16_t y,
                                      char *default_text, char **value_ptr){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->value = malloc(32);
        data->empty_string = malloc(32);
        memset(data->value,0,32);
        memset(data->empty_string,0,32);
        data->value_ptr = value_ptr;
        if(default_text){
            strncpy(data->empty_string,default_text,31);
        }else{
            strncpy(data->empty_string,"untitled",31);
        }
        item->interactive = 1;
        item->data = data;
        item->activate_proc = activate_text_input;
        item->draw_proc = draw_text_input;
        item->update_proc = update_text_input;
    }
    return item;
}