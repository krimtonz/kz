#include <stdlib.h>
#include <math.h>

#include "collision_view.h"
#include "kz.h"
#include "z2.h"
#include "zu.h"

#define ONE 0.525731112119133606f
#define TAU 0.850650808352039932f

typedef struct {
    Gfx        *p;
    Gfx        *d;
    Vtx         v[30];
    Gfx        *vtx_cmd;
    Gfx        *tri_cmd;
    int         vtx_cnt;
    uint32_t    prev_color;
} poly_writer_t;

static void poly_writer_init(poly_writer_t *writer, Gfx *gfx_p, Gfx *gfx_d){
    writer->p = gfx_p;
    writer->d = gfx_d;
    writer->vtx_cmd = NULL;
    writer->tri_cmd = NULL;
    writer->vtx_cnt = 0;
    writer->prev_color = 0;
}

static void poly_writer_flush(poly_writer_t *writer){
    if(writer->vtx_cmd){
        size_t vtx_size = sizeof(Vtx) * writer->vtx_cnt;
        Vtx *vtx_ptr = gDisplayListAlloc(&writer->d, vtx_size);
        memcpy(vtx_ptr, writer->v, vtx_size);
        *writer->vtx_cmd = gsSPVertex(vtx_ptr, writer->vtx_cnt, 0);
        writer->vtx_cmd = NULL;
    }

    if(writer->vtx_cnt % 6 != 0){
        int vtx_pos = writer->vtx_cnt;
        *writer->tri_cmd = gsSP1Triangle(vtx_pos - 3, vtx_pos - 2, vtx_pos - 1, 0);
    }

    writer->tri_cmd = NULL;
    writer->vtx_cnt = 0;
}

static void poly_writer_finish(poly_writer_t *writer, Gfx **gfx_p, Gfx **gfx_d){
    poly_writer_flush(writer);

    *gfx_p = writer->p;
    *gfx_d = writer->d;
}

static void poly_writer_append(poly_writer_t *writer, Vtx (*vtx)[3], uint32_t color){
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

        if(writer->vtx_cnt % 6 != 0){
            *writer->tri_cmd = gsSP1Triangle(vtx_pos - 3, vtx_pos - 2, vtx_pos - 1, 0);
            writer->tri_cmd = writer->p++;
            *writer->tri_cmd = gsSP1Triangle(vtx_pos, vtx_pos + 1, vtx_pos + 2, 0);
        }
        writer->prev_color = color;
    }

    if(vtx_pos % 6 == 0){
        writer->tri_cmd = writer->p++;
        *writer->tri_cmd = gsSP2Triangles(vtx_pos, vtx_pos + 1, vtx_pos + 2, 0,
                                         vtx_pos + 3, vtx_pos + 4, vtx_pos + 5, 0);
    }

    writer->vtx_cnt += 3;
    if(writer->vtx_cnt == 30){
        poly_writer_flush(writer);
    }
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

/* norm = (v1 - v0) cross (v2 - v0)
 * ((v1.x-v0.x),(v1.y-v0.y),(v1.z-v0.z)) cross ((v2.x-v0.x),(v2.y-v0.y),(v2.z-v0.z))
 * cross(a,b) = ((aybz - azby),(azbx - axbz),(axby - aybx))
 * norm = (
 *          ((v1.y-v0.y) * (v2.z-v0.z)) - ((v1.z-v0.z) * (v2.y-v0.y)),
 *          ((v1.z-v0.z) * (v2.x-v0.x)) - ((v1.x-v0.x) * (v2.z-v0.z)),
 *          ((v1.x-v0.x) * (v2.y-v0.y)) - ((v1.y-v0.y) * (v2.x-v0.x))
 *        )
*/
static void tri_norm(z2_xyzf_t *v0, z2_xyzf_t *v1, z2_xyzf_t *v2, z2_xyzf_t *norm){
    norm->x = ((v1->y - v0->y) * (v1->z - v0->z)) -
              ((v1->z - v0->z) * (v2->y - v0->y));
    norm->y = ((v1->z - v0->z) * (v2->x - v0->x)) -
              ((v1->x - v0->x) * (v2->z - v0->z));
    norm->z = ((v1->x - v0->x) * (v2->y - v0->y)) -
              ((v1->y - v1->y) * (v2->x - v0->x));

    float norm_dist = sqrtf(norm->x * norm->x + norm->y * norm->y + norm->z * norm->z);

    if(norm_dist > 0){
        norm_dist = 127.f / norm_dist;
        norm->x *= norm_dist;
        norm->y *= norm_dist;
        norm->z *= norm_dist;
    }
}

