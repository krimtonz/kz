#ifndef _GU_H
#define _GU_h

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

#endif