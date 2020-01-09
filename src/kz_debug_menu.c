#ifndef LITE
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "menu.h"
#include "kz.h"
#include "resource.h"
#include "scenes.h"
#include "input.h"

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
    menu_item_t    *actor_button;
    menu_t         *memory_menu;
    uint8_t         actor_idx;
    uint8_t         actor_type;
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
static menu_t               memory;
static uint32_t             memory_start_addr = 0x80000000;
static int8_t               memory_view_size = 1;
static menu_item_t         *memory_item_cells[80] = {0};
static int8_t               size_values[] = {
    1, 2, 4
};

static char                *size_text[] = {
    "byte", "halfword", "word"
};

// flags vars
static menu_t               flags;
static menu_item_t         *flags_item_cells[256] = {0};
static struct flags_data    flags_data;
static char                *flag_region_names[] = {
    "week_event_reg", "event_inf", "mask_mask_bit",
    "switch","chest","clear","collect",
};
static uint32_t flag_start_addr[] = {
    (uint32_t)&z2_file.week_event_inf[0],   (uint32_t)&z2_file.event_inf[0],
    (uint32_t)&z2_file.mask_mask_bit[0],    (uint32_t)&z2_game.actor_ctxt.switch_1,
    (uint32_t)&z2_game.actor_ctxt.chest,    (uint32_t)&z2_game.actor_ctxt.clear,
    (uint32_t)&z2_game.actor_ctxt.collectible_1,
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

static int memory_cell_event(event_handler_t *handler, menu_event_t event, void **event_data){
    void *ptr = (void*)(memory_start_addr + ((int)handler->callback_data));
    if(event == MENU_EVENT_NUMBER){
        set_mem_val(ptr, (uint32_t)*event_data);
    }else if(event == MENU_EVENT_UPDATE){
        menu_number_set(handler->subscriber, get_mem_val(ptr));
    }
    return 1;
}

static void memory_table_update(void){
    int idx = 0;
    for(int i = 0;i < 10;i++){
        for (int j = 0;j < 8;j++){
            if(memory_item_cells[idx]){
                menu_item_remove(memory_item_cells[idx]);
            }
            if(idx % memory_view_size == 0){
                memory_item_cells[idx] = menu_number_input_add(&memory, 9 + j * 2, 3 + i, 16, memory_view_size * 2);
                menu_item_register_event(memory_item_cells[idx], MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, memory_cell_event, (void*)idx);
            }else{
                memory_item_cells[idx] = NULL;
            }
            idx++;
        }
    }
}

static int memory_start_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_NUMBER){
        uint32_t value = (uint32_t)*event_data;
        value -= value % memory_view_size;
        memory_start_addr = value;
        memory_table_update();
    }else if(event == MENU_EVENT_UPDATE){
        menu_number_set(handler->subscriber, memory_start_addr);
    }
    return 1;
}

static int memory_size_onlist(event_handler_t *handler, menu_event_t event, void **event_data){
    memory_view_size = size_values[(int)*event_data];
    memory_start_addr -= memory_start_addr % memory_view_size;
    memory_table_update();
    return 1;
}

static void memory_view_draw(menu_item_t *item){
    int orig_x = menu_item_x(item);
    int x = orig_x + 56 + (memory_view_size * 0x10);
    int y = menu_item_y(item);

    for(int i = 0;i < 8 / memory_view_size;i++){
        gfx_printf(x, y, "%02"PRIx8, ((memory_start_addr + (i * memory_view_size)) & 0xFF));
        x += (2 * memory_view_size) + (memory_view_size * 16);
    }

    x = orig_x;
    y += 13;
    for(int i = 0;i < 10;i++){
        gfx_printf(x, y, "%08x"PRIx32, memory_start_addr + (i * 8));
        y += 13;
    }
}

static int memory_inc_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    uint32_t new_addr = memory_start_addr + ((input_pressed_raw() & BUTTON_Z) ? 0x50 : 0x8);
    if(new_addr >= MEMORY_MAX - 0x50){
        memory_start_addr = 0x80000000;
    }else{
        memory_start_addr = new_addr;
    }
    memory_table_update();
    return 1;
}

