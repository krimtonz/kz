#include <stdlib.h>
#include "collision_view.h"
#include "gfx.h"
#include "kz.h"

void kz_col_view(){

    static z64_disp_buf_t poly_disp;

    if(kz.col_gen){

        z64_col_hdr_t *col = z64_game.col_ctxt.col_hdr;
        size_t n_poly = col->n_poly;

        Mtx m;
        
        size_t poly_size = 9 * n_poly + 0x10;
        
        gfx_disp_buf_reset(&poly_disp,poly_size * sizeof(Gfx));

        uint32_t rm = Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | FORCE_BL | ZMODE_DEC;
        uint32_t blc1 = GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1);
        uint32_t blc2 = GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1);

        gDPPipeSync(poly_disp.p++);
        gDPSetRenderMode(poly_disp.p++,rm | blc1, rm | blc2);
        gSPTexture(poly_disp.p++,qu016(0.5),qu016(0.5),0,G_TX_RENDERTILE,G_OFF);
        gDPSetCycleType(poly_disp.p++, G_CYC_2CYCLE);
        gDPSetCombineMode(poly_disp.p++, G_CC_PRIMITIVE, G_CC_MODULATERGBA2);
        gSPLoadGeometryMode(poly_disp.p++,
                            G_SHADE | G_LIGHTING | G_ZBUFFER | G_CULL_BACK);
        m = gdSPDefMtx(1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1);
        gSPMatrix(poly_disp.p++,gDisplayListData(&poly_disp.d,m), G_MTX_MODELVIEW | G_MTX_LOAD);
        
        for(int i=0;i<n_poly;++i){
            z64_col_poly_t *poly = &col->poly[i];
            z64_col_type_t *type = &col->type[poly->type];
            z64_xyz_t *va  = &col->vtx[poly->va];
            z64_xyz_t *vb = &col->vtx[poly->vb];
            z64_xyz_t *vc = &col->vtx[poly->vc];
            _Bool skip = 0;

            uint8_t alpha = 0xFF;

            if(!kz.col_opaque) alpha = 0x80;
            
            if(type->flags_2.hookshot){
                gDPSetPrimColor(poly_disp.p++,0,0,0x00,0x00,0xFF,alpha);
            }
            else if(type->flags_1.interaction > 1){
                gDPSetPrimColor(poly_disp.p++,0,0,0xFF,0x00,0xFF,alpha);
            }
            else if(type->flags_1.special == 0x0C){
                gDPSetPrimColor(poly_disp.p++,0,0,0xFF,0x00,0x00,alpha);
            }
            else if(type->flags_1.exit!=0 || type->flags_1.special==0x05){
                gDPSetPrimColor(poly_disp.p++,0,0,0x00,0xFF,0xFF,alpha);
            }
            else if(type->flags_1.behavior!=0 || type->flags_2.wall_damage){
                gDPSetPrimColor(poly_disp.p++,0,0,0x00,0xFF,0x00,alpha);
            }
            else if(type->flags_2.terrain==0x01){
                gDPSetPrimColor(poly_disp.p++,0,0,0xFF,0xFF,0x00,alpha);
            }else{
                gDPSetPrimColor(poly_disp.p++,0,0,0xFF,0xFF,0xFF,alpha);
            }
            
            

            if(!skip){
                Vtx vt[3] = {
                    gdSPDefVtxN(va->x,va->y,va->z,0,0,poly->norm.x/0x100,poly->norm.y/0x100,poly->norm.z/0x100,0xFF),
                    gdSPDefVtxN(vb->x,vb->y,vb->z,0,0,poly->norm.x/0x100,poly->norm.y/0x100,poly->norm.z/0x100,0xFF),
                    gdSPDefVtxN(vc->x,vc->y,vc->z,0,0,poly->norm.x/0x100,poly->norm.y/0x100,poly->norm.z/0x100,0xFF),
                };
                gSPVertex(poly_disp.p++,gDisplayListData(&poly_disp.d,vt),3,0);
                gSP1Triangle(poly_disp.p++,0,1,2,0);
            }
        }
        gSPEndDisplayList(poly_disp.p++);
        kz.col_gen = 0;
    }
    if(!kz.col_enable){
        if(poly_disp.buf){
            gfx_disp_buf_destroy(&poly_disp);
        }
    }
    if(poly_disp.buf){
        gSPDisplayList(z64_game.common.gfx->poly_opa.p++,poly_disp.buf);
    }
}

