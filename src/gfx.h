#ifndef _GFX_H
#define _GFX_H
#include <n64.h>
#include <stdarg.h>
#include "z64.h"

typedef struct{
    void *data;
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t x_tiles;
    uint16_t y_tiles;
    size_t tile_size;
    g_ifmt_t img_fmt;
    g_isiz_t img_size;

} gfx_texture;

typedef struct{
    gfx_texture *texture;
    uint16_t c_width;
    uint16_t c_height;
    uint16_t cx_tile;
    uint16_t cy_tile;
} gfx_font;

void gfx_init();
void gfx_begin();
void gfx_finish();
void gfx_printf(uint16_t,uint16_t,const char*,...);
void gfx_printf_color(uint16_t,uint16_t,uint32_t,const char*,...);
void gfx_printf_va_color(uint16_t,uint16_t,uint32_t,const char*,va_list);
void gfx_printchars(gfx_font *, uint16_t, uint16_t, uint32_t, const char *, size_t);
void gfx_disp_buf_init(z64_disp_buf_t *db, size_t size);
void gfx_disp_buf_copy(z64_disp_buf_t *src, z64_disp_buf_t *dst);
void gfx_disp_buf_destroy(z64_disp_buf_t *db);
void gfx_disp_buf_reset(z64_disp_buf_t *db, size_t newsize);

extern gfx_font *kfont;

#endif