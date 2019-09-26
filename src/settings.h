#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <stdint.h>
#include <stddef.h>
#include "watches.h"
#include "io.h"
#include "commands.h"

#define SETTINGS_VER    2
#define SETTINGS_ADDR   0x1C200
#define SETTINGS_SIZE   (sizeof(struct settings_header) + sizeof(struct settings_data))
#define SETTINGS_PAD    ((IO_BLOCK_SIZE - (SETTINGS_SIZE & (IO_BLOCK_SIZE - 1))) & (IO_BLOCK_SIZE - 1))
#define SETTINGS_MAX    3

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