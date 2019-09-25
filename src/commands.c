#include <stdlib.h>
#include "commands.h"
#include "input.h"
#include "kz.h"
#include "settings.h"

struct command kz_commands[Z2_CMD_MAX] = {
    {"toggle menu",         COMMAND_PRESS,  NULL},
    {"levitate",            COMMAND_HOLD,   command_levitate},
    {"turbo",               COMMAND_PRESS,  command_turbo},
    {"void out",            COMMAND_PRESS,  command_void},
    {"break free",          COMMAND_PRESS,  command_break},
    {"pause",               COMMAND_PRESS,  command_pause},
    {"advance",             COMMAND_PRESS,  command_advance},
    {"return",              COMMAND_PRESS,  NULL },
    {"reset lag counter",   COMMAND_PRESS,  command_lag_reset},
    {"start/stop timer",    COMMAND_PRESS,  command_timer},
    {"reset timer",         COMMAND_PRESS,  command_timer_reset},
    {"save memfile",        COMMAND_PRESS,  command_save_memfile},
    {"load memfile",        COMMAND_PRESS,  command_load_memfile},
};

void command_timer(){
    kz.timer_running=!kz.timer_running;
}

void command_timer_reset(){
    kz.cpu_offset = -kz.cpu_cycle_counter;
    kz.cpu_prev = kz.cpu_cycle_counter;
}

void command_break(){
    z2_game.cutscene_state = 0x03;
    z2_game.message_state_1 = 0x43;
    z2_game.message_state_2 = 0x00;
    z2_game.message_state_3 = 0x02;
    z2_link.state_flags_1 = 0;
    z2_link.state_flags_2 = 0;
}

void command_levitate(){
    z2_link.common.vel_1.y=6.0f;
}

void command_turbo(){
    settings->cheats ^= (1 << CHEAT_TURBO);
}

void command_fall(){
    z2_link.common.pos_1.y=-32000.0f;
}

void command_void(){
    z2_file.void_flag = 1;
    z2_game.entrance_index = z2_file.exit & 0xFFFF;
    z2_game.common.execute_state = 0;
    z2_game.common.gamestate_ctor = z2_gamestate_table[3].vram_ctor;
    z2_game.common.ctxt_size = z2_gamestate_table[3].alloc_size;
}

void command_pause(){
    if(kz.pending_frames==0){
        kz.pending_frames=-1;
    }else{
        kz.pending_frames=0;
    }
}

void command_advance(){
    if(kz.pending_frames==0){
        kz.pending_frames++;
    }else{
        command_pause();
    }
}

void command_lag_reset(){
    kz.frames_offset = -z2_vi_counter;
    kz.frames = 0;
}

void command_save_memfile(){
    if(!kz.memfile[kz.memfile_slot]){
        memfile_t *newmemfile = malloc(sizeof(*newmemfile));
        memcpy(&newmemfile->file,&z2_file,sizeof(newmemfile->file));
        kz.memfile[kz.memfile_slot] = newmemfile;
    }
}

void command_load_memfile(){
    if(kz.memfile[kz.memfile_slot]){
        memcpy(&z2_file,&kz.memfile[kz.memfile_slot]->file,sizeof(z2_file));
    }
}