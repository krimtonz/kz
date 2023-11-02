#ifndef LITE
#include <stdint.h>
#include <set/set.h>
#include <mips.h>
#include "z2.h"
#include "state.h"
#include "zu.h"
#include "kzresource.h"
#include "kz.h"
#include "hb_heap.h"
#include "audio.h"
#include "vc.h"

#ifdef WIIVC
#define MAYBENOINLINE __attribute__ ((noinline))
#else
#define MAYBENOINLINE
#endif

static _Bool save_wr = 0;

static MAYBENOINLINE void st_write(void **dst, void *src, size_t len){
    char *p = *dst;

    if(save_wr) {
#ifdef WIIVC
        hmemcpy(p, src, len);
#else
        memcpy(p, src, len);
#endif
    }
    p += len;
    *dst = p;
}

static MAYBENOINLINE void st_read(void **src, void *dst, size_t len){
    char *p = *src;
#ifdef WIIVC
    hmemcpy(dst, p, len);
#else
    memcpy(dst, p, len);
#endif
    p += len;
    *src = p;
}

static void st_skip(void **dest, size_t len){
    char *p = *dest;
    p += len;
    *dest = p;
}

static _Bool addr_compare(void *a, void *b){
    uint32_t *a_32 = a;
    uint32_t *b_32 = b;
    return *a_32 < *b_32;
}

_Bool load_overlay(void **src, void **tab_addr, uint32_t vrom_start, uint32_t vrom_end, uint32_t vram_start, uint32_t vram_end){
    FORCE_CLEAN(load_overlay);
    z2_file_table_t *file = NULL;
    for(int i = 0;;i++){
        if(z2_file_table[i].vrom_start == vrom_start && z2_file_table[i].vrom_end == vrom_end){
            file = &z2_file_table[i];
            break;
        }
        if(z2_file_table[i].vrom_end == 0) return 0;
    }
    void *addr = NULL;
    st_read(src, &addr, sizeof(addr));
    if(*tab_addr != addr){
        *tab_addr = addr;
        z2_LoadOverlay(vrom_start, vrom_end, vram_start, vram_end, addr);
    }
    char *start = addr;
    char *end = start + (file->vrom_end - file->vrom_start);
    uint32_t *overlay_header_offset = (void*)(end - sizeof(*overlay_header_offset));
    if(overlay_header_offset == NULL){
        return 0;
    }

    z2_overlay_hdr_t *overlay_hdr = (void*)(end - *overlay_header_offset);
    char *data = start + overlay_hdr->text_size;
    char *bss = end;

    /* read data and bss segments */
    st_read(src, data, overlay_hdr->data_size);
    st_read(src, bss, overlay_hdr->bss_size);
    return 1;
}

void relocate_col_hdr(uint32_t hdr){
    z2_col_hdr_t *col_hdr = zu_segment_find(hdr);
    zu_segment_reloc(&col_hdr->vtx);
    zu_segment_reloc(&col_hdr->poly);
    zu_segment_reloc(&col_hdr->type);
    zu_segment_reloc(&col_hdr->camera);
    zu_segment_reloc(&col_hdr->water);
}