void draw_uv_sphere(Gfx **hit_view_p, Gfx **hit_view_d, int16_t radius, int16_t x, int16_t y, int16_t z){
    static Gfx *sphere_gfx = NULL;
    if(sphere_gfx == NULL){
        const int   STACKS  =  6;
        const int   SECTORS = 10;
        static Vtx *vtx     = NULL;
        int         vtx_cnt = (STACKS + 1) * (SECTORS + 1);
        int         idx_cnt = ((STACKS - 1) * (SECTORS) * 2) * 3;
        int16_t    *indices = malloc(idx_cnt * sizeof(*indices));
        vtx = malloc(vtx_cnt * sizeof(*vtx));

        // should probably figure out a calculation for gfx count 
        sphere_gfx = malloc(57 * sizeof(*sphere_gfx));

        float       stack_angle;
        float       sector_angle;
        float       stack_step = M_PI / STACKS;
        float       sector_step = 2.0f * M_PI / SECTORS;
        float       xz, fx, fy, fz;
        int         vx, vy, vz, nx, ny, nz;
        Vtx        *p_vtx = vtx;
        int         k1, k2;
        int16_t    *p_indices = indices;

        // generate vtx and indices
        for(int i = 0; i <= STACKS; i++){
            stack_angle = M_PI / 2.0f - i * stack_step;
            xz = cosf(stack_angle);
            fy = sinf(stack_angle);
            k1 = i * (SECTORS + 1);
            k2 = k1 + SECTORS + 1;
            for(int j = 0; j <= SECTORS; j++, k1++, k2++){
                sector_angle = j * sector_step;
                fx = xz * cosf(sector_angle);
                fz = xz * sinf(sector_angle);

                vx = floorf(fx * 50.0f);
                vy = floorf(fy * 50.0f);
                vz = floorf(fz * 50.0f);

                nx = fx * 49.0f;
                ny = fy * 49.0f;
                nz = fz * 49.0f;

                *p_vtx++ = gdSPDefVtxN(vx, vy, vz, 0, 0, nx, ny, nz, 0xFF);

                if(i != STACKS && j != SECTORS){
                    if(i != 0){
                        *p_indices++ = k1;
                        *p_indices++ = k2;
                        *p_indices++ = k1 + 1;
                    }

                    if(i != (STACKS - 1)){
                        *p_indices++ = k1 + 1;
                        *p_indices++ = k2;
                        *p_indices++ = k2 + 1;
                    }
                }
            }
        }

        Gfx *gfx_p = sphere_gfx;
        int vtxstart = -1;
        gSPSetGeometryMode(gfx_p++, G_CULL_BACK);

        // Generate tris
        for(int i = 0; i < idx_cnt; i += 6){
            // Get the min/max vtx index for 2 tris
            int min = indices[i], max = indices[i];
            for(int j = 1; j < 6; j++){
                if(indices[i + j] < min){
                    min = indices[i + j];
                } else if(indices[i + j] > max){
                    max = indices[i + j];
                }
            }
            
            Vtx *vtx_first = &vtx[min];
            // Load vertices if the first or last isn't loaded.  This probably could be further optimized 
            if(i == 0 || vtxstart > min || vtxstart + 31 < max){
                gSPVertex(gfx_p++, vtx_first, 32, 0);
                vtxstart = min;
            }

            gSP2Triangles(gfx_p++, indices[i + 2] - vtxstart, indices[i + 1] - vtxstart, indices[i] - vtxstart, 0,
                                   indices[i + 5] - vtxstart, indices[i + 4] - vtxstart, indices[i + 3] - vtxstart, 0);
        }
        gSPClearGeometryMode(gfx_p++, G_CULL_BACK);
        gSPEndDisplayList(gfx_p++);

        free(indices);
    }

    Mtx m;
    {
        MtxF mf;
        guTranslateF(&mf, x, y, z);
        MtxF ms;
        guScaleF(&ms, radius / 50.0f, radius / 50.0f, radius / 50.0f);
        guMtxCatF(&ms, &mf, &mf);
        guMtxF2L(&mf, &m);
    }

    gSPMatrix((*hit_view_p)++, gDisplayListData(hit_view_d, m), G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_PUSH);
    gSPDisplayList((*hit_view_p)++, sphere_gfx);
    gSPPopMatrix((*hit_view_p)++, G_MTX_MODELVIEW);
}

