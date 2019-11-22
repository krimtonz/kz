#ifndef _SCENES_H
#define _SCENES_H
#include <stdint.h>

struct kz_scene_category {
    char       *name;
    int         scene_cnt;
    uint8_t    *scenes;
};

struct kz_scene {
    uint16_t        scene;
    uint16_t        entrance_cnt;
    char           *scene_name;
    char          **entrances;
};

extern struct kz_scene          scenes[];
extern struct kz_scene_category scene_categories[10];

#endif