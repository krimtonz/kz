#ifndef LITE
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "menu.h"
#include "kz.h"
#include "resource.h"
#include "scenes.h"

#define MEMORY_MAX  0x80C00000

enum flags_region {
    FLAGS_WEEK_EVENT,
    FLAGS_EVENT_INF,
    FLAGS_MASK_BIT,
    FLAGS_SCENE_SWITCH,
    FLAGS_SCENE_CHEST,
    FLAGS_SCENE_CLEAR,
    FLAGS_SCENE_COLLECT
};

struct flags_data {
    int                 offset;
    enum flags_region   region;
};

struct actor_info_data {
    struct menu_item   *actor_button;
    struct menu        *memory_menu;
    uint8_t             actor_idx;
    uint8_t             actor_type;
};

// actor vars
static struct actor_info_data   actor_info;
static uint16_t                 actor_id = 0;
static uint16_t                 actor_var = 0;
static int16_t                  actor_pos[3];
static uint16_t                 actor_rot[3];
static int8_t                   actor_type_values[] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15
};

static char                    *actor_type_text[] = {
    "switch", "prop(1)", "player","bomb",
    "npc","enemy","prop","item/action",
    "misc","boss","door","chests",
    "unk1","unk2","unk3","unk4"
};

// object vars
static int16_t  obj_id = 0;

// memory vars
static uint32_t             memory_start_addr = 0x80000000;
static int8_t               memory_view_size = 1;
static struct menu_item    *memory_item_cells[80] = {0};
static int8_t               size_values[] = {
    1, 2, 4
};

static char                *size_text[] = {
    "byte", "halfword", "word"
};

// flags vars
static struct menu_item    *flags_item_cells[256] = {0};
static struct flags_data    flags_data;
static char *flag_region_names[] = {
    "week_event_reg", "event_inf", "mask_mask_bit",
    "switch","chest","clear","collect",
};
static uint32_t flag_start_addr[] = {
    (uint32_t)&z2_file.week_event_inf[0], (uint32_t)&z2_file.event_inf[0],
    (uint32_t)&z2_file.week_event_inf[0], (uint32_t)&z2_game.actor_ctxt.switch_1, (uint32_t)&z2_game.actor_ctxt.chest,
    (uint32_t)&z2_game.actor_ctxt.clear, (uint32_t)&z2_game.actor_ctxt.collectible_1
};
static uint8_t flag_sizes[] = {
    0x64, 0x8, 0x3, 0x10, 0x4, 0x4, 0x10
};

static uint32_t get_mem_val(void *ptr){
    switch(memory_view_size){
        case 1:
        return *(uint8_t*)ptr;
        case 2:
        return *(uint16_t*)ptr;
        case 4:
        default:
        return *(uint32_t*)ptr;
    }
}

static void set_mem_val(void *ptr, uint32_t val){
    switch(memory_view_size){
        case 1:
        *(uint8_t*)ptr = val & 0xFF;
        break;
        case 2:
        *(uint16_t*)ptr = val & 0xFFFF;
        break;
        case 4:
        default:
        *(uint32_t*)ptr = val;
        break;
    }
}

static int memory_cell_callback(struct menu_item *item, enum menu_callback callback, void *data, uint32_t val){
    void *ptr = (void*)(memory_start_addr + ((int)data));
    if(callback == MENU_CALLBACK_UPDATE){
        menu_number_set(item, get_mem_val(ptr));
    }else if(callback == MENU_CALLBACK_ACTIVATE){
        set_mem_val(ptr,val);
        return 1;
    }
    return 0;
}

static void memory_create_table(struct menu *menu){
    int idx = 0;
    for(int i = 0;i<10;i++){
        for (int j = 0;j<8;j++){
            if(memory_item_cells[idx]){
                menu_item_remove(memory_item_cells[idx]);
            }
            if(idx % memory_view_size == 0){
                memory_item_cells[idx] = menu_add_number_input(menu,9 + j * 2, 3 + i, memory_cell_callback, (void*)idx,16,memory_view_size * 2,NULL,memory_view_size);
            }else{
                memory_item_cells[idx] = NULL;
            }
            idx++;
        }
    }
}

