#include "commands.h"
#include "input.h"
#include "kz.h"

struct command kz_commands[Z2_CMD_MAX] = {
    {"toggle menu",         COMMAND_PRESS,  0,  NULL},
    {"levitate",            COMMAND_HOLD,   0,  command_levitate},
    {"turbo",               COMMAND_PRESS,  0,  command_turbo},
    {"void out",            COMMAND_PRESS,  0,  command_void},
    {"break free",          COMMAND_PRESS,  0,  command_break},
    {"pause",               COMMAND_PRESS,  0,  command_pause},
    {"advance",             COMMAND_PRESS,  0,  command_advance},
    {"return",              COMMAND_PRESS,  0,  NULL },
    {"reset lag counter",   COMMAND_PRESS,  0,  command_lag_reset}
};

void init_commands(){
    kz_commands[Z2_CMD_TOGGLE_MENU].bind = make_bind(2, BUTTON_R, BUTTON_L);
    kz_commands[Z2_CMD_LEVITATE].bind = make_bind(1, BUTTON_L);
    kz_commands[Z2_CMD_TURBO].bind = make_bind(1, BUTTON_D_LEFT);
    kz_commands[Z2_CMD_VOID].bind = make_bind(2, BUTTON_D_LEFT, BUTTON_A);
    kz_commands[Z2_CMD_BREAK].bind = make_bind(2, BUTTON_D_RIGHT, BUTTON_L);
    kz_commands[Z2_CMD_PAUSE].bind = make_bind(1, BUTTON_D_UP);
    kz_commands[Z2_CMD_ADVANCE].bind = make_bind(1, BUTTON_D_DOWN);
    kz_commands[Z2_CMD_RETURN].bind = make_bind(2, BUTTON_L, BUTTON_D_RIGHT);
    kz_commands[Z2_CMD_RESET_LAG].bind = make_bind(1,BUTTON_D_RIGHT);
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
    kz.cheats ^= (1 << CHEAT_TURBO);
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