static int memory_dec_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    uint32_t new_addr = memory_start_addr - ((input_pressed_raw() & BUTTON_Z) ? 0x50 : 0x8);
    if(memory_start_addr <= 0x80000000){
        memory_start_addr = MEMORY_MAX - 0x50;
    }else{
        memory_start_addr = new_addr;
    }
    memory_table_update();
    return 1;
}

static menu_item_t *menu_memory_view_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        item->draw_proc = memory_view_draw;
    }
    memory_table_update();
    return item;
}

static void objects_view_draw(menu_item_t *item){
    int orig_x = menu_item_x(item);
    int x = orig_x;
    int y = menu_item_y(item);
    for(int i = 0; i < z2_game.obj_ctx.obj_cnt;i++){
        z2_obj_t *obj = &z2_game.obj_ctx.obj[i];
        gfx_printf_color(x, y, COLOR_FADED, "%04" PRIx16 " %08" PRIx32, obj->id, obj->data);
        x += 112;
        if(i & 1){
            x = orig_x;
            y += 10;
        }
    }
}

static menu_item_t *menu_objects_view_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        item->draw_proc = objects_view_draw;
    }
    return item;
}

static int object_push_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(obj_id == 0){
        return 1;
    }
    int idx = z2_game.obj_ctx.obj_cnt++;
    z2_obj_t *obj = z2_game.obj_ctx.obj;
    obj[idx].id = -obj_id;
    obj[idx].loadfile.vrom_addr = 0;
    size_t size = z2_obj_table[obj_id].vrom_end - z2_obj_table[obj_id].vrom_start;
    obj[idx + 1].data = (char*)obj[idx].data + size;
    return 1;
}

static int object_pop_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(z2_game.obj_ctx.obj_cnt > 0){
        z2_game.obj_ctx.obj[--z2_game.obj_ctx.obj_cnt].id = 0;
    }
    return 1;
}

static void actor_info_draw(menu_item_t *item){
    struct actor_info_data *data = item->data;
    z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
    for(int i = 0;i < actor_info.actor_idx;i++){
        actor = actor->next;
    }

    int x = menu_item_x(item);

    if(!actor){
        strcpy(data->actor_button->text, "<none>");
        gfx_printf_color(x, menu_item_y(item), COLOR_FADED, "id:       n/a");
        item->y_cell++;
        gfx_printf_color(x, menu_item_y(item), COLOR_FADED, "variable: n/a");
    }else{
        sprintf(data->actor_button->text, "%08" PRIx32 , (uint32_t)actor);
        gfx_printf_color(x, menu_item_y(item), COLOR_FADED, "id:       %04"PRIx16, actor->id);
        item->y_cell++;
        gfx_printf_color(x, menu_item_y(item), COLOR_FADED, "variable: %04"PRIx16, actor->variable);

        /* create crosshair showing location of actor,
           because the debug menu is VC/PJ64 only we don't need l3dex */
        Mtx m;
        {
            MtxF mt;
            guTranslateF(&mt, actor->pos_2.x, actor->pos_2.y, actor->pos_2.z);
            MtxF mr;
            guRotateRPYF(&mr, actor->rot_2.x * M_PI / 0x8000,
                              actor->rot_2.y * M_PI / 0x8000,
                              actor->rot_2.z * M_PI / 0x8000);
            guMtxCatF(&mr, &mt, &mt);
            guMtxF2L(&mt, &m);
        }

        z2_disp_buf_t *xlu = &z2_game.common.gfx->poly_xlu;
        static Vtx v[6] = {
            gdSPDefVtxC(-8192,  0,      0,      0,  0,  0xFF,   0x00,   0x00,   0xFF),
            gdSPDefVtxC(8192,   0,      0,      0,  0,  0xFF,   0x00,   0x00,   0xFF),
            gdSPDefVtxC(0,      -8192,   0,     0,  0,  0x00,   0xFF,   0x00,   0xFF),
            gdSPDefVtxC(0,      8192,  0,       0,  0,  0x00,   0xFF,   0x00,   0xFF),
            gdSPDefVtxC(0,      0,      -8192,  0,  0,  0x00,   0x00,   0xFF,   0xFF),
            gdSPDefVtxC(0,      0,      8192,   0,  0,  0x00,   0x00,   0xFF,   0xFF),
        };
        static Gfx crosshair[] = {
            gsDPSetCycleType(G_CYC_1CYCLE),
            gsDPSetRenderMode(G_RM_AA_XLU_LINE, G_RM_AA_XLU_LINE2),
            gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
            gsSPLoadGeometryMode(G_ZBUFFER),
            gsSPTexture(0, 0, 0, 0, G_OFF),
            gsSPVertex(&v, 6, 0),
            gsDPSetPrimColor(0, 0, 0xFF, 0x00, 0x00, 0xFF),
            gsSPLine3D(0, 1, 0),
            gsDPSetPrimColor(0, 0, 0x00, 0xFF, 0x00, 0xFF),
            gsSPLine3D(2, 3, 0),
            gsDPSetPrimColor(0, 0, 0x00, 0x00, 0xFF, 0xFF),
            gsSPLine3D(4, 5, 0),
            gsSPEndDisplayList(),
        };
        gDPPipeSync(xlu->p++);
        gSPMatrix(xlu->p++, gDisplayListData(&xlu->d, m), G_MTX_LOAD | G_MTX_NOPUSH | G_MTX_MODELVIEW);
        gSPDisplayList(xlu->p++, crosshair);
    }

    item->y_cell--;

    x = menu_item_x(data->actor_button) + item->owner->cell_width * 10;
    int actor_cnt = z2_game.actor_ctxt.actor_list[actor_info.actor_type].count;
    int actor_idx = actor_cnt == 0 ? 0 : actor_info.actor_idx + 1;
    gfx_printf(x, menu_item_y(data->actor_button), "(%02d/%02d)", actor_idx, actor_cnt);
}

