#ifndef _POLY_WRITER_H
#define _POLY_WRITER_H

#include "z2.h"

typedef struct {
    Gfx        *p;
    Gfx        *d;
    Vtx         v[30];
    Gfx        *vtx_cmd;
    Gfx        *tri_cmd;
    int         vtx_cnt;
    uint32_t    prev_color;
} poly_writer_t;

void init_poly_list(Gfx **poly_p, Gfx **poly_d, _Bool xlu, _Bool decal);
void poly_writer_init(poly_writer_t *writer, Gfx *gfx_p, Gfx *gfx_d);
void poly_writer_append(poly_writer_t *writer, Vtx (*vtx)[3], uint32_t color);
void poly_writer_flush(poly_writer_t *writer);
void poly_writer_finish(poly_writer_t *writer, Gfx **gfx_p, Gfx **gfx_d);
#endif
