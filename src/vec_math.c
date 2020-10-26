#include <math.h>
#include "vec_math.h"

void vec3f_add(z2_xyzf_t *a, z2_xyzf_t *b, z2_xyzf_t *dest)
{
    dest->x = a->x + b->x;
    dest->y = a->y + b->y;
    dest->z = a->z + b->z;
}

void vec3f_sub(z2_xyzf_t *a, z2_xyzf_t *b, z2_xyzf_t *dest)
{
    dest->x = a->x - b->x;
    dest->y = a->y - b->y;
    dest->z = a->z - b->z;
}

void vec3f_scalar_mul(z2_xyzf_t *a, float s, z2_xyzf_t *dest)
{
    dest->x = a->x * s;
    dest->y = a->y * s;
    dest->z = a->z * s;
}

float vec3f_dot(z2_xyzf_t *a, z2_xyzf_t *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void vec3f_cross(z2_xyzf_t *a, z2_xyzf_t *b, z2_xyzf_t *dest)
{
    dest->x = a->y * b->z - a->z * b->y;
    dest->y = a->z * b->x - a->x * b->z;
    dest->z = a->x * b->y - a->z * b->x;
}

void vec3f_normalize(z2_xyzf_t *src, z2_xyzf_t *dest)
{
    float dist = sqrtf(src->x * src->x + src->y * src->y + src->z * src->z);
    dest->x = src->x / dist;
    dest->y = src->y / dist;
    dest->z = src->z / dist;
}

void vec3f_to_sph(z2_xyzf_t *vec, sph_coord_t *sph)
{
    sph->dist = sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
    sph->pitch = atan2f(sqrtf(vec->x * vec->x + vec->z * vec->z), vec->y);
    sph->yaw = atan2f(vec->x, vec->z);
}

void sph_to_vec3f(sph_coord_t *sph, z2_xyzf_t *vec)
{
    float sin_p = sinf(sph->pitch);
    float cos_p = cosf(sph->pitch);
    float sin_y = sinf(sph->yaw);
    float cos_y = cosf(sph->yaw);

    vec->x = sph->dist * sin_p * sin_y;
    vec->y = sph->dist * cos_p;
    vec->z = sph->dist * sin_p * cos_y;
}

void vec3f_to_geo(z2_xyzf_t *vec, sph_coord_t *geo)
{
    vec3f_to_sph(vec, geo);
    geo->pitch = (M_PI / 2.0f) - geo->pitch;
}

void geo_to_vec3f(sph_coord_t *geo, z2_xyzf_t *vec)
{
    sph_coord_t sph = *geo;
    sph.pitch = (M_PI / 2.0f) - geo->pitch;
    sph_to_vec3f(&sph, vec);
}