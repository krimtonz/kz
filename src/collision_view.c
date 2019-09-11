#include <stdlib.h>
#include "collision_view.h"
#include "gfx.h"
#include "kz.h"

void kz_col_view(){
    static Gfx *poly_disp;
    static Gfx *poly_disp_p;
    static Gfx *poly_disp_d;

    if(kz.collision_view_status == COL_VIEW_GENERATE){

        if(poly_disp){
            free(poly_disp);
        }

        z2_col_hdr_t *col = z2_game.col_ctxt.col_hdr;
        size_t n_poly = col->n_poly;
        uint8_t alpha = 0x80;
        uint32_t rm= Z_CMP | IM_RD | CVG_DST_CLAMP | FORCE_BL;
        uint32_t blc1;
        uint32_t blc2;
        Mtx m;

        size_t poly_size = 9 * n_poly + 0x10;

        poly_disp = malloc(poly_size * sizeof(*poly_disp));
        poly_disp_p = poly_disp;
        poly_disp_d = poly_disp + (sizeof(*poly_disp) * poly_size + sizeof(*poly_disp) - 1) / sizeof(*poly_disp);

        if(kz.collision_view_settings & COL_VIEW_OPAQUE){
            rm |= Z_UPD | ZMODE_DEC;
            alpha = 0xFF;
            blc1 = GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1);
            blc2 = GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1);
        }else{
            rm |= ZMODE_XLU;
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

        for(int i=0;i<n_poly;++i){
            z2_col_poly_t *poly = &col->poly[i];
            z2_col_type_t *type = &col->type[poly->type];
            z2_xyz_t *va  = &col->vtx[poly->va];
            z2_xyz_t *vb = &col->vtx[poly->vb];
            z2_xyz_t *vc = &col->vtx[poly->vc];

            if(type->flags_2.hookshot){
                gDPSetPrimColor(poly_disp_p++,0,0,0x00,0x00,0xFF,alpha);
            }
            else if(type->flags_1.interaction > 1){
                gDPSetPrimColor(poly_disp_p++,0,0,0xFF,0x00,0xFF,alpha);
            }
            else if(type->flags_1.special == 0x0C){
                gDPSetPrimColor(poly_disp_p++,0,0,0xFF,0x00,0x00,alpha);
            }
            else if(type->flags_1.exit!=0 || type->flags_1.special==0x05){
                gDPSetPrimColor(poly_disp_p++,0,0,0x00,0xFF,0xFF,alpha);
            }
            else if(type->flags_1.behavior!=0 || type->flags_2.wall_damage){
                gDPSetPrimColor(poly_disp_p++,0,0,0x00,0xFF,0x00,alpha);
            }
            else if(type->flags_2.terrain==0x01){
                gDPSetPrimColor(poly_disp_p++,0,0,0xFF,0xFF,0x00,alpha);
            }else{
                gDPSetPrimColor(poly_disp_p++,0,0,0xFF,0xFF,0xFF,alpha);
            }

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
    if(kz.collision_view_status == COL_VIEW_DESTROY){
        if(poly_disp){
            free(poly_disp);
            poly_disp = NULL;
        }
        kz.collision_view_status = COL_VIEW_NONE;
    }
    if(kz.collision_view_status == COL_VIEW_SHOW){
        Gfx *g = (kz.collision_view_settings & COL_VIEW_OPAQUE)?z2_game.common.gfx->poly_opa.p++:z2_game.common.gfx->poly_xlu.p++;
        gSPDisplayList(g,poly_disp);
    }
}