static int memory_start_callback(struct menu_item *item, enum menu_callback callback, void *data, uint32_t value){
    if(callback == MENU_CALLBACK_ACTIVATE){
        value -= value % memory_view_size;
        memory_start_addr = value;
        memory_create_table(item->owner);
        return 1;
    }
    return 0;
}

static int memory_size_callback(struct menu_item *item, enum menu_callback callback, void *data, int idx){
    if(callback == MENU_CALLBACK_ACTIVATE){
        memory_start_addr -= memory_start_addr % memory_view_size;
        memory_create_table(item->owner);
        return 1;
    }
    return 0;
}

static void memory_view_draw(struct menu_item *item){
    int x = get_item_x_pos(item) + 56 + (memory_view_size * 0x10);
    int y = get_item_y_pos(item);

    for(int i = 0;i<8 / memory_view_size;i++){
        gfx_printf(x,y,"%02"PRIx8,((memory_start_addr + (i * memory_view_size)) & 0xFF));
        x += (2 * memory_view_size) + (memory_view_size * 16);
    }
    x = get_item_x_pos(item);
    y+=13;
    for(int i=0;i<10;i++){
        gfx_printf(x,y,"%08x",memory_start_addr + (i * 8));
        y+=13;
    }
}

static int memory_inc_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(memory_start_addr >= MEMORY_MAX - 0x4A){
            memory_start_addr = 0x80000000  ;
        }else{
            memory_start_addr += 0x8;
        }
        memory_create_table(item->owner);
        return 1;
    }
    return 0;
}

static int memory_dec_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(memory_start_addr == 0x80000000){
            memory_start_addr = MEMORY_MAX - 0x4A;
        }else{
            memory_start_addr -= 0x8;
        }
        memory_create_table(item->owner);
        return 1;
    }
    return 0;
}

static struct menu_item *menu_add_memory_view(struct menu *menu, uint16_t x, uint16_t y){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        item->draw_proc = memory_view_draw;
    }
    memory_create_table(menu);
    return item;
}

static void objects_view_draw(struct menu_item *item){
    int x = get_item_x_pos(item);
    int y = get_item_y_pos(item);
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

static int actor_spawn_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(actor_id == 0){
            z2_link_spawn_obj = z2_link_form_obj_idx[z2_file.current_form];
        }
        z2_SpawnActor(&z2_game.actor_ctxt, &z2_game, actor_id,(float)actor_pos[0],(float)actor_pos[1],(float)actor_pos[2],actor_rot[0],actor_rot[1],actor_rot[2],actor_var,0x7F,0x3FF,NULL);
        return 1;
    }
    return 0;
}

static int actor_fetch_link_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        actor_pos[0] = (int16_t)z2_link.common.pos_2.x;
        actor_pos[1] = (int16_t)z2_link.common.pos_2.y;
        actor_pos[2] = (int16_t)z2_link.common.pos_2.z;
        actor_rot[0] = z2_link.common.rot_2.x;
        actor_rot[1] = z2_link.common.rot_2.y;
        actor_rot[2] = z2_link.common.rot_2.z;
        return 1;
    }
    return 0;
}

static int actor_type_change(struct menu_item *item, enum menu_callback callback, void *data, int idx){
    if(callback == MENU_CALLBACK_ACTIVATE){
        actor_info.actor_idx = 0;
        return 1;
    }
    return 0;
}

static void actor_info_draw(struct menu_item *item){
    struct actor_info_data *data = item->data;
    z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
    for(int i=0;i<actor_info.actor_idx;i++){
        actor = actor->next;
    }
    sprintf(data->actor_button->text,"%08" PRIx32 ,(uint32_t)actor);
}

