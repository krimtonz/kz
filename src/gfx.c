#include <n64.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "gfx.h"
#include "resource.h"

#define     GFX_SIZE 0x7500

static Gfx *gfx_disp;
static Gfx *gfx_disp_p;
static Gfx *gfx_disp_d;
static Gfx *gfx_disp_work;

gfx_font *kfont;

static Gfx kzgfx[] = {
    gsDPPipeSync(),

    gsSPLoadGeometryMode(0),
    gsDPSetScissor(G_SC_NON_INTERLACE,
              0, 0, Z2_SCREEN_WIDTH, Z2_SCREEN_HEIGHT),

    gsDPSetOtherMode(G_CYC_1CYCLE | G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TD_CLAMP | G_TP_NONE | G_TL_TILE | G_TT_NONE | G_PM_NPRIMITIVE | G_TF_POINT,
                    G_ZS_PRIM | G_AC_NONE | G_RM_XLU_SURF | G_RM_XLU_SURF2),
    gsDPSetCombine(G_CC_MODE(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM)),
    gsSPEndDisplayList()
};

void gfx_printf(uint16_t left, uint16_t top, const char *format, ...){
    va_list args;
    va_start(args,format);
    gfx_printf_va_color_scale(left,top,GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF),1.0f,1.0f,format,args);
    va_end(args);
}

void gfx_printf_scale(uint16_t left, uint16_t top, float x_scale, float y_scale, const char *format, ...){
    va_list args;
    va_start(args,format);
    gfx_printf_va_color_scale(left,top,GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF),x_scale,y_scale,format,args);
    va_end(args);
}

void gfx_printf_color_scale(uint16_t left, uint16_t top, uint32_t color, float x_scale, float y_scale, const char *format, ...){
    va_list args;
    va_start(args,format);
    gfx_printf_va_color_scale(left,top,color,x_scale,y_scale,format,args);
    va_end(args);
}

void gfx_printf_color(uint16_t left, uint16_t top, uint32_t color, const char *format, ...){
    va_list args;
    va_start(args,format);
    gfx_printf_va_color_scale(left,top,color,1.0f,1.0f,format,args);
    va_end(args);
}

void gfx_printf_va_color_scale(uint16_t left, uint16_t top, uint32_t color,float x_scale, float y_scale, const char *format, va_list va){
    const size_t max_len = 1024;
    char buf[max_len];
    int l = vsnprintf(buf,max_len,format,va);
    if(l>max_len-1) l=max_len-1;

    gfx_printchars(kfont, left, top, color, buf, l, x_scale, y_scale);
}

void gfx_init(){
    gfx_disp = malloc(GFX_SIZE * sizeof(*gfx_disp));
    gfx_disp_work = malloc(GFX_SIZE * sizeof(*gfx_disp));
    gfx_disp_p = gfx_disp;
    gfx_disp_d = gfx_disp + (GFX_SIZE + sizeof(*gfx_disp) - 1) / sizeof(*gfx_disp);

    kfont = malloc(sizeof(gfx_font));
    kfont->texture = resource_get(R_KZ_FONT);
    kfont->c_width = 8;
    kfont->c_height = 8;
    kfont->cx_tile = 2;
    kfont->cy_tile = 16;
}

void gfx_begin(){
    gSPDisplayList(gfx_disp_p++,&kzgfx);
}

void gfx_finish(){
    gSPEndDisplayList(gfx_disp_p++);
    gSPDisplayList(z2_game.common.gfx->overlay.p++, gfx_disp);
    Gfx *disp_w = gfx_disp_work;
    gfx_disp_work = gfx_disp;
    gfx_disp = disp_w;
    gfx_disp_p = gfx_disp;
    gfx_disp_d = gfx_disp + (GFX_SIZE + sizeof(*gfx_disp) - 1) / sizeof(*gfx_disp);
}

void gfx_push(Gfx gfx){
    *(gfx_disp_p++) = gfx;
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
}

