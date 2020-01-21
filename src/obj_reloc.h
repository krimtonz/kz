#ifndef _OBJ_RELOC_H
#define _OBJ_RELOC_H

#include <stdint.h>

typedef struct {
    uint16_t    obj_id;
    uint8_t     reloc_cnt;
    uint32_t   *relocs;
} obj_reloc_t;

extern obj_reloc_t  obj_relocs[55];

#endif