static void draw_cylinder(Gfx **hit_view_p, Gfx **hit_view_d, int16_t radius, int16_t height, int16_t x, int16_t y, int16_t z){
    static Gfx *cylinder_p = NULL;

    // Create a static cylinder of height 128, and radius 1, with 12 sides
    if(cylinder_p == NULL){
        static Gfx cylinder_gfx[28];
        static Vtx cylinder_vtx[26];
        cylinder_p = cylinder_gfx;
        Gfx *cyl_p = cylinder_p;
        cylinder_vtx[0] = gdSPDefVtxN(0, 0, 0, 0, 0, 0, -127, 0, 0xFF);
        cylinder_vtx[1] = gdSPDefVtxN(0, 128, 0, 0, 0, 0, 127, 0, 0xFF);
        for(int i = 0;i < 12;i++){
            // x = cos(2(pi)i / 12)
            // z = -sin(2(pi)i / 12)
            int vtx_x = floorf(0.5f + cosf(2.f * M_PI * i / 12) * 128.f);
            int vtx_z = floorf(0.5f - sinf(2.f * M_PI * i / 12) * 128.f);
            int norm_x = cosf(2.f * M_PI * i / 12) * 127.f;
            int norm_z = -sinf(2.f * M_PI * i / 12) * 127.f;

            cylinder_vtx[2 + i * 2] = gdSPDefVtxN(vtx_x, 0, vtx_z, 0, 0, norm_x, 0, norm_z, 0xFF);
            cylinder_vtx[3 + i * 2] = gdSPDefVtxN(vtx_x, 128, vtx_z, 0, 0, norm_x, 0, norm_z, 0xFF);
        }

        gSPSetGeometryMode(cyl_p++, G_CULL_BACK);
        gSPVertex(cyl_p++, cylinder_vtx, 26, 0);

        for(int i = 0;i < 12;i++){
            int pos = (i == 0 ? 12 : i) - 1;
            int v[4] = {
                2 + pos * 2 + 0,
                2 + i * 2 + 0,
                2 + i * 2 + 1,
                2 + pos * 2 + 1
            };
            gSP2Triangles(cyl_p++, v[0], v[1], v[2], 0, v[0], v[2], v[3], 0);
            gSP2Triangles(cyl_p++, 0, v[1], v[0], 0, 1, v[3], v[2], 0);
        }

        gSPClearGeometryMode(cyl_p++, G_CULL_BACK);
        gSPEndDisplayList(cyl_p++);
    }

    Mtx m;
    {
        MtxF mf;
        guTranslateF(&mf, x, y, z);
        MtxF mscale;
        guScaleF(&mscale, radius / 128.f, height / 128.f, radius / 128.f);
        guMtxCatF(&mscale, &mf, &mf);
        guMtxF2L(&mf, &m);
    }

    gSPMatrix((*hit_view_p)++, gDisplayListData(hit_view_d, m), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_PUSH);
    gSPDisplayList((*hit_view_p)++, cylinder_p);
    gSPPopMatrix((*hit_view_p)++, G_MTX_MODELVIEW);
}

