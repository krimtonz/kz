#include <stdlib.h>
#include "collision_view.h"
#include "gfx.h"
#include "kz.h"

inline uint32_t poly_color(z2_col_type_t *type){
    if(type->flags_2.hookshot){
        return 0x0000FF00;
    }
    else if(type->flags_1.interaction > 1){
        return 0xFF00FF00;
    }
    else if(type->flags_1.special == 0x0C){
        return 0xFF000000;
    }
    else if(type->flags_1.exit!=0 || type->flags_1.special==0x05){
        return 0x00FFFF00;
    }
    else if(type->flags_1.behavior!=0 || type->flags_2.wall_damage){
        return 0x00FF0000;
    }
    else if(type->flags_2.terrain==0x01){
        return 0xFFFF0000;
    }
    return 0xFFFFFF00;
}

void kz_col_view(){
    static Gfx *poly_disp;
    static Gfx *poly_disp_p;
    static Gfx *poly_disp_d;

    if(kz.collision_view_status == COL_VIEW_GENERATE){

        z2_col_hdr_t *col = z2_game.col_ctxt.col_hdr;
        size_t n_poly = col->n_poly;
        uint8_t alpha = 0x80;
        uint32_t rm = Z_CMP | IM_RD | FORCE_BL | ZMODE_DEC;
        uint32_t blc1;
        uint32_t blc2;
        Mtx m;
        if(kz.collision_view_settings & COL_VIEW_REDUX){
            for(int i=0;i<col->n_poly;i++){
                z2_col_poly_t *poly = &col->poly[i];
                z2_col_type_t *type = &col->type[poly->type];
                uint32_t color = poly_color(type);
                if(color==0xFFFFFF00) n_poly--;
            }
        }

        size_t poly_size = 9 * n_poly + 0x10;

        poly_disp = malloc(poly_size * sizeof(*poly_disp));
        poly_disp_p = poly_disp;
        poly_disp_d = poly_disp + poly_size;

        if(kz.collision_view_settings & COL_VIEW_OPAQUE){
            rm |= Z_UPD | CVG_DST_CLAMP;
            alpha = 0xFF;
            blc1 = GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1);
            blc2 = GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1);
        }else{
            rm |= ZMODE_XLU | CVG_DST_FULL;
            blc1 = GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA);
            blc2 = GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA);
        }

        gDPPipeSync(poly_disp_p++);
        gDPSetRenderMode(poly_disp_p++,rm | blc1, rm | blc2);
        gSPTexture(poly_disp_p++,qu016(0.5),qu016(0.5),0,G_TX_RENDERTILE,G_OFF);
        gDPSetCycleType(poly_disp_p++, G_CYC_2CYCLE);
        gDPSetCombineMode(poly_disp_p++, G_CC_PRIMITIVE, G_CC_MODULATERGBA2);
        gSPLoadGeometryMode(poly_disp_p++,
                            G_SHADE | G_LIGHTING | G_ZBUFFER | G_CULL_BACK);

        m = gdSPDefMtx(1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1);
        
        gSPMatrix(poly_disp_p++,gDisplayListData(&poly_disp_d,m), G_MTX_MODELVIEW | G_MTX_LOAD);

        for(int i=0;i<col->n_poly;i++){
            z2_col_poly_t *poly = &col->poly[i];
            z2_col_type_t *type = &col->type[poly->type];
            z2_xyz_t *va  = &col->vtx[poly->va];
            z2_xyz_t *vb = &col->vtx[poly->vb];
            z2_xyz_t *vc = &col->vtx[poly->vc];

            uint32_t color = poly_color(type) | alpha;
            if(kz.collision_view_settings & COL_VIEW_REDUX && color == (0xFFFFFF00 | alpha)){
                continue;
            }
            gDPSetPrimColor(poly_disp_p++,0,0,color >> 24, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
            
            Vtx vt[3] = {
                gdSPDefVtxN(va->x,va->y,va->z,0,0,poly->norm.x/0x100,poly->norm.y/0x100,poly->norm.z/0x100,0xFF),
                gdSPDefVtxN(vb->x,vb->y,vb->z,0,0,poly->norm.x/0x100,poly->norm.y/0x100,poly->norm.z/0x100,0xFF),
                gdSPDefVtxN(vc->x,vc->y,vc->z,0,0,poly->norm.x/0x100,poly->norm.y/0x100,poly->norm.z/0x100,0xFF),
            };
            gSPVertex(poly_disp_p++,gDisplayListData(&poly_disp_d,vt),3,0);
            gSP1Triangle(poly_disp_p++,0,1,2,0);
        }

        gSPEndDisplayList(poly_disp_p++);

        kz.collision_view_status = COL_VIEW_SHOW;
    }
    else if(kz.collision_view_status == COL_VIEW_SHOW){
        Gfx *g = (kz.collision_view_settings & COL_VIEW_OPAQUE)?z2_game.common.gfx->poly_opa.p++:z2_game.common.gfx->poly_xlu.p++;
        gSPDisplayList(g,poly_disp);
    }
    else if(kz.collision_view_status == COL_VIEW_DESTROY){
        kz.collision_view_status = COL_VIEW_KILL;
    }
    else if(kz.collision_view_status == COL_VIEW_KILL){
        if(poly_disp){
            free(poly_disp);
            poly_disp = NULL;
        }
        kz.collision_view_status = COL_VIEW_NONE;
    }
}

