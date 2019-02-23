#ifndef _Z64_H
#define _Z64_H
#include <n64.h>
#include <stdint.h>

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} z64_xyz_t;

typedef struct
{
    float x;
    float y;
    float z;
} z64_xyzf_t;

typedef uint16_t z64_angle_t;
typedef struct
{
    z64_angle_t x;
    z64_angle_t y;
    z64_angle_t z;
} z64_rot_t;

typedef enum {
    Z64_ITEM_NULL = -1,
    Z64_ITEM_OCARINA,
    Z64_ITEM_HEROS_BOW,
    Z64_ITEM_FIRE_ARROW,
    Z64_ITEM_ICE_ARROW,
    Z64_ITEM_LIGHT_ARROW,
    Z64_ITEM_FAIRY_OCARINA,
    Z64_ITEM_BOMB,
    Z64_ITEM_BOMBCHU,
    Z64_ITEM_STICK,
    Z64_ITEM_NUT,
    Z64_ITEM_MAGIC_BEAN,
    Z64_ITEM_SLINGSHOT,
    Z64_ITEM_POWDER_KEG,
    Z64_ITEM_PICTOGRAPH_BOX,
    Z64_ITEM_LENS,
    Z64_ITEM_HOOKSHOT,
    Z64_ITEM_GREAT_FAIRY_SWORD,
    Z64_ITEM_OOT_HOOKSHOT,
    Z64_ITEM_BOTTLE,
    Z64_ITEM_RED_POTION,
    Z64_ITEM_GREEN_POTION,
    Z64_ITEM_BLUE_POTION,
    Z64_ITEM_FAIRY,
    Z64_ITEM_DEKU_PRINCESS,
    Z64_ITEM_MILK,
    Z64_ITEM_HALF_MILK,
    Z64_ITEM_FISH,
    Z64_ITEM_BUGS,
    Z64_ITEM_BLUE_FIRE,
    Z64_ITEM_POE,
    Z64_ITEM_BIG_POE,
    Z64_ITEM_SPRING_WATER,
    Z64_ITEM_HOT_SPRING_WATER,
    Z64_ITEM_ZORA_EGG,
    Z64_ITEM_GOLD_DUST,
    Z64_ITEM_MUSHROOM,
    Z64_ITEM_SEAHORSE,
    Z64_ITEM_CHATEAU_ROMANI,
    Z64_ITEM_HYLIAN_LOACH,
    Z64_ITEM_BOTTLE2,
    Z64_ITEM_MOONS_TEAR,
    Z64_ITEM_LAND_DEED,
    Z64_ITEM_SWAP_DEED,
    Z64_ITEM_MOUNTAIN_DEED,
    Z64_ITEM_OCEAN_DEED,
    Z64_ITEM_ROOM_KEY,
    Z64_ITEM_MAMA_LETTER,
    Z64_ITEM_KAFEI_LETTER,
    Z64_ITEM_PENDANT,
    Z64_ITEM_UNK_MAP,
    Z64_MASK_DEKU,
    Z64_MASK_GORON,
    Z64_MASK_ZORA,
    Z64_MASK_FIERCE_DEITY,
    Z64_MASK_MASK_OF_TRUTH,
    Z64_MASK_KAFEI,
    Z64_MASK_ALL_NIGHT,
    Z64_MASK_BUNNY_HOOD,
    Z64_MASK_KEATON,
    Z64_MASK_GARO,
    Z64_MASK_ROMANI,
    Z64_MASK_CIRCUS_LEADER,
    Z64_MASK_POSTMAN,
    Z64_MASK_COUPLE,
    Z64_MASK_GREAT_FAIRY,
    Z64_MASK_GIBDO,
    Z64_MASK_DON_GERO,
    Z64_MASK_KAMARO,
    Z64_MASK_CAPTAINSHAT,
    Z64_MASK_STONE,
    Z64_MASK_BREMEN,
    Z64_MASK_BLAST,
    Z64_MASK_SCENTS,
    Z64_MASK_GIANT,
    Z64_ITEM_BOW_FIRE_ARROW,
    Z64_ITEM_BOW_ICE_ARROW,
    Z64_ITEM_BOW_LIGHT_ARROW,
    Z64_ITEM_KOKIRI_SWORD,
    Z64_ITEM_RAZOR_SWORD,
    Z64_ITEM_GILDED_SWORD,
    Z64_ITEM_DEITY_SWORD,
    Z64_ITEM_HERO_SHIELD,
    Z64_ITEM_MIRROR_SHIELD,
    Z64_ITEM_QUIVER_30,
    Z64_ITEM_QUIVER_40,
    Z64_ITEM_QUIVER_50,
    Z64_ITEM_BOMB_BAG_20,
    Z64_ITEM_BOMB_BAG_30,
    Z64_ITEM_BOMB_BAG_40,
    Z64_ITEM_MAGIC,
    Z64_ITEM_ADULTS_WALLET,
    Z64_ITEM_GIANTS_WALLET,
    Z64_ITEM_ODOLWAS_REMAINS,
    Z64_ITEM_GOHTS_REMAINS,
    Z64_ITEM_GYORGS_REMAINS,
    Z64_ITEM_TWINMOLDS_REMAINS,
    Z64_ITEM_BOMBERS_NOTEBOOK = 0x6D
} z64_item_t;

