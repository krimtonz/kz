#include <stdlib.h>
#include <stdio.h>
#include "commands.h"
#include "kz.h"
#include "settings.h"
#include "z2.h"
#include "state.h"
#include "hb_heap.h"

struct command kz_commands[KZ_CMD_MAX] = {
    {"toggle menu",         COMMAND_PRESS,  NULL},
    {"return",              COMMAND_PRESS,  NULL },
    {"levitate",            COMMAND_HOLD,   command_levitate},
    {"turbo",               COMMAND_PRESS,  command_turbo},
    {"fall",                COMMAND_HOLD,   command_fall},
    {"reload scene",        COMMAND_PRESS,  command_reloadarea},
    {"void out",            COMMAND_PRESS,  command_void},
    {"break free",          COMMAND_PRESS,  command_break},
    {"skip cutscene",       COMMAND_PRESS,  command_skip_cutscene},
    {"pause",               COMMAND_PRESS,  command_pause},
    {"advance",             COMMAND_PRESS,  command_advance},
    {"reset lag counter",   COMMAND_PRESS,  command_lag_reset},
    {"start/stop timer",    COMMAND_PRESS,  command_timer},
    {"reset timer",         COMMAND_PRESS,  command_timer_reset},
    {"title screen",        COMMAND_PRESS,  command_title_screen},
#ifndef LITE
    {"save state",          COMMAND_PRESS,  command_save_state},
    {"load state",          COMMAND_PRESS,  command_load_state},
    {"prev state",          COMMAND_PRESS,  command_prev_state},
    {"next state",          COMMAND_PRESS,  command_next_state},
#endif
    {"save memfile",        COMMAND_PRESS,  command_save_memfile},
    {"load memfile",        COMMAND_PRESS,  command_load_memfile},
    {"prev memfile",        COMMAND_PRESS,  command_prev_memfile},
    {"next memfile",        COMMAND_PRESS,  command_next_memfile},
    {"save position",       COMMAND_PRESS,  command_save_position},
    {"load position",       COMMAND_PRESS,  command_load_position},
    {"prev position",       COMMAND_PRESS,  command_prev_position},
    {"next position",       COMMAND_PRESS,  command_next_position},
};

#ifndef LITE
void command_prev_state(void) {
    kz.state_slot += STATE_MAX - 1;
    kz.state_slot %= STATE_MAX;
    kz_state_hdr_t *state = kz.states[kz.state_slot];
    if(state != NULL) {
#ifdef  WIIVC
        char name[64];
        hmemcpy(name, state->name, 64);
#else
        char *name = state->name;
#endif
        kz_log("select state slot %d (%s)", kz.state_slot, name);
    } else {
        kz_log("select state slot %d (no state)", kz.state_slot);
    }
}

void command_next_state(void) {
    kz.state_slot++;
    kz.state_slot %= STATE_MAX;
    kz_state_hdr_t *state = kz.states[kz.state_slot];
    if(state != NULL) {
#ifdef  WIIVC
        char name[64];
        hmemcpy(name, state->name, 64);
#else
        char *name = state->name;
#endif
        kz_log("select state slot %d (%s)", kz.state_slot, name);
    } else {
        kz_log("select state slot %d (no state)", kz.state_slot);
    }
}

void command_load_state(){
    if(!zu_is_ingame()) {
        kz_log("cannot load state here");
        return;
    }

    kz_state_hdr_t *state = kz.states[kz.state_slot];
    if(state != NULL) {
        load_state(state);
#ifdef WIIVC
        char name[64];
        hmemcpy(name, state->name, 64);
#else
        char *name = state->name;
#endif
        kz_log("loaded state %d (%s)", kz.state_slot, name);
    } else {
        kz_log("no state");
    }
}

void command_save_state(){
    if(!zu_is_ingame()) {
        kz_log("cannot save here");
        return;
    }

    kz_state_hdr_t *state = kz.states[kz.state_slot];
    char *tmp_name = NULL;
    if(state != NULL) {
        tmp_name = malloc(sizeof(state->name));
#ifdef WIIVC
        hmemcpy(tmp_name, state->name, sizeof(state->name));
        hfree(state);
#else
        memcpy(tmp_name, state->name, sizeof(state->name));
        free(state);
#endif
    }

    size_t state_size = save_state(NULL);

#ifdef WIIVC
    state = halloc(state_size);
#else
    state = malloc(state_size);
#endif
    if(state == NULL) {
        kz_log("could not alloc state %d", kz.state_slot);
        return;
    }
    size_t size = save_state(state);
#ifdef WIIVC
    kz_state_hdr_t kz_state;
    kz_state.size = size;
    kz_state.z2_version = Z2_VERSION;
    kz_state.state_version = STATE_VERSION;
    if(tmp_name != NULL) {
        memcpy(kz_state.name, tmp_name, sizeof(kz_state.name));
        free(tmp_name);
    } else {
        strcpy(kz_state.name, "untitled");
    }
    hmemcpy(state, &kz_state, sizeof(kz_state));
    kz_log("saved state %d (%s)", kz.state_slot, kz_state.name);
#else
    kz_state_hdr_t *kz_state = state;
    kz_state->size = size;
    kz_state->z2_version = Z2_VERSION;
    kz_state->state_version = STATE_VERSION;
    if(tmp_name != NULL) {
        memcpy(state->name, tmp_name, sizeof(state->name));
        free(tmp_name);
    } else {
        strcpy(state->name, "untitled");
    }
    kz_log("saved state %d (%s)", kz.state_slot, state->name);
#endif
    kz.states[kz.state_slot] = state;
}
#endif