static struct menu_item *menu_add_actor_info(struct menu *menu, uint16_t x, uint16_t y, struct actor_info_data *data){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        item->data = data;
        item->draw_proc = actor_info_draw;
    }
    return item;
}

static int goto_actor_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
        for(int i=0;i<actor_info.actor_idx;i++){
            actor = actor->next;
        }
        if(actor){
            z2_link.common.pos_2.x = actor->pos_2.x;
            z2_link.common.pos_2.y = actor->pos_2.y;
            z2_link.common.pos_2.z = actor->pos_2.z;
        }
        return 1;
    }
    return 0;
}

static int view_actor_callback(struct menu_item *item, enum menu_callback callback, void *data){
    static int viewing = 0;
    static _Bool hasviewed = 0;
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
        for(int i=0;i<actor_info.actor_idx;i++){
            actor = actor->next;
        }
        if(actor){
            z2_game.active_cameras[0]->focus = actor;
            viewing = 8;
            hasviewed = 1;
        }
        return 1;
    }else if(callback == MENU_CALLBACK_UPDATE){
        if(!viewing && hasviewed){
            z2_game.active_cameras[0]->focus = &z2_link.common;
            hasviewed = 0;
        }
        if(viewing) viewing--;
    }
    return 0;
}

static int delete_actor_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
        for(int i=0;i<actor_info.actor_idx;i++){
            actor = actor->next;
        }
        if(actor){
            z2_DeleteActor(&z2_game.actor_ctxt,actor,&z2_game);
        }
        return 1;
    }
    return 0;
}

static int actor_dec_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        int actor_cnt = z2_game.actor_ctxt.actor_list[actor_info.actor_type].count;
        if(actor_cnt == 0){
            return 1;
        }
        actor_info.actor_idx += actor_cnt - 1;
        actor_info.actor_idx %= actor_cnt;
        return 1;
    }
    return 0;
}

static int actor_inc_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        int actor_cnt = z2_game.actor_ctxt.actor_list[actor_info.actor_type].count;
        if(actor_cnt == 0){
            
            return 1;
        }
        actor_info.actor_idx++;
        actor_info.actor_idx %= actor_cnt;
        return 1;
    }
    return 0;
}

static int actor_button_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        menu_return(item,MENU_CALLBACK_RETURN,NULL);
        item->owner->parent->child = actor_info.memory_menu;
        actor_info.memory_menu->parent = item->owner->parent;
        item->owner->parent = NULL;
        z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
        for(int i=0;i<actor_info.actor_idx;i++){
            actor = actor->next;
        }
        memory_start_addr = (uint32_t)actor;
        memory_create_table(actor_info.memory_menu);
        return 1;
    }
    return 0;
}

static int flag_callback(struct menu_item *item, enum menu_callback callback, void *data){
        int idx = (int)data;
        uint8_t *addr = (uint8_t*)(flag_start_addr[flags_data.region] + flags_data.offset + (idx / 8));
    if(callback == MENU_CALLBACK_ACTIVATE){
        *addr = *addr ^ (1 << (idx % 8));
        return  1;
    }else if(callback == MENU_CALLBACK_UPDATE){
        if((*addr & (1 << (idx % 8))) > 0){
            menu_switch_set(item,1);
        }else{
            menu_switch_set(item,0);
        }
    }
    return 0;
}

static void refresh_flags_table(struct menu *menu){
    int idx = 0;
    for(int i=0;i<16;i++){
        for(int j = 0;j < 16;j++){
            int offset = (i * 2) + (j/8) + flags_data.offset;
            if(offset<flag_sizes[flags_data.region]){
                flags_item_cells[idx]->enabled = 1;
            }else{
                flags_item_cells[idx]->enabled = 0;
            }
            idx++;
        }
    }
}

