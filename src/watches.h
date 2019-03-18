#ifndef _WATCHES_H
#define _WATCHES_H

#include <stdint.h>

typedef enum {
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
}watch_type;

typedef struct {
    void *address;
    watch_type type;
    uint16_t x;
    uint16_t y;
    uint8_t floating;
} watch_t;

void watch_printf(watch_t *watch, char *buf);

#endif