#ifndef _VEC_MATH_H
#define _VEC_MATH_H

#include "z2.h"

typedef struct {
    float pitch;
    float yaw;
    float dist;
} sph_coord_t;

void geo_to_vec3f(sph_coord_t *geo, z2_xyzf_t *vec);
void vec3f_to_geo(z2_xyzf_t *vec, sph_coord_t *geo);
void sph_to_vec3f(sph_coord_t *sph, z2_xyzf_t *vec);
void vec3f_to_sph(z2_xyzf_t *vec, sph_coord_t *sph);
float vec3f_dot(z2_xyzf_t *a, z2_xyzf_t *b);
void vec3f_cross(z2_xyzf_t *a, z2_xyzf_t *b, z2_xyzf_t *dest);
void vec3f_scalar_mul(z2_xyzf_t *a, float s, z2_xyzf_t *dest);
void vec3f_sub(z2_xyzf_t *a, z2_xyzf_t *b, z2_xyzf_t *dest);
void vec3f_add(z2_xyzf_t *a, z2_xyzf_t *b, z2_xyzf_t *dest);
void vec3f_normalize(z2_xyzf_t *src, z2_xyzf_t *dest);

#endif