static void draw_triangle(Gfx **hit_view_p, Gfx **hit_view_d, z2_xyzf_t *v0, z2_xyzf_t *v1, z2_xyzf_t *v2){
    z2_xyzf_t norm;
    tri_norm(v0, v1, v2, &norm);
    Vtx v[3] = {
        gdSPDefVtxN(v0->x, v0->y, v0->z, 0, 0, norm.x, norm.y, norm.z, 0xFF),
        gdSPDefVtxN(v1->x, v1->y, v1->z, 0, 0, norm.x, norm.y, norm.z, 0xFF),
        gdSPDefVtxN(v2->x, v2->y, v2->z, 0, 0, norm.x, norm.y, norm.z, 0xFF),
    };

    gSPVertex((*hit_view_p)++, gDisplayListData(hit_view_d, v), 3, 0);
    gSP1Triangle((*hit_view_p)++, 0, 1, 2, 0);
}

static void draw_quad(Gfx **hit_view_p, Gfx **hit_view_d, z2_xyzf_t *v0, z2_xyzf_t *v1, z2_xyzf_t *v2, z2_xyzf_t *v3){
    z2_xyzf_t norm;
    tri_norm(v0, v1, v3, &norm);
    Vtx v[4] = {
        gdSPDefVtxN(v0->x, v0->y, v0->z, 0, 0, norm.x, norm.y, norm.z, 0xFF),
        gdSPDefVtxN(v1->x, v1->y, v1->z, 0, 0, norm.x, norm.y, norm.z, 0xFF),
        gdSPDefVtxN(v2->x, v2->y, v2->z, 0, 0, norm.x, norm.y, norm.z, 0xFF),
        gdSPDefVtxN(v3->x, v3->y, v3->z, 0, 0, norm.x, norm.y, norm.z, 0xFF),
    };

    gSPVertex((*hit_view_p)++, gDisplayListData(hit_view_d, v), 4, 0);
    gSP2Triangles((*hit_view_p)++, 0, 1, 2, 0, 0, 2, 3, 0);
}

static void do_hitbox_view(Gfx **hit_view_p, Gfx **hit_view_d, int hitbox_cnt, z2_hitbox_t **hitbox_list, uint32_t color){

    gDPSetPrimColor((*hit_view_p)++, 0, 0,
                    (color >> 16) & 0xFF,
                    (color >> 8) & 0xFF,
                    (color >> 0) & 0xFF,
                    0xFF);

    for(int i = 0;i < hitbox_cnt;i++){
        z2_hitbox_t *hitbox = hitbox_list[i];
        switch(hitbox->type){
            case Z2_HITBOX_SPHERE_LIST: {
                z2_hitbox_sphere_list_t *sphere_list = (z2_hitbox_sphere_list_t*)hitbox;
                for(int j = 0;j < sphere_list->entry_cnt;j++){
                    z2_hitbox_sphere_ent_t *entry = &sphere_list->entries[j];
                    int16_t radius = entry->radius;
                    if(radius == 0){
                        radius = 1;
                    }
                    draw_uv_sphere(hit_view_p, hit_view_d, radius, entry->pos.x, entry->pos.y, entry->pos.z);
                }
            }
            break;
            case Z2_HITBOX_CYLINDER: {
                z2_hitbox_cylinder_t *cylinder = (z2_hitbox_cylinder_t*)hitbox;
                int16_t radius = cylinder->radius;
                if(radius == 0){
                    radius = 1;
                }
                draw_cylinder(hit_view_p, hit_view_d, radius, cylinder->height, cylinder->pos.x, cylinder->pos.y + cylinder->y_offset, cylinder->pos.z);
            }
            break;
            case Z2_HITBOX_TRI_LIST: {
                z2_hitbox_tri_list_t *tri_list = (z2_hitbox_tri_list_t*)hitbox;
                for(int j = 0;j < tri_list->entry_cnt;j++){
                    z2_hitbox_tri_ent_t *entry = &tri_list->entries[j];
                    draw_triangle(hit_view_p, hit_view_d, &entry->vertices[0], &entry->vertices[2], &entry->vertices[1]);
                }
            }
            break;
            case Z2_HITBOX_QUAD: {
                z2_hitbox_quad_t *quad = (z2_hitbox_quad_t*)hitbox;
                draw_quad(hit_view_p, hit_view_d, &quad->vertices[0], &quad->vertices[2], &quad->vertices[3], &quad->vertices[1]);
            }
            break;
        }
    }
}