void load_state(void *state){
    FORCE_CLEAN(load_state);

    void *p = state;
    uint64_t save_time;
    uint64_t load_time;
    uint64_t offset;

    kz_state_hdr_t hdr;
    st_read(&p, &hdr, sizeof(hdr));

    load_time = osGetTime();

    st_read(&p, &save_time, sizeof(save_time));

    offset = load_time - save_time;

    z2_sfx_cmd_rd_pos = z2_sfx_cmd_wr_pos;
    z2_seq_cmd_rd_pos = z2_seq_cmd_wr_pos;
    z2_audio_ctxt.cmd_rd_pos = z2_audio_ctxt.cmd_wr_pos;
    z2_StopSfx();

    struct seq_info {
        _Bool active;
        uint16_t seq_idx;
        float volume;
        uint8_t vol_factor[4];
        uint16_t chan_mute;
    } seq_info[5];

    for(int i = 0; i < 5; i++) {
        struct seq_info *info = &seq_info[i];
        st_read(&p, &info->active, sizeof(info->active));
        st_read(&p, &info->seq_idx, sizeof(info->seq_idx));
        st_read(&p, &info->volume, sizeof(info->volume));
        st_read(&p, &info->vol_factor, sizeof(info->vol_factor));
        st_read(&p, &info->chan_mute, sizeof(info->chan_mute));
    }

    uint8_t cur_config = z2_afx_cfg;
    uint8_t saved_cfg;
    st_read(&p, &saved_cfg, sizeof(saved_cfg));
    if(cur_config != saved_cfg) {
        z2_afx_cfg = saved_cfg;
        z2_AfxConfig(saved_cfg);
        z2_AudioReset(cur_config);
        z2_AfxCmdWord(0xF8000000, 0x00000000);
    } else {
        for(int i = 0; i < 5; i++) {
            struct seq_info *info = &seq_info[i];
            z2_seq_ctl_t *ctl = &z2_seq_ctl[i];
            if (i == 2) {
                continue;
            }

            if(info->seq_idx == 0xFFFF || !info->active || info->seq_idx != ctl->seq_idx) {
                z2_AfxCmdWord(0x83000000 | (i << 16), 0x00000000);
            }
        }
        z2_AfxCmdFlush();
    }

    /* wait for rcp to finish task */
    osRecvMesg(&z2_ctxt.gfx->task_queue, NULL, OS_MESG_BLOCK);
    osSendMesg(&z2_ctxt.gfx->task_queue, NULL, OS_MESG_NOBLOCK);

    /* record current loaded data */
    int scene_index = z2_game.scene_index;
    _Bool is_cur_pause = z2_game.pause_ctx.state >= 3;

    struct alloc {
        uint16_t    id;
        void       *ptr;
    };

    struct alloc objects[35];
    for(int i = 0;i < 35;i++){
        z2_obj_t *obj = &z2_game.obj_ctx.obj[i];
        objects[i].id = obj->id;
        objects[i].ptr = obj->data;
    }

    struct alloc rooms[2];
    for(int i = 0;i < 2;i++){
        z2_room_t *room = &z2_game.room_ctx.rooms[i];
        rooms[i].id = room->idx;
        rooms[i].ptr = room->file;
    }

    struct set ovl_set;
    set_init(&ovl_set, sizeof(uint32_t), addr_compare);

    int next_ent;
    /* load actor overlays */
    int ovl_cnt = sizeof(z2_actor_ovl_table) / sizeof(*z2_actor_ovl_table);
    st_read(&p, &next_ent, sizeof(next_ent));
    for(int i = 0;i < ovl_cnt;i++){
        z2_actor_ovl_table_t *ent = &z2_actor_ovl_table[i];
        if(i == next_ent){
            st_read(&p, &ent->loaded_cnt, sizeof(ent->loaded_cnt));
            load_overlay(&p, &ent->ram, ent->vrom_start, ent->vrom_end, ent->vram_start, ent->vram_end);
            st_read(&p,&next_ent,sizeof(next_ent));
            set_insert(&ovl_set,&ent->ram);
        }else{
            ent->ram = NULL;
            ent->loaded_cnt = 0;
        }
    }

    /* load player/pause overlays */
    st_read(&p, &next_ent, sizeof(next_ent));
    ovl_cnt = sizeof(z2_player_ovl_table) / sizeof(*z2_player_ovl_table);
    for(int i = 0;i < ovl_cnt;i++){
        z2_player_ovl_table_t *ent = &z2_player_ovl_table[i];
        if(i == next_ent){
            load_overlay(&p, &ent->ram, ent->vrom_start, ent->vrom_end, ent->vram_start, ent->vram_end);
            ent->reloc_off = (int)ent->ram - (int)ent->vram_start;
            st_read(&p,&next_ent,sizeof(next_ent));
            set_insert(&ovl_set, &ent->ram);
            z2_player_ovl_cur = ent;
        }else{
            ent->ram = NULL;
            ent->reloc_off = 0;
        }
    }

    /* load particle overlays */
    st_read(&p, &next_ent, sizeof(next_ent));
    ovl_cnt = sizeof(z2_particle_ovl_table) / sizeof(*z2_particle_ovl_table);
    for(int i = 0;i < ovl_cnt;i++){
        z2_particle_ovl_table_t *ent = &z2_particle_ovl_table[i];
        if(i == next_ent){
            load_overlay(&p, &ent->ram, ent->vrom_start, ent->vrom_end, ent->vram_start, ent->vram_end);
            st_read(&p, &next_ent, sizeof(next_ent));
            set_insert(&ovl_set, &ent->ram);
        }else{
            ent->ram = NULL;
        }
    }

    /* load global context */
    st_read(&p, &z2_game, sizeof(z2_game));

    /* load file data */
    st_read(&p, &z2_file, sizeof(z2_file));

    st_read(&p, &z2_timer_change_val, sizeof(z2_timer_change_val));

    st_read(&p, &z2_timer_pause, sizeof(z2_timer_pause));

    z2_timer_change_val += offset;

    for(int i = 0; i < 6; i++) {
        if(z2_file.hsw_timer_active[i]) {
            z2_file.hsw_catch_start[i] += offset;
            //z2_file.hsw_offset[i] += offset;
        }
    }


    /* load static context */
    st_read(&p, z2_file.static_ctx, sizeof(*z2_file.static_ctx));

    /* load segment address table */
    st_read(&p, &z2_segment, sizeof(z2_segment));

    /* load arena nodes */
    st_read(&p, &z2_game_arena, sizeof(z2_game_arena));
    z2_arena_node_t *node = z2_game_arena.first;
    st_read(&p, &next_ent, sizeof(next_ent));
    while(node){
        node->magic = 0x7373;
        st_read(&p, &node->free, sizeof(node->free));
        st_read(&p, &node->size, sizeof(node->size));
        char *data = node->data;
        if(set_get(&ovl_set, &data) == NULL && node->free == 0){
            st_read(&p, data, node->size);
        }
        if(node == z2_game_arena.first){
            node->prev = NULL;
        }
        st_read(&p, &next_ent, sizeof(next_ent));
        if(next_ent == 0){
            node->next = (void*)&node->data[node->size];
            node->next->prev = node;
        }else{
            node->next = NULL;
        }
        node = node->next;
    }

    st_read(&p, &z2_light_queue, sizeof(z2_light_queue));

    /* load matrix stack */
    st_read(&p, &z2_mtx_stack, sizeof(z2_mtx_stack));
    st_read(&p, &z2_mtx_stack_top, sizeof(z2_mtx_stack_top));

    /* load dynamic particles */
    st_read(&p, &z2_particle_info, sizeof(z2_particle_info));
    st_read(&p, &next_ent, sizeof(next_ent));
    for(int i = 0;i < z2_particle_info.part_max;i++){
        z2_particle_t *particle = &z2_particle_info.part_space[i];
        /* particle space occupied */
        if(i == next_ent){
            st_read(&p, particle, sizeof(*particle));
            st_read(&p, &next_ent, sizeof(next_ent));
        /* particle space free */
        }else{
            memset(particle, 0, sizeof(*particle));
            particle->priority = 0x80;
            particle->id = 0x27;
            particle->time = -1;
        }
    }

    /* load static particles */
    z2_static_particle_ctxt_t *spart_ctx = &z2_static_particle_ctxt;

    /* load static dot particles */
    st_read(&p, &next_ent, sizeof(next_ent));
    int part_cnt = sizeof(spart_ctx->dots) / sizeof(*spart_ctx->dots);
    for(int i = 0;i < part_cnt;i++){
        z2_dot_t *dot = &spart_ctx->dots[i];
        /* position occupied */
        if(i == next_ent){
            st_read(&p, dot, sizeof(*dot));
            st_read(&p, &next_ent, sizeof(next_ent));
        /* position free */
        }else{
            dot->active = 0;
        }
    }

    /* load static trail particles */
    st_read(&p, &next_ent, sizeof(next_ent));
    part_cnt = sizeof(spart_ctx->trails) / sizeof(*spart_ctx->trails);
    for(int i = 0;i < part_cnt;i++){
        z2_trail_t *trail = &spart_ctx->trails[i];
        if(i == next_ent){
            st_read(&p, trail, sizeof(*trail));
            st_read(&p, &next_ent, sizeof(next_ent));
        }else{
            trail->active = 0;
        }
    }

    /* load static spark particles */
    st_read(&p, &next_ent, sizeof(next_ent));
    part_cnt = sizeof(spart_ctx->sparks) / sizeof(*spart_ctx->sparks);
    for(int i = 0;i < part_cnt;i++){
        z2_spark_t *spark = &spart_ctx->sparks[i];
        if(i == next_ent){
            st_read(&p, spark, sizeof(*spark));
            st_read(&p, &next_ent, sizeof(next_ent));
        }else{
            spark->active = 0;
        }
    }

    /* load static unknown particles */
    st_read(&p,&next_ent,sizeof(next_ent));
    part_cnt = sizeof(spart_ctx->unks) / sizeof(*spart_ctx->unks);
    for(int i = 0;i < part_cnt;i++){
        z2_unk_part_t *unk = &spart_ctx->unks[i];
        if(i == next_ent){
            st_read(&p, unk, sizeof(*unk));
            st_read(&p, &next_ent, sizeof(next_ent));
        }else{
            unk->active = 0;
        }
    }

    st_read(&p, z2_extern_scene_table, sizeof(z2_extern_scene_table));
    for(int i = 0; i < sizeof(z2_extern_scene_table) / sizeof(*z2_extern_scene_table); i++) {
        z2_extern_ent_tab_t *extern_ent = &z2_extern_scene_table[i];
        st_read(&p, extern_ent->entrances, sizeof(*extern_ent->entrances) * extern_ent->ent_cnt);
        for(int j = 0; j < extern_ent->ent_cnt; j++) {
            z2_entrance_rec_t *ent_rec = extern_ent->entrances[j];
            st_read(&p, ent_rec, sizeof(*ent_rec));
        }
    }

    /* load cutscene state */
    st_read(&p, &z2_cutscene_state, 0x16);

    st_read(&p, z2_cs_info, 0xCC);

    st_read(&p, z2_hud_state, 12);

    st_read(&p, &z2_cs_bars, sizeof(z2_cs_bars));

    st_read(&p, &z2_framerate_div, sizeof(z2_framerate_div));

    st_read(&p, &z2_random, sizeof(z2_random));

    st_read(&p, z2_quake_requests, sizeof(z2_quake_requests));

    st_read(&p, &z2_quake_request_cnt, sizeof(z2_quake_request_cnt));

    if(hdr.z2_version >= 8){
        st_read(&p, &z2_camera_wobble_effect, sizeof(z2_camera_wobble_effect));
    }

    {
        // Load Minimap Details
        st_read(&p, &z2_minimap_ctx, sizeof(z2_minimap_ctx));

        st_read(&p, *z2_minimap_ctx.room_info, sizeof(**z2_minimap_ctx.room_info) * z2_game.room_cnt);

        z2_minimap_room_info_t *room_info = &(*z2_minimap_ctx.room_info)[z2_minimap_ctx.room];

        if(room_info->map != 0xFFFF) {
            uint32_t tile_size = z2_get_mmap_tile_size(room_info->map);
            if(tile_size != 0) {
                z2_LoadArchiveFile2(z2_file_table[z2_minimap_assets].vrom_start, room_info->map - 0x100, z2_minimap_ctx.cur_tex, tile_size);
            }
        }

        if(z2_minimap_ctx.trnsn_room != -1) {
            room_info = &(*z2_minimap_ctx.room_info)[z2_minimap_ctx.trnsn_room];
            if(room_info->map != 0xFFFF) {
               uint32_t tile_size = z2_get_mmap_tile_size(room_info->map);
                if(tile_size != 0) {
                    z2_LoadArchiveFile2(z2_file_table[z2_minimap_assets].vrom_start, room_info->map - 0x100, z2_minimap_ctx.prev_tex, tile_size);
                }
            }
        }

        z2_minimap_room_cnt = z2_game.room_cnt;
    }

    /* load scene file, and create static collision if loading from a different scene */
    if(scene_index != z2_game.scene_index){
        z2_scene_table_ent_t *scene_ent = &z2_scene_table[z2_game.scene_index];
        size_t size = scene_ent->vrom_end - scene_ent->vrom_start;
        zu_file_load(scene_ent->vrom_start,z2_game.scene_addr,size);
        relocate_col_hdr((uint32_t)z2_game.col_ctxt.col_hdr);

        /* load area textures */
        z2_scene_cmd_t *cmd = z2_game.scene_addr;
        while(cmd->cmd != 0x14){
            if(cmd->cmd == 0x18 && z2_file.scene_setup > 0){
                z2_alternate_scene_cmd_t *alt_cmd = (z2_alternate_scene_cmd_t*)cmd;
                uint32_t *alt_hdrs = zu_segment_find(alt_cmd->addr);
                cmd = zu_segment_find(alt_hdrs[z2_file.scene_setup - 1]);
            }
            if(cmd->cmd == 0x11){
                z2_sky_scene_cmd_t *sky_cmd = (z2_sky_scene_cmd_t*)cmd;
                if(sky_cmd->tex_idx == 0){
                    break;
                }
                z2_rom_file_t *file = &z2_area_tex_table[sky_cmd->tex_idx];
                zu_file_load(file->vrom_start, z2_game.room_ctx.area_map_tex, file->vrom_end - file->vrom_start);
            }
            cmd++;
        }
        z2_game.col_ctxt.stc_list_pos = 0;
        z2_CreateStaticCollision(&z2_game.col_ctxt,&z2_game,z2_game.col_ctxt.stc_lut);
    }

    /* load transition actors */
    int trans_cnt = z2_game.room_ctx.transition_cnt;
    z2_room_trans_actor_t *trans = z2_game.room_ctx.transition_list;
    st_read(&p,trans,sizeof(*trans) * trans_cnt);

    /* load room files if not loaded */
    for(int i = 0;i < 2;i++){
        struct alloc *room = &rooms[i];
        z2_room_t *c_room = &z2_game.room_ctx.rooms[i];
        int p_id = room->id;
        int c_id = c_room->idx;
        void *p_ptr = room->ptr;
        void *c_ptr = c_room->file;
        if(c_ptr && c_id != -1 && (p_id != c_id || z2_game.scene_index != scene_index || p_ptr != c_ptr)){
            uint32_t start = z2_game.room_list[c_id].vrom_start;
            uint32_t end = z2_game.room_list[c_id].vrom_end;
            zu_file_load(start, c_ptr, end - start);
        }
    }

    /* load the room file */
    osSendMesg(&z2_file_msgqueue, &z2_game.room_ctx.loadfile, OS_MESG_NOBLOCK);

    // Load start button text.
    zu_file_load(z2_file_table[z2_do_action_static].vrom_start + 0x480, z2_game.hud_ctx.do_action_static + 0x300, 0x180);

    if(z2_game.pause_ctx.state != 0) {
        /* we need to load pause screen assets */
#if Z2_VERSION==NZSE
        z2_LoadArchiveFile(z2_file_table[z2_item_icon_archive].vrom_start, z2_game.pause_ctx.icon_item_static, 0);
        z2_LoadArchiveFile(z2_file_table[z2_icon_item_24_static].vrom_start, z2_game.pause_ctx.icon_item_24, 0);
        uint32_t rom = z2_game.pause_ctx.screen_idx == 1 ? z2_location_names : z2_item_names;
        z2_LoadArchiveFile2(z2_file_table[rom].vrom_start, z2_game.pause_ctx.selected_item, z2_game.pause_ctx.icon_name_text, 0x400);
        zu_file_idx_load(z2_pause_assets, z2_game.pause_ctx.bg_dlist);
#else
        zu_file_idx_load(z2_icon_item_static, z2_game.pause_ctx.icon_item_static);
        zu_file_idx_load(z2_icon_item_24_static, z2_game.pause_ctx.icon_item_24);
        z2_DecodeArchiveFile(z2_file_table[z2_item_names].prom_start, z2_game.pause_ctx.selected_item, z2_game.pause_ctx.icon_name_text, 0x400);
#endif

        zu_file_load(z2_file_table[z2_do_action_static].vrom_start + 0x480, z2_game.hud_ctx.do_action_static + 0x480, 0x180);
        zu_file_load(z2_file_table[z2_do_action_static].vrom_start + 0x480, z2_game.hud_ctx.do_action_static + 0x300, 0x180);
        if(z2_get_map_type(&z2_game) == 0) {
            zu_file_idx_load(z2_icon_item_field_static, z2_game.pause_ctx.icon_item_map);
        } else {
            zu_file_idx_load(11, z2_game.pause_ctx.icon_item_map);
            z2_load_pause_map(&z2_game, z2_game.pause_ctx.icon_item_map + 0x4BB0);
        }

        zu_file_idx_load(z2_map_name_static, z2_game.pause_ctx.icon_text);


        int rest_offset = z2_file.current_form * 0x72;
        for(int i = 0; i < Z2_END_MASK; i++){
            if(!z2_restriction_table[rest_offset + i]){
                gfx_texture_desaturate(zu_segment_find(z2_icon_item_static_table[i]), 0x1000);
            }
        }

    } else {
        if((z2_prerender_bgtask.flags & 2) && !(z2_prerender_bgtask.flags & 1)) {
            z2_BgTaskStop(&z2_prerender_bgtask);
        }
        /* load objects if they are not currently loaded in the same dram address */
        for(int i = 0; i < 35; i++) {
            struct alloc *p_obj = &objects[i];
            z2_obj_t *c_obj = &z2_game.obj_ctx.obj[i];
            int p_id = p_obj->id;
            int c_id = c_obj->id;
            if(c_id < 0){
                c_id = -c_id;
            }
            void *p_ptr = p_obj->ptr;
            void *c_ptr = c_obj->data;
            if(c_id != 0 && ((c_id != p_id || c_ptr != p_ptr) || is_cur_pause)){
                uint32_t start = z2_obj_table[c_id].vrom_start;
                uint32_t end = z2_obj_table[c_id].vrom_end;
                zu_file_load(start, c_ptr, end - start);
            }
        }

        // Relocate dynamic collision.
        for(int i = 0; i < sizeof(z2_game.col_ctxt.actors) / sizeof(*z2_game.col_ctxt.actors);i++){
            z2_col_chk_actor_t *col = &z2_game.col_ctxt.actors[i];
            if(col->actor == NULL){
                continue;
            }

            z2_obj_t *obj = &z2_game.obj_ctx.obj[col->actor->alloc_index];
            z2_segment.current_object = MIPS_KSEG0_TO_PHYS(obj->data);
            relocate_col_hdr((uint32_t)col->col_hdr);
        }
    }

    set_destroy(&ovl_set);

    /* load water boxes */
    z2_col_hdr_t *col_hdr = z2_game.col_ctxt.col_hdr;
    st_read(&p, &col_hdr->n_water, sizeof(col_hdr->n_water));
    st_read(&p, &col_hdr->water, sizeof(*col_hdr->water) * col_hdr->n_water);

    /* load dynamic collision */
    z2_col_ctxt_t *col = &z2_game.col_ctxt;
    st_read(&p, col->dyn.list, sizeof(*col->dyn.list) * col->dyn_list_max);
    st_read(&p, col->dyn_poly, sizeof(*col->dyn_poly) * col->dyn_poly_max);
    st_read(&p, col->dyn_vtx, sizeof(*col->dyn_vtx) * col->dyn_vtx_max);

    st_read(&p, z2_saturation, 0x20);
    st_read(&p, &z2_hide_clock, sizeof(z2_hide_clock));

    /* create skybox */
    if(z2_game.skybox_type != 0){
        if(z2_game.skybox_type == 5){
            z2_CreateSkyboxVtx(&z2_game.skybox_ctx, 6);
        }else{
            z2_CreateSkyboxVtx(&z2_game.skybox_ctx, 5);
        }

        z2_LoadSkyboxData(&z2_game, &z2_game.skybox_ctx, z2_game.skybox_type);
    }

    /* load hud textures */
    zu_file_idx_load(z2_parameter_static, z2_game.hud_ctx.parameter_static);

    /* update equip buttons */
    if(z2_file.form_button_item[z2_file.current_form == 4 ? 0 : z2_file.current_form].b != Z2_ITEM_NULL){
        z2_btnupdate(&z2_game, 0);
    }
    for(int i = 1;i < 4;i++){
        if(z2_file.form_button_item[0].button_item[i] != Z2_ITEM_NULL){
            z2_btnupdate(&z2_game, i);
        }
    }

    /* load labels for b and a buttons */
    z2_ActionLabelUpdate(&z2_game.hud_ctx, z2_game.hud_ctx.action, 0);
    z2_ActionLabelUpdate(&z2_game.hud_ctx, z2_game.hud_ctx.action, 1);

    /* load current day clock texture */
    uint32_t vrom = z2_file_table[z2_clock_face_days].vrom_start + (z2_file.day == 0 ? 0 : z2_file.day - 1) * 0x510;
    void *ram = (char*)z2_game.hud_ctx.do_action_static + 0x780;
    zu_file_load(vrom, ram, 0x510);

    /* load message box textures */
    zu_file_load(z2_file_table[z2_message_static].vrom_start + z2_message_static_offsets[z2_game.message_bg_type] * 0x1000, z2_game.message_bg_tex, 0x1000);

    /* load display lists */
    z2_gfx_t *gfx = z2_ctxt.gfx;
    zu_disp_ptr_t disp_ptr;
    st_read(&p, &disp_ptr, sizeof(disp_ptr));
    zu_disp_ptr_load(&disp_ptr);
    z2_disp_buf_t *z2_dlist[5] = {
        &gfx->work,
        &gfx->poly_opa,
        &gfx->poly_xlu,
        &gfx->overlay,
        &gfx->unk_0x1B8_buf,
    };
    for(int i = 0;i < 5;i++){
        z2_disp_buf_t *dlist = z2_dlist[i];
        size_t len = sizeof(Gfx);
        Gfx *end = dlist->buf + dlist->size / len;
        st_read(&p, dlist->buf, (dlist->p - dlist->buf) * len);
        st_read(&p, dlist->d, (end - dlist->d) * len);
    }


    void *seg_setup = &z2_disp[((gfx->frame_cnt_1 & 1) * Z2_DISP_SIZE) + 0x148];
    st_read(&p, seg_setup, 0x88);

    uint32_t frame_cnt_1;
    uint8_t frame_cnt_2;
    st_read(&p, &frame_cnt_1, sizeof(frame_cnt_1));
    st_read(&p, &frame_cnt_2, sizeof(frame_cnt_2));
    zu_gfx_reloc(frame_cnt_1 & 1, frame_cnt_2 & 1);

    for(int i = 0; i < 5; i++) {
        struct seq_info *info = &seq_info[i];
        z2_seq_ctl_t *ctl = &z2_seq_ctl[i];
        z2_sequencer_t *sequencer = &z2_audio_ctxt.sequencers[i];
        if(!(sequencer->enabled)) {
            ctl->unk_21B = 1;
        }
        memcpy(&ctl->vol_factor, &info->vol_factor, sizeof(info->vol_factor));
        ctl->vol_cur = info->volume;
        ctl->timer = 0;
        ctl->prev_seq_idx = ctl->seq_idx = info->seq_idx;

        if(i == 4 && ctl->seq_idx == 0x0001) {
            z2_night_sfx_t *night_sfx = &z2_night_sfx[z2_game.night_seq_idx];

            z2_AfxCmdByte(0x46040000, 1);
            z2_AfxCmdByte(0x46040004, ((night_sfx->unk_0x00 >> 8) & 0xFF));
            z2_AfxCmdByte(0x46040005, (night_sfx->unk_0x00 & 0xFF));

            afx_start_seq(4, 1);

            for(int j = 0; j < 16; j++) {
                if(!(night_sfx->unk_0x02 & (1 << j)) && (night_sfx->unk_0x00 & (1 << j))) {
                    z2_AfxCmdByte(0x06040000 | (j << 8) | 1, 1);
                }
            }

            for(int j = 0; j < 0x64; j += 3) {
                uint8_t chan = night_sfx->unk_0x04[j + 0];
                uint8_t param = night_sfx->unk_0x04[j + 1];
                uint8_t val = night_sfx->unk_0x04[j + 2];

                if(chan == 0xFF) {
                    break;
                }

                z2_AfxCmdByte(0x06040000 | (chan << 8) | (param << 0), val);
            }

            z2_AfxCmdByte(0x06040D07, 0);

            switch(z2_game.night_sfx_state) {
                case 3:
                case 4:
                    // channel 1 on.
                    z2_AfxCmdByte(0x06040101, 1);
                    break;
                case 5:
                case 6:
                    // channel 1 off.  channels 2, 3, and 4 on.
                    z2_AfxCmdByte(0x06040101, 0);
                    z2_AfxCmdByte(0x06040201, 1);
                    z2_AfxCmdByte(0x06040301, 1);
                    z2_AfxCmdByte(0x06040401, 1);
                    break;
                case 7:
                case 8:
                    // channels 2, 3, and 4 off, channels 5 and 6 on
                    z2_AfxCmdByte(0x06040201, 0);
                    z2_AfxCmdByte(0x06040301, 0);
                    z2_AfxCmdByte(0x06040401, 0);
                    z2_AfxCmdByte(0x06040501, 1);
                    z2_AfxCmdByte(0x06040601, 1);
                    break;
            }

            if(z2_game.rain_timer1 > 0 || z2_game.rain_timer2 > 0) {
                // channels 14 and 15 on.
                z2_AfxCmdByte(0x06040E01, 1);
                z2_AfxCmdByte(0x06040F01, 1);
            }

        } else if (info->seq_idx != 0xFFFF && info->active && !(sequencer->enabled)) {
            afx_start_seq(i, info->seq_idx);
        }

        z2_AfxCmdFloat(0x41000000, ctl->vol_cur);

        for(int j = 0; j < 16; j++) {
            _Bool is_mute = info->chan_mute & (1 << j);
            ctl->unk_0x00[j].unk_0x00 = 1.0f;
            ctl->unk_0x00[j].unk_0x18 = 0;

            z2_AfxCmdFloat(0x01000000 | (i << 16) | (j << 8), 1.0f);
            z2_AfxCmdWord(0x08000000 | (i << 16) | (j << 8), is_mute << 24);
        }
    }

    z2_AfxCmdWord(0xF2000000, 0x00000000);
    z2_AfxCmdFlush();

    uint8_t cmd_cnt;
    st_read(&p, &cmd_cnt, sizeof(cmd_cnt));
    for(uint8_t i = 0; i != cmd_cnt; i++) {
        st_read(&p, &z2_seq_cmd_buf[z2_seq_cmd_wr_pos++], sizeof(*z2_seq_cmd_buf));
    }

    st_read(&p, z2_song_state, 0xAC);
    st_read(&p, &z2_oca_note_pos, sizeof(z2_oca_note_pos));
    st_read(&p, &z2_stored_song, sizeof(z2_stored_song));
    st_read(&p, z2_oca_state, 0x74);
    st_read(&p, z2_staff_notes, 0x1E);
    st_read(&p, &z2_song_ptr, sizeof(z2_song_ptr));
    st_read(&p, &z2_unk_song_pos, sizeof(z2_unk_song_pos));

    uint32_t diff = z2_song_ctr - z2_oca_unk_ctr;
    z2_song_ctr = z2_audio_ctxt.unk_ctr;
    z2_oca_unk_ctr = z2_song_ctr - diff;
}