static menu_item_t *menu_actor_info_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        item->data = &actor_info;
        item->draw_proc = actor_info_draw;
    }
    return item;
}

static int actor_spawn_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(actor_id == 0){
        z2_link_spawn_obj = z2_link_form_obj_idx[z2_file.current_form];
    }
    z2_SpawnActor(&z2_game.actor_ctxt, &z2_game, actor_id, actor_pos[0], actor_pos[1], actor_pos[2],
                  actor_rot[0], actor_rot[1], actor_rot[2],
                  actor_var, 0x7F, 0x3FF, NULL);
    return 1;
}

static int actor_fetch_link_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    actor_pos[0] = z2_link.common.pos_2.x;
    actor_pos[1] = z2_link.common.pos_2.y;
    actor_pos[2] = z2_link.common.pos_2.z;
    actor_rot[0] = z2_link.common.rot_2.x;
    actor_rot[1] = z2_link.common.rot_2.y;
    actor_rot[2] = z2_link.common.rot_2.z;
    return 1;
}

static int actor_type_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_LIST){
        actor_info.actor_type = actor_type_values[(int)*event_data];
        actor_info.actor_idx = 0;
    }else if (event == MENU_EVENT_UPDATE){
        int idx = 0;
        for(int i = 0;i < sizeof(actor_type_values) / sizeof(*actor_type_values);i++){
            if(actor_info.actor_type == actor_type_values[i]){
                idx = i;
                break;
            }
        }
        menu_list_set(handler->subscriber, idx);
    }
    return 1;
}

static int goto_actor_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
    for(int i = 0;i < actor_info.actor_idx;i++){
        actor = actor->next;
    }
    if(actor){
        z2_link.common.pos_2.x = actor->pos_2.x;
        z2_link.common.pos_2.y = actor->pos_2.y;
        z2_link.common.pos_2.z = actor->pos_2.z;
    }
    return 1;
}

static int view_actor_event(event_handler_t *handler, menu_event_t event, void **event_data){
    static int viewing = 0;
    static _Bool hasviewed = 0;
    if(event == MENU_EVENT_ACTIVATE){
        z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
        for(int i = 0;i < actor_info.actor_idx;i++){
            actor = actor->next;
        }
        if(actor){
            z2_game.active_cameras[0]->focus = actor;
            viewing = 8;
            hasviewed = 1;
        }
    }else if(event == MENU_EVENT_UPDATE){
        if(!viewing && hasviewed){
            z2_game.active_cameras[0]->focus = &z2_link.common;
            hasviewed = 0;
        }
        if(viewing){
            viewing--;
        }
    }
    return 1;
}

