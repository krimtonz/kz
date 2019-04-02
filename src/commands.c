#include "commands.h"
#include "input.h"

struct command kz_commands[COMMAND_CNT] = {
    {"toggle menu", COMMAND_PRESS,  BUTTON_L | BUTTON_R,        NULL},
    {"levitate",    COMMAND_HOLD,   BUTTON_L,                   command_levitate},
    {"turbo",       COMMAND_HOLD,   BUTTON_D_DOWN,              command_turbo},
    {"void out",    COMMAND_PRESS,  BUTTON_D_LEFT | BUTTON_A,   command_void},
    {"break free",  COMMAND_PRESS,  BUTTON_D_RIGHT | BUTTON_L,  command_break},
};

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
    z2_link.linear_velocity=18.0f;
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