#include <stdlib.h>

#include "menu.h"
#include "watches.h"
#include "kz.h"
#include "resource.h"

struct item_data{
    menu_generic_callback   callback;
    void                   *callback_data;
};

struct move_data {
    watch_t *watch;
    _Bool moving;
};

struct watch_row{
    struct menu_item *delete_button;
    struct menu_item *anchor_button;
    struct menu_item *move_button;
    struct move_data *mdata;
    struct menu_item *addr_input;
    struct menu_item *type_list;
    struct menu_item *watch_item;
    watch_t *watch;
    int pos;
};

static const char *watch_type_names[] = {
    "u8",   "s8",   "x8",
    "u16",  "s16",  "x16",
    "u32",  "s32",  "x32",
    "float",
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

static int watch_update_callback(struct menu_item *item, enum menu_callback callback, void *data, uint32_t value){
    struct watch_row *row = data;
    watch_t *watch = row->watch;
    value -= value % watch_data_sizes[watch->type];
    watch->address = (void*)value;
    return 1;
}

int menu_watch_delete(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        struct watch_row *row = data;
        for(struct menu_item *next = list_next(row->watch_item);next;next = list_next(next)){
            next->y--;
        }
        struct list *list = &item->owner->items;
        void *next = item;
        void *cur = NULL;
        for(int i=0;i<6;i++){
            cur = next;
            next = list_next(cur);
            list_erase(list,cur);
        }
        list_erase(&kz.watches,row->watch);
        free(row);
        ((struct menu_item*)list_next(list->first))->y--;
        return 1;
    }
    return 0;
}

static int menu_watch_anchor(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        struct watch_row *row = data;
        row->watch->floating=!row->watch->floating;
        if(!row->watch->floating){
            row->watch->x = get_item_x_pos(row->watch_item);
            row->watch->y = get_item_y_pos(row->watch_item);
        }
        return 1;
    }
    return 0;
}

static int menu_watch_move(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        struct watch_row *row = data;
        row->mdata->moving = !row->mdata->moving;
        return 1;
    }
    return 0;
}

static int menu_watch_move_nav(struct menu_item *item, enum menu_nav nav){
    struct item_data *data = item->data;
    struct watch_row *row = data->callback_data;
    struct move_data *mdata = row->mdata;
    if(!mdata) return 0;
    if(!mdata->moving) return 0;
    if(!mdata->watch->floating) return 0;
    switch(nav){
        case MENU_NAV_DOWN:
            mdata->watch->y++;
            break;
        case MENU_NAV_UP:
            mdata->watch->y--;
            break;
        case MENU_NAV_LEFT:
            mdata->watch->x--;
            break;
        case MENU_NAV_RIGHT:
            mdata->watch->x++;
            break;
        default:
            break;
    }
    return 1;
}

void watch_add(watch_t *watch, struct menu_item *item, _Bool setpos){
    /* Add New Watch Row */
    int x = item->x;
    struct watch_row *row = malloc(sizeof(*row));
    draw_info_t draw = {
        resource_get(R_KZ_ICON), 0, -1, 1.f, 1.f, 8, 8, {{0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF}},1,NULL
    };
    row->delete_button = menu_add_gfx_button(item->owner,x,item->y,menu_watch_delete,row,&draw);
    draw.on_tile = 1;
    menu_add_gfx_button(item->owner,x+1,item->y,menu_watch_anchor,row,&draw);
    struct move_data *mdata = malloc(sizeof(*mdata));
    mdata->moving = 0;
    mdata->watch = watch;
    row->mdata = mdata;
    draw.on_tile = 2;
    struct menu_item *witem = menu_add_gfx_button(item->owner,x+2,item->y,menu_watch_move,row,&draw);
    row->move_button = witem;
    witem->navigate_proc = menu_watch_move_nav;
    row->addr_input = menu_add_number_input(item->owner,x+4,item->y,watch_update_callback,row,16,8,&watch->address,sizeof(watch->address));
    row->type_list = menu_add_list(item->owner,x + 13,item->y,watch_type_names,watch_type_values,sizeof(*watch_type_values),sizeof(watch_type_values)/sizeof(*watch_type_values),&(watch->type),NULL);
    witem = menu_add_watch(item->owner,x + 19,item->y,watch, setpos);
    if(setpos){
        watch->x = get_item_x_pos(witem);
        watch->y = get_item_y_pos(witem);
    }
    row->watch_item = witem;
    row->watch = watch;
    item->y++;
}

static int watches_button_add(struct menu_item *item, enum menu_callback callback, void *data){
    watch_t *watch = list_push_back(&kz.watches,NULL);
    static enum watch_type type = WATCH_TYPE_U8;
    watch->address = (void*)0x80000000;
    watch->type = type;
    watch->floating = 0;
    watch->x = 0;
    watch->y = 0;
    watch_add(watch,item,1);
    return 1;
}

struct menu *create_watches_menu(){
    static struct menu watches;
    menu_init(&watches,0,0);
    watches.selected_item = menu_add_button(&watches,0,0,"return",menu_return,NULL);
    struct menu_item *add_button = menu_add_button(&watches,0,1,"+",watches_button_add,NULL);
    for(watch_t *watch = kz.watches.first;watch;watch = list_next(watch)){
        watch_add(watch,add_button,0);
    }
    return &watches;
}