void kz_hitbox_view(){
#if 0 
    static Gfx *hit_view_disp[2];
    static int hit_view_idx = 0;
#endif

    if(kz.hitbox_view_status == COL_VIEW_GENERATE){
#if 0
        hit_view_disp[0] = malloc(0x800 * sizeof(*hit_view_disp[0]));
        hit_view_disp[1] = malloc(0x800 * sizeof(*hit_view_disp[1]));
#endif
        kz.hitbox_view_status = COL_VIEW_SHOW;
    }else if(kz.hitbox_view_status == COL_VIEW_KILL){
        kz.hitbox_view_status = COL_VIEW_DESTROY;
        return;
    }else if(kz.hitbox_view_status == COL_VIEW_DESTROY){
#if 0
        if(hit_view_disp[0]){
            free(hit_view_disp[0]);
        }
        if(hit_view_disp[1]){
            free(hit_view_disp[1]);
        }
#endif
        kz.hitbox_view_status = COL_VIEW_NONE;
        return;
    }

    _Bool show = kz.hitbox_view_status == COL_VIEW_SHOW &&
                 zu_is_ingame() &&
                 z2_game.pause_ctx.state == 0;

    if(show){
        Gfx *z2_gfx_p;
        _Bool xlu = !settings->hit_view_opq;
        if(xlu){
            z2_gfx_p = z2_ctxt.gfx->poly_xlu.p++;
        }else{
            z2_gfx_p = z2_ctxt.gfx->poly_opa.p++;
        }
#if 0
        Gfx *hit_view_start = hit_view_disp[hit_view_idx];
#endif
        Gfx *hit_view_start = (Gfx*)0x80383AC0;
        Gfx *hit_view_p = hit_view_start;
        Gfx *hit_view_d = hit_view_p + 0x800;
#if 0
        hit_view_idx = (hit_view_idx + 1) % 2;
#endif

        init_poly_list(&hit_view_p, &hit_view_d, xlu, 0);
        do_hitbox_view(&hit_view_p, &hit_view_d, z2_game.hitbox_ctxt.other_cnt, z2_game.hitbox_ctxt.other, 0x00FFFFFF);
        do_hitbox_view(&hit_view_p, &hit_view_d, z2_game.hitbox_ctxt.ac_cnt, z2_game.hitbox_ctxt.ac, 0x000000FF);
        do_hitbox_view(&hit_view_p, &hit_view_d, z2_game.hitbox_ctxt.attack_cnt, z2_game.hitbox_ctxt.attack, 0x00FF0000);
        gSPEndDisplayList(hit_view_p++);
        gSPDisplayList(z2_gfx_p, hit_view_start);
    }
}