void command_timer(void){
    kz.timer_running = !kz.timer_running;
}

void command_timer_reset(void){
    kz.cpu_offset = -kz.cpu_cycle_counter;
    kz.cpu_prev = kz.cpu_cycle_counter;
}

void command_break(void){
    if(z2_link.action != 0) {
        z2_link.action = 7;
    }
    if(z2_game.cs_ctx.state != 0) {
        z2_game.cs_ctx.state = 0x03;
    }

    if(z2_game.message_state_1 != 0) {
        z2_game.message_state_1 = 0x43;
        z2_game.message_state_2 = 0x00;
        z2_game.message_state_3 = 0x02;
    }

    z2_game.cameras[0].set = 1;

    for(int i = 1; i < 4; i++) {
        if(z2_game.cameras[i].param_flags != -1) {
            z2_game.cameras[i].param_flags = 0;
        }
    }

    z2_link.state_flags_1 = 0;
    z2_link.state_flags_2 = 0;
}

void command_skip_cutscene(void){
    z2_cutscene_ctx_t* cs_ctx = &z2_game.cs_ctx;
    z2_cutscene_data *cutscene_ptr = cs_ctx->cs_data;
    int32_t end_frame;

    if (!z2_cutscene_is_playing_cs(&z2_game)) {
        return;
    }

    if (cutscene_ptr == NULL) {
        return;
    }

    end_frame = cutscene_ptr[1].i;

    // Execute every command until the end of the cutscene data
    for (uint16_t currentFrame = cs_ctx->frames; currentFrame < end_frame-10; currentFrame++) {
        uint16_t frame_aux = cs_ctx->frames;

        z2_cutscene_process_cmd_wrp(&z2_game, cs_ctx);
        if (frame_aux == cs_ctx->frames) {
            // Bypass commands that manipulate the current frame, like textboxes
            cs_ctx->frames++;
        }
    }

    int16_t next_entrance_index = cs_ctx->scene_cs_list[cs_ctx->current_cs_index].next_entrance_index;
    if (next_entrance_index != -1) {
        // The end of this cutscene triggers another cutscene, so just trigger it immediately
        z2_cs_cmd_base_t cmd = {1,0,0,0};

        z2_cutscene_terminator_impl(&z2_game, cs_ctx, &cmd);
    }
}

void command_levitate(void){
    _Bool is_pause = z2_player_ovl_cur == &z2_player_ovl_table[0];

    if(!is_pause) {
        z2_link.common.vel_1.y = 6.0f;
    }
}

void command_turbo(void){
    if(settings->turbo_type == TURBO_HOLD){
        z2_link.linear_velocity = 18.0f;
    }else{
        settings->cheats ^= (1 << CHEAT_TURBO);
    }
}

void command_fall(void){
    z2_link.common.pos_1.y = -32000.0f;
}

void command_void(void){
    z2_file.void_flag = 1;
    z2_game.entrance_index = z2_file.exit & 0xFFFF;
    z2_game.common.execute_state = 0;
    z2_game.common.gamestate_ctor = z2_gamestate_table[3].vram_ctor;
    z2_game.common.ctxt_size = z2_gamestate_table[3].alloc_size;
}

void command_reloadarea(void){
    z2_game.entrance_index = z2_file.exit;
    z2_game.common.execute_state = 0;
    z2_game.common.gamestate_ctor = z2_gamestate_table[3].vram_ctor;
    z2_game.common.ctxt_size = z2_gamestate_table[3].alloc_size;
}

void command_title_screen(void){
    z2_ctxt.execute_state = 0;
    z2_ctxt.gamestate_ctor = z2_gamestate_table[4].vram_ctor;
    z2_ctxt.ctxt_size = z2_gamestate_table[4].alloc_size;
}

void command_pause(void){
    if(kz.pending_frames == 0){
        kz.pending_frames = -1;

    }else{
        kz.pending_frames = 0;
    }
}

void command_advance(void){
    if(kz.pending_frames == 0){
        kz.pending_frames++;
    }else{
        command_pause();
    }
}

void command_lag_reset(void){
    kz.frames_offset = -z2_vi_counter;
    kz.frames = 0;
}

