#include <stdlib.h>
#include <math.h>

#include "cache.h"
#include "collision_view.h"
#include "kz.h"
#include "z2.h"
#include "zu.h"
#include "hb_heap.h"
#include "poly_writer.h"
#include "line_writer.h"

typedef struct {
    int a;
    int b;
} line_t;

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
#ifdef WIIVC
        vtx = halloc(vtx_cnt * sizeof(*vtx));
#else
        vtx = malloc(vtx_cnt * sizeof(*vtx));
#endif

        // should probably figure out a calculation for gfx count
#ifdef WIIVC
        sphere_gfx = halloc(57 * sizeof(*sphere_gfx));
#else
        sphere_gfx = malloc(57 * sizeof(*sphere_gfx));
#endif

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
#ifdef WIIVC
                vtx_first = HB_SEG(vtx_first);
#endif
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

    void *dlist = sphere_gfx;
#ifdef WIIVC
    Mtx *data_m = gDisplayListDataHB(hit_view_d, m);
    data_m = HB_SEG(data_m);
    gSPMatrix((*hit_view_p)++, data_m, G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_PUSH);
    dlist = HB_SEG(dlist);
#else
    gSPMatrix((*hit_view_p)++, gDisplayListData(hit_view_d, m), G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_PUSH);
#endif
    gSPDisplayList((*hit_view_p)++, dlist);
    gSPPopMatrix((*hit_view_p)++, G_MTX_MODELVIEW);
}

