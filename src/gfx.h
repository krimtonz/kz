#ifndef _GFX_H
#define _GFX_H
#include <n64.h>
#include <stdarg.h>
#include "z2.h"

#define COLOR_GREEN GPACK_RGBA8888(0x00,0xFF,0x00,0xFF)

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
void gfx_push(Gfx gfx);
void gfx_printf(uint16_t,uint16_t,const char*,...);
void gfx_printf_color(uint16_t,uint16_t,uint32_t,const char*,...);
void gfx_printf_scale(uint16_t,uint16_t,float,float,const char*,...);
void gfx_printf_color_scale(uint16_t,uint16_t,uint32_t,float,float,const char*,...);
void gfx_printf_va_color_scale(uint16_t,uint16_t,uint32_t,float,float,const char*,va_list);
void gfx_printchars(gfx_font *, uint16_t, uint16_t, uint32_t, const char *, size_t, float, float);
void gfx_draw_sprite(gfx_texture *texture, int x, int y, int tile, int width, int height);
void gfx_draw_sprite_scale(gfx_texture *texture, int x, int y, int tile, int width, int height, float x_scale, float y_scale);
void gfx_draw_rectangle(int x, int y, int width, int height, uint32_t color);
void gfx_destroy_texture(gfx_texture *texture);
gfx_texture *gfx_load_icon_item_static(uint16_t file_idx, uint8_t start_tile, uint8_t end_tile, g_ifmt_t format, g_isiz_t size, uint16_t tile_width, uint16_t tile_height, _Bool desaturate);
gfx_texture *gfx_load_game_texture(g_ifmt_t format, g_isiz_t size, uint16_t width, uint16_t height, uint16_t x_tiles, uint16_t y_tiles, int file, uint32_t offset, _Bool desaturate);
void gfx_texture_desaturate(void *data, size_t len);

extern gfx_font *kfont;

#endif