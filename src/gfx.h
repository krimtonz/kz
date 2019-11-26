#ifndef _GFX_H
#define _GFX_H
#include <n64.h>
#include <stdarg.h>
#include "z2.h"

#define COLOR_GREEN GPACK_RGBA8888(0x00,0xFF,0x00,0xFF)

#define gfx_push(...)   {Gfx gfx__[]={__VA_ARGS__};     \
                        gfx_append(gfx__,sizeof(gfx__));}

enum loader_source{
    SOURCE_ARCHIVE,
    SOURCE_FILE,
};

enum rdp_mode{
    RDP_MODE_COLOR,
    RDP_MODE_COMBINE,
    RDP_MODE_FILTER,
    RDP_MODE_ALL
};

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

typedef struct{
    uint16_t            width;
    uint16_t            height;
    uint16_t            file;
    g_ifmt_t            format;
    g_isiz_t            size;
    uint8_t             start_item;
    uint8_t             end_item;
    _Bool               desaturate;
    enum loader_source  source;
    uint32_t            offset;
    uint8_t             num_tiles;
} texture_loader;

void gfx_init();
void gfx_begin();
void gfx_finish();
void *gfx_data_push(void *data, size_t size);
void gfx_printf(uint16_t,uint16_t,const char*,...);
void gfx_printf_color(uint16_t,uint16_t,uint32_t,const char*,...);
void gfx_printf_scale(uint16_t,uint16_t,float,float,const char*,...);
void gfx_printf_color_scale(uint16_t,uint16_t,uint32_t,float,float,const char*,...);
void gfx_printf_va_color_scale(uint16_t,uint16_t,uint32_t,float,float,const char*,va_list);
void gfx_printchars(gfx_font *, uint16_t, uint16_t, uint32_t, const char *, size_t, float, float);
void gfx_draw_sprite(gfx_texture *texture, int x, int y, int tile, int width, int height);
void gfx_draw_sprite_color(gfx_texture *texture, int x, int y, int tile, int width, int height, uint32_t color);
void gfx_draw_sprite_scale(gfx_texture *texture, int x, int y, int tile, int width, int height, float x_scale, float y_scale);
void gfx_draw_sprite_scale_color(gfx_texture *texture, int x, int y, int tile, int width, int height, float x_scale, float y_scale, uint32_t color);
void gfx_draw_rectangle(int x, int y, int width, int height, uint32_t color);
void gfx_destroy_texture(gfx_texture *texture);
void gfx_load_tile(gfx_texture *texture, uint16_t tilenum);
gfx_texture *gfx_load(texture_loader *loader);
void gfx_texture_desaturate(void *data, size_t len);
gfx_texture *gfx_load_item_texture(uint8_t item_id);
void gfx_append(Gfx *gfx, size_t size);
void rdp_mode_set(enum rdp_mode mode, uint64_t val);
void rdp_mode_replace(enum rdp_mode mode, uint64_t val);
void rdp_mode_set_apply(enum rdp_mode mode, uint64_t val);
void rdp_mode_pop(enum rdp_mode mode);

extern gfx_font *kfont;

#endif