typedef enum {
    Z64_SLOT_OCARINA,
    Z64_SLOT_BOW,
    Z64_SLOT_FIRE_ARROW,
    Z64_SLOT_ICE_ARROW,
    Z64_SLOT_LIGHT_ARROW,
    Z64_SLOT_QUEST_1,
    Z64_SLOT_BOMB,
    Z64_SLOT_BOMBCHU,
    Z64_SLOT_STICK,
    Z64_SLOT_NUT,
    Z64_SLOT_MAGIC_BEAN,
    Z64_SLOT_QUEST_2,
    Z64_SLOT_POWDER_KEG,
    Z64_SLOT_PICTOGRAPH_BOX,
    Z64_SLOT_LENS,
    Z64_SLOT_HOOKSHOT,
    Z64_SLOT_GREAT_FAIRY_SWORD,
    Z64_SLOT_QUEST_3,
    Z64_SLOT_BOTTLE_1,
    Z64_SLOT_BOTTLE_2,
    Z64_SLOT_BOTTLE_3,
    Z64_SLOT_BOTTLE_4,
    Z64_SLOT_BOTTLE_5,
    Z64_SLOT_BOTTLE_6
} z64_item_slot_t;

typedef enum {
    Z64_SLOT_POSTMAN,
    Z64_SLOT_ALL_NIGHT,
    Z64_SLOT_BLAST,
    Z64_SLOT_STONE,
    Z64_SLOT_GREAT_FAIRY,
    Z64_SLOT_DEKU,
    Z64_SLOT_KEATON,
    Z64_SLOT_BREMEN,
    Z64_SLOT_BUNNY_HOOD,
    Z64_SLOT_DON_GERO,
    Z64_SLOT_SCENTS,
    Z64_SLOT_GORON,
    Z64_SLOT_ROMANI,
    Z64_SLOT_CIRCUS_LEADER,
    Z64_SLOT_KAFEI,
    Z64_SLOT_COUPLE,
    Z64_SLOT_MASK_OF_TRUTH,
    Z64_SLOT_ZORA,
    Z64_SLOT_KAMARO,
    Z64_SLOT_GIBDO,
    Z64_SLOT_GARO,
    Z64_SLOT_CAPTAIN,
    Z64_SLOT_GIANT,
    Z64_SLOT_FIERCE_DEITY
} z64_mask_slot_t;

