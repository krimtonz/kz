#include <n64.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "gfx.h"

#define     GFX_SIZE 0x7500

/*static Gfx *gfx_disp;
static Gfx *gfx_disp_w;
static Gfx *gfx_disp_p;
static Gfx *gfx_disp_d;*/

static z64_disp_buf_t gfx_disp;
static z64_disp_buf_t gfx_disp_work;

extern char _raw_font[];

typedef struct{
    union{
        struct{
            uint32_t c : 8;
            uint32_t x : 12;
            uint32_t y : 12;
        };
        uint32_t ch;
    };
    struct{
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } color;;
} gfx_char_t;

gfx_font *kfont;

static Gfx kzgfx[] = {
    gsDPPipeSync(),

    gsSPLoadGeometryMode(0),
    gsDPSetScissor(G_SC_NON_INTERLACE,
              0, 0, Z64_SCREEN_WIDTH, Z64_SCREEN_HEIGHT),

    gsDPSetOtherMode(G_AD_DISABLE | G_CD_DISABLE |
        G_CK_NONE | G_TC_FILT |
        G_TD_CLAMP | G_TP_NONE |
        G_TL_TILE | G_TT_NONE |
        G_PM_NPRIMITIVE | G_CYC_1CYCLE |
        G_TF_BILERP, // HI
        G_AC_NONE | G_ZS_PRIM |
        G_RM_XLU_SURF | G_RM_XLU_SURF2), // LO
    
    gsSPEndDisplayList()
};

void gfx_printf(uint16_t left, uint16_t top, const char *format, ...){
    va_list args;
    va_start(args,format);
    gfx_printf_va_color(left,top,GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF),format,args);
    va_end(args);
}

void gfx_printf_color(uint16_t left, uint16_t top, uint32_t color, const char *format, ...){
    va_list args;
    va_start(args,format);
    gfx_printf_va_color(left,top,color,format,args);
    va_end(args);
}

void gfx_printf_va_color(uint16_t left, uint16_t top, uint32_t color, const char *format, va_list va){
    const size_t max_len = 1024;
    char buf[max_len];
    int l = vsnprintf(buf,max_len,format,va);
    if(l>max_len-1) l=max_len-1;

    gfx_printchars(kfont, left, top, color, buf, l);
}

void gfx_disp_buf_init(z64_disp_buf_t *db, size_t size){
    db->size = size;
    db->buf = malloc(size);
    db->p = db->buf;
    db->d = db->buf + (size + sizeof(*(db->buf)) - 1) / sizeof(db->buf);
}

void gfx_disp_buf_copy(z64_disp_buf_t *src, z64_disp_buf_t *dst){
    dst->buf = src->buf;
    dst->size = src->size;
    dst->p = dst->buf;
    dst->d = dst->buf + (dst->size + sizeof(*(dst->buf)) - 1) / sizeof(dst->buf);
}

void gfx_disp_buf_destroy(z64_disp_buf_t *db){
    if(db->buf){
        free(db->buf);
        db->buf = NULL;
    }
    db->size = 0;
    db->p = NULL;
    db->d = NULL;
}

void gfx_disp_buf_reset(z64_disp_buf_t *db, size_t newsize){
    if(db->buf==NULL){
        gfx_disp_buf_init(db,newsize);
        return;
    }
    db->size = newsize;
    db->buf = realloc(db->buf, newsize);
    db->p = db->buf;
    db->d = db->buf + (newsize + sizeof(*(db->buf)) - 1) / sizeof(db->buf);
}

void gfx_init(){
    gfx_disp_buf_init(&gfx_disp,GFX_SIZE);
    gfx_disp_buf_init(&gfx_disp_work, GFX_SIZE);
    
    kfont = malloc(sizeof(gfx_font));
    static gfx_texture f_tex;
    f_tex.data = _raw_font;
    f_tex.img_fmt = G_IM_FMT_I;
    f_tex.img_size = G_IM_SIZ_4b;
    f_tex.tile_width = 16;
    f_tex.tile_height = 128;
    f_tex.tile_size = ((f_tex.tile_width * f_tex.tile_height * G_SIZ_BITS(f_tex.img_size) + 7) / 8 + 63) / 64 * 64;
    f_tex.x_tiles = 1;
    f_tex.y_tiles = 3; 
    kfont->texture = &f_tex;
    kfont->c_width = 8;
    kfont->c_height = 8;
    kfont->cx_tile = 2;
    kfont->cy_tile = 16;
}

void gfx_begin(){
    gSPDisplayList(gfx_disp.p++,&kzgfx);
}

void gfx_finish(){
    gSPEndDisplayList(gfx_disp.p++);
    gSPDisplayList(z64_game.common.gfx->overlay.p++, gfx_disp.buf);
    z64_disp_buf_t disp_w;
    gfx_disp_buf_copy(&gfx_disp_work, &disp_w);
    gfx_disp_buf_copy(&gfx_disp,&gfx_disp_work);
    gfx_disp_buf_copy(&disp_w, &gfx_disp);
}
void gfx_load_tile(gfx_texture *texture, uint16_t tilenum){
    gDPLoadTextureTile_4b(gfx_disp.p++, texture->data + (texture->tile_size * tilenum),
        texture->img_fmt, texture->tile_width, texture->tile_height,
        0, 0, texture->tile_width - 1, texture->tile_height-1,
        0, 
        G_TX_NOMIRROR | G_TX_WRAP,
        G_TX_NOMIRROR | G_TX_WRAP,
        G_TX_NOMASK, G_TX_NOMASK,
        G_TX_NOLOD, G_TX_NOLOD);
}

void gfx_printchars(gfx_font *font, uint16_t x, uint16_t y, uint32_t color, const char *chars, size_t charcnt){

    gDPSetCombineMode(gfx_disp.p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    int chars_per_tile = font->cx_tile * font->cy_tile;

    for(int i=0;i<font->texture->x_tiles * font->texture->y_tiles;i++){
        int tile_start = chars_per_tile*i;
        int tile_end = tile_start + chars_per_tile;

        gfx_load_tile(font->texture,i);
        int char_x = 0;
        int char_y = 0;
        for(int j=0;j<charcnt;j++, char_x += font->c_width + 2){
            char c = chars[j];
            if(c<33) continue;
            c-=33;
            if(c<tile_start || c>=tile_end) continue;
            c-=tile_start;
            gDPSetPrimColor(gfx_disp.p++, 0, 0, 0x00,0x00,0x00, 0xFF);
            gSPScisTextureRectangle(gfx_disp.p++,
                         qs102(x + char_x + 1 ),
                         qs102(y + char_y + 1),
                         qs102(x + char_x + font->c_width + 1),
                         qs102(y + char_y + font->c_height + 1),
                         G_TX_RENDERTILE,
                         qu105(c % font->cx_tile *
                               font->c_width),
                         qu105(c / font->cx_tile *
                               font->c_height),
                         qu510(1), qu510(1));
            gDPSetPrimColor(gfx_disp.p++, 0, 0, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, 0xFF);
            gSPScisTextureRectangle(gfx_disp.p++,
                         qs102(x + char_x),
                         qs102(y + char_y),
                         qs102(x + char_x + font->c_width),
                         qs102(y + char_y + font->c_height),
                         G_TX_RENDERTILE,
                         qu105(c % font->cx_tile *
                               font->c_width),
                         qu105(c / font->cx_tile *
                               font->c_height),
                         qu510(1), qu510(1));
        }
    }
}