_Bool save_overlay(void **dst, void *src, uint32_t vrom_start, uint32_t vrom_end){
    FORCE_CLEAN(save_overlay);
    z2_file_table_t *file = NULL;
    for(int i = 0;;i++){
        if(z2_file_table[i].vrom_start == vrom_start && z2_file_table[i].vrom_end == vrom_end){
            file = &z2_file_table[i];
            break;
        }
        if(z2_file_table[i].vrom_end == 0) {
            return 0;
        }
    }
    st_write(dst, &src, sizeof(src));
    char *start = src;
    char *end = start + (file->vrom_end - file->vrom_start);
    uint32_t *overlay_header_offset = (void*)(end - sizeof(*overlay_header_offset));
    if(overlay_header_offset == NULL){
        return 0;
    }

    /* save data and bss segments */
    z2_overlay_hdr_t *overlay_hdr = (void*)(end - *overlay_header_offset);
    char *data = start + overlay_hdr->text_size;
    char *bss = end;
    st_write(dst, data, overlay_hdr->data_size);
    st_write(dst, bss, overlay_hdr->bss_size);
    return 1;
}

size_t save_state(void *state){
    FORCE_CLEAN(save_state);
    void *p = state;

    save_wr = (p != NULL);

    /* indicators for table entries */
    int ent_start = 0;
    int ent_end = -1;

    st_skip(&p, sizeof(kz_state_hdr_t));

    uint64_t cur_time = osGetTime();

    st_write(&p, &cur_time, sizeof(cur_time));

    for(int i = 0; i < 5; i++) {
        z2_sequencer_t *sequencer = &z2_audio_ctxt.sequencers[i];
        z2_seq_ctl_t *seq_ctl = &z2_seq_ctl[i];
        _Bool seq_active = sequencer->enabled || z2_afx_cfg_state;
        st_write(&p, &seq_active, sizeof(seq_active));
        st_write(&p, &seq_ctl->seq_idx, sizeof(seq_ctl->seq_idx));
        if(seq_ctl->timer != 0) {
            st_write(&p, &seq_ctl->vol_target, sizeof(seq_ctl->vol_target));
        } else {
            st_write(&p, &seq_ctl->vol_cur, sizeof(&seq_ctl->vol_cur));
        }

        st_write(&p, &seq_ctl->vol_factor, sizeof(seq_ctl->vol_factor));

        uint16_t chan_mutes = 0;
        for(int j = 0; j < 16; j++) {
            _Bool is_mute = sequencer->channels[j]->flags & 0x10000000;

            chan_mutes |= (is_mute << j);
        }

        st_write(&p, &chan_mutes, sizeof(chan_mutes));
    }

    st_write(&p, &z2_afx_cfg, sizeof(z2_afx_cfg));

    struct set node_set;
    set_init(&node_set, sizeof(uint32_t), addr_compare);

    /* save actor overlay information */
    int ovl_cnt = sizeof(z2_actor_ovl_table) / sizeof(*z2_actor_ovl_table);
    for(int i = 0;i < ovl_cnt;i++){
        z2_actor_ovl_table_t *ovl = &z2_actor_ovl_table[i];
        if(ovl->ram){
            st_write(&p, &i, sizeof(i));
            st_write(&p, &ovl->loaded_cnt, sizeof(ovl->loaded_cnt));
            save_overlay(&p, ovl->ram, ovl->vrom_start, ovl->vrom_end);
            set_insert(&node_set, &ovl->ram);
        }
    }
    st_write(&p,&ent_end,sizeof(ent_end));

    /* save player/pause overlay information */
    ovl_cnt = sizeof(z2_player_ovl_table) / sizeof(*z2_player_ovl_table);
    for(int i = 0;i < ovl_cnt;i++){
        z2_player_ovl_table_t *ovl = &z2_player_ovl_table[i];
        if(ovl->ram){
            st_write(&p, &i, sizeof(i));
            save_overlay(&p, ovl->ram, ovl->vrom_start, ovl->vrom_end);
            set_insert(&node_set, &ovl->ram);
        }
    }
    st_write(&p,&ent_end,sizeof(ent_end));

    /* save loaded particle overlay information */
    ovl_cnt = sizeof(z2_particle_ovl_table) / sizeof(*z2_particle_ovl_table);
    for(int i = 0;i < ovl_cnt;i++){
        z2_particle_ovl_table_t *ovl = &z2_particle_ovl_table[i];
        if(ovl->ram){
            st_write(&p, &i, sizeof(i));
            save_overlay(&p, ovl->ram, ovl->vrom_start, ovl->vrom_end);
            set_insert(&node_set, &ovl->ram);
        }
    }
    st_write(&p, &ent_end, sizeof(ent_end));

    /* save global context */
    st_write(&p, &z2_game, sizeof(z2_game));

    /* save currently loaded file data */
    st_write(&p, &z2_file, sizeof(z2_file));

    st_write(&p, &z2_timer_change_val, sizeof(z2_timer_change_val));

    st_write(&p, &z2_timer_pause, sizeof(z2_timer_pause));

    /* save static context */
    st_write(&p, z2_file.static_ctx, sizeof(*z2_file.static_ctx));

    /* save segment address table */
    st_write(&p, &z2_segment, sizeof(z2_segment));

    /* save game area nodes */
    st_write(&p, &z2_game_arena, sizeof(z2_game_arena));
    for(z2_arena_node_t *node = z2_game_arena.first;node;node = node->next){
        st_write(&p, &ent_start, sizeof(ent_start));
        st_write(&p, &node->free, sizeof(node->free));
        st_write(&p, &node->size, sizeof(node->size));
        char *data = node->data;
        if(set_get(&node_set, &data) == NULL && node->free == 0){
            st_write(&p, data, node->size);
        }
    }
    st_write(&p, &ent_end, sizeof(ent_end));

    set_destroy(&node_set);

    st_write(&p, &z2_light_queue, sizeof(z2_light_queue));

    /* save matrix stack */
    st_write(&p, &z2_mtx_stack, sizeof(z2_mtx_stack));
    st_write(&p, &z2_mtx_stack_top, sizeof(z2_mtx_stack_top));

    /* save dynamic particles */
    st_write(&p, &z2_particle_info, sizeof(z2_particle_info));
    for(int i = 0;i < z2_particle_info.part_max;i++){
        z2_particle_t *particle = &z2_particle_info.part_space[i];
        if(particle->time >= 0){
            st_write(&p, &i, sizeof(i));
            st_write(&p, particle, sizeof(*particle));
        }
    }
    st_write(&p,&ent_end,sizeof(ent_end));

    /* save static particles */
    z2_static_particle_ctxt_t *spart_ctx = &z2_static_particle_ctxt;

    /* save static dot particles */
    int part_cnt = sizeof(spart_ctx->dots)/sizeof(*spart_ctx->dots);
    for(int i = 0;i < part_cnt;i++){
        z2_dot_t *dot = &spart_ctx->dots[i];
        if(dot->active){
            st_write(&p, &i, sizeof(i));
            st_write(&p, dot, sizeof(*dot));
        }
    }
    st_write(&p, &ent_end, sizeof(ent_end));

    /* save static trail particles */
    part_cnt = sizeof(spart_ctx->trails)/sizeof(*spart_ctx->trails);
    for(int i = 0;i < part_cnt;i++){
        z2_trail_t *trail = &spart_ctx->trails[i];
        if(trail->active){
            st_write(&p, &i, sizeof(i));
            st_write(&p, trail, sizeof(*trail));
        }
    }
    st_write(&p, &ent_end, sizeof(ent_end));

    /* save static spark particles */
    part_cnt = sizeof(spart_ctx->sparks)/sizeof(*spart_ctx->sparks);
    for(int i = 0;i < part_cnt;i++){
        z2_spark_t *spark = &spart_ctx->sparks[i];
        if(spark->active){
            st_write(&p, &i, sizeof(i));
            st_write(&p, spark, sizeof(*spark));
        }
    }
    st_write(&p, &ent_end, sizeof(ent_end));

    /* save static unknown particles */
    part_cnt = sizeof(spart_ctx->unks) / sizeof(*spart_ctx->unks);
    for(int i = 0;i < part_cnt;i++){
        z2_unk_part_t *unk = &spart_ctx->unks[i];
        if(unk->active){
            st_write(&p, &i,sizeof(i));
            st_write(&p, unk, sizeof(*unk));
        }
    }
    st_write(&p, &ent_end, sizeof(ent_end));

    st_write(&p, z2_extern_scene_table, sizeof(z2_extern_scene_table));
    for(int i = 0; i < sizeof(z2_extern_scene_table) / sizeof(*z2_extern_scene_table); i++) {
        z2_extern_ent_tab_t *extern_ent = &z2_extern_scene_table[i];
        st_write(&p, extern_ent->entrances, sizeof(*extern_ent->entrances) * extern_ent->ent_cnt);
        for(int j = 0; j < extern_ent->ent_cnt; j++) {
            z2_entrance_rec_t *ent_rec = extern_ent->entrances[j];
            st_write(&p, ent_rec, sizeof(*ent_rec));
        }
    }

    /* save current cutscene state */
    st_write(&p, &z2_cutscene_state, 0x16);

    st_write(&p, z2_cs_info, 0xCC);

    st_write(&p, z2_hud_state, 12);

    st_write(&p, &z2_cs_bars, sizeof(z2_cs_bars));

    st_write(&p, &z2_framerate_div, sizeof(z2_framerate_div));

    st_write(&p, &z2_random, sizeof(z2_random));

    st_write(&p, z2_quake_requests, sizeof(z2_quake_requests));

    st_write(&p, &z2_quake_request_cnt, sizeof(z2_quake_request_cnt));

    st_write(&p, z2_camera_wobble_effect, sizeof(z2_camera_wobble_effect));

    st_write(&p, &z2_minimap_ctx, sizeof(z2_minimap_ctx));

    st_write(&p, *z2_minimap_ctx.room_info, sizeof(**z2_minimap_ctx.room_info) * z2_game.room_cnt);

    /* save room transition actors */
    int trans_cnt = z2_game.room_ctx.transition_cnt;
    z2_room_trans_actor_t *trans = z2_game.room_ctx.transition_list;
    st_write(&p,trans,sizeof(*trans) * trans_cnt);

    z2_col_ctxt_t *dynamic_col = &z2_game.col_ctxt;
    /* save water boxes */
    z2_col_hdr_t *col_hdr = dynamic_col->col_hdr;
    st_write(&p, &col_hdr->n_water, sizeof(col_hdr->n_water));
    st_write(&p, &col_hdr->water, sizeof(*col_hdr->water) * col_hdr->n_water);

    /* save dynamic collision */
    st_write(&p, dynamic_col->dyn.list, sizeof(*dynamic_col->dyn.list) * dynamic_col->dyn_list_max);
    st_write(&p, dynamic_col->dyn_poly, sizeof(*dynamic_col->dyn_poly) * dynamic_col->dyn_poly_max);
    st_write(&p, dynamic_col->dyn_vtx, sizeof(*dynamic_col->dyn_vtx) * dynamic_col->dyn_vtx_max);

    st_write(&p, z2_saturation, 0x20);
    st_write(&p, &z2_hide_clock, sizeof(z2_hide_clock));

    /* save display lists */
    z2_gfx_t *gfx = z2_ctxt.gfx;

    zu_disp_ptr_t disp_ptr;
    zu_disp_ptr_save(&disp_ptr);
    st_write(&p, &disp_ptr, sizeof(disp_ptr));
    z2_disp_buf_t *z2_dlist[5] = {
        &gfx->work,
        &gfx->poly_opa,
        &gfx->poly_xlu,
        &gfx->overlay,
        &gfx->unk_0x1B8_buf,
    };
    for(int i = 0;i < 5;i++){
        z2_disp_buf_t *dlist = z2_dlist[i];
        size_t len = sizeof(Gfx);
        Gfx *end = dlist->buf + dlist->size / len;
        st_write(&p, dlist->buf, (dlist->p - dlist->buf) * len);
        st_write(&p, dlist->d, (end - dlist->d) * len);
    }

    void *seg_setup = &z2_disp[((gfx->frame_cnt_1 & 1) * Z2_DISP_SIZE) + 0x148];
    st_write(&p, seg_setup, 0x88);

    st_write(&p, &gfx->frame_cnt_1, sizeof(gfx->frame_cnt_1));
    st_write(&p, &gfx->frame_cnt_2, sizeof(gfx->frame_cnt_2));

    uint8_t cmd_cnt = z2_seq_cmd_wr_pos - z2_seq_cmd_rd_pos;
    st_write(&p, &cmd_cnt, sizeof(cmd_cnt));
    for(uint8_t i = z2_seq_cmd_rd_pos; i != z2_seq_cmd_wr_pos; i++) {
        st_write(&p, &z2_seq_cmd_buf[i], sizeof(*z2_seq_cmd_buf));
    }

    st_write(&p, z2_song_state, 0xAC);
    st_write(&p, &z2_oca_note_pos, sizeof(z2_oca_note_pos));
    st_write(&p, &z2_stored_song, sizeof(z2_stored_song));
    st_write(&p, z2_oca_state, 0x74);
    st_write(&p, z2_staff_notes, 0x1E);
    st_write(&p, &z2_song_ptr, sizeof(z2_song_ptr));
    st_write(&p, &z2_unk_song_pos, sizeof(z2_unk_song_pos));
    return (char*)p - (char*)state;
}

int state_is_valid(kz_state_hdr_t *state) {
    if(state->z2_version != Z2_VERSION) {
        return -1;
    }

    if(state->settings_version == STATE_VERSION){
        return 0;
    }

    for(int i = 0; i < sizeof(compatabilities) / sizeof(*compatabilities);i++){
        struct state_compat *compat = &compatabilities[i];

        if(!compat->state_version == STATE_VERSION){
            continue;
        }

        for(int j = 0; j < sizeof(compat->compat_versions) / sizeof(*compat->compat_versions); j++){
            if(compat->compat_versions[j] == -1){
                return -2;
            }

            if(compat->compat_versions[j] == state->settings_version){
                return 0;
            }
        }
    }

    return 0;
}

struct state_compat {
    uint16_t    state_version;
    int16_t     compat_versions[10];
};

static struct state_compat compatabilities[] = {
    {
        .state_version = 8,
        .compat_versions = {
            7, -1
        }
    }
};

#endif
