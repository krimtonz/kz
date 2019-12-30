/*
* gu.h
*
* definitions for graphics utilities
*/

#ifndef _GU_H
#define _GU_H

#include <n64/gbi.h>

#define                           M_PI 3.14159265358979323846
#define guDefMtxF(xx,xy,xz,xw,  \
                  yx,yy,yz,yw,  \
                  zx,zy,zz,zw,  \
                  wx,wy,wz,ww)    {.f={xx,xy,xz,xw,                           \
                                       yx,yy,yz,yw,                           \
                                       zx,zy,zz,zw,                           \
                                       wx,wy,wz,ww}}

typedef float MtxF_t[4][4];

typedef union {
    MtxF_t mf;
    float f[16];
    struct {
        float xx, xy, xz, xw,
              yx, yy, yz, yw,
              zx, zy, zz, zw,
              wx, wy, wz, ww;
    };
}MtxF;

void    guRotateF       (MtxF *mf, float a, float x, float y, float z);
void    guMtxCatF       (const MtxF *m, const MtxF *n, MtxF *r);
void    guMtxF2L        (const MtxF *mf, Mtx *m);
void    guTranslateF    (MtxF *mf, float x, float y, float z);
void    guPerspectiveF  (MtxF *mf, uint16_t *perspNorm, float fovy, float aspect, float near, float far, float scale);
void    guMtxIdent      (Mtx *m);
void    guScaleF        (MtxF *mf, float x, float y, float z);
#endif