static void do_poly_list(poly_writer_t *writer, z2_xyz_t *vtx, z2_col_poly_t *polys, z2_col_type_t *types, int poly_cnt, _Bool redux){
    for(int i = 0;i < poly_cnt; i++){
        z2_col_poly_t *poly = &polys[i];
        z2_col_type_t *type = &types[poly->type];
        z2_xyz_t *va = &vtx[poly->va];
        z2_xyz_t *vb = &vtx[poly->vb];
        z2_xyz_t *vc = &vtx[poly->vc];
        uint32_t color;
        _Bool skip = 0;
        if(type->flags_2.hookshot){
            color = 0x0000FFFF;
        }else if(type->flags_1.interaction > 1){
            color = 0xFF00FFFF;
        }else if(type->flags_1.special == 0xC){
            color = 0xFF0000FF;
        }else if(type->flags_1.exit != 0 || type->flags_1.special == 0x5){
            color = 0x00FFFFFF;
        }else if(type->flags_1.behavior != 0 || type->flags_2.wall_damage){
            color = 0x00FF00FF;
        }else if(type->flags_2.terrain == 1){
            color = 0xFFFF00FF;
        }else if(redux){
            skip = 1;
        }else{
            color = 0xFFFFFFFF;
        }

        if(!skip){
            gDPSetPrimColor(writer->p++, 0, 0,
                            (color >> 24) & 0xFF,
                            (color >> 16) & 0xFF,
                            (color >> 8) & 0xFF,
                            color & 0xFF);

            Vtx v[3] = {
                gdSPDefVtxN(va->x, va->y, va->z, 0, 0,
                            poly->norm.x / 0x100, poly->norm.y / 0x100, poly->norm.z / 0x100,
                            0xFF),
                gdSPDefVtxN(vb->x, vb->y, vb->z, 0, 0,
                            poly->norm.x / 0x100, poly->norm.y / 0x100, poly->norm.z / 0x100,
                            0xFF),
                gdSPDefVtxN(vc->x, vc->y, vc->z, 0, 0,
                            poly->norm.x / 0x100, poly->norm.y / 0x100, poly->norm.z / 0x100,
                            0xFF),
            };

            poly_writer_append(writer, &v, color);
        }
    }
}

static void do_dynamic_list(poly_writer_t *writer, z2_col_hdr_t *hdr, uint16_t idx){
    z2_col_ctxt_t *col_ctx = &z2_game.col_ctxt;
    while(idx != 0xFFFF){
        z2_col_list_t *list = &col_ctx->dyn.list[idx];
        do_poly_list(writer, col_ctx->dyn_vtx, &col_ctx->dyn_poly[list->poly_idx],
                     hdr->type, 1, 0);
        idx = list->list_next;
    }
}

static void do_dynamic_collision(poly_writer_t *writer){
    z2_col_ctxt_t *col_ctx = &z2_game.col_ctxt;
    for(int i = 0;i < 32;i++){
        if(col_ctx->dynamic_flags[i].active){
            z2_col_chk_actor_t *col = &col_ctx->actors[i];
            do_dynamic_list(writer, col->col_hdr, col->ceiling_list_idx);
            do_dynamic_list(writer, col->col_hdr, col->wall_list_idx);
            do_dynamic_list(writer, col->col_hdr, col->floor_list_idx);
        }
    }
}

