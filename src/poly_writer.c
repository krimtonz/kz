#include "z2.h"
#include "gfx.h"
#include "poly_writer.h"
#include "hb_heap.h"

void init_poly_list(Gfx **poly_p, Gfx **poly_d, _Bool xlu, _Bool decal){
    uint32_t    render_mode;
    uint32_t    blend_cyc1;
    uint32_t    blend_cyc2;
    uint8_t     alpha;
    uint64_t    combiner;
    uint32_t    geometry;

    if(xlu){
        render_mode = Z_CMP | IM_RD | CVG_DST_FULL | FORCE_BL;
        blend_cyc1 = GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA);
        blend_cyc2 = GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA);
        alpha = 0x80;
    }else{
        render_mode = Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | FORCE_BL;
        blend_cyc1 = GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1);
        blend_cyc2 = GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1);
        alpha = 0xFF;
    }

    if(decal){
        render_mode |= ZMODE_DEC;
    }else if(xlu){
        render_mode |= ZMODE_XLU;
    }else{
        render_mode |= ZMODE_OPA;
    }

    combiner = G_CC_MODE(G_CC_MODULATERGB_PRIM_ENVA, G_CC_MODULATERGB_PRIM_ENVA);
    geometry = G_ZBUFFER | G_SHADE | G_LIGHTING;

    Mtx *mtx_mv = gDisplayListAlloc(poly_d, sizeof(*mtx_mv));
    guMtxIdent(mtx_mv);

    gSPLoadGeometryMode((*poly_p)++, geometry);
    gSPTexture((*poly_p)++, qu016(0.5), qu016(0.5), 0, G_TX_RENDERTILE, G_OFF);
    gSPMatrix((*poly_p)++, mtx_mv, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync((*poly_p)++);
    gDPSetCycleType((*poly_p)++, G_CYC_1CYCLE);
    gDPSetRenderMode((*poly_p)++, render_mode | blend_cyc1, render_mode | blend_cyc2);
    gDPSetCombine((*poly_p)++, combiner);
    gDPSetEnvColor((*poly_p)++, 0xFF, 0xFF, 0xFF, alpha);
}

void poly_writer_init(poly_writer_t *writer, Gfx *gfx_p, Gfx *gfx_d){
    writer->p = gfx_p;
    writer->d = gfx_d;
    writer->vtx_cmd = NULL;
    writer->tri_cmd = NULL;
    writer->vtx_cnt = 0;
    writer->prev_color = 0;
}

void poly_writer_flush(poly_writer_t *writer){
    if(writer->vtx_cmd){
        size_t vtx_size = sizeof(Vtx) * writer->vtx_cnt;
        Vtx *vtx_ptr = gDisplayListAlloc(&writer->d, vtx_size);
#ifdef WIIVC
        hmemcpy(vtx_ptr, writer->v, vtx_size);
        vtx_ptr = HB_SEG(vtx_ptr);
#else
        memcpy(vtx_ptr, writer->v, vtx_size);
#endif
        *writer->vtx_cmd = gsSPVertex(vtx_ptr, writer->vtx_cnt, 0);
        writer->vtx_cmd = NULL;
    }

    if((writer->vtx_cnt % 6) != 0){
        int vtx_pos = writer->vtx_cnt;
        *writer->tri_cmd = gsSP1Triangle(vtx_pos - 3, vtx_pos - 2, vtx_pos - 1, 0);
    }

    writer->tri_cmd = NULL;
    writer->vtx_cnt = 0;
}

void poly_writer_finish(poly_writer_t *writer, Gfx **gfx_p, Gfx **gfx_d){
    poly_writer_flush(writer);

    *gfx_p = writer->p;
    *gfx_d = writer->d;
}

void poly_writer_append(poly_writer_t *writer, Vtx (*vtx)[3], uint32_t color){
    if(!writer->vtx_cmd){
        writer->vtx_cmd = writer->p++;
    }
    int vtx_pos = writer->vtx_cnt;

    memcpy(&writer->v[vtx_pos], vtx, sizeof(*vtx));

    if(writer->prev_color != color){
        // new surface
        gDPSetPrimColor(writer->p++, 0, 0,
                        (color >> 24) & 0xFF,
                        (color >> 16) & 0xFF,
                        (color >> 8) & 0xFF,
                        color & 0xFF);

        if((writer->vtx_cnt % 6) != 0){
            *writer->tri_cmd = gsSP1Triangle(vtx_pos - 3, vtx_pos - 2, vtx_pos - 1, 0);
            writer->tri_cmd = writer->p++;
            *writer->tri_cmd = gsSP1Triangle(vtx_pos, vtx_pos + 1, vtx_pos + 2, 0);
        }
        writer->prev_color = color;
    }

    if((vtx_pos % 6) == 0){
        writer->tri_cmd = writer->p++;
        *writer->tri_cmd = gsSP2Triangles(vtx_pos, vtx_pos + 1, vtx_pos + 2, 0,
                                         vtx_pos + 3, vtx_pos + 4, vtx_pos + 5, 0);
    }

    writer->vtx_cnt += 3;
    if(writer->vtx_cnt == 30){
        poly_writer_flush(writer);
    }
}