static void draw_cylinder(Gfx **hit_view_p, Gfx **hit_view_d, int16_t radius, int16_t height, int16_t x, int16_t y, int16_t z){
    static Gfx *cylinder_p = NULL;

    // Create a static cylinder of height 128, and radius 1, with 12 sides
    if(cylinder_p == NULL){
#ifdef WIIVC
        static Gfx *cylinder_gfx = NULL;
        static Vtx *cylinder_vtx = NULL;
        if(cylinder_gfx == NULL) {
            cylinder_gfx = halloc(28 * sizeof(*cylinder_gfx));
            cylinder_vtx = halloc(27 * sizeof(*cylinder_vtx));
        }
#else
        static Gfx cylinder_gfx[28];
        static Vtx cylinder_vtx[26];
#endif
        cylinder_p = cylinder_gfx;
        Gfx *cyl_p = cylinder_p;
        cylinder_vtx[0] = gdSPDefVtxN(0, 0, 0, 0, 0, 0, -127, 0, 0xFF);
        cylinder_vtx[1] = gdSPDefVtxN(0, 128, 0, 0, 0, 0, 127, 0, 0xFF);
        for(int i = 0;i < 12;i++){

            int vtx_x = floorf(0.5f + cosf(2.f * M_PI * i / 12) * 128.f);
            int vtx_z = floorf(0.5f - sinf(2.f * M_PI * i / 12) * 128.f);
            int norm_x = cosf(2.f * M_PI * i / 12) * 127.f;
            int norm_z = -sinf(2.f * M_PI * i / 12) * 127.f;

            cylinder_vtx[2 + i * 2] = gdSPDefVtxN(vtx_x, 0, vtx_z, 0, 0, norm_x, 0, norm_z, 0xFF);
            cylinder_vtx[3 + i * 2] = gdSPDefVtxN(vtx_x, 128, vtx_z, 0, 0, norm_x, 0, norm_z, 0xFF);
        }

        gSPSetGeometryMode(cyl_p++, G_CULL_BACK);
#ifdef WIIVC
        cylinder_vtx = (Vtx*)HB_SEG(cylinder_vtx);
#endif
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
    void *dlist = cylinder_p;
#if WIIVC
    Mtx *data_m = gDisplayListDataHB(hit_view_d, m);
    data_m = HB_SEG(data_m);
    gSPMatrix((*hit_view_p)++, data_m, G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_PUSH);
    dlist = HB_SEG(dlist);
#else
    gSPMatrix((*hit_view_p)++, gDisplayListData(hit_view_d, m), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_PUSH);
#endif
    gSPDisplayList((*hit_view_p)++, dlist);
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
#if WIIVC
    Vtx *data_v = (Vtx*)gDisplayListDataHB(hit_view_d, v);
    data_v = HB_SEG(data_v);
    gSPVertex((*hit_view_p)++, data_v, 3, 0);
#else
    gSPVertex((*hit_view_p)++, gDisplayListData(hit_view_d, v), 3, 0);
#endif
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

#if WIIVC
    Vtx *data_v = (Vtx*)gDisplayListDataHB(hit_view_d, v);
    data_v = HB_SEG(data_v);
    gSPVertex((*hit_view_p)++, data_v, 4, 0);
#else
    gSPVertex((*hit_view_p)++, gDisplayListData(hit_view_d, v), 4, 0);
#endif

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
    static Gfx *hit_view_disp[2];
    static int hit_view_idx = 0;

    if(kz.hitbox_view_status == COL_VIEW_START){
#ifdef WIIVC
        hit_view_disp[0] = halloc(0x800 * sizeof(*hit_view_disp[0]));
        hit_view_disp[1] = halloc(0x800 * sizeof(*hit_view_disp[1]));
#else
        hit_view_disp[0] = malloc(0x800 * sizeof(*hit_view_disp[0]));
        hit_view_disp[1] = malloc(0x800 * sizeof(*hit_view_disp[1]));
#endif
        kz.hitbox_view_status = COL_VIEW_ACTIVE;
    }else if(kz.hitbox_view_status == COL_VIEW_BEGIN_STOP){
        kz.hitbox_view_status = COL_VIEW_STOP;
        return;
    }else if(kz.hitbox_view_status == COL_VIEW_STOP){

        if(hit_view_disp[0]){
#ifdef WIIVC
            hfree(hit_view_disp[0]);
#else
            free(hit_view_disp[0]);
#endif
        }
        if(hit_view_disp[1]){
#ifdef WIIVC
            hfree(hit_view_disp[1]);
#else
            free(hit_view_disp[1]);
#endif
        }
        kz.hitbox_view_status = COL_VIEW_INACTIVE;
        return;
    }

    _Bool show = kz.hitbox_view_status == COL_VIEW_ACTIVE &&
                 zu_is_ingame() &&
                 z2_game.pause_ctx.state == 0;

    if(show){
        Gfx **z2_gfx_p;
        Gfx **z2_gfx_d;
        _Bool xlu = !settings->hit_view_opq;

        if(xlu){
            z2_gfx_p = &z2_ctxt.gfx->poly_xlu.p;
            z2_gfx_d = &z2_ctxt.gfx->poly_xlu.d;
        }else{
            z2_gfx_p = &z2_ctxt.gfx->poly_opa.p;
            z2_gfx_d = &z2_ctxt.gfx->poly_xlu.d;
        }

        Gfx *hit_view_start = hit_view_disp[hit_view_idx];
        hit_view_idx = (hit_view_idx + 1) % 2;

        Gfx *hit_view_p = hit_view_start;
        Gfx *hit_view_d = hit_view_p + 0x800;

        init_poly_list(z2_gfx_p, z2_gfx_d, xlu, 0);
        do_hitbox_view(&hit_view_p, &hit_view_d, z2_game.hitbox_ctxt.other_cnt, z2_game.hitbox_ctxt.other, 0x00FFFFFF);
        do_hitbox_view(&hit_view_p, &hit_view_d, z2_game.hitbox_ctxt.ac_cnt, z2_game.hitbox_ctxt.ac, 0x000000FF);
        do_hitbox_view(&hit_view_p, &hit_view_d, z2_game.hitbox_ctxt.attack_cnt, z2_game.hitbox_ctxt.attack, 0x00FF0000);
        gSPEndDisplayList(hit_view_p++);

        void *dlist = hit_view_start;
#ifdef WIIVC
        set_hb_seg(z2_gfx_p);
        dlist = HB_SEG(dlist);
#endif
        gSPDisplayList((*z2_gfx_p)++, dlist);
#ifdef WIIVC
        restore_hb_seg(z2_gfx_p);
#endif
    }
}

static void do_poly_list(poly_writer_t *writer, line_writer_t *line_writer, struct vector *line_set, z2_xyz_t *vtx, z2_col_poly_t *polys, z2_col_type_t *types, int poly_cnt, _Bool redux){
    for(int i = 0;i < poly_cnt; i++)
    {
        z2_col_poly_t *poly = &polys[i];
        z2_col_type_t *type = &types[poly->type];
        z2_xyz_t *va = &vtx[poly->va];
        z2_xyz_t *vb = &vtx[poly->vb];
        z2_xyz_t *vc = &vtx[poly->vc];
        uint32_t color;
        _Bool skip = 0;

        if(type->flags_2.hookshot)
        {
            color = 0x0000FFFF;
        }
        else if(type->flags_1.interaction > 1)
        {
            color = 0xFF00FFFF;
        }
        else if(type->flags_1.special == 0xC)
        {
            color = 0xFF0000FF;
        }
        else if(type->flags_1.exit != 0 || type->flags_1.special == 0x5)
        {
            color = 0x00FFFFFF;
        }
        else if(type->flags_1.behavior != 0 || type->flags_2.wall_damage)
        {
            color = 0x00FF00FF;
        }
        else if(type->flags_2.terrain == 1)
        {
            color = 0xFFFF00FF;
        }
        else if(redux)
        {
            skip = 1;
        }
        else
        {
            color = 0xFFFFFFFF;
        }

        if(!skip)
        {
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

        if(line_writer != NULL) {
            line_t lab = { polys->va, polys->vb };
            line_t lbc = { polys->vb, polys->vc };
            line_t lac = { polys->va, polys->vc };

            _Bool ab = 1;
            _Bool bc = 1;
            _Bool ca = 1;

            if(line_set != NULL) {
                for(int i = 0; i < line_set->size; i++) {
                    line_t *l = vector_at(line_set, i);
                    if((l->a == lab.a && l->b == lab.b) || (l->a == lab.b && l->b == lab.a)) {
                        ab = 0;
                    }

                    if((l->a == lbc.a && l->b == lbc.b) || (l->a == lbc.b && l->b == lbc.a)) {
                        bc = 0;
                    }

                    if((l->a == lac.a && l->b == lac.b) || (l->a == lac.b && l->b == lac.a)) {
                        ca = 0;
                    }
                }
            }

            Vtx v[3];
            int vtx_cnt = 0;
            if(ab || ca) {
                v[vtx_cnt++] = gdSPDefVtxC(va->x, va->y, va->z, 0, 0, 0, 0, 0, 0xFF);
            }

            if(ab || bc) {
                v[vtx_cnt++] = gdSPDefVtxC(vb->x, vb->y, vb->z, 0, 0, 0, 0, 0, 0xFF);
            }

            if(bc || ca) {
                v[vtx_cnt++] = gdSPDefVtxC(vc->x, vc->y, vc->z, 0, 0, 0, 0, 0, 0xFF);
            }

            line_writer_append(line_writer, v, vtx_cnt);
        }
    }
}

static void do_dynamic_list(poly_writer_t *writer, line_writer_t *line_writer, z2_col_hdr_t *hdr, uint16_t idx){
    z2_col_ctxt_t *col_ctx = &z2_game.col_ctxt;
    while(idx != 0xFFFF){
        z2_col_list_t *list = &col_ctx->dyn.list[idx];
        do_poly_list(writer, line_writer, NULL, col_ctx->dyn_vtx, &col_ctx->dyn_poly[list->poly_idx],
                     hdr->type, 1, 0);
        idx = list->list_next;
    }
}

static void do_dynamic_collision(poly_writer_t *writer, line_writer_t *line_writer){
    z2_col_ctxt_t *col_ctx = &z2_game.col_ctxt;
    for(int i = 0;i < 32;i++){
        if(col_ctx->dynamic_flags[i].active){
            z2_col_chk_actor_t *col = &col_ctx->actors[i];
            do_dynamic_list(writer, line_writer, col->col_hdr, col->ceiling_list_idx);
            do_dynamic_list(writer, line_writer, col->col_hdr, col->wall_list_idx);
            do_dynamic_list(writer, line_writer, col->col_hdr, col->floor_list_idx);
        }
    }
}

static size_t calc_poly_cnt(void) {
    z2_col_hdr_t *col_hdr = z2_game.col_ctxt.col_hdr;

    if(!settings->col_view_red) {
        return col_hdr->n_poly;
    }

    size_t poly_cnt = 0;
    z2_col_poly_t *polys = col_hdr->poly;
    z2_col_type_t *types = col_hdr->type;

    for(int i = 0; i < col_hdr->n_poly; i++) {
        z2_col_type_t *type = &types[polys[i].type];

        if(type->flags_2.hookshot || type->flags_1.interaction > 1 || type->flags_1.special == 0xC ||
           type->flags_1.exit != 0 || type->flags_1.special == 5 || type->flags_1.behavior != 0 ||
           type->flags_2.wall_damage || type->flags_2.terrain == 1) {
               poly_cnt++;
        }
    }
}

void alloc_col_buf(void **buf, size_t size) {
#ifdef WIIVC
    *buf = halloc(size);
#else
    *buf = malloc(size);
#endif
}

void free_col_buf(void **buf) {
    if(*buf == NULL) {
        return;
    }

#ifdef WIIVC
    hfree(*buf);
#else
    free(*buf);
#endif

    *buf = NULL;
}

void kz_col_view(){
    const size_t dyn_poly_size = 0x1000;
    const size_t dyn_line_size = 0x1000;

    static Gfx *static_col = NULL;
    static Gfx *static_line = NULL;
    static Gfx *dynamic_gfx[2] = { NULL, NULL };
    static Gfx *dynamic_line[2] = { NULL, NULL };
    static int dynamic_idx = 0;
    static uint16_t col_view_scene = 0;
    static uint32_t col_view_prev_line;
    static uint32_t col_view_prev_reduced;

    poly_writer_t poly_writer;
    line_writer_t line_writer;

    _Bool line_enable = settings->col_view_line;

    bool enabled = zu_is_ingame() && z2_game.pause_ctx.state == 0;
    bool init = kz.collision_view_status == COL_VIEW_START || kz.collision_view_status == COL_VIEW_RESTART;

    if(enabled && kz.collision_view_status == COL_VIEW_ACTIVE && settings->col_view_upd &&
        (col_view_scene != z2_game.scene_index || col_view_prev_line != settings->col_view_line ||
         col_view_prev_reduced != settings->col_view_red)) {
             kz.collision_view_status = COL_VIEW_BEGIN_RESTART;
    }

    switch(kz.collision_view_status) {
        case COL_VIEW_BEGIN_STOP:
            kz.collision_view_status = COL_VIEW_STOP;
            break;
        case COL_VIEW_BEGIN_RESTART:
            kz.collision_view_status = COL_VIEW_RESTART;
            break;
        case COL_VIEW_STOP:
            kz.collision_view_status = COL_VIEW_INACTIVE;
        case COL_VIEW_RESTART:
            free_col_buf(&static_col);
            free_col_buf(&static_line);
            free_col_buf(&dynamic_gfx[0]);
            free_col_buf(&dynamic_gfx[1]);
            free_col_buf(&dynamic_line[0]);
            free_col_buf(&dynamic_line[1]);
            break;
    }

    if (enabled && init) {
        z2_col_hdr_t *col_hdr = z2_game.col_ctxt.col_hdr;
        col_view_scene = z2_game.scene_index;
        col_view_prev_line = settings->col_view_line;
        col_view_prev_reduced = settings->col_view_red;

        size_t poly_cnt = calc_poly_cnt();
        size_t poly_size = (poly_cnt * 9) + 11;

        alloc_col_buf(&static_col, poly_size * sizeof(*static_col));
        alloc_col_buf(&dynamic_gfx[0], dyn_poly_size * sizeof(*dynamic_gfx[0]));
        alloc_col_buf(&dynamic_gfx[1], dyn_poly_size * sizeof(*dynamic_gfx[1]));

        Gfx *static_col_p = static_col;
        Gfx *static_col_d = static_col + poly_size; 
        poly_writer_init(&poly_writer, static_col_p, static_col_d);

        Gfx *static_line_p = NULL;
        Gfx *static_line_d = NULL;
        line_writer_t *line_writer_p = NULL;
        struct vector line_set;
        if(line_enable) {
            size_t line_size = (poly_cnt * 20) + 24;
            alloc_col_buf(&static_line, line_size * sizeof(*static_line));
            alloc_col_buf(&dynamic_line[0], dyn_line_size * sizeof(*dynamic_line[0]));
            alloc_col_buf(&dynamic_line[1], dyn_line_size * sizeof(*dynamic_line[1]));

            static_line_p = static_line;
            static_line_d = static_line + line_size;
            line_writer_p = &line_writer;
            line_writer_init(line_writer_p, static_line_p, static_line_d);
            vector_init(&line_set, sizeof(line_t));
        }

        do_poly_list(&poly_writer, line_writer_p, &line_set, col_hdr->vtx, col_hdr->poly, col_hdr->type, col_hdr->n_poly, settings->col_view_red);

        poly_writer_finish(&poly_writer, &static_col_p, &static_col_d);
        gSPEndDisplayList(static_col_p++);

        if(line_enable) {
            line_writer_finish(line_writer_p, &static_line_p, &static_line_d);
            gSPEndDisplayList(static_line_p++);
            vector_destroy(&line_set);
        }

        kz.collision_view_status = COL_VIEW_ACTIVE;
    }

    bool active = kz.collision_view_status == COL_VIEW_ACTIVE;

    if(enabled && (init || (active && settings->col_view_upd))) {
        Gfx *dyn_poly = NULL;
        Gfx *dyn_poly_p = NULL;
        Gfx *dyn_poly_d = NULL;
        Gfx *dyn_line = NULL;
        Gfx *dyn_line_p = NULL;
        Gfx *dyn_line_d = NULL;
        line_writer_t *line_writer_p = NULL;

        dynamic_idx = (dynamic_idx + 1) % 2;

        dyn_poly = dynamic_gfx[dynamic_idx];
        dyn_poly_p = dyn_poly;
        dyn_poly_d = dyn_poly + dyn_poly_size;
        poly_writer_init(&poly_writer, dyn_poly_p, dyn_poly_d);

        if(line_enable) {
            dyn_line = dynamic_line[dynamic_idx];
            dyn_line_p = dyn_line;
            dyn_line_d = dyn_line + dyn_line_size;
            line_writer_p = &line_writer;
            
            line_writer_init(line_writer_p, dyn_line_p, dyn_line_d);
        }

        do_dynamic_collision(&poly_writer, line_writer_p);

        poly_writer_finish(&poly_writer, &dyn_poly_p, &dyn_poly_d);
        gSPEndDisplayList(dyn_poly_p++);

        if(line_enable) {
            line_writer_finish(line_writer_p, &dyn_line_p, &dyn_line_d);
            gSPEndDisplayList(dyn_line_p++);
        }
    }

    if(enabled && active) {
        Gfx **gfx_p;
        Gfx **gfx_d;
        if(settings->col_view_opq) {
            gfx_p = &z2_game.common.gfx->poly_opa.p;
            gfx_d = &z2_game.common.gfx->poly_opa.d;
        } else {
            gfx_p = &z2_game.common.gfx->poly_xlu.p;
            gfx_d = &z2_game.common.gfx->poly_xlu.d;
        }

        init_poly_list(gfx_p, gfx_d, !settings->col_view_opq, true);
        
        gSPSetGeometryMode((*gfx_p)++, G_CULL_BACK);
#ifdef WIIVC
        set_hb_seg(gfx_p);
        gSPDisplayList((*gfx_p)++, HB_SEG(static_col));
        gSPDisplayList((*gfx_p)++, HB_SEG(dynamic_gfx[dynamic_idx]));
#else
        gSPDisplayList((*gfx_p)++, static_col);
        gSPDisplayList((*gfx_p)++, dynamic_gfx[dynamic_idx]);
#endif

        if(line_enable) {
            init_line_gfx(gfx_p, gfx_d, !settings->col_view_opq);

#ifdef WIIVC
            gSPDisplayList((*gfx_p)++, HB_SEG(static_line));
            gSPDisplayList((*gfx_p)++, HB_SEG(dynamic_line[dynamic_idx]));
#else
            gSPDisplayList((*gfx_p)++, static_line);
            gSPDisplayList((*gfx_p)++, dynamic_line[dynamic_idx]);
#endif
        }

#ifdef WIIVC
        restore_hb_seg(gfx_p);
#endif
    }
}