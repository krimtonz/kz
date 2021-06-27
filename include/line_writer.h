#ifndef _LINE_WRITER_H
#define _LINE_WRITER_H

#include "z2.h"

typedef struct {
    Gfx *p;
    Gfx *d;
    Vtx vtx[32];
    Gfx *vtx_cmd;
    int vtx_cnt;
} line_writer_t;

void init_line_gfx(Gfx **gfx_p, Gfx **gfx_d, _Bool xlu);
void line_writer_init(line_writer_t *writer, Gfx *gfx_p, Gfx *gfx_d);
void line_writer_append(line_writer_t *writer, Vtx *vtx, size_t vtx_cnt);
void line_writer_flush(line_writer_t *writer);
void line_writer_finish(line_writer_t *writer, Gfx **gfx_p, Gfx **gfx_d);

#endif