typedef struct {
    union {
        struct {
            uint32_t            : 16;
            uint32_t scene      : 7;
            uint32_t entrance   : 5;
            uint32_t offset     : 4;
        };
        uint32_t    exit;                           // 0x0000
    };
    uint8_t         mask;                           // 0x0004
    uint8_t         intro_flag;                     // 0x0005
    uint8_t         mash_timer;                     // 0x0006
    uint8_t         pad_0x07_;                      // 0x0007
    uint32_t        cutscene_id;                    // 0x0008
    uint16_t        time_of_day;                    // 0x000C
    uint16_t        owl_load;                       // 0x000E
    uint32_t        daynight;                       // 0x0010
    int32_t         timespeed;                      // 0x0014
    uint32_t        day;                            // 0x0018
    uint32_t        elapsed_days;                   // 0x001C
    uint8_t         current_form;                   // 0x0020
    uint8_t         unk_0x21_;                      // 0x0021
    uint8_t         tatl_flag;                      // 0x0022
    uint8_t         owl_save;                       // 0x0023
    char            zelda3[0x06];                   // 0x0024
    uint16_t        sot_count;                      // 0x002A
    char            name[0x08];                     // 0x002C
    uint16_t        max_health;                     // 0x0034
    uint16_t        current_health;                 // 0x0036
    uint8_t         magic_level;                    // 0x0038
    uint8_t         current_magic;                  // 0x0039
    uint16_t        rupees;                         // 0x003A
    uint32_t        tatl_timer;                     // 0x003C
    uint8_t         has_magic;                      // 0x0040
    uint8_t         has_double_magic;               // 0x0041
    uint16_t        has_double_defense;             // 0x0042
    char            unk_0x44_[0x02];                // 0x0044 
    union {
        struct {
            uint16_t hidden_owl         : 1;
            uint16_t                    : 4;
            uint16_t dungeon_entrance   : 1;
            uint16_t stone_tower        : 1;
            uint16_t ikana_canyon;      : 1;
            uint16_t southern_swap      : 1;
            uint16_t woodfall           : 1;
            uint16_t milk_road          : 1;
            uint16_t clock_town         : 1;
            uint16_t mountain_village   : 1;
            uint16_t snowhead           : 1;
            uint16_t zora_cape          : 1;
            uint16_t great_bay          : 1;
        };
        uint16_t    owls_hit;                       // 0x0046
    };
    char            unk_0x48_[0x04];                // 0x0048
    struct {
        uint8_t b;
        uint8_t cleft;
        uint8_t cdown;
        uint8_t cright;
    }               form_button_item[0x04];         // 0x004C
    struct {
        uint8_t b;
        uint8_t cleft;
        uint8_t cdown;
        uint8_t cright;
    }               form_button_slot[0x04];         // 0x005C
    union {
        struct {
            uint16_t        : 10;
            uint16_t shield : 2;
            uint16_t        : 2;
            uint16_t sword  : 2;
        };
        uint16_t    equipment;                      // 0x006C
    };
    char            unk_0x6E_[0x02];                // 0x006E
    int8_t          items[0x18];                    // 0x0070
    int8_t          masks[0x18];                    // 0x0088
    uint8_t         ammo[0x18];                     // 0x00A0
    union {
        struct {
            uint32_t                        : 9;    // & 0xFF800000 >> 0x17
            uint32_t nut_upgrade            : 3;    // & 0x00700000 >> 0x14
            uint32_t stick_upgade           : 3;    // & 0x000E0000 >> 0x11
            uint32_t                        : 2;    // & 0x00018000 >> 0x0F
            uint32_t wallet_upgrade         : 3;    // & 0x00007000 >> 0x0C
            uint32_t                        : 6;    // & 0x00000FC0 >> 0x06
            uint32_t bomb_bag               : 3;    // & 0x00000038 >> 0x03
            uint32_t quiver                 : 3;    // & 0x00000007 >> 0x00
        };
        uint32_t    equipment_upgrades;             // 0x00B8
    };
    union {
        struct {
            uint32_t heart_piece            : 4;
            uint32_t                        : 3;
            uint32_t lullaby_intro          : 1;
            uint32_t                        : 5;
            uint32_t bombers_notebook       : 1;
            uint32_t suns_song              : 1;
            uint32_t song_of_storms         : 1;
            uint32_t song_of_soaring        : 1;
            uint32_t eponas_song            : 1;
            uint32_t song_of_healing        : 1;
            uint32_t song_of_time           : 1;
            uint32_t sarias_song            : 1;
            uint32_t oath_to_order          : 1;
            uint32_t elegy_of_emptiness     : 1;
            uint32_t new_wave_bossa_nova    : 1;
            uint32_t goron_lullaby          : 1;
            uint32_t sonata_of_awakening    : 1;
            uint32_t                        : 2;
            uint32_t twinmolds_remains      : 1;
            uint32_t gyorgs_remains         : 1;
            uint32_t gohts_remains          : 1;
            uint32_t odolwas_remains        : 1;
        };
        uint32_t    quest_status;                   // 0x00BC
    };
    union {
        struct {
            uint8_t                         : 5;
            uint8_t map                     : 1;
            uint8_t compass                 : 1;
            uint8_t boss_key                : 1;
        };
        uint8_t dungeon_item;
    }               dungeon_items[0xA];             // 0x00C0
    uint8_t         dungeon_keys[0x9];              // 0x00CA
    uint8_t         defense_hearts;                 // 0x00D3
    uint8_t         stray_fairies[0xA];             // 0x00D4
    char            form_name[0x6][0x3]             // 0x00DE
    char            unk_0x00F6_[0x2];               // 0x00F6
    struct
    {
        uint32_t    chest;              // 0x00
        uint32_t    switch_1;           // 0x04
        uint32_t    switch_2;           // 0x08
        uint32_t    clear;              // 0x0C
        uint32_t    collect;            // 0x10
        uint32_t    unk_0x14;           // 0x14
        uint32_t    unk_0x18;           // 0x18
    }               scene_flags[0x78];             // 0x00F8
    char            unk_0x0E18_[0x62];             // 0x0E18
    char            unk_0x0E7A_[0x2E26];           // 0x0E7A 

    int32_t         file_index;                    // 0x3CA0
} z64_file_t;

