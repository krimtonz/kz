#include <math.h>
#include <mips.h>
#include "kz.h"
#include "z2.h"
#include "input.h"
#include "vec_math.h"
#include "hb_heap.h"

void kz_camera_calc(z2_camera_t *camera) {
    sph_coord_t *geo = &kz.cam_sph;

    geo->yaw -= input_x() / 500.0f;
    geo->pitch += input_y() / 500.0f;

    if(geo->pitch < -((M_PI / 2.0f) + 0.02f) ){
        geo->pitch = -((M_PI / 2.0f) + 0.02f);
    } else if(geo->pitch > (M_PI / 2.0f) - 0.02f){
        geo->pitch = (M_PI / 2.0f) - 0.02f;
    }

    z2_xyzf_t move, strafe;
    geo_to_vec3f(geo, &move);
    sph_coord_t shift;
    shift.pitch = 0.0f;
    shift.yaw = geo->yaw - (M_PI / 2.0f);
    geo_to_vec3f(&shift, &strafe);
    if(input_pressed() & BUTTON_C_UP){
        vec3f_scalar_mul(&move, -45.0f, &move);
        vec3f_add(&kz.kz_at, &move, &kz.kz_at);
    }
    if(input_pressed() & BUTTON_C_DOWN){
        vec3f_scalar_mul(&move, 45.0f, &move);
        vec3f_add(&kz.kz_at, &move, &kz.kz_at);
    }
    if(input_pressed() & BUTTON_C_LEFT){
        vec3f_scalar_mul(&strafe, 15.0f, &strafe);
        vec3f_add(&kz.kz_at, &strafe, &kz.kz_at);
    }
    if(input_pressed() & BUTTON_C_RIGHT){
        vec3f_scalar_mul(&strafe, -15.0f, &strafe);
        vec3f_add(&kz.kz_at, &strafe, &kz.kz_at);
    }

    z2_xyzf_t geo_vec;

    geo_to_vec3f(geo, &geo_vec);
    vec3f_add(&kz.kz_at, &geo_vec, &kz.kz_eye);
}

