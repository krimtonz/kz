#include <math.h>
#include "gu.h"
#include "vec_math.h"

void guPerspectiveF(MtxF *mf, uint16_t *perspNorm, float fovy, float aspect,
                    float near, float far, float scale)
{
    float cot = cos(fovy / 2.f) / sin(fovy / 2.f);
    mf->xx = cot / aspect * scale;
    mf->xy = 0.f;
    mf->xz = 0.f;
    mf->xw = 0.f;
    mf->yx = 0.f;
    mf->yy = cot * scale;
    mf->yz = 0.f;
    mf->yw = 0.f;
    mf->zx = 0.f;
    mf->zy = 0.f;
    mf->zz = (near + far) / (near - far) * scale;
    mf->zw = -1.f * scale;
    mf->wx = 0.f;
    mf->wy = 0.f;
    mf->wz = 2.f * near * far / (near - far) * scale;
    mf->ww = 0.f;
}

void guLookAtF(MtxF *mf,
                float eye_x, float eye_y, float eye_z,
                float at_x, float at_y, float at_z,
                float up_x, float  up_y, float up_z){
    z2_xyzf_t eye, at, up, forward, right, up_calc, trans;
    eye.x = eye_x;
    eye.y = eye_y;
    eye.z = eye_z;

    at.x = at_x;
    at.y = at_y;
    at.z = at_z;

    up.x = up_x;
    up.y = up_y;
    up.z = up_z;

    vec3f_sub(&at, &eye, &forward);   
    vec3f_normalize(&forward, &forward);

    vec3f_cross(&forward, &up, &right);
    vec3f_normalize(&right, &right);

    vec3f_cross(&right, &forward, &up_calc);
    vec3f_normalize(&up_calc, &up_calc);

    trans.x = -(eye.x * right.x + eye.y * right.y + eye.z * right.z);
    trans.y = -(eye.x * up_calc.x + eye.y * up_calc.y + eye.z * up_calc.z);
    trans.z = -(eye.x * forward.x + eye.y * forward.y + eye.z * forward.z);

    *mf = (MtxF)guDefMtxF(
        right.x, up.x, -forward.x, 0.0f,
        right.y, up.y, -forward.y, 0.0f,
        right.z, up.z, -forward.z, 0.0f,
        trans.x, trans.y, -trans.z, 1.0f 
    );

}

void guMtxCatF(const MtxF *m, const MtxF *n, MtxF *r)
{
    MtxF t;
    t.xx = m->xx * n->xx + m->xy * n->yx + m->xz * n->zx + m->xw * n->wx;
    t.xy = m->xx * n->xy + m->xy * n->yy + m->xz * n->zy + m->xw * n->wy;
    t.xz = m->xx * n->xz + m->xy * n->yz + m->xz * n->zz + m->xw * n->wz;
    t.xw = m->xx * n->xw + m->xy * n->yw + m->xz * n->zw + m->xw * n->ww;
    t.yx = m->yx * n->xx + m->yy * n->yx + m->yz * n->zx + m->yw * n->wx;
    t.yy = m->yx * n->xy + m->yy * n->yy + m->yz * n->zy + m->yw * n->wy;
    t.yz = m->yx * n->xz + m->yy * n->yz + m->yz * n->zz + m->yw * n->wz;
    t.yw = m->yx * n->xw + m->yy * n->yw + m->yz * n->zw + m->yw * n->ww;
    t.zx = m->zx * n->xx + m->zy * n->yx + m->zz * n->zx + m->zw * n->wx;
    t.zy = m->zx * n->xy + m->zy * n->yy + m->zz * n->zy + m->zw * n->wy;
    t.zz = m->zx * n->xz + m->zy * n->yz + m->zz * n->zz + m->zw * n->wz;
    t.zw = m->zx * n->xw + m->zy * n->yw + m->zz * n->zw + m->zw * n->ww;
    t.wx = m->wx * n->xx + m->wy * n->yx + m->wz * n->zx + m->ww * n->wx;
    t.wy = m->wx * n->xy + m->wy * n->yy + m->wz * n->zy + m->ww * n->wy;
    t.wz = m->wx * n->xz + m->wy * n->yz + m->wz * n->zz + m->ww * n->wz;
    t.ww = m->wx * n->xw + m->wy * n->yw + m->wz * n->zw + m->ww * n->ww;
    *r = t;
}