static int delete_actor_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
    for(int i = 0;i < actor_info.actor_idx;i++){
        actor = actor->next;
    }
    if(actor){
        z2_DeleteActor(&z2_game.actor_ctxt, actor, &z2_game);
        if(actor_info.actor_idx != 0 && actor_info.actor_idx >= z2_game.actor_ctxt.actor_list[actor_info.actor_type].count){
            actor_info.actor_idx--;
        }
    }
    return 1;
}

static int copy_actor_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
    for(int i = 0;i < actor_info.actor_idx;i++){
        actor = actor->next;
    }
    if(actor){
        actor_id = actor->id;
        actor_var = actor->variable;
        actor_pos[0] = actor->pos_2.x;
        actor_pos[1] = actor->pos_2.y;
        actor_pos[2] = actor->pos_2.z;
        actor_rot[0] = actor->rot_2.x;
        actor_rot[1] = actor->rot_2.y;
        actor_rot[2] = actor->rot_2.z;
    }
    return 1;
}

static int actor_dec_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    int actor_cnt = z2_game.actor_ctxt.actor_list[actor_info.actor_type].count;
    if(actor_cnt == 0 || actor_info.actor_idx == 0){
        actor_info.actor_type += sizeof(actor_type_values) / sizeof(*actor_type_values) - 1;
        actor_info.actor_type %= sizeof(actor_type_values) / sizeof(*actor_type_values);
        actor_info.actor_idx = 0;
        return 1;
    }
    actor_info.actor_idx += actor_cnt - 1;
    actor_info.actor_idx %= actor_cnt;
    return 1;
}

static int actor_inc_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    int actor_cnt = z2_game.actor_ctxt.actor_list[actor_info.actor_type].count;
    if(actor_cnt == 0 || actor_info.actor_idx >= actor_cnt - 1){
        actor_info.actor_type++;
        actor_info.actor_type %= sizeof(actor_type_values) / sizeof(*actor_type_values);
        actor_info.actor_idx = 0;
        return 1;
    }
    actor_info.actor_idx++;
    actor_info.actor_idx %= actor_cnt;
    return 1;
}

static int actor_button_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    z2_actor_t *actor = z2_game.actor_ctxt.actor_list[actor_info.actor_type].first;
    for(int i = 0;i < actor_info.actor_idx;i++){
        actor = actor->next;
    }
    memory_start_addr = (uint32_t)actor;
    memory_table_update();
    menu_trigger_event(&kz.main_menu, MENU_EVENT_ENTER, (void**)&actor_info.memory_menu);
    return 1;
}

static void refresh_flags_table(void){
    int idx = 0;
    for(int i = 0;i < 16;i++){
        for(int j = 0;j < 16;j++){
            int offset = (i * 2) + (j / 8) + flags_data.offset;
            if(offset < flag_sizes[flags_data.region]){
                menu_item_enable(flags_item_cells[idx]);
            }else{
                menu_item_disable(flags_item_cells[idx]);
            }
            idx++;
        }
    }
}

static void flags_draw(menu_item_t *item){
    int x = menu_item_x(item);
    int y = menu_item_y(item);

    gfx_printf(x + 80, y - 8, "%s", flag_region_names[flags_data.region]);
    gfx_printf(x + 56, y, "0123456701234567");
    y += 8;
    for(int i = 0;i < 16; i++){
        uint32_t offset = flags_data.offset + (i * 2);
        if(offset > flag_sizes[flags_data.region]-1){
            break;
        }
        gfx_printf(x, y, "0x%04" PRIx16, offset);
        y += 8;
    }
}

menu_item_t *menu_flags_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        item->draw_proc = flags_draw;
    }
    return item;
}


