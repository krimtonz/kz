#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <stdint.h>
#include <stddef.h>
#include "watches.h"
#include "io.h"

#define SETTINGS_VER    1
#define SETTINGS_ADDR   0x1C200
#define SETTINGS_PAD    ((IO_BLOCK_SIZE - (sizeof(struct settings) % IO_BLOCK_SIZE ) % IO_BLOCK_SIZE))
#define SETTINGS_MAX    3

struct settings_header{
    char        magic[4];
    uint32_t    version;
};

struct settings_data{
    uint32_t            watch_addresses[WATCHES_MAX];
    uint16_t            watch_x[WATCHES_MAX];
    uint16_t            watch_y[WATCHES_MAX];
    union watch_info    watch_info[WATCHES_MAX];
    size_t              watch_cnt;
};

struct settings {
    struct settings_header  header;
    struct settings_data    data;
};

void save_settings_to_flashram(int profile);
void load_settings_from_flashram(int profile);
void load_default_settings();
void kz_apply_settings();

extern struct settings_data *settings;

#endif