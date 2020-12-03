#include <n64.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <vector/vector.h>
#include "gfx.h"
#include "kzresource.h"
#include "zu.h"
#include "menu.h"
#include "cache.h"
#include "hb_heap.h"
#include "mips.h"
#include "printf.h"

#define TILE_SIZE(x,y,z)    ((x * y * G_SIZ_BITS(z) + 63) / 64 * 8)

#define RDP_STACK_LEN 5

#ifndef LITE
#define GFX_SIZE    (0x4000 * sizeof(Gfx))
#else
#define GFX_SIZE    (0x1500 * sizeof(Gfx))
#endif

typedef struct {
    int tile;
    uint32_t color;
    uint16_t x;
    uint16_t y;
} gfx_char_t;

static struct vector gfx_chars[8];
static Gfx     *gfx_disp;
static Gfx     *gfx_disp_p;
static Gfx     *gfx_disp_d;
static Gfx     *gfx_disp_work;

static _Bool    rdp_synced;

static uint64_t rdp_modes[RDP_MODE_ALL];
static uint64_t rdp_mode_stack[RDP_MODE_ALL][RDP_STACK_LEN];
static int      rdp_stack_pos[RDP_MODE_ALL];

static char tex_buf[0x1000];

gfx_font       *kfont;

static Gfx rcp_init[] = {
    gsSPLoadGeometryMode(0),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
    gsDPSetScissor(G_SC_NON_INTERLACE,
                  0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
    gsDPSetAlphaDither(G_AD_DISABLE),
    gsDPSetColorDither(G_CD_DISABLE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetDepthSource(G_ZS_PRIM),
    gsDPSetCombineKey(G_CK_NONE),
    gsDPSetTextureConvert(G_TC_FILT),
    gsDPSetTextureDetail(G_TD_CLAMP),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPPipelineMode(G_PM_NPRIMITIVE),
    gsSPEndDisplayList(),
};

static void draw_chars(void);

/* rdp mode functions */

static inline void rdp_sync(void){
    if(!rdp_synced){
        gDPPipeSync(gfx_disp_p++);
        rdp_synced = 1;
    }
}

static void rdp_mode_apply(enum rdp_mode mode){
    rdp_sync();
    switch(mode){
        case RDP_MODE_ALL:
        case RDP_MODE_COLOR:
        {
            uint32_t color = rdp_modes[RDP_MODE_COLOR];
            gDPSetPrimColor(gfx_disp_p++, 0, 0,
                            (color >> 24) & 0xFF,
                            (color >> 16) & 0xFF,
                            (color >> 8) & 0xFF,
                            color & 0xFF);

            if(mode != RDP_MODE_ALL){
                break;
            }
        }
        case RDP_MODE_COMBINE:{
            gDPSetCombine(gfx_disp_p++, rdp_modes[RDP_MODE_COMBINE]);
            if(mode != RDP_MODE_ALL){
                break;
            }
        }
        case RDP_MODE_FILTER:{
            gDPSetTextureFilter(gfx_disp_p++, rdp_modes[RDP_MODE_FILTER]);
            if(mode != RDP_MODE_ALL){
                break;
            }
        }
        default:
            break;
    }
}

void rdp_mode_set(enum rdp_mode mode, uint64_t val){
    if(mode == RDP_MODE_ALL){
        return;
    }
    rdp_modes[mode] = val;
}

static void rdp_mode_set_apply(enum rdp_mode mode, uint64_t val){
    rdp_mode_set(mode, val);
    rdp_mode_apply(mode);
}

static void rdp_mode_push(enum rdp_mode mode){
    if(mode == RDP_MODE_ALL){
        for(int i = 0;i < RDP_MODE_ALL;i++){
            int *p = &rdp_stack_pos[i];
            rdp_mode_stack[i][*p] = rdp_modes[i];
            *p = (*p + 1) % RDP_STACK_LEN;
        }
    }else{
        int *p = &rdp_stack_pos[mode];
        rdp_mode_stack[mode][*p] = rdp_modes[mode];
        *p = (*p + 1) % RDP_STACK_LEN;
    }
}

static void rdp_mode_pop(enum rdp_mode mode){
    if(mode == RDP_MODE_ALL){
        for(int i = 0;i < RDP_MODE_ALL;i++){
            int *p = &rdp_stack_pos[i];
            *p = (*p + RDP_STACK_LEN - 1) % RDP_STACK_LEN;
            rdp_mode_set(mode, rdp_mode_stack[i][*p]);
        }
    }else{
        int *p = &rdp_stack_pos[mode];
        *p = (*p + RDP_STACK_LEN - 1) % RDP_STACK_LEN;
        rdp_mode_set(mode, rdp_mode_stack[mode][*p]);
    }
    rdp_mode_apply(mode);
}

static void rdp_mode_replace(enum rdp_mode mode, uint64_t val){
    rdp_mode_push(mode);
    rdp_mode_set(mode, val);
    rdp_mode_apply(mode);
}

/* start/finish gfx processing functions */

void gfx_init(){
#ifdef WIIVC
    gfx_disp = (Gfx*)halloc(GFX_SIZE);
    gfx_disp_work = (Gfx*)halloc(GFX_SIZE);
#else
    gfx_disp = (Gfx*)malloc(GFX_SIZE);
    gfx_disp_work = (Gfx*)malloc(GFX_SIZE);
#endif
    gfx_disp_p = gfx_disp;
    gfx_disp_d = gfx_disp + ((GFX_SIZE + sizeof(*gfx_disp_d) - 1) / (sizeof(*gfx_disp_d)));

    kfont = malloc(sizeof(gfx_font));
    kfont->texture = resource_get(R_KZ_FONT);
    kfont->c_width = 8;
    kfont->c_height = 8;
    kfont->cx_tile = 2;
    kfont->cy_tile = 16;

    for(int i = 0; i < 8; i++) {
        vector_init(&gfx_chars[i], sizeof(gfx_char_t));
    }
}

void gfx_begin(void){
    rdp_sync();
    gSPDisplayList(gfx_disp_p++, &rcp_init);
    rdp_mode_apply(RDP_MODE_ALL);
}

void gfx_finish(void){
    draw_chars();
    gSPEndDisplayList(gfx_disp_p++);
    void *dl = gfx_disp;
#ifdef WIIVC
    dl = (void*)(((uint32_t)dl - 0xA8060000) | 0x0B000000);
    gSPSegment(z2_ctxt.gfx->overlay.p++, 0xB, 0xA8060000);
#endif
    gSPDisplayList(z2_ctxt.gfx->overlay.p++, dl);
#ifdef WIIVC
    gSPSegment(z2_ctxt.gfx->overlay.p++, 0xB, MIPS_PHYS_TO_KSEG0(z2_segment.segments[0xB]));
#endif
    Gfx *disp_w = gfx_disp_work;
    gfx_disp_work = gfx_disp;
    gfx_disp = disp_w;
    gfx_disp_p = gfx_disp;
    gfx_disp_d = gfx_disp + ((GFX_SIZE + sizeof(*gfx_disp_d) - 1) / (sizeof (*gfx_disp_d) * sizeof(gfx_disp_d)));

    /* write out data cache to memory */
    osWritebackDCache(gfx_disp_work, GFX_SIZE);
}

/* Custom display list functions */

void gfx_append(Gfx *gfx, size_t size){
#ifdef WIIVC
    hmemcpy(gfx_disp_p, gfx, size);
#else
    memcpy(gfx_disp_p, gfx,size);
#endif
    gfx_disp_p += (size + sizeof(*gfx_disp_p) - 1) / sizeof(*gfx_disp_p);
}

void *gfx_data_push(void *data, size_t size){
    gfx_disp_d -= (size + sizeof(*gfx_disp_d) - 1) / sizeof(*gfx_disp_d);
#ifdef WIIVC
    hmemcpy(gfx_disp_d, data, size);
#else
    memcpy(gfx_disp_d, data, size);
#endif
    return gfx_disp_d;
}

void gfx_load_tile(gfx_texture *texture, uint16_t tilenum){
    void *data = NULL;
    if(texture->source == TEX_SRC_DRAM){
        data = texture->data;
    } else {
        gSPSegment(gfx_disp_p++, 0xB, 0xA8060000);
        data = (void*)(((uint32_t)texture->data - 0xA8060000) | 0x0B000000);
    }
    data = (void*)((uint32_t)data + (texture->tile_size * tilenum));
    if(texture->img_size == G_IM_SIZ_4b){
        gDPLoadTextureTile_4b(gfx_disp_p++, data,
            texture->img_fmt, texture->tile_width, texture->tile_height,
            0, 0, texture->tile_width - 1, texture->tile_height-1,
            0,
            G_TX_NOMIRROR | G_TX_WRAP,
            G_TX_NOMIRROR | G_TX_WRAP,
            G_TX_NOMASK, G_TX_NOMASK,
            G_TX_NOLOD, G_TX_NOLOD);
    }else{
        gDPLoadTextureTile(gfx_disp_p++, data,
            texture->img_fmt, texture->img_size,
            texture->tile_width, texture->tile_height,
            0, 0, texture->tile_width - 1, texture->tile_height-1,
            0,
            G_TX_NOMIRROR | G_TX_WRAP,
            G_TX_NOMIRROR | G_TX_WRAP,
            G_TX_NOMASK, G_TX_NOMASK,
            G_TX_NOLOD, G_TX_NOLOD);
    }
    rdp_synced = 1;
}

/* texture loading functions */

void gfx_texture_desaturate(void *data, size_t len){
    struct rgba {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    MtxF m = guDefMtxF(0.3086f, 0.6094f, 0.0820f, 0.f,
                       0.3086f, 0.6094f, 0.0820f, 0.f,
                       0.3086f, 0.6094f, 0.0820f, 0.f,
                       0.f,     0.f,     0.f,     1.f);

    struct rgba *datargba = data;
    for(int i = 0;i < len / sizeof(struct rgba);i++){
        struct rgba p = datargba[i];
        float r = p.r * m.xx + p.g * m.xy + p.g * m.xz + p.a * m.xw;
        float g = p.r * m.yx + p.g * m.yy + p.g * m.yz + p.a * m.yw;
        float b = p.r * m.zx + p.g * m.zy + p.g * m.zz + p.a * m.zw;
        float a = p.r * m.wx + p.g * m.wy + p.g * m.wz + p.a * m.ww;
        struct rgba n = {
            r < 0 ? 0 : r > 0xFF ? 0xFF : r,
            g < 0 ? 0 : g > 0xFF ? 0xFF : g,
            b < 0 ? 0 : g > 0xFF ? 0xFF : b,
            a < 0 ? 0 : a > 0xFF ? 0xFF : a,
        };
        datargba[i] = n;
    }
}

void gfx_destroy_texture(gfx_texture *texture){
    if(texture){
        if(texture->data){
            free(texture->data);
        }
        free(texture);
    }
}

/* sprite drawing functions */

void gfx_draw_sprite_color(gfx_texture *texture, int x, int y, int tile, int width, int height, uint32_t color){
    gfx_load_tile(texture, tile);
    rdp_mode_set_apply(RDP_MODE_COLOR, color);
    rdp_mode_replace(RDP_MODE_COMBINE, G_CC_MODE(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM));
    float x_scale_calc = ((float)width / (float)texture->tile_width);
    float y_scale_calc = ((float)height / (float)texture->tile_height);
    if(x_scale_calc != 1.f || y_scale_calc != 1.f){
        rdp_mode_replace(RDP_MODE_FILTER, G_TF_BILERP);
    }else{
        rdp_mode_replace(RDP_MODE_FILTER, G_TF_POINT);
    }
    gSPScisTextureRectangle(gfx_disp_p++,
                         qs102(x) & ~3,
                         qs102(y) & ~3,
                         qs102(x + texture->tile_width * x_scale_calc + 1) & ~3,
                         qs102(y + texture->tile_height * y_scale_calc + 1) & ~3,
                         G_TX_RENDERTILE,
                         qu105(0),
                         qu105(0),
                         qu510(1.0f / x_scale_calc), qu510(1.0f / y_scale_calc));
    rdp_mode_pop(RDP_MODE_FILTER);
    rdp_mode_pop(RDP_MODE_COMBINE);
    rdp_synced = 0;
}

void gfx_draw_sprite(gfx_texture *texture, int x, int y, int tile, int width, int height){
    gfx_draw_sprite_color(texture, x, y, tile, width, height, DEFAULT_COLOR);
}

void gfx_draw_rectangle(int x, int y, int width, int height, uint32_t color){
    rdp_mode_set_apply(RDP_MODE_COLOR, color);
    rdp_mode_replace(RDP_MODE_COMBINE, G_CC_MODE(G_CC_PRIMITIVE,G_CC_PRIMITIVE));
    gSPScisTextureRectangle(gfx_disp_p++,
                            qs102(x), qs102(y),
                            qs102(x + width), qs102(y + height),
                            0, 0, 0, 0, 0);
    rdp_mode_pop(RDP_MODE_COMBINE);
}

/* text printing functions */

static void gfx_printf_va_color_fast(uint16_t left, uint16_t top, uint32_t color, const char *format, va_list va) {
    const size_t buf_len = 1024;
    char buf[buf_len];
    gfx_font *font = kfont;
    int s_len = vsnprintf(buf, buf_len, format, va);
    if(s_len >= buf_len) {
        s_len = buf_len - 1;
    }

    uint16_t x = left + 1;
    int chars_per_tile = font->cx_tile * font->cy_tile;
    uint32_t tmp_color = color & 0xFF;
    for(int i = 0; i < s_len; i++, x += font->c_width) {
        char c = buf[i];
        if(c < 33) {
            continue;
        }
        c -= 33;
        int tile_idx = c / chars_per_tile;
        int tile_char = c % chars_per_tile;
        gfx_char_t gchar = {
            tile_char, tmp_color, x, top + 1
        };
        vector_push_back(&gfx_chars[tile_idx], 1, &gchar);
    }

    x = left;
    for(int i = 0; i < s_len; i++, x += font->c_width) {
        char c = buf[i];
        if(c < 33) {
            continue;
        }
        c -= 33;
        int tile_idx = c / chars_per_tile;
        int tile_char = c % chars_per_tile;
        gfx_char_t gchar = {
            tile_char, color, x, top
        };
        vector_push_back(&gfx_chars[tile_idx], 1, &gchar);
    }
}

static void draw_chars(void) {
    const gfx_font *font = kfont;
    uint32_t color = 0x00000000;
    _Bool first = 1;
    for(int i = 0; i < 8; i++) {
        struct vector *tile_vec = &gfx_chars[i];
        for(int j = 0; j < tile_vec->size; j++) {
            gfx_char_t *gchar = vector_at(tile_vec, j);
            if(j == 0) {
                gfx_load_tile(font->texture, i);
            }
            if(first || color != gchar->color) {
                color = gchar->color;
                rdp_sync();
                gDPSetPrimColor(gfx_disp_p++, 0, 0,
                                (color >> 24) & 0xFF, (color >> 16) & 0xFF,
                                (color >> 8) & 0xFF, color & 0xFF);
            }
            first = 0;
            gSPScisTextureRectangle(gfx_disp_p++,
                                    qs102(gchar->x),
                                    qs102(gchar->y),
                                    qs102(gchar->x + font->c_width),
                                    qs102(gchar->y + font->c_height),
                                    G_TX_RENDERTILE,
                                    qu105(gchar->tile % font->cx_tile * font->c_width),
                                    qu105(gchar->tile / font->cx_tile * font->c_height),
                                    qu510(1), qu510(1));
            rdp_synced = 0;
        }
        vector_clear(tile_vec);
    }
}

void gfx_printf(uint16_t left, uint16_t top, const char *format, ...){
    va_list args;
    va_start(args, format);
    gfx_printf_va_color_fast(left, top, DEFAULT_COLOR, format, args);
    va_end(args);
}

void gfx_printf_color(uint16_t left, uint16_t top, uint32_t color, const char *format, ...){
    va_list args;
    va_start(args, format);
    gfx_printf_va_color_fast(left, top, color, format, args);
    va_end(args);
}

gfx_texture *gfx_load_item_texture(uint8_t item_id){
    const size_t size = TILE_SIZE(32, 32, G_IM_SIZ_32b);
    gfx_texture *texture = malloc(sizeof(*texture));

    if(texture){
        texture->tile_size = size;
        texture->x_tiles = 1;
        texture->y_tiles = 2;
        texture->tile_width = 32;
        texture->tile_height = 32;
        texture->img_fmt = G_IM_FMT_RGBA;
        texture->img_size = G_IM_SIZ_32b;        
#ifdef WIIVC
        texture->source = TEX_SRC_HB;
        texture->data = halloc(size * 2);
#else
        texture->source = TEX_SRC_DRAM;
        texture->data = malloc(size * 2);
#endif
        z2_DecodeArchiveFile(z2_file_table[z2_item_icon_archive].prom_start, item_id, tex_buf, size);
        // Copy colored texture to hb heap

#ifdef WIIVC
        hmemcpy(texture->data, tex_buf, size);
#else
        memcpy(texture->data, tex_buf, size);
#endif
        gfx_texture_desaturate(tex_buf, size);

        // copy grey texture to hb heap
#ifdef WIIVC
        hmemcpy(texture->data + size, tex_buf, size);
#else
        memcpy(texture->data + size, tex_buf, size);
#endif

    }
    return texture;
}

static gfx_texture *gfx_load_archive(texture_loader *loader){
    gfx_texture *texture = malloc(sizeof(*texture));
    if(texture){
        int tiles_cnt = loader->end_item - loader->start_item + 1;
        size_t tile_size = TILE_SIZE(loader->width, loader->height, loader->size);
        texture->tile_size = tile_size;
        texture->img_fmt = loader->format;
        texture->img_size = loader->size;
        texture->tile_width = loader->width;
        texture->tile_height = loader->height;
        texture->x_tiles = 1;
        texture->y_tiles = tiles_cnt * (loader->desaturate ? 2 : 1);

#if WIIVC
        texture->source = TEX_SRC_HB;
        texture->data = halloc(tile_size * texture->y_tiles);
#else
        texture->source= TEX_SRC_DRAM;
        texture->data = malloc(tile_size * texture->y_tiles);
#endif

        int i;
        uint8_t j;
        for(i = 0, j = loader->start_item; i < tiles_cnt; i++, j++){
            z2_DecodeArchiveFile(z2_file_table[loader->file].prom_start, j, tex_buf, tile_size);
            hmemcpy((char*)texture->data + (tile_size * i), tex_buf, tile_size);
            if(loader->desaturate){
                // Copy newly decoded file and desaturate
                gfx_texture_desaturate(tex_buf, tile_size);
                hmemcpy((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i), tex_buf, tile_size);
            }
        }
    }
    return texture;
}

static gfx_texture *gfx_load_from_rom(texture_loader *loader){
    gfx_texture *texture = malloc(sizeof(*texture));
    if(texture){
        size_t tile_size = TILE_SIZE(loader->width, loader->height, loader->size);
        texture->tile_size = tile_size;
        texture->img_fmt = loader->format;
        texture->img_size = loader->size;
        texture->tile_width = loader->width;
        texture->tile_height = loader->height;
        texture->x_tiles = 1;
        texture->y_tiles = loader->num_tiles * (loader->desaturate ? 2 : 1);
#if WIIVC
        texture->source = TEX_SRC_HB;
        texture->data = halloc(texture->tile_size * texture->y_tiles);
#else
        texture->source = TEX_SRC_DRAM;
        texture->data = malloc(texture->tile_size * texture->y_tiles);
#endif
        uint16_t file = loader->file;
        void *tempdata = malloc(z2_file_table[file].vrom_end - z2_file_table[file].vrom_start);
        zu_file_idx_load(file, tempdata);

        for(int i = 0; i < loader->num_tiles; i++) 
        {
#ifdef WIIVC
            hmemcpy(texture->data + (i * tile_size), (char*)tempdata + loader->offset + (i * tile_size), tile_size);
#else
            memcpy(texture->data + (i * tile_size), (char*)tempdata + loader->offset + (i * tile_size), tile_size);
#endif
            if(loader->desaturate)
            {
                gfx_texture_desaturate((char*)tempdata + loader->offset + (i * tile_size), tile_size);
#ifdef WIIVC
                hmemcpy((char*)texture->data + (tile_size * loader->num_tiles) + (tile_size * i), (char*)tempdata + loader->offset + (i * tile_size), tile_size);
#else
                memcpy((char*)texture->data + (tile_size * loader->num_tiles) + (tile_size * i), (char*)tempdata + loader->offset + (i * tile_size), tile_size);
#endif
            }
        }

        free(tempdata);
    }
    return texture;
}

gfx_texture *gfx_load(texture_loader *loader){
    switch(loader->source){
        case SOURCE_ARCHIVE:
            return gfx_load_archive(loader);
        case SOURCE_FILE:
            return gfx_load_from_rom(loader);
        default:
            return NULL;
    }
}
