#include <math.h>
#include "kz.h"
#include "z2.h"
#include "input.h"
#include "vec_math.h"

void kz_camera_calc(void) {
    sph_coord_t geo;
    z2_xyzf_t diff;
    vec3f_sub(&kz.kz_eye, &kz.kz_at, &diff);
    vec3f_to_geo(&diff, &geo);

    geo.yaw -= input_x() / 500.0f;
    geo.pitch += input_y() / 500.0f;

    if(geo.pitch < -((M_PI / 2.0f) + 0.02f) ){
        geo.pitch = -((M_PI / 2.0f) + 0.02f);
    } else if(geo.pitch > (M_PI / 2.0f) - 0.02f){
        geo.pitch = (M_PI / 2.0f) - 0.02f;
    }

    z2_xyzf_t move, strafe;
    sph_coord_t shift = geo;
    geo_to_vec3f(&geo, &move);
    shift.pitch = 0.0f;
    shift.yaw = shift.yaw - (M_PI / 2.0f);
    geo_to_vec3f(&shift, &strafe);
    if(input_pressed() & BUTTON_C_UP){
        vec3f_scalar_mul(&move, -45.0f / geo.dist, &move);
        vec3f_add(&kz.kz_at, &move, &kz.kz_at);
    }
    if(input_pressed() & BUTTON_C_DOWN){
        vec3f_scalar_mul(&move, 45.0f / geo.dist, &move);
        vec3f_add(&kz.kz_at, &move, &kz.kz_at);
    }
    if(input_pressed() & BUTTON_C_LEFT){
        vec3f_scalar_mul(&strafe, 15.0f / geo.dist, &strafe);
        vec3f_add(&kz.kz_at, &strafe, &kz.kz_at);
    }
    if(input_pressed() & BUTTON_C_RIGHT){
        vec3f_scalar_mul(&strafe, -15.0f / geo.dist, &strafe);
        vec3f_add(&kz.kz_at, &strafe, &kz.kz_at);
    }

    z2_xyzf_t geo_vec;

    geo_to_vec3f(&geo, &geo_vec);
    vec3f_add(&kz.kz_at, &geo_vec, &kz.kz_eye);
}