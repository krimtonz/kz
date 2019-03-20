#include "commands.h"

void command_levitate(){
    z2_link.common.vel_1.y=6.0f;
}

void command_tubo(){
    z2_link.linear_velocity=18.0f;
}

void command_fall(){
    z2_link.common.pos_1.y=-32000.0f;
}

void command_void(){
    z2_file.void_flag = 1;
    z2_game.entrance_index = z2_file.exit & 0xFFFF;
    //z2_game.scene_load_flag = 0x14;
    z2_game.common.execute_state = 0;
    z2_game.common.gamestate_ctor = z2_gamestate_table[3].vram_ctor;
    z2_game.common.ctxt_size = z2_gamestate_table[3].alloc_size;
}