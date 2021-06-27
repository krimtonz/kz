#include "line_writer.h"
#include "kz.h"

void init_line_gfx(Gfx **gfx_p, Gfx **gfx_d, _Bool xlu) {
    uint32_t render_mode1;
    uint32_t render_mode2;
    uint8_t alpha;

    if(xlu) {
        render_mode1 = G_RM_AA_XLU_LINE;
        render_mode2 = G_RM_AA_XLU_LINE2;
        alpha = 0x80;
    } else {
        render_mode1 = G_RM_AA_ZB_DEC_LINE;
        render_mode2 = G_RM_AA_ZB_DEC_LINE2;
        alpha = 0xFF;
    }

    Mtx *mtx_mv = gDisplayListAlloc(gfx_d, sizeof(*mtx_mv));
    guMtxIdent(mtx_mv);

    gSPLoadGeometryMode((*gfx_p)++, G_ZBUFFER);
    gSPTexture((*gfx_p)++, qu016(0.5), qu016(0.5), 0, G_TX_RENDERTILE, G_OFF);
    gSPMatrix((*gfx_p)++, mtx_mv, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);

    gDPPipeSync((*gfx_p)++);
    gDPSetCycleType((*gfx_p)++, G_CYC_1CYCLE);
    gDPSetRenderMode((*gfx_p)++, render_mode1, render_mode2);
    gDPSetCombineMode((*gfx_p)++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor((*gfx_p)++, 0, 0, 0, 0, 0, alpha);
}

void line_writer_init(line_writer_t *writer, Gfx *gfx_p, Gfx *gfx_d) {
    writer->p = gfx_p;
    writer->d = gfx_d;
    writer->vtx_cmd = NULL;
    writer->vtx_cnt = 0;
}

void line_writer_flush(line_writer_t *writer) {
    if(writer->vtx_cmd != NULL) {
        size_t vtx_size = sizeof(*writer->vtx) * writer->vtx_cnt;
        Vtx *vtx_ptr = gDisplayListAlloc(&writer->d, vtx_size);
#ifdef WIIVC
        hmemcpy(vtx_ptr, writer->vtx, vtx_size);
        vtx_ptr = HB_SEG(vtx_ptr);
#else
        memcpy(vtx_ptr, writer->vtx, vtx_size);
#endif
        *writer->vtx_cmd = gsSPVertex(vtx_ptr, writer->vtx_cnt, 0);
        writer->vtx_cmd = NULL;
    }

    writer->vtx_cnt = 0;
}

void line_writer_finish(line_writer_t *writer, Gfx **gfx_p, Gfx **gfx_d) {
    line_writer_flush(writer);

    *gfx_p = writer->p;
    *gfx_d = writer->d;
}

void line_writer_append(line_writer_t *writer, Vtx *vtx, size_t vtx_cnt) {
    if(vtx_cnt < 2) {
        return;
    }

    if(writer->vtx_cnt + vtx_cnt > 32) {
        line_writer_flush(writer);
    }

    if(writer->vtx_cmd == NULL) {
        writer->vtx_cmd = writer->p++;
    }

    memcpy(&writer->vtx[writer->vtx_cnt], vtx, sizeof(*vtx) * vtx_cnt);

    for(int i = 0; i < vtx_cnt; i++) {
        for(int j = 0; j < vtx_cnt; j++) {
            gSPLine3D(writer->p++, writer->vtx_cnt + i, writer->vtx_cnt + j, 0);
        }
    }

    writer->vtx_cnt += vtx_cnt;
}