static void flags_draw(struct menu_item *item){
    int x = get_item_x_pos(item);
    int y = get_item_y_pos(item);

    gfx_printf(x + 80,y-8,"%s",flag_region_names[flags_data.region]);
    gfx_printf(x + 56,y,"0123456701234567");
    y+=8;
    for(int i = 0;i<16; i++){
        uint32_t offset = flags_data.offset + (i*2);
        if(offset>flag_sizes[flags_data.region]-1) break;
        gfx_printf(x,y,"0x%04" PRIx16,offset);
        y += 8;
    }
}

static int flags_prev_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(flags_data.region == FLAGS_WEEK_EVENT){
            flags_data.region = FLAGS_SCENE_COLLECT;
        }else{
            flags_data.region--;
        }
        refresh_flags_table(item->owner);
        flags_data.offset = 0;
        return 1;
    }
    return 0;
}

static int flags_next_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(flags_data.region == FLAGS_SCENE_COLLECT){
            flags_data.region = FLAGS_WEEK_EVENT;
        }else{
            flags_data.region++;
        }
        refresh_flags_table(item->owner);
        flags_data.offset = 0;
        return 1;
    }
    return 0;
}

static struct menu_item *menu_add_flags(struct menu *menu, uint16_t x, uint16_t y){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        item->draw_proc = flags_draw;
    }
    return item;
}

static int flags_dec_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(flags_data.offset == 0){
            flags_data.offset = flag_sizes[flags_data.region] - 32;
        }else{
            flags_data.offset-=2;
        }
        refresh_flags_table(item->owner);
        return 1;
    }
    return 0;
}

static int flags_inc_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        if(flags_data.offset + 32 < flag_sizes[flags_data.region]){
            flags_data.offset+=2;
        }else{
            flags_data.offset = 0;
        }
        refresh_flags_table(item->owner);
        return 1;
    }
    return 0;
}

struct menu *create_debug_menu(){
    static struct menu debug;
    static struct menu memory;
    static struct menu objects;
    static struct menu actors;
    static struct menu flags;

    menu_init(&debug, kz.main_menu.x, kz.main_menu.y);
    menu_set_padding(&debug,0,2);
    static draw_info_t dec_scroll_draw = { 
        NULL, 0,0,1.0f, 1.0f, 8,8,{{0xFF,0xFF,0xFF,0xFF}}, {{0xFF,0xFF,0xFF,0xFF}}, 1, NULL
    };

    dec_scroll_draw.texture = resource_get(R_KZ_ARROWS);
    static draw_info_t inc_scroll_draw;
    memcpy(&inc_scroll_draw,&dec_scroll_draw,sizeof(inc_scroll_draw));
    inc_scroll_draw.on_tile = 1;
    
    debug.selected_item = menu_add_button(&debug,0,0,"return",menu_return,NULL);
    {
        // Create memory menu
        menu_init(&memory, 0, 0);
        menu_set_padding(&memory,1,5);
        memory.selected_item = menu_add_button(&memory,0,0,"return",menu_return,NULL);
        menu_add_number_input(&memory,0,1,memory_start_callback, NULL, 16, 8, &memory_start_addr, 4);
        menu_add_list(&memory, 10,1,size_text, size_values, 1, sizeof(size_values)/sizeof(*size_values),&memory_view_size,memory_size_callback,NULL);
        menu_add_gfx_button(&memory,0,3,memory_dec_callback,NULL,&dec_scroll_draw);
        menu_add_gfx_button(&memory,0,12,memory_inc_callback,NULL,&inc_scroll_draw);
        menu_add_memory_view(&memory,1,2);
    }

    {
        // Create Objects menu
        menu_init(&objects, 0, 0);
        menu_set_padding(&objects,0,2);
        objects.selected_item = menu_add_button(&objects,0,0,"return",menu_return,NULL);
        menu_add(&objects,0,1,"id:");
        menu_add_number_input(&objects,4,1,NULL,NULL,16,4,&obj_id,2);
        menu_add_button(&objects,12,1,"push",object_push_callback,NULL);
        menu_add_button(&objects,20,1,"pop",object_pop_callback,NULL);
        menu_add_objects_view(&objects,0,2);
    }

