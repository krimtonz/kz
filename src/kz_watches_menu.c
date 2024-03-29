#include <stdlib.h>

#include "kz.h"
#include "kzresource.h"

struct watch_row {
    union{
        struct {
            menu_item_t *delete_button;
            menu_item_t *anchor_button;
            menu_item_t *move_button;
            menu_item_t *addr_input;
            menu_item_t *type_list;
            menu_item_t *watch_item;
            menu_item_t *label_item;
        };
        menu_item_t *row_items[7];
    };
    watch_t *watch;
    menu_sprite_t *anchor_sprite;
};

static struct list  watch_rows;
static menu_item_t *add_button = NULL;
static menu_t       watches_menu;
static int          prev_addr = 0x80000000;
static enum watch_type prev_type = WATCH_TYPE_U8;

static char *watch_type_names[] = {
    "u8",   "s8",   "x8",
    "u16",  "s16",  "x16",
    "u32",  "s32",  "x32",
    "f32",
};

static enum watch_type watch_type_values[] = {
    WATCH_TYPE_U8,  WATCH_TYPE_S8,  WATCH_TYPE_X8,
    WATCH_TYPE_U16, WATCH_TYPE_S16, WATCH_TYPE_X16,
    WATCH_TYPE_U32, WATCH_TYPE_S32, WATCH_TYPE_X32,
    WATCH_TYPE_FLOAT
};

static uint8_t watch_data_sizes[] = {
    1, 1, 1,
    2, 2, 2,
    4, 4, 4,
    4,
};

const int watch_types_cnt = sizeof(watch_type_values) / sizeof(*watch_type_values);

static menu_sprite_t delete_sprite = {
    NULL,   0,  0,  DEFAULT_COLOR,  DEFAULT_COLOR,
    8,      8,      NULL,           0,
    0,      0,      NULL
};

static menu_sprite_t anchor_sprite = {
    NULL,   2,  2,  DEFAULT_COLOR,  DEFAULT_COLOR,
    8,      8,      NULL,           0,
    0,      0,      NULL
};

static void watch_add(watch_t *watch, _Bool setpos);

#if defined(WIIVC) || defined(LITE)
#define MEMMAX 0x80800000
#else
#define MEMMAX 0x80C00000
#endif

static int watch_address_onnumber(event_handler_t *handler, menu_event_t event, void **event_data){
    struct watch_row *row = handler->callback_data;
    watch_t *watch = row->watch;
    uint32_t value = (uint32_t)*event_data;
    value -= value % watch_data_sizes[watch->type];
    if(value >= MEMMAX) {
        kz_log("address too high");
        value = 0x80000000;
    } else if(value < 0x80000000) {
        kz_log("address too low");
        value = 0x80000000;
    }
    menu_number_set(handler->subscriber, value);
    watch->address = (void*)value;
    prev_addr = value;
    return 1;
}

static struct watch_row *delete_row(struct watch_row *row){
    struct watch_row *prev_row = list_prev(row);
    watch_t *watch = row->watch;

    if(row->anchor_sprite) {
        free(row->anchor_sprite);
    }

    for(int i = 0;i < sizeof(row->row_items) / sizeof(*row->row_items);i++){
        if(row->row_items[i]){
            menu_item_remove(row->row_items[i]);
        }
    }
    if(watch){
        if(watch->label){
            free(watch->label);
        }
        list_erase(&kz.watches,row->watch);
    }
    list_erase(&watch_rows,row);
    return prev_row;
}

static int menu_watch_delete_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    struct watch_row *prev = delete_row(handler->callback_data);
    struct watch_row *next = NULL;
    if(prev){
        next = list_next(prev);
    }else{
        next = watch_rows.first;
    }
    if(next){
        item->owner->selected_item = next->delete_button;
    }else{
        item->owner->selected_item = add_button;
    }
    while(next){
        for(int i = 0;i < sizeof(next->row_items) / sizeof(*next->row_items);i++){
            next->row_items[i]->y_cell -= 2;
        }
        next = list_next(next);
    }
    add_button->y_cell -= 2;
    return 1;
}

void clear_watches(void){
    struct watch_row *row = watch_rows.first;
    while(row){
        struct watch_row *next = list_next(row);
        delete_row(row);
        row = next;
    }

    if(add_button) {
        add_button->y_cell = 1;
    }
}

