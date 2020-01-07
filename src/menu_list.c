#include <stdlib.h>
#include "menu.h"

struct item_data {
    char                  **text;
    int                     option_cnt;
    int                     selected_idx;
    int                     active;
};

static int menu_list_event(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    struct item_data *data = item->data;
    if(event == MENU_EVENT_ACTIVATE){
        if(data->active){
            *event_data = (void*)data->selected_idx;
            menu_item_trigger_event(item, MENU_EVENT_LIST, event_data);
        }
        data->active = !data->active;
        *event_data = (void*)data->active;
        return 0;
    }
    else if(event == MENU_EVENT_NAVIGATE){
        menu_nav_t nav = (menu_nav_t)event_data;
        if(data->active){
            if(nav == MENU_NAV_UP){
                data->selected_idx++;
                data->selected_idx %= data->option_cnt;
            }else if(nav == MENU_NAV_DOWN){
                data->selected_idx += data->option_cnt - 1;
                data->selected_idx %= data->option_cnt;
            }
            return 1;
        }
        return 0;
    }else if(event == MENU_EVENT_UPDATE && data->active){
        return 1;
    }
    return 0;
}

static void menu_list_draw(menu_item_t *item){
    struct item_data *data = item->data;
    uint32_t color = DEFAULT_COLOR;
    if(data->active){
        color = COLOR_GREEN;
    }else if(item->owner->selected_item == item){
        color = SELECTED_COLOR;
    }
    gfx_printf_color(menu_item_x(item), menu_item_y(item), color,
                     "%s", data->text[data->selected_idx]);
}

void menu_list_set(menu_item_t *item, int idx){
    struct item_data *data = item->data;
    if(idx < 0 || idx >= data->option_cnt){
        return;
    }
    data->selected_idx = idx;
}

static void menu_list_remove(menu_item_t *item){
    free(item->data);
    item->data = NULL;
}

menu_item_t *menu_list_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell,
                           char **text, int option_cnt){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        struct item_data *data = malloc(sizeof(*data));
        data->text = text;
        data->option_cnt = option_cnt;
        data->selected_idx = 0;
        data->active = 0;
        item->draw_proc = menu_list_draw;
        item->remove_proc = menu_list_remove;
        item->data = data;
        item->interactive = 1;
        menu_item_register_event(item, MENU_EVENT_NAVIGATE | MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, menu_list_event, NULL);
    }
    return item;
}
