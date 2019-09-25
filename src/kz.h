#ifndef _KZ_H
#define _KZ_H

#include <stdint.h>
#include <vector/vector.h>
#include "z2.h"
#include "menu.h"
#include "settings.h"
#include "collision_view.h"

#define KZ_MEMFILE_MAX 3

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
    CHEAT_MAX
};

struct disp_p
{
  uint32_t        work_p;
  uint32_t        work_d;
  uint32_t        poly_opa_p;
  uint32_t        poly_opa_d;
  uint32_t        poly_xlu_p;
  uint32_t        poly_xlu_d;
  uint32_t        overlay_p;
  uint32_t        overlay_d;
};

typedef struct{
    z2_file_t file;
    z2_xyzf_t link_pos;
}memfile_t;

typedef struct  {
    _Bool                   ready;
    struct list             watches;
    enum col_view_settings  collision_view_settings;
    enum col_view_status    collision_view_status;
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
    int32_t                 pending_frames;
    struct disp_p           disp_p;
    uint8_t                 memfile_slot;
    memfile_t             **memfile;
} kz_ctxt_t;

extern kz_ctxt_t kz;

struct menu *create_warps_menu();
struct menu *create_inventory_menu();
struct menu *create_scene_menu();
struct menu *create_watches_menu();
struct menu *create_cheats_menu();
struct menu *create_file_menu();
struct menu *create_equips_menu();
struct menu *create_settings_menu();

#endif