void init_watch_rows(void){
    for(watch_t *watch = kz.watches.first;watch;watch = list_next(watch)){
        if(!watch->label){
            watch->label = malloc(21);
            memset(watch->label, 0, 21);
        }
        watch_add(watch, 0);
    }
}

static int menu_watch_anchor_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    struct watch_row *row = handler->callback_data;
    menu_sprite_t *sprite = menu_button_sprite_get(handler->subscriber);
    row->watch->floating = !row->watch->floating;
    if(!row->watch->floating){
        row->watch->x = menu_item_x(row->watch_item);
        row->watch->y = menu_item_y(row->watch_item);
        row->move_button->enabled = 0;
        sprite->on_tile = 7;
    }else{
        row->move_button->enabled = 1;
        sprite->on_tile = 1;
    }
    return 1;
}

static int watch_type_onlist(event_handler_t *handler, menu_event_t event, void **event_data){
    struct watch_row *row = handler->callback_data;
    int idx = (int)*event_data;
    prev_type = row->watch->type = watch_type_values[idx];
    row->watch->address -= (uint32_t)row->watch->address % watch_data_sizes[idx];
    menu_number_set(row->addr_input, (uint32_t)row->watch->address);
    return 1;
}

static void watch_type_set(menu_item_t *item, enum watch_type type) {
    for(int i = 0; i < watch_types_cnt; i++) {
        if(watch_type_values[i] == type) {
            menu_list_set(item, i);
            break;
        }
    }
}

static void watch_add(watch_t *watch, _Bool setpos){
    /* Add New Watch Row item = add button */
    int x = add_button->x_cell;
    int y = add_button->y_cell;

    struct watch_row *row = list_push_back(&watch_rows, NULL);

    row->delete_button = menu_gfx_button_add(&watches_menu, x, y, &delete_sprite, menu_watch_delete_onactivate, row);

    menu_sprite_t *anchor = malloc(sizeof(*anchor));
    memcpy(anchor, &anchor_sprite, sizeof(*anchor));
    anchor->on_tile = watch->floating ? 1 : 7;
    row->anchor_button = menu_gfx_button_add(&watches_menu, x + 1, y, anchor, menu_watch_anchor_onactivate, row);

    row->move_button = menu_move_button_add(&watches_menu, x + 2, y, &watch->x, &watch->y);
    row->move_button->enabled = watch->floating;

    row->addr_input = menu_number_input_add(&watches_menu, x + 4, y, 16, 8);
    menu_item_register_event(row->addr_input, MENU_EVENT_NUMBER, watch_address_onnumber, row);
    menu_number_set(row->addr_input, (uint32_t)watch->address);

    row->type_list = menu_list_add(&watches_menu, x + 14, y, watch_type_names, watch_types_cnt);
    menu_item_register_event(row->type_list, MENU_EVENT_LIST, watch_type_onlist, row);
    watch_type_set(row->type_list, prev_type);

    row->watch_item = menu_watch_add(&watches_menu, x + 18, y, watch, setpos);
    if(setpos){
        watch->x = menu_item_x(row->watch_item);
        watch->y = menu_item_y(row->watch_item);
    }

    row->anchor_sprite = anchor;
    row->watch = watch;
    row->label_item = menu_text_input_add(&watches_menu,x + 4, y + 1, "label", &watch->label, 21);
    add_button->y_cell += 2;
}

static int watches_button_add_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(kz.watches.size >= WATCHES_MAX){
        kz_log("watches limit reached");
        return 1;
    }
    watch_t *watch = list_push_back(&kz.watches, NULL);
    watch->address = (void*)prev_addr;
    watch->type = prev_type;
    watch->floating = 0;
    watch->x = 0;
    watch->y = 0;
    watch->label = malloc(21);
    memset(watch->label, 0, 21);
    watch_add(watch, 1);
    return 1;
}

menu_t *create_watches_menu(void){
    menu_init(&watches_menu, 0, 0);
    delete_sprite.texture = resource_get(R_Z2_ICON);
    anchor_sprite.texture = resource_get(R_Z2_ICON);
    list_init(&watch_rows, sizeof(struct watch_row));
    watches_menu.selected_item = menu_button_add(&watches_menu, 0, 0, "return", menu_return, NULL);
    add_button = menu_button_add(&watches_menu, 0, 1,"+", watches_button_add_onactivate, NULL);
    init_watch_rows();
    return &watches_menu;
}
