#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "z2.h"

enum{
    Z2_CMD_TOGGLE_MENU,
    Z2_CMD_LEVITATE,
    Z2_CMD_TURBO,
    Z2_CMD_VOID,
    Z2_CMD_BREAK,
    Z2_CMD_PAUSE,
    Z2_CMD_ADVANCE,
    Z2_CMD_RETURN,
    Z2_CMD_RESET_LAG,
    Z2_CMD_TIMER,
    Z2_CMD_TIMER_RESET,
    Z2_CMD_MAX
};

enum command_type{
    COMMAND_HOLD,
    COMMAND_PRESS
};

struct command {
    const char         *text;
    enum command_type   type;
    void              (*proc)();
};

void command_levitate();
void command_void();
void command_reloadarea();
void command_turbo();
void command_break();
void command_pause();
void command_advance();
void command_return();
void command_lag_reset();
void command_timer();
void command_timer_reset();

extern struct command kz_commands[Z2_CMD_MAX];

#endif