/*
* kz.h
*
* definitions for the main kz system
*/

#ifndef _KZ_H
#define _KZ_H

#include <stdint.h>
#include <vector/vector.h>
#include "z2.h"
#include "settings.h"
#include "collision_view.h"
#include "menu.h"
#include "watches.h"
#include "zu.h"
#include "vec_math.h"

#define KZ_MEMFILE_MAX 3
#define KZ_POSITION_MAX 3

#define KZ_LITE 0x00
#define KZ_FULL 0x01
#define KZ_VC   0x02

#ifndef KZ_VERSION
#define KZ_VERSION KZ_FULL
#endif

#ifdef LITE
#define KZ_LOG_MAX 3
#else
#define KZ_LOG_MAX 10
#endif

#define STATE_MAX 20

enum cheats {
    /* 0x00 */ CHEAT_STICKS,
    /* 0x01 */ CHEAT_NUTS,
    /* 0x02 */ CHEAT_ARROWS,
    /* 0x03 */ CHEAT_BOMBS,
    /* 0x04 */ CHEAT_BOMBCHUS,
    /* 0x05 */ CHEAT_POWDER_KEG,
    /* 0x06 */ CHEAT_HEALTH,
    /* 0x07 */ CHEAT_MAGIC,
    /* 0x08 */ CHEAT_BLAST_MASK,
    /* 0x09 */ CHEAT_RESTRICTION,
    /* 0x0A */ CHEAT_ISG,
    /* 0x0B */ CHEAT_RUPEES,
    /* 0x0C */ CHEAT_TURBO,
    /* 0x0D */ CHEAT_FREEZE_TIME,
    /* 0x0E */ CHEAT_MAX
};

typedef struct {
    uint16_t    z2_version;
    uint16_t    timer_boundaries[3];
    z2_file_t   file;
    uint32_t    scene_flags[5];
    uint16_t    scene;
} memfile_t;

typedef struct {
    z2_xyzf_t   pos;
    z2_rot_t    rot;
} position_t;

struct log {
    char   *mesg;
    int     time;
};

typedef struct {
    _Bool                   ready;
    struct list             watches;
    enum col_view_status    collision_view_status;
    enum col_view_status    hitbox_view_status;
    menu_t                  main_menu;
    uint8_t                 settings_profile;
    _Bool                   menu_active;
    int64_t                 cpu_cycle_counter;
    int64_t                 cpu_offset;
    int64_t                 cpu_prev;
    _Bool                   timer_running;
    int32_t                 frames;
    int32_t                 frames_offset;
    _Bool                   lag_counter;
    _Bool                   hide_actors;
    _Bool                   hide_room;
    int32_t                 pending_frames;
    zu_disp_ptr_t           disp_p;
    uint8_t                 memfile_slot;
    memfile_t             **memfile;
    uint8_t                 pos_slot;
    position_t            **position_save;
    const char             *tooltip;
    struct log              log[KZ_LOG_MAX];
    int                     prev_timespeed;
    _Bool                   free_cam_active;
    _Bool                   free_cam_locked;
    _Bool                   draw_camera;
    z2_xyzf_t               cam_at;
    z2_xyzf_t               cam_eye;
    z2_xyzf_t               kz_eye;
    z2_xyzf_t               kz_at;
    sph_coord_t             cam_sph;
    uint16_t                input_mask;
    uint8_t                 stick_x_mask, stick_y_mask;
#ifndef LITE
    void                   *states[STATE_MAX];
    int                     state_slot;
#endif
} kz_ctxt_t;

menu_t *create_warps_menu       (void);
menu_t *create_inventory_menu   (void);
menu_t *create_scene_menu       (void);
menu_t *create_watches_menu     (void);
menu_t *create_cheats_menu      (void);
menu_t *create_file_menu        (void);
menu_t *create_equips_menu      (void);
menu_t *create_settings_menu    (void);
#ifndef LITE
menu_t *create_debug_menu       (void);
#endif
void    kz_log                  (const char *format, ...);

extern kz_ctxt_t    kz;
extern char         restriction_table[0x23A];

#endif