void gfx_texture_desaturate(void *data, size_t len){
    struct rgba{
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
    for(int i=0;i<len/sizeof(struct rgba);i++){
        struct rgba p = datargba[i];
        float r = p.r * m.xx + p.g*m.xy + p.g*m.xz + p.a*m.xw;
        float g = p.r * m.yx + p.g*m.yy + p.g*m.yz + p.a*m.yw;
        float b = p.r * m.zx + p.g*m.zy + p.g*m.zz + p.a*m.zw;
        float a = p.r * m.wx + p.g*m.wy + p.g*m.wz + p.a*m.ww;
        struct rgba n = {
            r<0?0:r>0xFF?0xFF:r,
            g<0?0:g>0xFF?0xFF:g,
            b<0?0:g>0xFF?0xFF:b,
            a<0?0:a>0xFF?0xFF:a,
        };
        datargba[i] = n;
    }
}

inline size_t calc_tile_size(int width, int height, g_isiz_t size){
    return ((width * height * G_SIZ_BITS(size) + 7) / 8 + 63) / 64 * 64;
}

gfx_texture *gfx_load_item_texture(uint8_t item_id){
    size_t size = calc_tile_size(32,32,G_IM_SIZ_32b);
    gfx_texture *texture = malloc(sizeof(*texture));
    if(texture){
        texture->tile_size = size;
        texture->x_tiles = 1;
        texture->y_tiles = 2;
        texture->tile_width = 32;
        texture->tile_height = 32;
        texture->img_fmt = G_IM_FMT_RGBA;
        texture->img_size = G_IM_SIZ_32b;
        texture->data = memalign(64,size * 2);
        z2_DecodeArchiveFile(z2_file_table[ICON_ITEM_STATIC].prom_start,item_id,texture->data);
        memcpy((char*)texture->data + size,(char*)texture->data,size);
        gfx_texture_desaturate((char*)texture->data + size, size);
    }
    return texture;
}

static gfx_texture *gfx_load_archive(texture_loader *loader){
    gfx_texture *texture = malloc(sizeof(*texture));
    if(texture){
        int tiles_cnt = loader->end_item - loader->start_item + 1;
        size_t tile_size = calc_tile_size(loader->width,loader->height,loader->size);
        texture->tile_size = tile_size;
        texture->img_fmt = loader->format;
        texture->img_size = loader->size;
        texture->tile_width = loader->width;
        texture->tile_height = loader->height;
        texture->x_tiles = 1;
        texture->y_tiles = tiles_cnt * (loader->desaturate?2:1);

        texture->data = memalign(64, (tile_size * tiles_cnt) * (loader->desaturate?2:1));
        int i;
        uint8_t j;
        for(i=0,j=loader->start_item;i<tiles_cnt;i++,j++){
            z2_DecodeArchiveFile(z2_file_table[loader->file].prom_start,j,(char*)texture->data + (tile_size * i));
            if(loader->desaturate){
                // Copy newly decoded file and desaturate
                memcpy((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i), (char*)texture->data + (tile_size * i), tile_size);
                gfx_texture_desaturate((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i),tile_size);
            }
        }
    }
    return texture;
}

static gfx_texture *gfx_load_from_rom(texture_loader *loader){
    gfx_texture *texture = malloc(sizeof(*texture));
    if(texture){
        size_t tile_size = calc_tile_size(loader->width,loader->height,loader->size);
        texture->tile_size = tile_size;
        texture->data = memalign(64, (texture->tile_size * loader->num_tiles) * (loader->desaturate?2:1));
        texture->img_fmt = loader->format;
        texture->img_size = loader->size;
        texture->tile_width = loader->width;
        texture->tile_height = loader->height;
        texture->x_tiles = 1;
        texture->y_tiles = loader->num_tiles * (loader->desaturate?2:1);
        uint16_t file = loader->file;
        void *tempdata = malloc(z2_file_table[file].vrom_end - z2_file_table[file].vrom_start);
        OSMesgQueue queue;
        OSMesg msg;
        osCreateMesgQueue(&queue,&msg,1);
        z2_getfile_t getfile = {
            z2_file_table[file].vrom_start, tempdata,
            z2_file_table[file].vrom_end - z2_file_table[file].vrom_start,
            NULL, 0, 0,
            &queue, 0
        };
        osSendMesg(&z2_file_msgqueue, &getfile, OS_MESG_NOBLOCK);
        osRecvMesg(&queue,NULL,OS_MESG_BLOCK);
        memcpy(texture->data,(char*)tempdata + loader->offset,tile_size * loader->num_tiles);
        free(tempdata);
        if(loader->desaturate){
            int tiles_cnt = loader->num_tiles;
            for(int i=0;i<tiles_cnt;i++){
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

void gfx_draw_sprite_scale(gfx_texture *texture, int x, int y, int tile, int width, int height, float x_scale, float y_scale){
    gfx_load_tile(texture, tile);
    float x_scale_calc = ((float)width / (float)texture->tile_width) * x_scale;
    float y_scale_calc = ((float)height / (float)texture->tile_height) * y_scale;
    gSPScisTextureRectangle(gfx_disp_p++,
                         qs102(x) & ~3,
                         qs102(y) & ~3,
                         qs102(x + texture->tile_width * x_scale_calc + 1) & ~3,
                         qs102(y + texture->tile_height * y_scale_calc + 1) & ~3,
                         G_TX_RENDERTILE,
                         qu105(0),
                         qu105(0),
                         qu510(1.0f / x_scale_calc), qu510(1.0f / y_scale_calc));
}

void gfx_draw_sprite(gfx_texture *texture, int x, int y, int tile, int width, int height){
    gfx_draw_sprite_scale(texture,x,y,tile,width,height,1,1);
}

void gfx_draw_rectangle(int x, int y, int width, int height, uint32_t color){
    gDPSetCombineMode(gfx_disp_p++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(gfx_disp_p++,0,0,(color >> 24) & 0xFF,(color >> 16) & 0xFF,(color >> 8) & 0xFF,color & 0xFF);
    gDPPipeSync(gfx_disp_p++);
    gDPFillRectangle(gfx_disp_p++,x,y,x + width, y + height);
}

void gfx_destroy_texture(gfx_texture *texture){
    if(texture){
        if(texture->data) free(texture->data);
        free(texture);
    }
}

void gfx_printchars(gfx_font *font, uint16_t x, uint16_t y, uint32_t color, const char *chars, size_t charcnt, float x_scale, float y_scale){

    int chars_per_tile = font->cx_tile * font->cy_tile;

    for(int i=0;i<font->texture->x_tiles * font->texture->y_tiles;i++){
        int tile_start = chars_per_tile*i;
        int tile_end = tile_start + chars_per_tile;

        gfx_load_tile(font->texture,i);
        int char_x = 0;

        gDPSetPrimColor(gfx_disp_p++, 0, 0, 0x00,0x00,0x00, 0xFF);
        for(int j=0;j<charcnt;j++, char_x += font->c_width*x_scale){
            char c = chars[j];
            if(c<33) continue;
            c-=33;
            if(c<tile_start || c>=tile_end) continue;
            c-=tile_start;

            gSPScisTextureRectangle(gfx_disp_p++,
                         qs102(x + char_x + 1 ),
                         qs102(y + 1),
                         qs102(x + char_x + font->c_width*x_scale + 1),
                         qs102(y + font->c_height*y_scale + 1),
                         G_TX_RENDERTILE,
                         qu105(c % font->cx_tile *
                               font->c_width),
                         qu105(c / font->cx_tile *
                               font->c_height),
                         qu510(1.0f/x_scale), qu510(1.0f/y_scale));
        }
        char_x=0;
        gDPSetPrimColor(gfx_disp_p++, 0, 0, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, 0xFF);
        for(int j=0;j<charcnt;j++, char_x += font->c_width*x_scale){
            char c = chars[j];
            if(c<33) continue;
            c-=33;
            if(c<tile_start || c>=tile_end) continue;
            c-=tile_start;
            gSPScisTextureRectangle(gfx_disp_p++,
                         qs102(x + char_x),
                         qs102(y),
                         qs102(x + char_x + font->c_width*x_scale),
                         qs102(y + font->c_height*y_scale),
                         G_TX_RENDERTILE,
                         qu105(c % font->cx_tile *
                               font->c_width),
                         qu105(c / font->cx_tile *
                               font->c_height),
                         qu510(1.0f/x_scale), qu510(1.0f/y_scale));
        }
    }
}