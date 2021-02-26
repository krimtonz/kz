#include <math.h>
#include <mips.h>
#include "kz.h"
#include "z2.h"
#include "input.h"
#include "vec_math.h"
#include "hb_heap.h"
#include "poly_writer.h"

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

/*
void camera_draw(void) {
    static Gfx *cam_gfx[2] = { NULL, NULL };
    static int cam_gfx_idx = 0;

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
*/
