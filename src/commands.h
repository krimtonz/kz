/*
* commands.h
*
* definitions for kz commands
*/

#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "z2.h"

enum{
    KZ_CMD_TOGGLE_MENU,
    KZ_CMD_RETURN,
    KZ_CMD_LEVITATE,
    KZ_CMD_TURBO,
    KZ_CMD_FALL,
    KZ_CMD_RELOAD,
    KZ_CMD_VOID,
    KZ_CMD_BREAK,
    KZ_CMD_PAUSE,
    KZ_CMD_ADVANCE,
    KZ_CMD_TOGGLE_INPUT,
    KZ_CMD_RESET_LAG,
    KZ_CMD_TIMER,
    KZ_CMD_TIMER_RESET,
    KZ_CMD_TITLE_SCREEN,
    KZ_CMD_SAVE_MEMFILE,
    KZ_CMD_LOAD_MEMFILE,
    KZ_CMD_NEXT_MEMFILE,
    KZ_CMD_PREV_MEMFILE,
    KZ_CMD_SAVE_POSITION,
    KZ_CMD_LOAD_POSITION,
    KZ_CMD_NEXT_POSITION,
    KZ_CMD_PREV_POSITION,
    KZ_CMD_MAX
};

enum command_type{
    COMMAND_HOLD,
    COMMAND_PRESS
};

struct command {
    char               *text;
    enum command_type   type;
    void              (*proc)();
};

void    command_levitate        ();
void    command_void            ();
void    command_reloadarea      ();
void    command_fall            ();
void    command_turbo           ();
void    command_break           ();
void    command_pause           ();
void    command_advance         ();
void    command_return          ();
void    command_lag_reset       ();
void    command_timer           ();
void    command_timer_reset     ();
void    command_save_memfile    ();
void    command_load_memfile    ();
void    command_next_memfile    ();
void    command_prev_memfile    ();
void    command_save_position   ();
void    command_load_position   ();
void    command_next_position   ();
void    command_prev_position   ();
void    command_toggle_input    ();
void    command_title_screen    ();

extern struct command kz_commands[KZ_CMD_MAX];

#endif