#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <stdint.h>
#include <stddef.h>
#include "watches.h"
#include "commands.h"

#define IO_BLOCK_SIZE       0x80
#define SIZE_TO_BLOCK(x)    ((x + IO_BLOCK_SIZE - 1) / IO_BLOCK_SIZE)

#define SETTINGS_VER        4
#define SETTINGS_ADDR       0x20000
#define SETTINGS_SIZE       (sizeof(struct settings_header) + sizeof(struct settings_data))
#define SETTINGS_PAD        ((IO_BLOCK_SIZE - (SETTINGS_SIZE & (IO_BLOCK_SIZE - 1))) & (IO_BLOCK_SIZE - 1))
#define SETTINGS_MAX        3

#define MEMFILE_NONE        0
#define MEMFILE_VOID        1
#define MEMFILE_LOAD        2
#define MEMFILE_POS         4

struct settings_header{
    char        magic[4];
    uint32_t    version;
};

struct settings_data{
    uint32_t            watch_addresses[WATCHES_MAX];
    uint16_t            watch_x[WATCHES_MAX];
    uint16_t            watch_y[WATCHES_MAX];
    struct watch_info   watch_info[WATCHES_MAX];
    size_t              watch_cnt;
    char                watch_labels[20][WATCHES_MAX];
    uint8_t             lag_counter;
    int16_t             lag_x;
    int16_t             lag_y;
    uint8_t             timer;
    int16_t             timer_x;
    int16_t             timer_y;
    uint8_t             input_display;
    int16_t             id_x;
    int16_t             id_y;
    int16_t             menu_x;
    int16_t             menu_y;
    uint32_t            cheats;
    uint8_t             memfile_action;
    uint16_t            binds[Z2_CMD_MAX];
};

struct settings {
    struct settings_header  header;
    struct settings_data    data;
    char                    pad[SETTINGS_PAD];
};

void save_settings_to_flashram(int profile);
void load_settings_from_flashram(int profile);
void load_default_settings();
void kz_apply_settings();

extern struct settings_data *settings;

#endif