typedef struct z64_actor_s z64_actor_t;
struct z64_actor_s
{
    int16_t         id;               /* 0x0000 */
    uint8_t         type;             /* 0x0002 */
    int8_t          room;             /* 0x0003 */
    uint32_t        flags;            /* 0x0004 */
    z64_xyzf_t      pos_1;            /* 0x0008 */
    z64_rot_t       rot_init;         /* 0x0014 */
    char            unk_01_[0x0002];  /* 0x001A */
    uint16_t        variable;         /* 0x001C */
    uint8_t         alloc_index;      /* 0x001E */
    char            unk_02_;          /* 0x001F */
    uint16_t        sound_effect;     /* 0x0020 */
    char            unk_03_[0x0002];  /* 0x0022 */
    z64_xyzf_t      pos_2;            /* 0x0024 */
    z64_rot_t       rot_dir;          /* 0x0030 */
    char            unk_0x36[0x0002]; /* 0x0036 */
    int8_t          unk_0x38;         /* 0x0038 */
    uint8_t         unk_0x39;         /* 0x0039 */
    char            unk_0x3A[0x002];  /* 0x003A */
    z64_xyzf_t      pos_3;            /* 0x003C */
    z64_rot_t       rot_1;            /* 0x0048 */
    char            unk_06_[0x0002];  /* 0x004E */ //padding?
    uint32_t        unk_0x50;         /* 0x0050 */
    float           unk_0x54;         /* 0x0054 */
    z64_xyzf_t      scale;            /* 0x0058 */
    z64_xyzf_t      vel_1;            /* 0x0064 */
    float           xz_speed;         /* 0x0070 */
    float           gravity;          /* 0x0074 */
    float           min_vel_y;        /* 0x0078 */

                                      /* struct bgcheck common */
    z64_col_poly_t *wall_poly;                /* 0x007C */
    z64_col_poly_t *floor_poly;               /* 0x0080 */
    uint8_t         wall_poly_source;         /* 0x0084 */
    uint8_t         floor_poly_source;        /* 0x0085 */
    int16_t         wall_rot;                 /* 0x0086 */
    float           floor_height;             /* 0x0088 */ //maybe?
    float           water_surface_dist;       /* 0x008C */
    uint16_t        bgcheck_flags;            /* 0x0090 */
    int16_t         unk_0x92_rot;             /* 0x0092 */
    float           unk_0x94;                 /* 0x0094 */
    float           dist_from_link_xz;        /* 0x0098 */
    float           dist_from_link_y;         /* 0x009C */

                                              /* struct collision_check common */
    void           *damage_table;     /* 0x00A0 */
    z64_xyzf_t      vel_2;            /* 0x00A4 */
    char            unk_0C_[0x0006];  /* 0x00B0 */
    uint8_t         mass;             /* 0x00B6 */
    uint8_t         health;           /* 0x00B7 */
    uint8_t         damage;           /* 0x00B8 */
    uint8_t         damage_effect;    /* 0x00B9 */
    uint8_t         impact_effect;    /* 0x00BA */
    uint8_t         unk_0xBB;         /* 0x00BB */

                                      /* struct start */
    z64_rot_t       rot_2;            /* 0x00BC */
    char            unk_0F_[0x0002];  /* 0x00C2 */
    float           unk_0xC4;         /* 0x00C4 */
    void           *draw_drop_shadow; /* 0x00C8 */
    float           unk_0xCC;         /* 0x00CC */
    uint8_t         unk_0xD0;         /* 0x00D0 */
    char            pad_0xD1_[0x0003];/* 0x00D1 */
                                      /* struct end */