static int flag_event(event_handler_t *handler, menu_event_t event, void **event_data){
    int idx = (int)handler->callback_data;
    uint8_t *addr = (uint8_t*)(flag_start_addr[flags_data.region] + flags_data.offset + (idx / 8));
    if(event == MENU_EVENT_ACTIVATE){
        *addr = *addr ^ (1 << (idx % 8));
    }else if(event == MENU_EVENT_UPDATE){
        menu_item_t *item = handler->subscriber;
        if((*addr & (1 << (idx % 8))) > 0){
            menu_switch_set(item, 1);
        }else{
            menu_switch_set(item, 0);
        }
    }
    return 1;
}

static int flags_prev_onactivate(event_handler_t *callback, menu_event_t event, void **event_data){
    if(flags_data.region == FLAGS_WEEK_EVENT){
        flags_data.region = FLAGS_SCENE_COLLECT;
    }else{
        flags_data.region--;
    }
    refresh_flags_table();
    flags_data.offset = 0;
    return 1;
}

static int flags_next_onactivate(event_handler_t *callback, menu_event_t event, void **event_data){
    if(flags_data.region == FLAGS_SCENE_COLLECT){
        flags_data.region = FLAGS_WEEK_EVENT;
    }else{
        flags_data.region++;
    }
    refresh_flags_table();
    flags_data.offset = 0;
    return 1;
}

static int flags_dec_onactivate(event_handler_t *callback, menu_event_t event, void **event_data){
    if(flags_data.offset == 0){
        flags_data.offset = flag_sizes[flags_data.region] - 32;
    }else{
        flags_data.offset -= 2;
    }
    refresh_flags_table();
    return 1;
}

static int flags_inc_onactivate(event_handler_t *callback, menu_event_t event, void **event_data){
    if(flags_data.offset + 32 < flag_sizes[flags_data.region]){
        flags_data.offset += 2;
    }else{
        flags_data.offset = 0;
    }
    refresh_flags_table();
    return 1;
}