static void init_poly_list(Gfx **poly_p, Gfx **poly_d, _Bool xlu, _Bool decal){
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
        alpha = 0xFF;
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

void camera_draw(void) {
    static Gfx *cam_gfx[2] = { NULL, NULL };
    static int cam_gfx_idx = 0;
    static Gfx *arrow_gfx = NULL;
    static Vtx *cam_vtx_p = NULL;

    static Gfx *arrow_tris = NULL;
    static Gfx *cam_tris = NULL;

    static Vtx cam_vtx[] = {
        gdSPDefVtxC(-5, 5, -5, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(5, 5, -5, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(5, -5, -5, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(-5, -5, -5, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(5, 5, -20, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(-5, 5, -20, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(-5, -5, -20, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(5, -5, -20, 0, 0, 0xFF, 0, 0, 0xFF),

        // arrow
        gdSPDefVtxC(0, 0, 0, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(-10, 10, -5, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(10, 10, -5, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(10, -10, -5, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(-10, -10, -5, 0, 0, 0xFF, 0, 0, 0xFF),

        // camera
        gdSPDefVtxC(0, 0, -5, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(-3, 3, 0, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(3, 3, 0, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(3, -3, 0, 0, 0, 0xFF, 0, 0, 0xFF),
        gdSPDefVtxC(-3, -3, 0, 0, 0, 0xFF, 0, 0, 0xFF),
    };

    static Gfx draw_arrow_tris[] = {
        gsSPVertex(cam_vtx, sizeof(cam_vtx) / sizeof(*cam_vtx), 0),
        gsDPSetPrimColor(0, 0, 0xFF, 0x00, 0x00, 0xFF),
        gsSP2Triangles(1, 0, 3, 0, 1, 3, 2, 0),
        gsSP2Triangles(4, 1, 2, 0, 4, 2, 7, 0),
        gsSP2Triangles(0, 5, 6, 0, 0, 6, 3, 0),
        gsSP2Triangles(5, 4, 7, 0, 5, 7, 6, 0),
        gsSP2Triangles(4, 5, 0, 0, 4, 0, 1, 0),
        gsSP2Triangles(2, 3, 6, 0, 2, 6, 7, 0),
        gsSP2Triangles(9, 10, 11, 0, 9, 11, 12, 0),
        gsSP2Triangles(10, 9, 8, 0, 10, 8, 11, 0),
        gsSP2Triangles(12, 11, 8, 0, 12, 8, 9, 0),
        gsSPEndDisplayList(),
    };

    static Gfx draw_cam_tris[] = {
        gsSPVertex(cam_vtx, sizeof(cam_vtx) / sizeof(*cam_vtx), 0),
        gsDPSetPrimColor(0, 0, 0x00, 0x00, 0xFF, 0xFF),
        gsSP2Triangles(1, 0, 3, 0, 1, 3, 2, 0),
        gsSP2Triangles(4, 1, 2, 0, 4, 2, 7, 0),
        gsSP2Triangles(0, 5, 6, 0, 0, 6, 3, 0),
        gsSP2Triangles(5, 4, 7, 0, 5, 7, 6, 0),
        gsSP2Triangles(4, 5, 0, 0, 4, 0, 1, 0),
        gsSP2Triangles(2, 3, 6, 0, 2, 6, 7, 0),
        gsSP2Triangles(14, 15, 16, 0, 14, 16, 17, 0),
        gsSP2Triangles(15, 14, 13, 0, 15, 13, 16, 0),
        gsSP2Triangles(17, 16, 13, 0, 17, 13, 14, 0),
        gsSPEndDisplayList(),
    };

    if(cam_gfx[0] == NULL) {
        cam_gfx[0] = malloc(100 * sizeof(*cam_gfx[0]));
        cam_gfx[1] = malloc(100 * sizeof(*cam_gfx[1]));
    }

    Gfx *cam_gfx_start = cam_gfx[cam_gfx_idx];
    Gfx *cam_gfx_p = cam_gfx_start;
    Gfx *cam_gfx_d = cam_gfx_p + 100;
    cam_gfx_idx = (cam_gfx_idx + 1) % 2;

    Mtx m;
    Mtx mcam;
    {
        z2_xyzf_t diff;
        sph_coord_t sph;
        vec3f_sub(&kz.cam_at, &kz.cam_eye, &diff);
        vec3f_to_geo(&diff, &sph);

        MtxF mf, mfcam;
        guTranslateF(&mf, kz.cam_at.x, kz.cam_at.y, kz.cam_at.z);
        guTranslateF(&mfcam, kz.cam_eye.x, kz.cam_eye.y, kz.cam_eye.z);
        MtxF mr;
        guRotateF(&mr, sph.yaw, 0.0f, 1.0f, 0.0f);
        guMtxCatF(&mr, &mf, &mf);
        guMtxCatF(&mr, &mfcam, &mfcam);
        guRotateF(&mr, -sph.pitch, 1.0f, 0.0f, 0.0f);
        guMtxCatF(&mr, &mf, &mf);
        guMtxCatF(&mr, &mfcam, &mfcam);
        guMtxF2L(&mf, &m);
        guMtxF2L(&mfcam, &mcam);
    }
    
    gSPSetGeometryMode(cam_gfx_p++, G_CULL_BACK);
    gSPMatrix(cam_gfx_p++, gDisplayListData(&cam_gfx_d, m), G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_PUSH);
    gSPDisplayList(cam_gfx_p++, draw_arrow_tris);
    gSPPopMatrix(cam_gfx_p++, G_MTX_MODELVIEW);

    gSPMatrix(cam_gfx_p++, gDisplayListData(&cam_gfx_d, mcam), G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_PUSH);
    gSPDisplayList(cam_gfx_p++, draw_cam_tris);
    gSPPopMatrix(cam_gfx_p++, G_MTX_MODELVIEW);
    gSPClearGeometryMode(cam_gfx_p++, G_CULL_BACK);
    gSPEndDisplayList(cam_gfx_p++);

    z2_disp_buf_t *xlu = &z2_ctxt.gfx->poly_xlu;

    init_poly_list(&xlu->p, &xlu->d, 1, 0);

    gSPDisplayList(xlu->p++, cam_gfx_start);

}