    z64_xyzf_t      unk_0xD4;         /* 0x00D4 */
    z64_xyzf_t      unk_0xE0;         /* 0x00E0 */
    z64_xyzf_t      unk_0xEC;         /* 0x00EC */
    float           unk_0xF8;         /* 0x00F8 */
    float           unk_0xFC;         /* 0x00FC */
    float           unk_0x100;        /* 0x0100 */
    float           unk_0x104;        /* 0x0104 */
    z64_xyzf_t      pos_4;            /* 0x0108 */
    uint16_t        unk_10_;          /* 0x0114 */
    uint16_t        text_id;          /* 0x0116 */
    int16_t         frozen;           /* 0x0118 */
    char            unk_11_[0x0003];  /* 0x011A */
    uint8_t         active;           /* 0x011D */
    char            unk_0x11E;        /* 0x011E */
    uint8_t         tatl_enemy_text_id; /*0x011F */
    z64_actor_t     *attached_a;        /* 0x0120 */
    z64_actor_t     *attached_b;        /* 0x0124 */
    z64_actor_t     *prev;              /* 0x0128 */
    z64_actor_t     *next;              /* 0x012C */
    void           *ctor;               /* 0x0130 */
    void           *dtor;               /* 0x0134 */
    void           *main_proc;          /* 0x0138 */
    void           *draw_proc;          /* 0x013C */
    void           *code_entry;         /* 0x0140 */
                                        /* 0x0144 */
};

typedef struct {
    union {
        struct {
            uint16_t    a   : 1;
            uint16_t    b   : 1;
            uint16_t    z   : 1;
            uint16_t    s   : 1;
            uint16_t    du  : 1;
            uint16_t    dd  : 1;
            uint16_t    dl  : 1;
            uint16_t    dr  : 1;
            uint16_t        : 2;
            uint16_t    l   : 1;
            uint16_t    r   : 1;
            uint16_t    cu  : 1;
            uint16_t    cd  : 1;
            uint16_t    cl  : 1;
            uint16_t    cr  : 1;
        };
        uint16_t        pad;                // 0x00
    };
    int8_t              x;                  // 0x02
    int8_t              y;                  // 0x03
} z64_controller_t;                         // 0x04

typedef struct {
    z64_controller_t    raw;                // 0x00
    uint16_t            status;             // 0x04
    z64_controller_t    raw_prev;           // 0x06
    uint16_t            status_prev;        // 0x0A
    uint16_t            pad_pressed;        // 0x0C
    int8_t              x_diff;             // 0x0E
    int8_t              y_diff;             // 0x0F
    char                unk_0x10_[0x02];    // 0x10
    uint16_t            pad_released;       // 0x12
    int8_t              adjusted_x;         // 0x14
    int8_t              adjusted_y;         // 0x15
    char                unk_0x16[0x02];     // 0x16
} z64_input_t;                              // 0x18

typedef struct {
    uint32_t    size;   // 0x00
    void        *buf;   // 0x04
    void        *p;     // 0x08
    void        *d;     // 0x0C
} z64_disp_buf_t;       // 0x10

typedef struct {
    Gfx             *poly_opa_d_start;      // 0x000
    Gfx             *poly_xlu_d_start;      // 0x004
    char            unk_0x08_[0x08];        // 0x008
    Gfx             *overlay_d_start;       // 0x010
    char            unk_0x18_[0x24];        // 0x014
    OSMesg          task_mesg[8];           // 0x038
    char            unk_0x58_[0x04];        // 0x058
    OSMesgQueue     task_queue;             // 0x05C
    char            pad_0x74_[0x04];        // 0x074
    OSScTask        task;                   // 0x078
    char            unk_0xD0_[0xE0];        // 0x0D0
    Gfx             *work_d_start;          // 0x1B0
    z64_disp_buf_t  work;                   // 0x1B4
    char            unk_0x1C4_[0xE4];       // 0x1C4
    z64_disp_buf_t  overlay;                // 0x2A8
    z64_disp_buf_t  poly_opa;               // 0x2B8
    z64_disp_buf_t  poly_xlu;               // 0x2C8
    uint32_t        frame_cnt_1;            // 0x2D8
    void            *frame_buffer;          // 0x2DC
    char            unk_0x2E0_[0x08];       // 0x2E0
    uint32_t        frame_cnt_2;            // 0x2E8
}z64_gfx_t;                                  // 0x2EC

typedef struct {
    void            *gfx;                   // 0x00
    void            *gamestate_update;      // 0x04
    void            *gamestate_dtor;        // 0x08
    void            *gamestate_ctor;        // 0x0C
    void            *ctxt_size;             // 0x10
    z64_input_t     input[4];               // 0x14
    uint32_t        gamestate_heap_size;    // 0x74
    void            *gamestate_heap_ptr;    // 0x78
    void            *heap_append_start;     // 0x7C
    void            *heap_append_end;       // 0x80
    void            *gamestate_heap_node;   // 0x84
    char            unk_0x88_[0x10];        // 0x88
    int32_t         execute_state;          // 0x98
    int32_t         gamestate_frames;       // 0x9C
    int32_t         unk_0xA0;               // 0xA0
} z64_ctxt_t;                               // 0xA4

