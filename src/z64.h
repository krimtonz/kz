#ifndef _Z64_H
#define _Z64_H
#include <n64.h>
#include <stdint.h>

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
    Z64_ITEM_BOW_FIRE_ARROW,
    Z64_MASK_DEKU = 0x32,
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
};                                          // 0x2EC

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

typedef struct {
    z64_ctxt_t  common;                     // 0x00
} z64_game_t;                               // 0xA4

extern z64_game_t z64_game;

#endif