void guRotateF(MtxF *mf, float a, float x, float y, float z)
{
    float s = sin(a);
    float c = cos(a);
    mf->xx = x * x + c * (1.f - x * x);
    mf->xy = x * y * (1.f - c) + z * s;
    mf->xz = x * z * (1.f - c) - y * s;
    mf->xw = 0.f;
    mf->yx = y * x * (1.f - c) - z * s;
    mf->yy = y * y + c * (1.f - y * y);
    mf->yz = y * z * (1.f - c) + x * s;
    mf->yw = 0.f;
    mf->zx = z * x * (1.f - c) + y * s;
    mf->zy = z * y * (1.f - c) - x * s;
    mf->zz = z * z + c * (1.f - z * z);
    mf->zw = 0.f;
    mf->wx = 0.f;
    mf->wy = 0.f;
    mf->wz = 0.f;
    mf->ww = 1.f;
}

void guRotateX(MtxF *mf, float x) {
    float cos = cosf(x);
    float sin = sinf(x);
    mf->xx = 1.0f;
    mf->xy = 0.0f;
    mf->xz = 0.0f;
    mf->xw = 0.0f;
    mf->yx = 0.0f;
    mf->yy = cos;
    mf->yz = -sin;
    mf->yw = 0.0f;
    mf->zx = 0.0f;
    mf->zy = sin;
    mf->zz = cos;
    mf->zw = 0.0f;
    mf->wx = 0.0f;
    mf->wy = 0.0f;
    mf->wz = 0.0f;
    mf->ww = 1.0f;
}

void guRotateRPYF(MtxF *mf, float r, float p, float h){
    float cosr = cosf(r);
    float cosp = cosf(p);
    float cosh = cosf(h);
    float sinr = sinf(r);
    float sinp = sinf(p);
    float sinh = sinf(h);

    mf->xx = cosp * cosh;
    mf->xy = cosp * sinh;
    mf->xz = -sinp;
    mf->yx = sinr * sinp * cosh - cosr * sinh;
    mf->yy = sinr * sinp * sinh + cosr * cosh;
    mf->yz = sinr * cosp;
    mf->zx = cosr * sinp * cosh + sinr * sinh;
    mf->zy = cosr * sinp * sinh - sinr * cosh;
    mf->zz = cosr * cosp;
    mf->zw = 0.f;
    mf->wx = 0.f;
    mf->wy = 0.f;
    mf->wz = 0.f;
    mf->ww = 1.f;
}

void guTranslateF(MtxF *mf, float x, float y, float z)
{
  *mf = (MtxF)guDefMtxF(1.f, 0.f, 0.f, 0.f,
                        0.f, 1.f, 0.f, 0.f,
                        0.f, 0.f, 1.f, 0.f,
                        x,   y,   z,   1.f);
}

void guMtxF2L(const MtxF *mf, Mtx *m)
{
    for (int i = 0; i < 16; ++i) {
        qs1616_t n = qs1616(mf->f[i]);
        m->i[i] = (n >> 16) & 0x0000FFFF;
        m->f[i] = (n >> 0)  & 0x0000FFFF;
    }
}

void guMtxIdent(Mtx *m){
    *m = (Mtx)gdSPDefMtx(1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, 1, 0,
                         0, 0, 0, 1);
}

void guScaleF(MtxF *mf, float x, float y, float z){
    *mf = (MtxF)guDefMtxF(x,    0.f,    0.f,    0.f,
                          0.f,  y,      0.f,    0.f,
                          0.f,  0.f,    z,      0.f,
                          0.f,  0.f,    0.f,    1.f);
}