typedef struct 
{
    char            view_magic[4];          /* 0x0000 */
    z64_gfx_t       *gfx;                  /* 0x0004 */
    struct
    {
        uint32_t        top;                /* 0x0008 */
        uint32_t        bottom;             /* 0x000C */
        uint32_t        left;               /* 0x0010 */
        uint32_t        right;              /* 0x0014 */
    } screen;
    float           camera_distance;        /* 0x0018 */
    float           fog_distance;           /* 0x001C */
    float           z_distance;             /* 0x0020 */
    char            unk_0x24[0x004];        /* 0x0024 */
    z64_xyzf_t      unk_0x28;               /* 0x0028 */
    z64_xyzf_t      unk_0x34;
    z64_xyzf_t      unk_0x40;
    Vp              viewport_movemem;       /* 0x0050 */
    Mtx             unk_mtx_0x60;
    Mtx             unk_mtx_0xA0;
    char            unk_0x00E0[0x40];       /* 0x00E0 */
    Mtx            *unk_mtx_0x60_task;      /* 0x0120 */
    Mtx            *unk_mtx_0xA0_task;      /* 0x0124 */
    float           unk_0x128;
    float           unk_0x12C;
    float           unk_0x130;
    float           unk_0x134;
    float           unk_0x138;
    float           unk_0x13C;
    float           unk_0x140;
    float           unk_0x144;
    float           unk_0x148;
    char            unk_0x14C[0x10];

    uint16_t        perspnorm_scale;        /* 0x015C */
    uint32_t        unk_0x160;
    uint32_t        unk_0x164;
    /* 0x0168 */
} z64_view_t;

typedef struct /* z2_camera_t */
{
    union
    {
        struct
        {
            z64_xyzf_t unk_0x00;
            struct /* 0x24 */
            {
                float       unk_0x00;
                float       unk_0x04;
                int16_t     unk_0x08;
                int16_t     unk_0x0A;
                int16_t     unk_0x0C;
                float       unk_0x10;
            };

        } t1;
        struct
        {
            uint16_t unk_0x00;
        } t2;
        char    unk_0x00[0x50];
    };
    /* 0x0000 */

    z64_xyzf_t      unk_0x50;
    z64_xyzf_t      unk_0x5C;
    z64_xyzf_t      unk_0x68;

    char            unk_0x74[0x0C];

    z64_xyzf_t      unk_0x80;
    z64_ctxt_t     *game;                   /* 0x008C */
    z64_actor_t     *focus;                  /* 0x0090 */
    z64_xyzf_t      focus_pos;              /* 0x0094 */
    uint32_t        unk_0xA0;
    uint32_t        unk_0xA4;
    uint32_t        unk_0xA8;
    char            unk_0xAC[0xCC];
    /* 0x0178 */
} z64_camera_t;

