/*
* settings.h
*
* defintions for storing kz-related save data to flash ram
*/

#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <stdint.h>
#include <stddef.h>
#include "watches.h"
#include "commands.h"
#include "kz.h"

#define IO_BLOCK_SIZE       0x80
#define SIZE_TO_BLOCK(x)    ((x + IO_BLOCK_SIZE - 1) / IO_BLOCK_SIZE)

#define SETTINGS_VER        9
#define SETTINGS_ADDR       0x20000
#define SETTINGS_SIZE       (sizeof(struct settings_header) + sizeof(struct settings_data))
#define SETTINGS_PAD        ((IO_BLOCK_SIZE - (SETTINGS_SIZE & (IO_BLOCK_SIZE - 1))) & (IO_BLOCK_SIZE - 1))
#define SETTINGS_MAX        3
#define FULL_SETTINGS       ((uint32_t)(((uint32_t)Z2_VERSION << 16) | ((uint32_t)KZ_VERSION << 8) | (uint32_t)SETTINGS_VER))

#define MEMFILE_NONE        0
#define MEMFILE_VOID        1
#define MEMFILE_POS         2

#define TURBO_TOGGLE        0
#define TURBO_HOLD          1

struct settings_header {
    char        magic[4];
    uint32_t    version;
};

struct settings_data {
    uint32_t            watch_addresses[WATCHES_MAX];
    uint16_t            watch_x[WATCHES_MAX];
    uint16_t            watch_y[WATCHES_MAX];
    size_t              watch_cnt;
    char                watch_labels[WATCHES_MAX][20];
    uint32_t            cheats;
    union{
        struct {
            uint32_t                    : 22;
            uint32_t    hit_view_opq    : 1;
            uint32_t    col_view_upd    : 1;
            uint32_t    col_view_red    : 1;
            uint32_t    col_view_opq    : 1;
            uint32_t    memfile_action  : 2;
            uint32_t    input_display   : 1;
            uint32_t    lag_counter     : 1;
            uint32_t    timer           : 1;
            uint32_t    turbo_type      : 1;
        };
        uint32_t    settings_bits;
    };
    struct watch_info   watch_info[WATCHES_MAX];
    uint16_t            binds[KZ_CMD_MAX];
    int16_t             lag_x;
    int16_t             lag_y;
    int16_t             timer_x;
    int16_t             timer_y;
    int16_t             id_x;
    int16_t             id_y;
    int16_t             menu_x;
    int16_t             menu_y;
};

struct settings {
    struct settings_header  header;
    struct settings_data    data;
    char                    pad[SETTINGS_PAD];
};

void    save_settings_to_flashram   (int profile);
void    load_settings_from_flashram (int profile);
void    load_default_settings       (void);
void    kz_apply_settings           (void);

extern struct settings_data    *settings;

#endif