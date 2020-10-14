#include <n64.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <n64.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "gfx.h"
#include "kzresource.h"
#include "zu.h"
#include "menu.h"
#include "cache.h"

#define TILE_SIZE(x,y,z)    ((x * y * G_SIZ_BITS(z) + 63) / 64 * 8)

#define TILE_SIZE(x,y,z)    ((x * y * G_SIZ_BITS(z) + 63) / 64 * 8)

#define RDP_STACK_LEN 5

static Gfx     *gfx_disp;
static Gfx     *gfx_disp_p;
static Gfx     *gfx_disp_d;
static Gfx     *gfx_disp_work;
static Gfx    **p_gfx_p;
static size_t   gfx_size;

static _Bool    rdp_synced;

static uint64_t rdp_modes[RDP_MODE_ALL];
static uint64_t rdp_mode_stack[RDP_MODE_ALL][RDP_STACK_LEN];
static int      rdp_stack_pos[RDP_MODE_ALL];

gfx_font       *kfont;

static Gfx      rcp_init[] = {
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

/* rdp mode functions */

static inline void rdp_sync(void){
    if(!rdp_synced){
        gDPPipeSync(gfx_disp_p++);
        rdp_synced = 1;
    }
}

static void rdp_mode_apply(enum rdp_mode mode){
    Gfx append_dl[RDP_MODE_ALL];
    Gfx *append_ptr = append_dl;
    switch(mode){
        case RDP_MODE_ALL:
        case RDP_MODE_COLOR:
        {
            uint32_t color = rdp_modes[RDP_MODE_COLOR];
            gDPSetPrimColor(append_ptr++, 0, 0,
                            (color >> 24) & 0xFF,
                            (color >> 16) & 0xFF,
                            (color >> 8) & 0xFF,
                            color & 0xFF);

            if(mode != RDP_MODE_ALL){
                break;
            }
        }
        case RDP_MODE_COMBINE:{
            gDPSetCombine(append_ptr++, rdp_modes[RDP_MODE_COMBINE]);
            if(mode != RDP_MODE_ALL){
                break;
            }
        }
        case RDP_MODE_FILTER:{
            gDPSetTextureFilter(append_ptr++, rdp_modes[RDP_MODE_FILTER]);
            if(mode != RDP_MODE_ALL){
                break;
            }
        }
        default:
            break;
    }
    size_t size = append_ptr - append_dl;
    if(size > 0){
        rdp_sync();
        memcpy(gfx_disp_p, append_dl, size * sizeof(*append_dl));
        gfx_disp_p += size;
    }
}

static void rdp_mode_set(enum rdp_mode mode, uint64_t val){
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

void gfx_init(size_t size, gfx_texture *font, Gfx **gfx_append_p){
    gfx_disp = malloc(size);
    gfx_disp_work = malloc(size);
    gfx_disp_p = gfx_disp;
    gfx_disp_d = gfx_disp + ((gfx_size + sizeof(*gfx_disp_d) - 1) / (sizeof (*gfx_disp_d) * sizeof(gfx_disp_d)));
    gfx_size = size;
    p_gfx_p = gfx_append_p;

    kfont = malloc(sizeof(gfx_font));
    kfont->texture = font;
    kfont->c_width = 8;
    kfont->c_height = 8;
    kfont->cx_tile = 2;
    kfont->cy_tile = 16;
}

void gfx_begin(void){
    rdp_sync();
    gSPDisplayList(gfx_disp_p++, &rcp_init);
    rdp_mode_apply(RDP_MODE_ALL);
}

void gfx_finish(void){
    gSPEndDisplayList(gfx_disp_p++);
    gSPDisplayList((*p_gfx_p)++, gfx_disp);
    Gfx *disp_w = gfx_disp_work;
    gfx_disp_work = gfx_disp;
    gfx_disp = disp_w;
    gfx_disp_p = gfx_disp;
    gfx_disp_d = gfx_disp + ((gfx_size + sizeof(*gfx_disp_d) - 1) / (sizeof (*gfx_disp_d) * sizeof(gfx_disp_d)));

    /* write out data cache to memory */
    osWritebackDCache(gfx_disp_work, gfx_size);
}

void gfx_finish_set(Gfx **gfx_append_p){
    p_gfx_p = gfx_append_p;
}

/* Custom display list functions */

void gfx_append(Gfx *gfx, size_t size){
    memcpy(gfx_disp_p,  gfx,size);
    gfx_disp_p += (size + sizeof(*gfx_disp_p) - 1) / sizeof(*gfx_disp_p);
}

void *gfx_data_push(void *data, size_t size){
    gfx_disp_d -= (size + sizeof(*gfx_disp_d) - 1) / sizeof(*gfx_disp_d);
    memcpy(gfx_disp_d, data, size);
    return gfx_disp_d;
}

void gfx_load_tile(gfx_texture *texture, uint16_t tilenum){
    if(texture->img_size == G_IM_SIZ_4b){
        gDPLoadTextureTile_4b(gfx_disp_p++, texture->data + (texture->tile_size * tilenum),
            texture->img_fmt, texture->tile_width, texture->tile_height,
            0, 0, texture->tile_width - 1, texture->tile_height-1,
            0,
            G_TX_NOMIRROR | G_TX_WRAP,
            G_TX_NOMIRROR | G_TX_WRAP,
            G_TX_NOMASK, G_TX_NOMASK,
            G_TX_NOLOD, G_TX_NOLOD);
    }else{
        gDPLoadTextureTile(gfx_disp_p++, texture->data + (texture->tile_size * tilenum),
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

static void gfx_printchars(gfx_font *font, uint16_t x, uint16_t y, uint32_t color, const char *chars, size_t charcnt){
    rdp_mode_set_apply(RDP_MODE_COLOR, color);
    rdp_mode_replace(RDP_MODE_COMBINE, G_CC_MODE(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM));
    int chars_per_tile = font->cx_tile * font->cy_tile;

    for(int i = 0;i < font->texture->x_tiles * font->texture->y_tiles;i++){
        int tile_start = chars_per_tile * i;
        int tile_end = tile_start + chars_per_tile;
        gfx_load_tile(font->texture, i);
        int char_x = 0;
        rdp_mode_replace(RDP_MODE_COLOR, color & 0xFF);
        for(int j = 0;j < charcnt; j++, char_x += font->c_width){
            char c = chars[j];
            if(c < 33){
                continue;
            }
            c -= 33;
            if(c < tile_start || c >= tile_end){
                continue;
            }
            c -= tile_start;

            gSPScisTextureRectangle(gfx_disp_p++,
                         qs102(x + char_x + 1 ),
                         qs102(y + 1),
                         qs102(x + char_x + font->c_width + 1),
                         qs102(y + font->c_height + 1),
                         G_TX_RENDERTILE,
                         qu105(c % font->cx_tile *
                               font->c_width),
                         qu105(c / font->cx_tile *
                               font->c_height),
                         qu510(1.0f), qu510(1.0f));
        }
        rdp_mode_pop(RDP_MODE_COLOR);
        rdp_sync();
        char_x = 0;
        for(int j = 0;j < charcnt;j++, char_x += font->c_width){
            char c = chars[j];
            if(c < 33){
                continue;
            }
            c -= 33;
            if(c < tile_start || c >= tile_end){
                continue;
            }
            c -= tile_start;
            gSPScisTextureRectangle(gfx_disp_p++,
                         qs102(x + char_x),
                         qs102(y),
                         qs102(x + char_x + font->c_width),
                         qs102(y + font->c_height),
                         G_TX_RENDERTILE,
                         qu105(c % font->cx_tile *
                               font->c_width),
                         qu105(c / font->cx_tile *
                               font->c_height),
                         qu510(1.0f), qu510(1.0f));
        }
        rdp_synced = 0;
    }
}

static void gfx_printf_va_color(uint16_t left, uint16_t top, uint32_t color, const char *format, va_list va){
    const size_t max_len = 1024;
    char buf[max_len];
    int l = vsnprintf(buf, max_len, format, va);
    if(l > max_len - 1){
        l = max_len - 1;
    }

    gfx_printchars(kfont, left, top, color, buf, l);
}

void gfx_printf(uint16_t left, uint16_t top, const char *format, ...){
    va_list args;
    va_start(args, format);
    gfx_printf_va_color(left, top, DEFAULT_COLOR, format, args);
    va_end(args);
}

void gfx_printf_color(uint16_t left, uint16_t top, uint32_t color, const char *format, ...){
    va_list args;
    va_start(args, format);
    gfx_printf_va_color(left, top, color, format, args);
    va_end(args);
}

gfx_texture *gfx_load_item_texture(uint8_t item_id){
    size_t size = TILE_SIZE(32, 32, G_IM_SIZ_32b);
    gfx_texture *texture = malloc(sizeof(*texture));
    if(texture){
        texture->tile_size = size;
        texture->x_tiles = 1;
        texture->y_tiles = 2;
        texture->tile_width = 32;
        texture->tile_height = 32;
        texture->img_fmt = G_IM_FMT_RGBA;
        texture->img_size = G_IM_SIZ_32b;
        texture->data = malloc(size * 2);
        z2_DecodeArchiveFile(z2_file_table[z2_item_icon_archive].prom_start, item_id, texture->data, size);
        memcpy((char*)texture->data + size, (char*)texture->data,size);
        gfx_texture_desaturate((char*)texture->data + size, size);
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

        texture->data = malloc((tile_size * tiles_cnt) * (loader->desaturate ? 2 : 1));
        int i;
        uint8_t j;
        for(i = 0, j = loader->start_item;i < tiles_cnt;i++, j++){
            z2_DecodeArchiveFile(z2_file_table[loader->file].prom_start, j, (char*)texture->data + (tile_size * i), tile_size);
            if(loader->desaturate){
                // Copy newly decoded file and desaturate
                memcpy((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i), (char*)texture->data + (tile_size * i), tile_size);
                gfx_texture_desaturate((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i), tile_size);
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
        texture->data = malloc((texture->tile_size * loader->num_tiles) * (loader->desaturate ? 2 : 1));
        texture->img_fmt = loader->format;
        texture->img_size = loader->size;
        texture->tile_width = loader->width;
        texture->tile_height = loader->height;
        texture->x_tiles = 1;
        texture->y_tiles = loader->num_tiles * (loader->desaturate ? 2 : 1);
        uint16_t file = loader->file;
        void *tempdata = malloc(z2_file_table[file].vrom_end - z2_file_table[file].vrom_start);
        zu_file_idx_load(file, tempdata);
        memcpy(texture->data, (char*)tempdata + loader->offset,tile_size * loader->num_tiles);
        free(tempdata);
        if(loader->desaturate){
            int tiles_cnt = loader->num_tiles;
            for(int i = 0;i < tiles_cnt;i++){
                memcpy((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i), (char*)texture->data + (tile_size * i), tile_size);
                gfx_texture_desaturate((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i),tile_size);
            }
        }
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
