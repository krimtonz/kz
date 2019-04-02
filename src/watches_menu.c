#include "menu.h"
#include "watches.h"
#include "kz.h"

struct item_data {
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

static void watch_update_callback(struct menu_item *item, void *data, uint32_t value){
    watch_t *watch = data;
    value -= value % watch_data_sizes[watch->type];
    watch->address = (void*)value;
}

static void menu_watch_delete(struct menu_item *item){
    
}

static void menu_watch_anchor(struct menu_item *item){
    watch_t *watch = item->data;
    watch->floating = !watch->floating;
}

static void menu_watch_move(struct menu_item *item){
    
}

void watch_add(watch_t *watch, struct menu_item *item){
    /* Add New Watch Row */
    int x = item->x;
    menu_add_button(item->owner,x,item->y,"X",menu_watch_delete,watch);
    menu_add_button(item->owner,x+1,item->y,"V",menu_watch_anchor,watch);
    menu_add_button(item->owner,x+2,item->y,"<>",menu_watch_move,watch);
    menu_add_number_input(item->owner,x+4,item->y,watch_update_callback,watch,16,8,(int)watch->address);
    menu_add_list(item->owner,x + 13,item->y,watch_type_names,watch_type_values,sizeof(*watch_type_values),sizeof(watch_type_values)/sizeof(*watch_type_values),&(watch->type));
    struct menu_item *witem = menu_add_watch(item->owner,x + 19,item->y,watch);
    watch->x = get_item_x_pos(witem);
    watch->y = get_item_y_pos(witem);

    item->y++;
}

void watches_button_add(struct menu_item *item){
    watch_t *watch = vector_push_back(&kz.watches,1,NULL);
    static void *address = (void*)0x80000000;
    static enum watch_type type = WATCH_TYPE_U8;
    watch->address = address;
    watch->type = type;
    watch->floating = 0;
    kz.watch_cnt++;    
    watch_add(watch,item);
}

struct menu *create_watches_menu(){
    static struct menu watches;
    menu_init(&watches,0,0);
    watches.selected_item = menu_add_button(&watches,0,0,"return",menu_return,NULL);
    struct menu_item *add_button = menu_add_button(&watches,0,1,"+",watches_button_add,NULL);
    for(int i=0;i<kz.watch_cnt;i++){
        watch_add((watch_t*)vector_at(&kz.watches,i),add_button);
    }
    return &watches;
}