void kz_col_view(){
    static Gfx *static_col = NULL;
#if 0
    static Gfx *dynamic_gfx[2] = { NULL, NULL };
    static int dynamic_idx = 0;
#endif
    static Gfx* dynamic_gfx = NULL;

    static uint16_t col_view_scene = 0;

    poly_writer_t writer;

    if(settings->col_view_upd && col_view_scene != z2_game.scene_index && kz.collision_view_status == COL_VIEW_SHOW){
        kz.collision_view_status = COL_VIEW_REGENERATE;
        return;
    }

    if(kz.collision_view_status == COL_VIEW_REGENERATE){
        if(static_col){
            free(static_col);
            static_col = NULL;
        }
        kz.collision_view_status = COL_VIEW_GENERATE;
    }else if(kz.collision_view_status == COL_VIEW_GENERATE){
        col_view_scene = z2_game.scene_index;
        z2_col_hdr_t *hdr = z2_game.col_ctxt.col_hdr;
        int static_col_cnt = hdr->n_poly;
        size_t static_col_size;
        if(settings->col_view_red){
            static_col_size = 0;
            z2_col_poly_t *polys = hdr->poly;
            z2_col_type_t *types = hdr->type;
            for(int i = 0;i < static_col_cnt;i++){
                z2_col_poly_t *poly = &polys[i];
                z2_col_type_t *type = &types[poly->type];
                if(type->flags_2.hookshot){
                    static_col_size++;
                }else if(type->flags_1.interaction > 1){
                    static_col_size++;
                }else if(type->flags_1.special == 0xC){
                    static_col_size++;
                }else if(type->flags_1.exit != 0 || type->flags_1.special == 0x5){
                    static_col_size++;
                }else if(type->flags_1.behavior != 0 || type->flags_2.wall_damage){
                    static_col_size++;
                }else if(type->flags_2.terrain == 1){
                    static_col_size++;
                }
            }
            static_col_size *= 9;
        }else{
            static_col_size = 9 * static_col_cnt;
        }

        static_col_size += 0x11;

        //static_col = malloc(sizeof(*static_col) * static_col_size);
        static_col = (Gfx*)0x80008500;
        Gfx *static_col_p = static_col;
        Gfx *static_col_d = static_col + static_col_size;
        poly_writer_init(&writer, static_col_p, static_col_d);
        do_poly_list(&writer, hdr->vtx, hdr->poly, hdr->type, hdr->n_poly, settings->col_view_red);
        poly_writer_finish(&writer, &static_col_p, &static_col_d);
        gSPEndDisplayList(static_col_p++);

        kz.collision_view_status = COL_VIEW_SHOW;
    }else if(kz.collision_view_status == COL_VIEW_KILL){
        kz.collision_view_status = COL_VIEW_DESTROY;
    }else if(kz.collision_view_status == COL_VIEW_DESTROY){
        if(static_col){
#if 0
            free(static_col);
            static_col = NULL;
#endif
        }
        kz.collision_view_status = COL_VIEW_NONE;
    }

    _Bool show = kz.collision_view_status == COL_VIEW_SHOW &&
                                             zu_is_ingame() &&
                                             z2_game.pause_ctx.state == 0;

    if(show){
        Gfx **gfx_p;
        Gfx **gfx_d;
        _Bool xlu = !settings->col_view_opq;
        if(xlu){
            gfx_p = &z2_ctxt.gfx->poly_xlu.p;
            gfx_d = &z2_ctxt.gfx->poly_xlu.d;
        }else{
            gfx_p = &z2_ctxt.gfx->poly_opa.p;
            gfx_d = &z2_ctxt.gfx->poly_opa.d;
        }

        init_poly_list(gfx_p, gfx_d, xlu, 1);

        gSPSetGeometryMode((*gfx_p)++, G_CULL_BACK);
        gSPDisplayList((*gfx_p)++, static_col);

        if(settings->col_view_upd){
#if 0
            if(dynamic_gfx[dynamic_idx] == NULL){
                dynamic_gfx[0] = malloc(0x1000 * sizeof(*dynamic_gfx[0]));
                dynamic_gfx[1] = malloc(0x1000 * sizeof(*dynamic_gfx[1]));
            }
#endif
            Gfx *dynamic_buf = NULL;
            Gfx *dynamic_gfx_p = NULL;
            Gfx *dynamic_gfx_d = NULL;
            poly_writer_t *dynamic_writer = NULL;
#if 0
            dynamic_buf = dynamic_gfx[dynamic_idx];
#endif
            dynamic_buf = (Gfx*)0x80000500;
            dynamic_gfx_p = dynamic_buf;
            dynamic_gfx_d = dynamic_buf + 0x1000;
            dynamic_writer = &writer;
            poly_writer_init(dynamic_writer, dynamic_gfx_p, dynamic_gfx_d);
            do_dynamic_collision(dynamic_writer);
            poly_writer_finish(dynamic_writer, &dynamic_gfx_p, &dynamic_gfx_d);
            gSPEndDisplayList(dynamic_gfx_p++);
            gSPDisplayList((*gfx_p)++, dynamic_buf);
        }
    }
}