/*
* commands.h
*
* definitions for kz commands
*/

#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "z2.h"

enum {
    KZ_CMD_SKIP_CUTSCENE,
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
    KZ_CMD_RESET_LAG,
    KZ_CMD_TIMER,
    KZ_CMD_TIMER_RESET,
    KZ_CMD_TITLE_SCREEN,
#ifndef LITE
    KZ_CMD_SAVE_STATE,
    KZ_CMD_LOAD_STATE,
    KZ_CMD_PREV_STATE,
    KZ_CMD_NEXT_STATE,
#endif
    KZ_CMD_SAVE_MEMFILE,
    KZ_CMD_LOAD_MEMFILE,
    KZ_CMD_PREV_MEMFILE,
    KZ_CMD_NEXT_MEMFILE,
    KZ_CMD_SAVE_POSITION,
    KZ_CMD_LOAD_POSITION,
    KZ_CMD_PREV_POSITION,
    KZ_CMD_NEXT_POSITION,
    KZ_CMD_MAX
};

enum command_type {
    COMMAND_HOLD,
    COMMAND_PRESS
};

struct command {
    char               *text;
    enum command_type   type;
    void              (*proc)();
};

void    command_skip_cutscene   (void);
void    command_levitate        (void);
void    command_void            (void);
void    command_reloadarea      (void);
void    command_fall            (void);
void    command_turbo           (void);
void    command_break           (void);
void    command_pause           (void);
void    command_advance         (void);
void    command_return          (void);
void    command_lag_reset       (void);
void    command_timer           (void);
void    command_timer_reset     (void);
void    command_save_memfile    (void);
void    command_load_memfile    (void);
void    command_next_memfile    (void);
void    command_prev_memfile    (void);
void    command_save_position   (void);
void    command_load_position   (void);
void    command_next_position   (void);
void    command_prev_position   (void);
void    command_title_screen    (void);
#ifndef LITE
void    command_load_state      (void);
void    command_save_state      (void);
void    command_prev_state      (void);
void    command_next_state      (void);
#endif

extern struct command kz_commands[KZ_CMD_MAX];

#endif