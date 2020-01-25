/*
* gfx.h
*
* definitions for graphics drawing
*/

#ifndef _KZGFX_H
#define _KZGFX_H
#include <n64.h>
#include <stdarg.h>
#include "z2.h"

enum loader_source{
    SOURCE_ARCHIVE,
    SOURCE_FILE,
};

typedef struct {
    uint32_t            offset;         /* 0x0000 */
    uint16_t            width;          /* 0x0004 */
    uint16_t            height;         /* 0x0006 */
    uint16_t            file;           /* 0x0008 */
    g_ifmt_t            format;         /* 0x000A */
    g_isiz_t            size;           /* 0x000B */
    uint8_t             start_item;     /* 0x000C */
    uint8_t             end_item;       /* 0x000D */
    _Bool               desaturate;     /* 0x000E */
    uint8_t             num_tiles;      /* 0x000F */
    enum loader_source  source;         /* 0x0010 */
} texture_loader;                       /* 0x0014 */

gfx_texture    *gfx_load                    (texture_loader *loader);
void            gfx_texture_desaturate      (void *data, size_t len);
gfx_texture    *gfx_load_item_texture       (uint8_t item_id);
void            gfx_destroy_texture         (gfx_texture *texture);

extern gfx_font *kfont;

#endif