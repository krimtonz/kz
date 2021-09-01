/*
* scenes.h
*
* defintions for the game's scenes
*/

#ifndef _SCENES_H
#define _SCENES_H
#include <stdint.h>
#include "z2.h"

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

#if Z2_VERSION==NZSJ10
#define CAT_CNT 10
#else
#define CAT_CNT 9
#endif

extern struct kz_scene          scenes[];
extern struct kz_scene_category scene_categories[CAT_CNT];

#define NUMARGS(...) (sizeof((char*[]){__VA_ARGS__})/sizeof(char*))
#define NUMARGSI8(...) (sizeof((uint8_t[]){__VA_ARGS__})/sizeof(uint8_t))

#define SCENE_INIT(__idx, __name, ...) \
{ \
    __idx, NUMARGS(__VA_ARGS__), \
    __name, \
    (char*[]){ \
        __VA_ARGS__ \
    } \
}

#define SCENE_CATEGORY_INIT(__name, ...) \
{ \
    __name, NUMARGSI8(__VA_ARGS__), \
    (uint8_t[]){ \
        __VA_ARGS__ \
    } \
}

#endif
