#ifndef _INVENTORY_MAP_H
#define _INVENTORY_MAP_H

#include <stdint.h>

#ifndef LITE
struct item_map_row {
    int8_t     *slot;
    int8_t      item;
};
#else
struct item_map_row {
    char       *tooltip;
    int8_t     *slot;
    int8_t      item;
};
#endif

extern struct item_map_row item_map_table[24];
extern struct item_map_row mask_map_table[24];

#endif
