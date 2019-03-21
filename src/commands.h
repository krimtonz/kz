#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "z2.h"

#define COMMAND_CNT 10

enum command_type{
    COMMAND_HOLD,
    COMMAND_PRESS
};

struct command {
    const char         *text;
    enum command_type   type;
    uint16_t            bind;
    void              (*proc)();
};

void command_levitate();
void command_void();
void command_reloadarea();
void command_turbo();

extern struct command kz_commands[COMMAND_CNT];

#endif