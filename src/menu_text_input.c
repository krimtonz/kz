#include <string.h>
#include <malloc.h>

#include "menu.h"
#include "commands.h"

struct item_data {
    char                   *value;
    char                  **value_ptr;
    _Bool                   editing;
    int                     edit_idx;
    menu_generic_callback   callback;
    void                   *callback_data;
};

void draw_text_input(struct menu_item *item){
    struct item_data *data = item->data;
    int value_len = strlen(data->value);
    if(!data->editing){
        uint32_t color = 0xFFFFFFFF;
        if(item->owner->selected_item == item){
            color = MENU_SELECTED_COLOR.color;
        }
        gfx_printf_color(get_item_x_pos(item),get_item_y_pos(item),color,"%s",data->value);
        return;
    }
    
    char empty[21] = {0};
    gfx_printf_color(get_item_x_pos(item) + (8 * value_len),get_item_y_pos(item),MENU_SELECTED_COLOR.color,"%s",empty);

    if(data->edit_idx){
        char preedit[20] = {0};
        strncpy(preedit,data->value,data->edit_idx);
        gfx_printf_color(get_item_x_pos(item),get_item_y_pos(item),MENU_SELECTED_COLOR.color,"%s",preedit);
    }

    char edit = data->value[data->edit_idx];
    if(!edit) edit = '_';
    gfx_printf_color(get_item_x_pos(item) + (8 * data->edit_idx),get_item_y_pos(item),0x00FF00FF,"%c",edit);

    if(data->edit_idx!=20){
        char postedit[20] = {0};
        strncpy(postedit,data->value + data->edit_idx + 1, 20 - data->edit_idx);
        gfx_printf_color(get_item_x_pos(item) + (8 * data->edit_idx) + 8,get_item_y_pos(item),MENU_SELECTED_COLOR.color,"%s",postedit);
    }
}

void activate_text_input(struct menu_item *item){
    struct item_data *data = item->data;
    if(data->editing){
        int first_null = 0;
        int last_non_null = 0;
        for(int i=0;i<20;i++){
            if(data->value[i]==0 && !first_null){
                first_null = i;
            }
            if(data->value[i]!=0){
                last_non_null = i;
            }
        }
        if(last_non_null>first_null){
            for(int i=first_null;i<last_non_null;i++){
                data->value[i] = ' ';
            }
        }
        if(data->value_ptr){
            strncpy(*data->value_ptr,data->value,20);
        }
    }
    data->editing = !data->editing;
}

int navigate_text_input(struct menu_item *item, enum menu_nav nav){
    struct item_data *data = item->data;
    if(!data->editing) return 0;
    switch(nav){
        case MENU_NAV_DOWN:
            data->value[data->edit_idx]--;
            break;
        case MENU_NAV_UP:
            data->value[data->edit_idx]++;
            if(data->value[data->edit_idx]<' ') data->value[data->edit_idx] = ' ';
            break;
        case MENU_NAV_LEFT:
            data->edit_idx += 19;
            data->edit_idx %= 20;
            break;
        case MENU_NAV_RIGHT:
            data->edit_idx++;
            data->edit_idx %= 20;
            break;
        default:
            break;
    }
    if(data->value[data->edit_idx]<0) data->value[data->edit_idx] = 'z';
    else if(data->value[data->edit_idx]<' ') data->value[data->edit_idx] = 0;
    else if(data->value[data->edit_idx]>'z') data->value[data->edit_idx] = 0;
    return 1;
}

struct menu_item *menu_add_text_input(struct menu *menu, uint16_t x, uint16_t y,
                                      char *default_text, char **value_ptr){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->value = malloc(21);
        data->value_ptr = value_ptr;
        memset(data->value,0,21);
        data->editing = 0;
        data->edit_idx = 0;
        if(default_text){
            strncpy(data->value,default_text,20);
        }else{
            strncpy(data->value,"untitled",20);
        }
        item->interactive = 1;
        item->data = data;
        item->activate_proc = activate_text_input;
        item->draw_proc = draw_text_input;
        item->navigate_proc = navigate_text_input;
    }
    return item;
}