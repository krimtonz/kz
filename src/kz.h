#ifndef _KZ_H
#define _KZ_H

#include <stdint.h>
#include <vector/vector.h>
#include "z2.h"
#include "menu.h"
#include "settings.h"
#include "collision_view.h"

#define KZ_MEMFILE_MAX 3
#define KZ_POSITION_MAX 3

#define KZ_LITE 0x00
#define KZ_FULL 0x01

#ifndef KZ_VERSION
#define KZ_VERSION KZ_FULL
#endif

#ifdef LITE
#define KZ_LOG_MAX 3
#else
#define KZ_LOG_MAX 10
#endif

enum cheats {
    CHEAT_STICKS,
    CHEAT_NUTS,
    CHEAT_ARROWS,
    CHEAT_BOMBS,
    CHEAT_BOMBCHUS,
    CHEAT_POWDER_KEG,
    CHEAT_HEALTH,
    CHEAT_MAGIC,
    CHEAT_BLAST_MASK,
    CHEAT_RESTRICTION,
    CHEAT_ISG,
    CHEAT_RUPEES,
    CHEAT_TURBO,
    CHEAT_FREEZE_TIME,
    CHEAT_MAX
};

struct disp_p
{
    uint32_t    work_p;
    uint32_t    work_d;
    uint32_t    poly_opa_p;
    uint32_t    poly_opa_d;
    uint32_t    poly_xlu_p;
    uint32_t    poly_xlu_d;
    uint32_t    overlay_p;
    uint32_t    overlay_d;
};

typedef struct{
    uint16_t    z2_version;
    uint16_t    timer_boundaries[3];
    z2_file_t   file;
    uint32_t    scene_flags[5];
    uint16_t    scene;
} memfile_t;

typedef struct{
    z2_xyzf_t   pos;
    z2_rot_t    rot;
} position_t;

struct log {
    char   *mesg;
    int     time;
};

typedef struct  {
    _Bool                   ready;
    struct list             watches;
    enum col_view_status    collision_view_status;
    enum col_view_status    hitbox_view_status;
    struct menu             main_menu;
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
    struct disp_p           disp_p;
    uint8_t                 memfile_slot;
    memfile_t             **memfile;
    uint8_t                 pos_slot;
    position_t            **position_save;
    const char             *tooltip;
    struct log              log[KZ_LOG_MAX];
    _Bool                   debug_active;
    int                     prev_timespeed;
} kz_ctxt_t;

extern kz_ctxt_t kz;
extern char restriction_table[0x23A];

struct menu *create_warps_menu();
struct menu *create_inventory_menu();
struct menu *create_scene_menu();
struct menu *create_watches_menu();
struct menu *create_cheats_menu();
struct menu *create_file_menu();
struct menu *create_equips_menu();
struct menu *create_settings_menu();
#ifndef LITE
struct menu *create_debug_menu();
#endif
void kz_log(const char *format, ...);

#endif