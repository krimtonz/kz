#include <stdint.h>
#include <inttypes.h>
#include "menu.h"
#include "kz.h"

static uint32_t memory_start_addr = 0x80000000;
static int8_t view_size = 1;
static int16_t obj_id = 0;

static int8_t size_values[] = {
    1, 2, 4
};

static const char *size_text[] = {
    "byte", "halfword", "word"
};

static int memory_start_callback(struct menu_item *item, enum menu_callback callback, void *data, uint32_t value){
    if(callback == MENU_CALLBACK_ACTIVATE){
        value -= value % size_values[view_size];
        memory_start_addr = value;
        return 1;
    }
    return 0;
}

static int memory_size_callback(struct menu_item *item, enum menu_callback callback, void *data, int idx){
    if(callback == MENU_CALLBACK_ACTIVATE){
        memory_start_addr -= memory_start_addr % size_values[idx];
        return 1;
    }
    return 0;
}

 void memory_view_draw(struct menu_item *item){
    int x = get_item_x_pos(item);
    int y = get_item_y_pos(item);
    for(int i = 0; i < 0x10; i++){
        for (int j = 0; j < 0x10/view_size; j++ ){
            uint32_t addr = memory_start_addr + (0x10 * i) + (view_size * j);
            switch(view_size){
                case 1:
                gfx_printf(x,y,"%02X",*((uint8_t*)addr));
                x+=18;
                break;
                case 2:
                gfx_printf(x,y,"%04X",*((uint16_t*)addr));
                x+=34;
                break;
                case 4:
                default:
                gfx_printf(x,y,"%08X",*((uint32_t*)addr));
                x+=66;
                break;
            }
        }
        x = get_item_x_pos(item);
        y += 10;
    }
}

static void objects_view_draw(struct menu_item *item){
    int x = get_item_x_pos(item);
    int y = get_item_y_pos(item);
    gfx_printf(x,y,"start    - end");
    gfx_printf(x,y+10,"%8X - %8X",z2_game.obj_ctx.obj_space_start, z2_game.obj_ctx.obj_space_end);
    y+=20;
    for(int i = 0; i < z2_game.obj_ctx.obj_cnt;i++){
        z2_obj_t *obj = &z2_game.obj_ctx.obj[i];
        gfx_printf(x,y,"%04" PRIx16 " %08" PRIx32,obj->id,obj->data);
        x+=112;
        if(i & 1){
            x = get_item_x_pos(item);
            y+=10;
        }
    }
}

static struct menu_item *menu_add_memory_view(struct menu *menu, uint16_t x, uint16_t y){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        item->draw_proc = memory_view_draw;
    }
    return item;
}

static struct menu_item *menu_add_objects_view(struct menu *menu, uint16_t x, uint16_t y){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        item->draw_proc = objects_view_draw;
    }
    return item;
}

static int object_push_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(obj_id == 0) return 1;
        int idx = z2_game.obj_ctx.obj_cnt++;
        z2_obj_t *obj = z2_game.obj_ctx.obj;
        obj[idx].id = -obj_id;
        obj[idx].loadfile.vrom_addr= 0;
        size_t size = z2_obj_table[obj_id].vrom_end - z2_obj_table[obj_id].vrom_start;
        obj[idx+1].data = (char*)obj[idx].data + size;
        return 1;
    }
    return 0;
}

static int object_pop_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(z2_game.obj_ctx.obj_cnt>0){
            z2_game.obj_ctx.obj[--z2_game.obj_ctx.obj_cnt].id = 0;
        }
        return 1;
    }
    return 0;
}

struct menu *create_debug_menu(){
    static struct menu debug;
    static struct menu memory;
    static struct menu objects;

    menu_init(&debug, kz.main_menu.x, kz.main_menu.y);
    debug.selected_item = menu_add_button(&debug,0,0,"return",menu_return,NULL);
    {
        // Create memory menu
        menu_init(&memory, 0, 0);
        menu_set_padding(&memory,0,5);
        memory.selected_item = menu_add_button(&memory,0,0,"return",menu_return,NULL);
        menu_add_number_input(&memory,0,1,memory_start_callback, NULL, 16, 8, &memory_start_addr, 4);
        menu_add_list(&memory, 10,1,size_text, size_values, 1, sizeof(size_values)/sizeof(*size_values),&view_size,memory_size_callback,NULL);
        menu_add_memory_view(&memory,0,2);
    }

    {
        // Create Objects menu
        menu_init(&objects, 0, 0);
        objects.selected_item = menu_add_button(&objects,0,0,"return",menu_return,NULL);
        menu_add(&objects,0,1,"id:");
        menu_add_number_input(&objects,4,1,NULL,NULL,16,4,&obj_id,2);
        menu_add_button(&objects,12,1,"push",object_push_callback,NULL);
        menu_add_button(&objects,20,1,"pop",object_pop_callback,NULL);
        menu_add_objects_view(&objects,0,2);
    }

    menu_add_submenu(&debug,0,1,&memory,"memory");
    menu_add_submenu(&debug,0,2,&objects,"objects");
    return &debug;
}