typedef struct /* z2_actor_ctxt_t */
{
    /* game_t 0x1CA0 */
    uint8_t         unk_0x0000;             /* 0x0000 */
    uint8_t         unk_0x0001;             /* 0x0001 */
    uint8_t         unk_0x0002;             /* 0x0002 */
    uint8_t         unk_0x0003;             /* 0x0003 */
    int8_t          unk_0x0004;             /* 0x0004 */
    uint8_t         unk_0x0005;             /* 0x0005 */
    char            unk_0x0006[5];          /* 0x0006 */
    int8_t          unk_0x000B;             /* 0x000B */
    char            unk_0x000C[2];          /* 0x000C */
    uint8_t         n_actors_loaded;        /* 0x000E */
    uint8_t         unk_0x000F;             /* 0x000F */
    struct
    {
        int32_t     count;                  /* 0x0000 */
        z2_actor_t *first;                  /* 0x0004 */
        int32_t     unk;                    /* 0x0008 */
                                            /* 0x000C */

    }               actor_list[16];         /* 0x0010 */
    char            unk_0x00D0[0x50];       /* 0x00D0 */
    struct
    {
        MtxF            unk_0x0120;             /* 0x0120 */
        float           unk_0x0160;             /* 0x0160 */
        float           unk_0x0164;             /* 0x0164 */
        int16_t         unk_0x0168;             /* 0x0168 */
        uint8_t         unk_0x016A;             /* 0x016A */
        uint8_t         unk_0x016B;             /* 0x016B */
        int8_t          unk_0x016C;             /* 0x016C */
        struct
        {
            float           unk_0x00;
            float           unk_0x04;
            float           unk_0x08;
            float           unk_0x0C;
            int32_t         unk_0x10;
            /* 0x0014 */

        }               unk_0x0170[3];      /* 0x0170 */
        int32_t         unk_0x01AC;         /* 0x01AC */
        int32_t         unk_0x01B0;         /* 0x01B0 */
        int32_t         unk_0x01B4;         /* 0x01B4 */
    };
    int32_t         switch_1;           /* 0x01B8 */ //perm
    int32_t         switch_2;           /* 0x01BC */ //perm
    int32_t         switch_3;           /* 0x01C0 */
    int32_t         switch_4;           /* 0x01C4 */
    int32_t         chest;              /* 0x01C8 */
    int32_t         clear;              /* 0x01CC */
    int32_t         unk_0x01D0;         /* 0x01D0 */
    int32_t         collectible_1;      /* 0x01D4 */ //Perm
    int32_t         collectible_2;      /* 0x01D8 */
    int32_t         collectible_3;      /* 0x01DC */
    int32_t         collectible_4;      /* 0x01E0 */
    struct
    {
        char        unk_0x00[0x0A];
        int8_t      unk_0x0A;
        int8_t      unk_0x0B;
        int16_t     unk_0x0C;
        int16_t     unk_0x0E;
    }   unk_0x01E4;                     /* 0x01E4 */
    int8_t          unk_0x01F4;
    uint8_t         unk_0x01F5;
    float           unk_0x01F8;
    char            unk_0x01FC[0x54];

    int32_t         unk_0x0250;
    char            unk_0x0254[0x14];
    uint8_t         unk_0x0268;         /* 0x0268 */
    char            unk_0x0269[3];
    char            unk_0x026C[0x18];   /* 0x026C */
                                        /* 0x0284 */

} z64_actor_ctxt_t;

typedef struct
{
    int16_t           poly_idx;                 /* 0x0000 */
    uint16_t          list_next;                /* 0x0002 */
                                                /* 0x0004 */
} z64_col_list_t;

typedef struct
{
    /* index of z64_col_type in scene file */
    uint16_t    type;
    /* vertex indices, a and b are bitmasked for some reason */
    struct
    {
        uint16_t  unk_00_ : 3;
        uint16_t  va : 13;
    };
    struct
    {
        uint16_t  unk_01_ : 3;
        uint16_t  vb : 13;
    };
    uint16_t    vc;
    /* normal vector */
    z64_xyz_t   norm;
    /* plane distance from origin */
    int16_t     dist;
} z64_col_poly_t;

typedef struct
{
    struct
    {
        uint32_t  unk_00_ : 1;
        uint32_t  drop : 1; /* link drops one unit into the floor */
        uint32_t  special : 4;
        uint32_t  interaction : 5;
        uint32_t  unk_01_ : 3;
        uint32_t  behavior : 5;
        uint32_t  exit : 5;
        uint32_t  camera : 8;
    } flags_1;                    /* 0x0000 */
    struct
    {
        uint32_t  pad_00_ : 4;
        uint32_t  wall_damage : 1;
        uint32_t  unk_00_ : 6;
        uint32_t  unk_01_ : 3;
        uint32_t  hookshot : 1;
        uint32_t  echo : 6;
        uint32_t  unk_02_ : 5;
        uint32_t  terrain : 2;
        uint32_t  material : 4;
    } flags_2;                    /* 0x0004 */
} z64_col_type_t;

typedef struct
{
    z64_xyz_t     pos;
    int16_t       width;
    int16_t       depth;
    struct
    {
        uint32_t    unk_00_ : 12;
        uint32_t    active : 1;
        uint32_t    group : 6; /* ? */
        uint32_t    unk_01_ : 5;
        uint32_t    camera : 8;
    } flags;
} z64_col_water_t;

typedef struct
{
    z64_xyz_t         min;
    z64_xyz_t         max;
    uint16_t          n_vtx;
    z64_xyz_t        *vtx;
    uint16_t          n_poly;
    z64_col_poly_t   *poly;
    z64_col_type_t   *type;
    z64_camera_t     *camera;
    uint16_t          n_water;
    z64_col_water_t  *water;
} z64_col_hdr_t;

