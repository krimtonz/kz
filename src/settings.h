#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <stdint.h>
#include <stddef.h>
#include "watches.h"

#define SETTINGS_VER    1

struct settings {
    char                magic[4];
    uint32_t            version;
    uint32_t            watch_addresses[WATCHES_MAX];
    uint16_t            watch_x[WATCHES_MAX];
    uint16_t            watch_y[WATCHES_MAX];
    struct watch_info   watch_info[WATCHES_MAX];
    size_t              watch_cnt;
};

void save_settings_to_flashram(struct settings *settings);
void load_settings_from_flashram(struct settings *settings);
void kz_apply_settings();

#endif