menu_t *create_debug_menu(void){
    static menu_t debug;
    static menu_t objects;
    static menu_t actors;

    menu_init(&debug, 0, 0);
    menu_padding_set(&debug, 0, 2);
    
    menu_item_t *item;

    debug.selected_item = menu_button_add(&debug, 0, 0, "return", menu_return, NULL);
    {
        // Create memory menu
        menu_init(&memory, 0, 0);
        menu_padding_set(&memory, 1, 5);
        memory.selected_item = menu_button_add(&memory, 0, 0, "return", menu_return, NULL);
        
        item = menu_number_input_add(&memory, 0, 1, 16, 8);
        menu_number_set(item, 0x80000000);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, memory_start_event, NULL);

        item = menu_list_add(&memory, 10, 1, size_text, sizeof(size_values) / sizeof(*size_values));
        menu_item_register_event(item, MENU_EVENT_LIST, memory_size_onlist, NULL);

        menu_gfx_button_add(&memory, 0, 3, scroll_up_sprite, memory_dec_onactivate, NULL);
        menu_gfx_button_add(&memory, 0, 12, scroll_down_sprite, memory_inc_onactivate, NULL);

        menu_memory_view_add(&memory, 1, 2);
    }

    {
        // Create Objects menu
        menu_init(&objects, 0, 0);
        menu_padding_set(&objects, 0, 2);
        objects.selected_item = menu_button_add(&objects, 0, 0, "return", menu_return, NULL);
        
        menu_label_add(&objects, 0, 1, "id:");
        item = menu_number_input_add(&objects, 4, 1, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER, menu_number_halfword_event, &obj_id);
        
        menu_button_add(&objects, 12, 1, "push", object_push_onactivate, NULL);

        menu_button_add(&objects, 20, 1, "pop", object_pop_onactivate, NULL);
        
        menu_objects_view_add(&objects, 0, 2);
    }

    {
        // Create Actors Menu
        menu_init(&actors, 0, 0);
        menu_padding_set(&actors, 0, 2);

        actors.selected_item = menu_button_add(&actors, 0, 0, "return", menu_return, NULL);
        actor_info.actor_type = 0;
        actor_info.actor_idx = 0;
        actor_info.memory_menu = &memory;

        menu_label_add(&actors, 0, 1, "type");
        item = menu_list_add(&actors, 6, 1, actor_type_text, sizeof(actor_type_values) / sizeof(*actor_type_values));
        menu_item_register_event(item, MENU_EVENT_LIST | MENU_EVENT_UPDATE, actor_type_event, NULL);
        
        char *actor_button_text = malloc(9);
        strcpy(actor_button_text,"<none>");

        menu_button_add(&actors, 0, 2, "<", actor_dec_onactivate, NULL);
        menu_button_add(&actors, 3, 2, ">", actor_inc_onactivate,NULL);
        actor_info.actor_button = menu_button_add(&actors, 0, 3, actor_button_text, actor_button_onactivate, NULL);

        menu_actor_info_add(&actors, 0, 4);
        
        menu_button_add(&actors, 0, 6, "go to", goto_actor_onactivate, NULL);
        item = menu_label_add(&actors, 6, 6, "view");
        item->interactive = 1;
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, view_actor_event, NULL);
        
        menu_button_add(&actors, 11, 6, "delete", delete_actor_onactivate, NULL);

        menu_button_add(&actors, 18, 6, "copy", copy_actor_onactivate, NULL);

        menu_label_add(&actors, 0, 8, "actor id");
        item = menu_number_input_add(&actors, 9, 8, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_halfword_event, &actor_id);

        menu_label_add(&actors, 0, 9, "variable");
        item = menu_number_input_add(&actors, 9, 9, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_halfword_event, &actor_var);

        menu_label_add(&actors, 0, 10, "position");
        item = menu_number_input_add(&actors, 9, 10, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_halfword_event, &actor_pos[0]);
        item = menu_number_input_add(&actors, 14, 10, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_halfword_event, &actor_pos[1]);
        item = menu_number_input_add(&actors, 19, 10, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_halfword_event, &actor_pos[2]);

        menu_label_add(&actors, 0, 11, "rotation");
        item = menu_number_input_add(&actors, 9, 11, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_halfword_event, &actor_rot[0]);
        item = menu_number_input_add(&actors, 14, 11, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_halfword_event, &actor_rot[1]);
        item = menu_number_input_add(&actors, 19, 11, 16, 4);
        menu_item_register_event(item, MENU_EVENT_NUMBER | MENU_EVENT_UPDATE, menu_number_halfword_event, &actor_rot[2]);

        menu_button_add(&actors, 0, 12, "spawn", actor_spawn_onactivate, NULL);
        menu_button_add(&actors, 7, 12, "fetch from link", actor_fetch_link_onactivate, NULL);
    }

    {
        // Create Flags Menu
        menu_init(&flags, 0, 0);
        flags.selected_item = menu_button_add(&flags, 0, 0, "return", menu_return, NULL);
        flags_data.offset = 0;
        flags_data.region = FLAGS_WEEK_EVENT;

        menu_button_add(&flags, 9, 1, "<", flags_prev_onactivate, NULL);
        menu_button_add(&flags, 11, 1, ">", flags_next_onactivate, NULL);

        menu_gfx_button_add(&flags, 0, 3, scroll_up_sprite, flags_dec_onactivate, NULL);
        menu_gfx_button_add(&flags, 0, 18, scroll_down_sprite, flags_inc_onactivate, NULL);

        menu_flags_add(&flags, 2, 2);

        gfx_texture *flags_texture = resource_get(R_KZ_FLAGS);
        for(int i = 0;i < 256;i++){
            flags_item_cells[i] = menu_switch_add(&flags, (i % 16) + 9, (i / 16) + 3, flags_texture, NULL, DEFAULT_COLOR, DEFAULT_COLOR,
                            0, 1, 8, 8, NULL);
            menu_item_register_event(flags_item_cells[i], MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, flag_event, (void*)i);
        }

        refresh_flags_table();
    }

    menu_submenu_add(&debug, 0, 1, "memory", &memory);
    menu_submenu_add(&debug, 0, 2, "objects", &objects);
    menu_submenu_add(&debug, 0, 3, "actors", &actors);
    menu_submenu_add(&debug, 0, 4, "flags", &flags);

    return &debug;
}
#endif