typedef struct
{
    z64_actor_t       *actor;                    /* 0x0000 */
    z64_col_hdr_t    *col_hdr;                  /* 0x0004 */
    uint16_t          unk_0x08;                 /* 0x0008 */
    uint16_t          unk_0x0A;                 /* 0x000A */
    uint16_t          unk_0x0C;                 /* 0x000C */
    int16_t           unk_0x0E;                 /* 0x000E */ //number of polys?
    int16_t           unk_0x10;                 /* 0x0010 */
    z64_xyzf_t        scale_1;                  /* 0x0014 */
    z64_rot_t         rot_1;                    /* 0x0020 */
    z64_xyzf_t        pos_1;                    /* 0x0028 */
    z64_xyzf_t        scale_2;                  /* 0x0034 */
    z64_rot_t         rot_2;                    /* 0x0040 */
    z64_xyzf_t        pos_2;                    /* 0x0048 */
    int16_t           unk_0x54;                 /* 0x0054 */
    int16_t           unk_0x56;                 /* 0x0056 */
    int16_t           unk_0x58;                 /* 0x0058 */
    int16_t           unk_0x5A;                 /* 0x005A */
    char              unk_0x5C[0x8];            /* 0x005C */
                                                /* 0x0064 */
} z64_col_chk_actor_t;

typedef struct
{
    uint16_t          floor_list_idx;           /* 0x0000 */
    uint16_t          wall_list_idx;            /* 0x0002 */
    uint16_t          ceil_list_idx;            /* 0x0004 */
                                                /* 0x0006 */
} z64_col_lut_t;

typedef struct /* z64_col_ctxt_t */
{
    /* static collision stuff */
    z64_col_hdr_t      *col_hdr;                  /* 0x0000 */
    z64_xyzf_t          bbox_min;                 /* 0x0004 */
    z64_xyzf_t          bbox_max;                 /* 0x0010 */
    int32_t             n_sect_x;                 /* 0x001C */
    int32_t             n_sect_y;                 /* 0x0020 */
    int32_t             n_sect_z;                 /* 0x0024 */
    z64_xyzf_t          sect_size;                /* 0x0028 */
    z64_xyzf_t          sect_inv;                 /* 0x0034 */
    z64_col_lut_t      *stc_lut;                  /* 0x0040 */
    uint16_t            stc_list_max;             /* 0x0044 */
    uint16_t            stc_list_pos;             /* 0x0046 */
    z64_col_list_t     *stc_list;                 /* 0x0048 */
    uint8_t            *stc_check;                /* 0x004C */

                                                  /* bg actor collision struct start */
    int8_t              unk_0x0050;               /* 0x0050 */
    z64_col_chk_actor_t  actors[50];               /* 0x0054 */
    uint16_t            actor_loaded[50];         /* 0x13DC */
                                                  /* dynamic collision stuff */
    z64_col_poly_t     *dyn_poly;                 /* 0x1440 */
    z64_xyz_t          *dyn_vtx;                  /* 0x1444 */
    int32_t             unk_0x1448;               /* 0x1448 */
    void               *unk_0x144C;               /* 0x144C */
                                                  /* struct */
    struct
    {
        z64_col_list_t   *list;                 /* 0x1450 */
        int32_t           count;                /* 0x1454 */
        int32_t           max;                  /* 0x1458 */
    } dyn;
    /* bg actor collision struct end */
    uint32_t            dyn_list_max;             /* 0x145C */
    uint32_t            dyn_poly_max;             /* 0x1460 */
    uint32_t            dyn_vtx_max;              /* 0x1464 */
    uint32_t            mem_size;                 /* 0x1468 */
    uint32_t            unk_0x146C;               /* 0x146C */
                                                  /* 0x1470 */
} z64_col_ctxt_t;

typedef struct {
    z64_ctxt_t          common;                 // 0x0000
    uint16_t            scene_index;            // 0x00A4 */
    uint8_t             scene_draw_id;          // 0x00A6 */
    char                unk_0x000A7[9];         // 0x00A7 */
    void*               scene_addr;             // 0x00B0 */
    char                unk_0x00B4[4];          // 0x00B4 */
    z64_view_t          view_scene;             // 0x00B8 */
    z64_camera_t        cameras[4];             // 0x0220 */
    z64_camera_t       *active_cameras[4];      // 0x0800 */
    int16_t             camera_cur;             // 0x0810 */
    int16_t             camera_next;            // 0x0812 */
    char                unk_0x814[0x1C];        // 0x0814 */
    z64_col_ctxt_t      col_ctxt;               // 0x0830 */
    z64_actor_ctxt_t    actor_ctxt;             // 0x1CA0 */
} z64_game_t;                               // 

extern z64_game_t z64_game;

#endif