    {
        // Create Actors Menu
        menu_init(&actors, 0 ,0);
        menu_set_padding(&actors,0,2);
        actors.selected_item = menu_add_button(&actors,0,0,"return",menu_return,NULL);
        menu_add(&actors,0,1,"type");
        menu_add_list(&actors,6,1,actor_type_text, actor_type_values,1,sizeof(actor_type_values)/sizeof(*actor_type_values),&actor_info.actor_type,actor_type_change, NULL);
        actor_info.actor_type = 0;
        actor_info.actor_idx = 0;
        actor_info.memory_menu = &memory;
        char *actor_button_text = malloc(9);
        actor_button_text[0] = 0;
        menu_add_button(&actors,0,2,"<",actor_dec_callback,NULL);
        menu_add_button(&actors,3,2,">",actor_inc_callback,NULL);
        actor_info.actor_button = menu_add_button(&actors,0,3,actor_button_text,actor_button_callback,NULL);
        menu_add_actor_info(&actors,0,0,&actor_info);
        menu_add_button(&actors,0,4,"go to",goto_actor_callback,NULL);
        menu_add_button(&actors,6,4, "view",view_actor_callback,NULL);
        menu_add_button(&actors,11,4,"delete",delete_actor_callback,NULL);

        menu_add(&actors,0,6,"actor id");
        menu_add_number_input(&actors,9,6,NULL,NULL,16,4,&actor_id,2);
        menu_add(&actors,0,7,"variable");
        menu_add_number_input(&actors,9,7,NULL,NULL,16,4,&actor_var,2);
        menu_add(&actors,0,8,"position");
        menu_add_number_input(&actors,9,8,NULL,NULL,16,4,&actor_pos[0],2);
        menu_add_number_input(&actors,14,8,NULL,NULL,16,4,&actor_pos[1],2);
        menu_add_number_input(&actors,19,8,NULL,NULL,16,4,&actor_pos[2],2);
        menu_add(&actors,0,9,"rotation");
        menu_add_number_input(&actors,9,9,NULL,NULL,16,4,&actor_rot[0],2);
        menu_add_number_input(&actors,14,9,NULL,NULL,16,4,&actor_rot[1],2);
        menu_add_number_input(&actors,19,9,NULL,NULL,16,4,&actor_rot[2],2);
        menu_add_button(&actors,0,10,"spawn",actor_spawn_callback,NULL);
        menu_add_button(&actors,7,10,"fetch from link",actor_fetch_link_callback,NULL);
    }

    {
        // Create Flags Menu
        menu_init(&flags,0,0);
        flags.selected_item = menu_add_button(&flags, 0, 0, "return", menu_return, NULL);
        flags_data.offset = 0;
        flags_data.region = FLAGS_WEEK_EVENT;
        menu_add_button(&flags,9,1,"<",flags_prev_callback,NULL);
        menu_add_button(&flags,11,1,">",flags_next_callback,NULL);
        menu_add_gfx_button(&flags,0,3,flags_dec_callback,NULL,&dec_scroll_draw);
        menu_add_gfx_button(&flags,0,18,flags_inc_callback,NULL,&inc_scroll_draw);
        menu_add_flags(&flags,2,2);
        for(int i = 0;i<256;i++){
            flags_item_cells[i] = menu_add_switch(&flags,(i % 16) + 9, (i / 16) + 3,flag_callback,(void*)i,
                                resource_get(R_KZ_FLAGS),NULL, 0xFFFFFFFF, 0xFFFFFFFF, 0, 1,
                                8,8,NULL);
        }
        refresh_flags_table(&flags);
    }

    menu_add_submenu(&debug,0,1,&memory,"memory");
    menu_add_submenu(&debug,0,2,&objects,"objects");
    menu_add_submenu(&debug,0,3,&actors,"actors");
    menu_add_submenu(&debug,0,4,&flags,"flags");
    return &debug;
}
#endif