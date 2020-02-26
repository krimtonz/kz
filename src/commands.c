#include <stdlib.h>
#include <stdio.h>
#include <libundermine.h>
#include "commands.h"
#include "kz.h"
#include "settings.h"
#include "z2.h"
#include "state.h"

struct command kz_commands[KZ_CMD_MAX] = {
    {"toggle menu",         COMMAND_PRESS,  NULL},
    {"return",              COMMAND_PRESS,  NULL },
    {"levitate",            COMMAND_HOLD,   command_levitate},
    {"turbo",               COMMAND_PRESS,  command_turbo},
    {"fall",                COMMAND_HOLD,   command_fall},
    {"reload scene",        COMMAND_PRESS,  command_reloadarea},
    {"void out",            COMMAND_PRESS,  command_void},
    {"break free",          COMMAND_PRESS,  command_break},
    {"pause",               COMMAND_PRESS,  command_pause},
    {"advance",             COMMAND_PRESS,  command_advance},
    {"toggle input",        COMMAND_PRESS,  command_toggle_input},
    {"reset lag counter",   COMMAND_PRESS,  command_lag_reset},
    {"start/stop timer",    COMMAND_PRESS,  command_timer},
    {"reset timer",         COMMAND_PRESS,  command_timer_reset},
    {"title screen",        COMMAND_PRESS,  command_title_screen},
    {"save memfile",        COMMAND_PRESS,  command_save_memfile},
    {"load memfile",        COMMAND_PRESS,  command_load_memfile},
    {"next memfile",        COMMAND_PRESS,  command_next_memfile},
    {"prev memfile",        COMMAND_PRESS,  command_prev_memfile},
    {"save position",       COMMAND_PRESS,  command_save_position},
    {"load position",       COMMAND_PRESS,  command_load_position},
    {"next position",       COMMAND_PRESS,  command_next_position},
    {"prev position",       COMMAND_PRESS,  command_prev_position},
#ifndef LITE
    {"load state",          COMMAND_PRESS,  command_load_state},
    {"save state",          COMMAND_PRESS,  command_save_state},
#endif
};

#ifndef LITE
static void *state = NULL;
void command_load_state(){
    if(state){
        load_state(state);
        kz_log("loaded state");
    }else{
        kz_log("no state");
    }
}

void command_save_state(){
    if(state){
        free(state);
    }
    state = malloc(0x20000);
    kz_state_hdr_t *kz_state = state;
    kz_state->size = save_state(state);
    kz_state->z2_version = Z2_VERSION;
    kz_state->settings_version = 0;
    state = realloc(state, kz_state->size);
    kz_log("saved state");
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
    z2_game.cutscene_state = 0x03;
    z2_game.message_state_1 = 0x43;
    z2_game.message_state_2 = 0x00;
    z2_game.message_state_3 = 0x02;
    z2_link.state_flags_1 = 0;
    z2_link.state_flags_2 = 0;
}

void command_levitate(void){
    z2_link.common.vel_1.y = 6.0f;
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
        kz.z2_input_enabled = 1;

    }else{
        kz.pending_frames = 0;
        kz.z2_input_enabled = 0;
    }
}

void command_advance(void){
    if(kz.pending_frames == 0){
        kz.pending_frames++;
    }else{
        command_pause();
    }
}

void command_toggle_input(void){
    kz.z2_input_enabled = !kz.z2_input_enabled;
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

    char mesg[100];
    snprintf(mesg, 100, "memfile %d saved", kz.memfile_slot);
    kz_log(mesg);
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

            char mesg[100];
            snprintf(mesg, 100, "memfile %d loaded", kz.memfile_slot);
            kz_log(mesg);
        }else{
            kz_log("mismatched mm version");
        }
    }
    else{
        char mesg[100];
        snprintf(mesg,100,"no memfile %d",kz.memfile_slot);
        kz_log(mesg);
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

    char mesg[100];
    snprintf(mesg, 100, "position %d saved", kz.pos_slot);
    kz_log(mesg);
}

void command_load_position(void){
    position_t *position = kz.position_save[kz.pos_slot];
    if(position){
        memcpy(&z2_link.common.pos_1, &position->pos, sizeof(position->pos));
        memcpy(&z2_link.common.pos_2, &position->pos, sizeof(position->pos));
        memcpy(&z2_link.common.rot_2, &position->rot, sizeof(position->rot));

        char mesg[100];
        snprintf(mesg, 100, "position %d loaded", kz.pos_slot);
        kz_log(mesg);
    }
    else{
        char mesg[100];
        snprintf(mesg, 100, "no position %d", kz.pos_slot);
        kz_log(mesg);
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