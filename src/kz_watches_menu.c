#include <stdlib.h>

#include "menu.h"
#include "watches.h"
#include "kz.h"
#include "resource.h"
#include "input.h"

struct item_data{
    menu_generic_callback   callback;
    void                   *callback_data;
};

struct move_data {
    watch_t *watch;
    _Bool moving;
};

struct watch_row{
    struct move_data *mdata;
    union{
        struct {
            struct menu_item *delete_button;
            struct menu_item *anchor_button;
            struct menu_item *move_button;
            struct menu_item *addr_input;
            struct menu_item *type_list;
            struct menu_item *watch_item;
            struct menu_item *label_item;
        };
        struct menu_item *row_items[7];
    };
    watch_t *watch;
};

static struct list watch_rows;
static struct menu_item *add_button = NULL;
static struct menu watches;

static const char *watch_type_names[] = {
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

static void watch_add(watch_t *watch, struct menu_item *item, _Bool setpos);

static int watch_update_callback(struct menu_item *item, enum menu_callback callback, void *data, uint32_t value){
    struct watch_row *row = data;
    watch_t *watch = row->watch;
    value -= value % watch_data_sizes[watch->type];
    watch->address = (void*)value;
    return 1;
}

static struct watch_row *delete_row(struct watch_row *row){
    struct watch_row *prev_row = list_prev(row);
    struct menu *menu = &watches;
    for(int i = 0;i<sizeof(row->row_items)/sizeof(*row->row_items);i++){
        if(row->row_items[i]){
            list_erase(&menu->items,row->row_items[i]);
        }
    }
    watch_t *watch = row->watch;
    if(watch){
        if(watch->label){
            free(watch->label);
        }
        list_erase(&kz.watches,row->watch);
    }
    if(row->mdata){
        free(row->mdata);
    }
    list_erase(&watch_rows,row);
    return prev_row;
}

static int menu_watch_delete(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        struct watch_row *prev = delete_row((struct watch_row*)data);
        if(prev){
            item->owner->selected_item = prev->delete_button;
            struct watch_row *next = list_next(prev);
            while(next){
                for(int i = 0;i<sizeof(next->row_items)/sizeof(*next->row_items);i++){
                    next->row_items[i]->y--;
                }
                next = list_next(next);
            }
        }else{
            item->owner->selected_item = add_button;
        }
        add_button->y--;
        return 1;
    }
    return 0;
}

void clear_watches(){
    struct watch_row *row = watch_rows.first;
    while(row){
        struct watch_row *next = list_next(row);
        delete_row(row);
        row = next;
    }
    add_button->y = 1;
}

void init_watch_rows(){
    for(watch_t *watch = kz.watches.first;watch;watch = list_next(watch)){
        if(!watch->label){
            watch->label = malloc(21);
            memset(watch->label,0,21);
        }
        watch_add(watch,add_button,0);
    }
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
    int amt = input_pressed_raw() & BUTTON_Z?5:3;
    switch(nav){
        case MENU_NAV_DOWN:
            mdata->watch->y+=amt;
            break;
        case MENU_NAV_UP:
            mdata->watch->y-=amt;
            break;
        case MENU_NAV_LEFT:
            mdata->watch->x-=amt;
            break;
        case MENU_NAV_RIGHT:
            mdata->watch->x+=amt;
            break;
        default:
            break;
    }
    return 1;
}

static void watch_add(watch_t *watch, struct menu_item *item, _Bool setpos){
    /* Add New Watch Row */
    int x = item->x;
    struct watch_row *row = list_push_back(&watch_rows,NULL);
    draw_info_t draw = {
        resource_get(R_KZ_ICON), 0, -1, 0.8f, 0.8f, 10, 10, {{0xFF,0xFF,0xFF,0xFF}},{{0xFF,0xFF,0xFF,0xFF}},1,NULL
    };
    row->delete_button = menu_add_gfx_button(item->owner,x,item->y,menu_watch_delete,row,&draw);
    draw.on_tile = 1;
    row->anchor_button = menu_add_gfx_button(item->owner,x+1,item->y,menu_watch_anchor,row,&draw);
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
    witem = menu_add_watch(item->owner,x + 17,item->y,watch, setpos);
    if(setpos){
        watch->x = get_item_x_pos(witem);
        watch->y = get_item_y_pos(witem);
    }
    row->watch_item = witem;
    row->watch = watch;
    row->label_item = menu_add_text_input(item->owner,x+28,item->y,"label",&watch->label);
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
    watch->label = malloc(21);
    memset(watch->label,0,21);
    watch_add(watch,item,1);
    return 1;
}

struct menu *create_watches_menu(){
    menu_init(&watches,0,0);
    list_init(&watch_rows,sizeof(struct watch_row));
    watches.selected_item = menu_add_button(&watches,0,0,"return",menu_return,NULL);
    add_button = menu_add_button(&watches,0,1,"+",watches_button_add,NULL);
    init_watch_rows();
    return &watches;
}