void command_save_memfile(void){
    memfile_t *newmemfile;
    if(!kz.memfile[kz.memfile_slot]){
        newmemfile = malloc(sizeof(*newmemfile));
        kz.memfile[kz.memfile_slot] = newmemfile;
    }else{
        newmemfile = kz.memfile[kz.memfile_slot];
    }

    // Save 3 day clock day switching boundaries to prevent day/night advancing when loading outside current day/night
    for(z2_actor_t *actor = z2_game.actor_ctxt.actor_list[0].first;actor;actor = actor->next){
        if(actor->id == 0x15A){
            z2_timer_t *timer = (z2_timer_t*)actor;
            memcpy(&newmemfile->timer_boundaries, &timer->timer_boundaries, sizeof(newmemfile->timer_boundaries));
            break;
        }
    }
    newmemfile->scene_flags[0] = z2_game.actor_ctxt.chest;
    newmemfile->scene_flags[1] = z2_game.actor_ctxt.switch_1;
    newmemfile->scene_flags[2] = z2_game.actor_ctxt.switch_2;
    newmemfile->scene_flags[3] = z2_game.actor_ctxt.clear;
    newmemfile->scene_flags[4] = z2_game.actor_ctxt.collectible_1;
    newmemfile->scene = z2_game.scene_index;
    newmemfile->z2_version = Z2_VERSION;
    memcpy(&newmemfile->file, &z2_file, sizeof(newmemfile->file));

    kz_log("memfile %d saved", kz.memfile_slot);
}

void command_load_memfile(void){
    memfile_t *memfile = kz.memfile[kz.memfile_slot];
    if(memfile){
        if(memfile->z2_version == Z2_VERSION){
            for(z2_actor_t *actor = z2_game.actor_ctxt.actor_list[0].first;actor;actor = actor->next){
                if(actor->id == 0x15A){
                    z2_timer_t *timer = (z2_timer_t*)actor;
                    memcpy(&timer->timer_boundaries, &memfile->timer_boundaries, sizeof(timer->timer_boundaries));
                }
            }
            if(memfile->scene == z2_game.scene_index){
                z2_game.actor_ctxt.chest = memfile->scene_flags[0];
                z2_game.actor_ctxt.switch_1 = memfile->scene_flags[1];
                z2_game.actor_ctxt.switch_2 = memfile->scene_flags[2];
                z2_game.actor_ctxt.clear = memfile->scene_flags[3];
                z2_game.actor_ctxt.collectible_1 = memfile->scene_flags[4];
            }
            _Bool dovoid = 0;
            if(z2_file.exit != memfile->file.exit){
                dovoid = 1;
            }
            memcpy(&z2_file, &memfile->file, sizeof(z2_file));
            for(int i = 0;i < 4;i++){
                z2_btnupdate(&z2_game,i);
            }
            if(settings->memfile_action & MEMFILE_VOID){
                dovoid = 1;
            }
            if(settings->memfile_action & MEMFILE_POS){
                command_load_position();
            }
            if(dovoid){
                command_void();
            }

            kz_log("memfile %d loaded", kz.memfile_slot);
        }else{
            kz_log("mismatched mm version");
        }
    }
    else{
        kz_log("no memfile %d", kz.memfile_slot);
    }
}

void command_next_memfile(void){
    kz.memfile_slot++;
    kz.memfile_slot %= KZ_MEMFILE_MAX;
}

void command_prev_memfile(void){
    kz.memfile_slot += KZ_MEMFILE_MAX - 1;
    kz.memfile_slot %= KZ_MEMFILE_MAX;
}

void command_save_position(void){
    position_t *newpos;
    if(!kz.position_save[kz.pos_slot]){
        newpos = malloc(sizeof(*newpos));
        kz.position_save[kz.pos_slot] = newpos;
    }else{
        newpos = kz.position_save[kz.pos_slot];
    }
    memcpy(&newpos->pos, &z2_link.common.pos_2, sizeof(newpos->pos));
    memcpy(&newpos->rot, &z2_link.common.rot_2, sizeof(newpos->rot));

    kz_log("position %d saved", kz.pos_slot);
}

void command_load_position(void){
    position_t *position = kz.position_save[kz.pos_slot];
    if(position){
        memcpy(&z2_link.common.pos_1, &position->pos, sizeof(position->pos));
        memcpy(&z2_link.common.pos_2, &position->pos, sizeof(position->pos));
        memcpy(&z2_link.common.rot_2, &position->rot, sizeof(position->rot));

        kz_log("position %d loaded", kz.pos_slot);
    }
    else{
        kz_log("no position %d", kz.pos_slot);
    }
}

void command_next_position(void){
    kz.pos_slot++;
    kz.pos_slot %= KZ_POSITION_MAX;
}

void command_prev_position(void){
    kz.pos_slot += KZ_POSITION_MAX - 1;
    kz.pos_slot %= KZ_POSITION_MAX;
}
