#ifndef _WATCHES_H
#define _WATCHES_H

#include <stdint.h>

#define WATCHES_MAX 10

struct watch_info{
    union{
        uint8_t type        : 4;
        uint8_t floating    : 4;
    };
    uint8_t wi;
};

enum watch_type{
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
    void *address;
    enum watch_type type;
    uint16_t x;
    uint16_t y;
    uint8_t floating;
} watch_t;

void watch_printf(watch_t *watch);

#endif