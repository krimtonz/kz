/*
* watches.h
*
* Memory watch definitions
*/

#ifndef _WATCHES_H
#define _WATCHES_H

#include <stdint.h>

#ifndef LITE
#define WATCHES_MAX 10
#else
#define WATCHES_MAX 5
#endif

struct watch_info {
    uint8_t type        : 4;
    uint8_t floating    : 4;
};

enum watch_type {
    WATCH_TYPE_S8,
    WATCH_TYPE_U8,
    WATCH_TYPE_X8,
    WATCH_TYPE_S16,
    WATCH_TYPE_U16,
    WATCH_TYPE_X16,
    WATCH_TYPE_S32,
    WATCH_TYPE_U32,
    WATCH_TYPE_X32,
    WATCH_TYPE_FLOAT
};

typedef struct {
    void               *address;
    enum watch_type     type;
    int16_t             x;
    int16_t             y;
    uint8_t             floating;
    char               *label;
} watch_t;

void watch_printf       (watch_t *watch, uint32_t color);
void clear_watches      (void);
void init_watch_rows    (void);

#endif