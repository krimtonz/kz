/*
* z2.h
*
* Majora's Mask related information, addresses, and structure definitions
*/

#ifndef _Z2_H
#define _Z2_H
#include <n64.h>
#include <stdint.h>
#include "gu.h"

#define Z2_SCREEN_WIDTH    320
#define Z2_SCREEN_HEIGHT   240

#ifndef Z2_VERSION
#error no z2 version specified
#endif

#define NZSE    0x00
#define NZSJ    0x01
#define NZSJ10  0x02

typedef union {
    struct {
        /* 0x0000 */ uint8_t r;
        /* 0x0001 */ uint8_t g;
        /* 0x0002 */ uint8_t b;
        /* 0x0003 */ uint8_t a;
    };
    /* 0x0000 */ uint32_t color; /* 0x00 */
} z2_rgba32_t; // size = 0x4

typedef struct
{
    /* 0x0000 */ int16_t x;
    /* 0x0002 */ int16_t y;
    /* 0x0004 */ int16_t z;
} z2_xyz_t; // size = 0x6

typedef struct
{
    /* 0x0000 */ float x;
    /* 0x0004 */ float y;
    /* 0x0008 */ float z;
} z2_xyzf_t; // size = 0xC

typedef uint16_t z2_angle_t;
typedef struct
{
    /* 0x0000 */ z2_angle_t  x;
    /* 0x0002 */ z2_angle_t  y;
    /* 0x0004 */ z2_angle_t  z;
} z2_rot_t; // size = 0x6

typedef enum {
    /* 0xFF 0xFF */ Z2_ITEM_NULL = -1,
    /* 0x00 0x00 */ Z2_ITEM_OCARINA,
    /* 0x01 0x01 */ Z2_ITEM_HEROS_BOW,
    /* 0x02 0x02 */ Z2_ITEM_FIRE_ARROW,
    /* 0x03 0x03 */ Z2_ITEM_ICE_ARROW,
    /* 0x04 0x04 */ Z2_ITEM_LIGHT_ARROW,
    /* 0x05 0x05 */ Z2_ITEM_FAIRY_OCARINA,
    /* 0x06 0x06 */ Z2_ITEM_BOMB,
    /* 0x07 0x07 */ Z2_ITEM_BOMBCHU,
    /* 0x08 0x08 */ Z2_ITEM_STICK,
    /* 0x09 0x09 */ Z2_ITEM_NUT,
    /* 0x0A 0x0A */ Z2_ITEM_MAGIC_BEAN,
    /* 0x0B 0x0B */ Z2_ITEM_SLINGSHOT,
    /* 0x0C 0x0C */ Z2_ITEM_POWDER_KEG,
    /* 0x0D 0x0D */ Z2_ITEM_PICTOGRAPH_BOX,
    /* 0x0E 0x0E */ Z2_ITEM_LENS,
    /* 0x0F 0x0F */ Z2_ITEM_HOOKSHOT,
    /* 0x10 0x10 */ Z2_ITEM_GREAT_FAIRY_SWORD,
    /* 0x11 0x11 */ Z2_ITEM_OOT_HOOKSHOT,
    /* 0x12 0x12 */ Z2_ITEM_BOTTLE,
    /* 0x13 0x13 */ Z2_ITEM_RED_POTION,
    /* 0x14 0x14 */ Z2_ITEM_GREEN_POTION,
    /* 0x15 0x15 */ Z2_ITEM_BLUE_POTION,
    /* 0x16 0x16 */ Z2_ITEM_FAIRY,
    /* 0x17 0x17 */ Z2_ITEM_DEKU_PRINCESS,
    /* 0x18 0x18 */ Z2_ITEM_MILK,
    /* 0x19 0x19 */ Z2_ITEM_HALF_MILK,
    /* 0x1A 0x1A */ Z2_ITEM_FISH,
    /* 0x1B 0x1B */ Z2_ITEM_BUGS,
    /* 0x1C 0x1C */ Z2_ITEM_BLUE_FIRE,
    /* 0x1D 0x1D */ Z2_ITEM_POE,
    /* 0x1E 0x1E */ Z2_ITEM_BIG_POE,
    /* 0x1F 0x1F */ Z2_ITEM_SPRING_WATER,
    /* 0x20 0x20 */ Z2_ITEM_HOT_SPRING_WATER,
    /* 0x21 0x21 */ Z2_ITEM_ZORA_EGG,
    /* 0x22 0x22 */ Z2_ITEM_GOLD_DUST,
    /* 0x23 0x23 */ Z2_ITEM_MUSHROOM,
#if Z2_VERSION!=NZSE
    /*      0x24 */ Z2_ITEM_BOTTLE2,
    /*      0x25 */ Z2_ITEM_BOTTLE3,
#endif
    /* 0x24 0x26 */ Z2_ITEM_SEAHORSE,
    /* 0x25 0x27 */ Z2_ITEM_CHATEAU_ROMANI,
    /* 0x26 0x28 */ Z2_ITEM_HYLIAN_LOACH,
#if Z2_VERSION==NZSE
    /* 0x27      */ Z2_ITEM_BOTTLE2,
#else
    /* 0x00 0x29 */ Z2_ITEM_BOTTLE4,
    /* 0x00 0x2A */ Z2_ITEM_BOTTLE5,
    /* 0x00 0x2B */ Z2_ITEM_BOTTLE6,
    /* 0x00 0x2C */ Z2_ITEM_BOTTLE7,
    /* 0x00 0x2D */ Z2_ITEM_BOTTLE8,
    /* 0x00 0x2E */ Z2_ITEM_BOTTLE9,
    /* 0x00 0x2F */ Z2_ITEM_BOTTLE10,
#endif
    /* 0x28 0x30 */ Z2_END_BOTTLES,
    /* 0x28 0x30 */ Z2_ITEM_MOONS_TEAR = Z2_END_BOTTLES,
    /* 0x29 0x31 */ Z2_ITEM_LAND_DEED,
    /* 0x2A 0x32 */ Z2_ITEM_SWAP_DEED,
    /* 0x2B 0x33 */ Z2_ITEM_MOUNTAIN_DEED,
    /* 0x2C 0x34 */ Z2_ITEM_OCEAN_DEED,
#if Z2_VERSION!=NZSE
    /*      0x35 */ Z2_ITEM_POACHERS_SAW,
    /*      0x36 */ Z2_ITEM_BROKEN_GORON_SWORD,
    /*      0x37 */ Z2_ITEM_PRESCRIPTION,
    /*      0x38 */ Z2_ITEM_SPEEDFROG,
    /*      0x39 */ Z2_ITEM_EYEDROPS,
#endif
    /* 0x2D 0x3A */ Z2_ITEM_ROOM_KEY,
    /* 0x2E 0x3B */ Z2_ITEM_MAMA_LETTER,
#if Z2_VERSION!=NZSE
    /*      0x3C */ Z2_ITEM_MOONS_TEAR2,
    /*      0x3D */ Z2_ITEM_MOONS_TEAR3,
    /*      0x3E */ Z2_ITEM_MOONS_TEAR4,
    /*      0x3F */ Z2_ITEM_MOONS_TEAR5,
    /*      0x40 */ Z2_ITEM_MOONS_TEAR6,
    /*      0x41 */ Z2_ITEM_MOONS_TEAR7,
    /*      0x42 */ Z2_ITEM_MOONS_TEAR8,
    /*      0x43 */ Z2_ITEM_MOONS_TEAR9,
#endif
    /* 0x2F 0x44 */ Z2_ITEM_KAFEI_LETTER,
    /* 0x30 0x45 */ Z2_ITEM_PENDANT,
#if Z2_VERSION!=NZSE
    /*      0x46 */ Z2_ITEM_MOONS_TEAR10,
    /*      0x47 */ Z2_ITEM_MOONS_TEAR11,
    /*      0x48 */ Z2_ITEM_MOONS_TEAR12,
    /*      0x49 */ Z2_ITEM_MOONS_TEAR13,
    /*      0x4A */ Z2_ITEM_MOONS_TEAR14,
    /*      0x4B */ Z2_ITEM_MOONS_TEAR15,
    /*      0x4C */ Z2_ITEM_MOONS_TEAR16,
#endif
    /* 0x31 0x4D */ Z2_END_TRADE,
    /* 0x31 0x4D */ Z2_ITEM_UNK_MAP = Z2_END_TRADE,
    /* 0x32 0x4E */ Z2_MASK_DEKU,
    /* 0x33 0x4F */ Z2_MASK_GORON,
    /* 0x34 0x50 */ Z2_MASK_ZORA,
    /* 0x35 0x51 */ Z2_MASK_FIERCE_DEITY,
    /* 0x36 0x52 */ Z2_MASK_MASK_OF_TRUTH,
    /* 0x37 0x53 */ Z2_MASK_KAFEI,
    /* 0x38 0x54 */ Z2_MASK_ALL_NIGHT,
    /* 0x39 0x55 */ Z2_MASK_BUNNY_HOOD,
    /* 0x3A 0x56 */ Z2_MASK_KEATON,
    /* 0x3B 0x57 */ Z2_MASK_GARO,
    /* 0x3C 0x58 */ Z2_MASK_ROMANI,
    /* 0x3D 0x59 */ Z2_MASK_CIRCUS_LEADER,
    /* 0x3E 0x5A */ Z2_MASK_POSTMAN,
    /* 0x3F 0x5B */ Z2_MASK_COUPLE,
    /* 0x40 0x5C */ Z2_MASK_GREAT_FAIRY,
    /* 0x41 0x5D */ Z2_MASK_GIBDO,
    /* 0x42 0x5E */ Z2_MASK_DON_GERO,
    /* 0x43 0x5F */ Z2_MASK_KAMARO,
    /* 0x44 0x60 */ Z2_MASK_CAPTAINSHAT,
    /* 0x45 0x61 */ Z2_MASK_STONE,
    /* 0x46 0x62 */ Z2_MASK_BREMEN,
    /* 0x47 0x63 */ Z2_MASK_BLAST,
    /* 0x48 0x64 */ Z2_MASK_SCENTS,
    /* 0x49 0x65 */ Z2_MASK_GIANT,
    /* 0x4A 0x66 */ Z2_END_MASK,
    /* 0x4A 0x66 */ Z2_ITEM_BOW_FIRE_ARROW = Z2_END_MASK,
    /* 0x4B 0x67 */ Z2_ITEM_BOW_ICE_ARROW,
    /* 0x4C 0x68 */ Z2_ITEM_BOW_LIGHT_ARROW,
#if Z2_VERSION!=NZSE
    /*      0x69 */ Z2_ITEM_DRUM,
    /*      0x6A */ Z2_ITEM_GUITAR,
    /*      0x6B */ Z2_ITEM_PIPES,
#endif
    /* 0x4D 0x6C */ Z2_ITEM_KOKIRI_SWORD,
    /* 0x4E 0x6D */ Z2_ITEM_RAZOR_SWORD,
    /* 0x4F 0x6E */ Z2_ITEM_GILDED_SWORD,
    /* 0x50 0x6F */ Z2_ITEM_DEITY_SWORD,
    /* 0x51 0x70 */ Z2_ITEM_HERO_SHIELD,
    /* 0x52 0x71 */ Z2_ITEM_MIRROR_SHIELD,
#if Z2_VERSION!=NZSE
    /*      0x72 */ Z2_ITEM_OOT_MIRROR,
#endif
    /* 0x53 0x73 */ Z2_ITEM_QUIVER_30,
    /* 0x54 0x74 */ Z2_ITEM_QUIVER_40,
    /* 0x55 0x75 */ Z2_ITEM_QUIVER_50,
    /* 0x56 0x76 */ Z2_ITEM_BOMB_BAG_20,
    /* 0x57 0x77 */ Z2_ITEM_BOMB_BAG_30,
    /* 0x58 0x78 */ Z2_ITEM_BOMB_BAG_40,
    /* 0x59 0x79 */ Z2_ITEM_MAGIC,
    /* 0x5A 0x7A */ Z2_ITEM_ADULTS_WALLET,
    /* 0x5B 0x7B */ Z2_ITEM_GIANTS_WALLET,
    /* 0x5C 0x7C */ Z2_ITEM_FISHING_ROD,
    /* 0x5D 0x7D */ Z2_ITEM_ODOLWAS_REMAINS,
    /* 0x5E 0x7E */ Z2_ITEM_GOHTS_REMAINS,
    /* 0x5F 0x7F */ Z2_ITEM_GYORGS_REMAINS,
    /* 0x60 0x80 */ Z2_ITEM_TWINMOLDS_REMAINS,
    /* 0x61 0x81 */ Z2_ITEM_BOMBERS_NOTEBOOK,
    /* 0x62 0x82 */ Z2_ITEM_END
} z2_item_t;

typedef enum {
    /* 0x00 */ Z2_SLOT_OCARINA,
    /* 0x01 */ Z2_SLOT_BOW,
    /* 0x02 */ Z2_SLOT_FIRE_ARROW,
    /* 0x03 */ Z2_SLOT_ICE_ARROW,
    /* 0x04 */ Z2_SLOT_LIGHT_ARROW,
    /* 0x05 */ Z2_SLOT_QUEST_1,
    /* 0x06 */ Z2_SLOT_BOMB,
    /* 0x07 */ Z2_SLOT_BOMBCHU,
    /* 0x08 */ Z2_SLOT_STICK,
    /* 0x09 */ Z2_SLOT_NUT,
    /* 0x0A */ Z2_SLOT_MAGIC_BEAN,
    /* 0x0B */ Z2_SLOT_QUEST_2,
    /* 0x0C */ Z2_SLOT_POWDER_KEG,
    /* 0x0D */ Z2_SLOT_PICTOGRAPH_BOX,
    /* 0x0E */ Z2_SLOT_LENS,
    /* 0x0F */ Z2_SLOT_HOOKSHOT,
    /* 0x10 */ Z2_SLOT_GREAT_FAIRY_SWORD,
    /* 0x11 */ Z2_SLOT_QUEST_3,
    /* 0x12 */ Z2_SLOT_BOTTLE_1,
    /* 0x13 */ Z2_SLOT_BOTTLE_2,
    /* 0x14 */ Z2_SLOT_BOTTLE_3,
    /* 0x15 */ Z2_SLOT_BOTTLE_4,
    /* 0x16 */ Z2_SLOT_BOTTLE_5,
    /* 0x17 */ Z2_SLOT_BOTTLE_6,
    /* 0x18 */ Z2_SLOT_ITEM_MAX
} z2_item_slot_t;

typedef enum {
    /* 0x00 */ Z2_SLOT_POSTMAN,
    /* 0x01 */ Z2_SLOT_ALL_NIGHT,
    /* 0x02 */ Z2_SLOT_BLAST,
    /* 0x03 */ Z2_SLOT_STONE,
    /* 0x04 */ Z2_SLOT_GREAT_FAIRY,
    /* 0x05 */ Z2_SLOT_DEKU,
    /* 0x06 */ Z2_SLOT_KEATON,
    /* 0x07 */ Z2_SLOT_BREMEN,
    /* 0x08 */ Z2_SLOT_BUNNY_HOOD,
    /* 0x09 */ Z2_SLOT_DON_GERO,
    /* 0x0A */ Z2_SLOT_SCENTS,
    /* 0x0B */ Z2_SLOT_GORON,
    /* 0x0C */ Z2_SLOT_ROMANI,
    /* 0x0D */ Z2_SLOT_CIRCUS_LEADER,
    /* 0x0E */ Z2_SLOT_KAFEI,
    /* 0x0F */ Z2_SLOT_COUPLE,
    /* 0x10 */ Z2_SLOT_MASK_OF_TRUTH,
    /* 0x11 */ Z2_SLOT_ZORA,
    /* 0x12 */ Z2_SLOT_KAMARO,
    /* 0x13 */ Z2_SLOT_GIBDO,
    /* 0x14 */ Z2_SLOT_GARO,
    /* 0x15 */ Z2_SLOT_CAPTAIN,
    /* 0x16 */ Z2_SLOT_GIANT,
    /* 0x17 */ Z2_SLOT_FIERCE_DEITY,
    /* 0x18 */ Z2_SLOT_MASK_MAX
} z2_mask_slot_t;

enum pause_page {
    /* 0x00 */ Z2_PAUSE_ITEM,
    /* 0x01 */ Z2_PAUSE_MAP,
    /* 0x02 */ Z2_PAUSE_QUEST,
    /* 0x03 */ Z2_PAUSE_MASK
};

enum quest_cell {
    /* 0x00 */ Z2_QUEST_ODOWLA,
    /* 0x01 */ Z2_QUEST_GOHT,
    /* 0x02 */ Z2_QUEST_GYORG,
    /* 0x03 */ Z2_QUEST_TWINMOLD,
    /* 0x04 */ Z2_QUEST_SHIELD,
    /* 0x05 */ Z2_QUEST_SWORD,
    /* 0x06 */ Z2_QUEST_SONATA,
    /* 0x07 */ Z2_QUEST_LULLABY,
    /* 0x08 */ Z2_QUEST_NWBN,
    /* 0x09 */ Z2_QUEST_ELEGY,
    /* 0x0A */ Z2_QUEST_OATH,
    /* 0x0C */ Z2_QUEST_SOT = 12,
    /* 0x0D */ Z2_QUEST_HEALTING,
    /* 0x0E */ Z2_QUEST_EPONA,
    /* 0x0F */ Z2_QUEST_SOARING,
    /* 0x10 */ Z2_QUEST_STORMS,
    /* 0x12 */ Z2_QUEST_BOMBERS = 18,
    /* 0x13 */ Z2_QUEST_QUIVER,
    /* 0x14 */ Z2_QUEST_BOMB,
    /* 0x16 */ Z2_QUEST_HEART = 22
};

typedef struct {
    /* 0x0000 */ char        unk_0x00_[0x32];
    /* 0x0032 */ int16_t     time_speed;
    /* 0x0034 */ char        unk_0x34_[0x06];
    /* 0x003A */ int16_t     acceleration;
    /* 0x003C */ char        unk_0x3C_[0x0E];
    /* 0x004A */ int16_t     turn_speed;
    /* 0x004C */ char        unk_0x4E_[0x50];
    /* 0x009C */ int16_t     gravity;
    /* 0x0093 */ char        unk_0x9E_[0x72];
    /* 0x0110 */ uint16_t    update_rate;
    /* 0x0112 */ char        unk_0x112[0x14CE];
} z2_static_ctxt_t; // size = 0x15E0

typedef struct {
    /* 0x0000 */
    union {
        struct {
            uint32_t            : 16;
            uint32_t scene      : 7;
            uint32_t entrance   : 5;
            uint32_t offset     : 4;
        };
        /* 0x0000 */ uint32_t    exit;
        // size = 0x04
    };
    /* 0x0004 */ uint8_t             mask;
    /* 0x0005 */ uint8_t             intro_flag;
    /* 0x0006 */ uint8_t             mash_timer;
    ///* 0x0007 */ uint8_t             pad_0x07_;
    /* 0x0008 */ uint32_t            cutscene_id;
    /* 0x000C */ uint16_t            time_of_day;
    /* 0x000E */ uint16_t            owl_load;
    /* 0x0010 */ uint32_t            daynight;
    /* 0x0014 */ int32_t             timespeed;
    /* 0x0018 */ uint32_t            day;
    /* 0x001C */ uint32_t            elapsed_days;
    /* 0x0020 */ uint8_t             current_form;
    /* 0x0021 */ uint8_t             unk_0x21_;
    /* 0x0022 */ uint8_t             tatl_flag;
    /* 0x0023 */ uint8_t             owl_save;
    /* 0x0024 */ char                zelda3[0x06];
    /* 0x002A */ uint16_t            sot_count;
    /* 0x002C */ char                name[0x08];
    /* 0x0034 */ uint16_t            max_health;
    /* 0x0036 */ uint16_t            current_health;
    /* 0x0038 */ uint8_t             magic_level;
    /* 0x0039 */ uint8_t             current_magic;
    /* 0x003A */ uint16_t            rupees;
    /* 0x003C */ uint32_t            tatl_timer;
    /* 0x0040 */ uint8_t             has_magic;
    /* 0x0041 */ uint8_t             has_double_magic;
    /* 0x0042 */ uint8_t             has_double_defense;
    /* 0x0043 */ char                unk_0x43_[0x03];
    /* 0x0046 */
    union {
        struct {
            uint16_t hidden_owl         : 1;
            uint16_t                    : 4;
            uint16_t dungeon_entrance   : 1;
            uint16_t stone_tower        : 1;
            uint16_t ikana_canyon       : 1;
            uint16_t southern_swap      : 1;
            uint16_t woodfall           : 1;
            uint16_t milk_road          : 1;
            uint16_t clock_town         : 1;
            uint16_t mountain_village   : 1;
            uint16_t snowhead           : 1;
            uint16_t zora_cape          : 1;
            uint16_t great_bay          : 1;
        };
        /* 0x0046 */ uint16_t    owls_hit;
        // size = 0x02
    };
    /* 0x0048 */ char            unk_0x48_[0x04];
    /* 0x004C */
    union {
        struct {
            /* 0x0000 */ int8_t  b;
            /* 0x0001 */ int8_t  cleft;
            /* 0x0002 */ int8_t  cdown;
            /* 0x0003 */ int8_t  cright;
        };
        /* 0x004C */ int8_t button_item[4];
        // size = 0x04
    }               form_button_item[0x04];
    /* 0x05C */
    union {
        struct {
            /* 0x0000 */ int8_t b;
            /* 0x0001 */ int8_t cleft;
            /* 0x0002 */ int8_t cdown;
            /* 0x0003 */ int8_t cright;
        };
        /* 0x05C */ int8_t button_slot[4];
        // size = 0x04
    }               form_button_slot[0x04];
    /* 0x006C */
    union {
        struct {
            uint16_t        : 10;
            uint16_t shield : 2;
            uint16_t        : 2;
            uint16_t sword  : 2;
        };
        /* 0x006C */ uint16_t    equipment;
        // size = 0x02
    };
    /* 0x006E */ char            unk_0x6E_[0x02];
    /* 0x0070 */ int8_t          items[0x18];
    /* 0x0088 */ int8_t          masks[0x18];
    /* 0x00A0 */ uint8_t         ammo[0x18];
    /* 0x00B8 */
    union {
        struct {
            uint32_t                        : 9;
            uint32_t nut_upgrade            : 3;
            uint32_t stick_upgade           : 3;
            uint32_t                        : 3;
            uint32_t wallet_upgrade         : 2;
            uint32_t                        : 6;
            uint32_t bomb_bag               : 3;
            uint32_t quiver                 : 3;
        };
        /* 0x00B8 */ uint32_t    equipment_upgrades;
        // size = 0x04
    };
    /* 0x00BC */
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
        /* 0x00BC */ uint32_t    quest_status;
        // size = 0x04
    };
    /* 0x00C0 */
    union {
        struct {
            uint8_t                         : 5;
            uint8_t map                     : 1;
            uint8_t compass                 : 1;
            uint8_t boss_key                : 1;
        };
        /* 0x00C0 */ uint8_t dungeon_item;
        // size = 0x01
    }                   dungeon_items[0xA];
    /* 0x00CA */ uint8_t             dungeon_keys[0x9];
    /* 0x00D3 */ uint8_t             defense_hearts;
    /* 0x00D4 */ uint8_t             stray_fairies[0xA];
    /* 0x00DE */ char                form_name[0x8][0x3];
    ///* 0x00F6 */ char                unk_0x00F6_[0x2];
    /* 0x00F8 */
    struct
    {
        /* 0x0000 */ uint32_t    chest;
        /* 0x0004 */ uint32_t    switch_1;
        /* 0x0008 */ uint32_t    switch_2;
        /* 0x000C */ uint32_t    clear;
        /* 0x0010 */ uint32_t    collect;
        /* 0x0014 */ uint32_t    unk_0x14;
        /* 0x0018 */ uint32_t    unk_0x18;
        // size = 0x1C
    }                   save_scene_flags[0x78];
    /* 0x0E18 */ char                unk_0x0E18_[0xE0];
    /* 0x0EF8 */ char                week_event_inf[0x64];
    /* 0x0F5C */ char                unk_0x0F5C[0x90];
#if Z2_VERSION!=NZSE
    /* 0x0FEC */ char                unk_0x0FEC_[0x2F64];
#else
    /* 0x0FEC */ char                unk_0x0FEC_[0x20];
    /* 0x100C */ char                event_inf[0x8];
    /* 0x1014 */ char                unk_0x1014[0x2C8C];
#endif
    /* 0x3CA0   0x3F50 */ int32_t             file_index;
    /* 0x3CA4   0x3F54 */ char                unk_0x3CA4[0x08];
    /* 0x3CAC   0x3F5C */ uint32_t            scene_setup;
    /* 0x3CB0   0x3F60 */ int                 void_flag;
#if Z2_VERSION==NZSE
    /* 0x3CB4          */ char                unk_0x03CB4[0x264];
#else
    /*          0x3F64 */ char                unk_0x03F64[0x338];
#endif
    /* 0x3F18   0x429C */ uint8_t             restriction_flags[0x4];
#if Z2_VERSION==NZSE
    /* 0x3F1C          */ char                unk_0x3F1C[0x4C];
#else
    /*          0x42A0 */ char                unk_0x42A0[0x1A];
    /*          0x42BA */ char                event_inf[0x8];
    /*          0x42C2 */ char                unk_0x42C2[0x32];
#endif
    /* 0x3F68   0x42F4 */
    struct {
        /* 0x0000 */ uint32_t    chest;
        /* 0x0004 */ uint32_t    switch1;
        /* 0x0008 */ uint32_t    switch2;
        /* 0x000C */ uint32_t    clear;
        /* 0x0010 */ uint32_t    collectible;
        // size = 0x14
    }                   scene_flags[0x78];
    /* 0x48C8   0x4C54 */ char                unk_0x48C8[2];
    /* 0x48CA   0x4C56 */ char                mask_mask_bit[3];
#if Z2_VERSION==NZSE
    /* 0x48CD          */ char                unk_0x48CD[0x23];
#else
    /*          0x4C59 */ char                unk_0x48CD[0x27];
#endif
    /* 0x48F0   0x4C80 */ z2_static_ctxt_t   *static_ctx;
} z2_file_t; // size = 0x48F4   0x4C84

typedef struct
{
    /* index of z2_col_type in scene file */
    /* 0x0000 */ uint16_t    type;
    /* vertex indices, a and b are bitmasked for some reason */
    /* 0x0002 */
    struct
    {
        uint16_t  unk_00_ : 3;
        uint16_t  va : 13;
    };
    /* 0x0004 */
    struct
    {
        uint16_t  unk_01_ : 3;
        uint16_t  vb : 13;
    };
    /* 0x0006 */ uint16_t    vc;
    /* normal vector */
    /* 0x0008 */ z2_xyz_t    norm;
    /* plane distance from origin */
    /* 0x000E */ int16_t     dist;
} z2_col_poly_t; // size = 0x10

typedef struct z2_actor_s z2_actor_t;
struct z2_actor_s
{
    /* 0x0000 */ int16_t         id;
    /* 0x0002 */ uint8_t         type;
    /* 0x0003 */ int8_t          room;
    /* 0x0004 */ uint32_t        flags;
    /* 0x0008 */ z2_xyzf_t       pos_1;
    /* 0x0014 */ z2_rot_t        rot_init;
    /* 0x001A */ char            unk_01_[0x0002];
    /* 0x001C */ uint16_t        variable;
    /* 0x001E */ uint8_t         alloc_index;
    ///* 0x001F */ char            unk_02_;
    /* 0x0020 */ uint16_t        sound_effect;
    ///* 0x0022 */ char            unk_03_[0x0002];
    /* 0x0024 */ z2_xyzf_t       pos_2;
    /* 0x0030 */ z2_rot_t        rot_dir;
    /* 0x0036 */ char            unk_0x36[0x0002];
    /* 0x0038 */ int8_t          unk_0x38;
    /* 0x0039 */ uint8_t         unk_0x39;
    ///* 0x003A */ char            unk_0x3A[0x002];
    /* 0x003C */ z2_xyzf_t       pos_3;
    /* 0x0048 */ z2_rot_t        rot_1;
    //char            unk_06_[0x0002];
    /* 0x0050 */ uint32_t        unk_0x50;
    /* 0x0054 */ float           unk_0x54;
    /* 0x0058 */ z2_xyzf_t       scale;
    /* 0x0064 */ z2_xyzf_t       vel_1;
    /* 0x0070 */ float           xz_speed;
    /* 0x0074 */ float           gravity;
    /* 0x0078 */ float           min_vel_y;

                                                /* struct bgcheck common */
    /* 0x007C */ z2_col_poly_t  *wall_poly;
    /* 0x0080 */ z2_col_poly_t  *floor_poly;
    /* 0x0084 */ uint8_t         wall_poly_source;
    /* 0x0085 */ uint8_t         floor_poly_source;
    /* 0x0086 */ z2_angle_t      wall_rot;
    /* 0x0088 */ float           floor_height;
    /* 0x008C */ float           water_surface_dist;
    /* 0x0090 */ uint16_t        bgcheck_flags;
    /* 0x0092 */ int16_t         unk_0x92_rot;
    /* 0x0094 */ float           unk_0x94;
    /* 0x0098 */ float           dist_from_link_xz;
    /* 0x009C */ float           dist_from_link_y;

                                                /* struct collision_check common */
    /* 0x00A0 */ void           *damage_table;
    /* 0x00A4 */ z2_xyzf_t       vel_2;
    /* 0x00B0 */ char            unk_0C_[0x0006];
    /* 0x00B6 */ uint8_t         mass;
    /* 0x00B7 */ uint8_t         health;
    /* 0x00B8 */ uint8_t         damage;
    /* 0x00B9 */ uint8_t         damage_effect;
    /* 0x00BA */ uint8_t         impact_effect;
    /* 0x00BB */ uint8_t         unk_0xBB;

                                                /* struct start */
    /* 0x00BC */ z2_rot_t        rot_2;
    //char            unk_0F_[0x0002];
    /* 0x00C4 */ float           unk_0xC4;
    /* 0x00C8 */ void           *draw_drop_shadow;
    /* 0x00CC */ float           unk_0xCC;
    /* 0x00D0 */ uint8_t         unk_0xD0;
    //char            pad_0xD1_[0x0003];
                                                /* struct end */

    /* 0x00D4 */ z2_xyzf_t       unk_0xD4;
    /* 0x00E0 */ z2_xyzf_t       unk_0xE0;
    /* 0x00EC */ z2_xyzf_t       unk_0xEC;
    /* 0x00F8 */ float           unk_0xF8;
    /* 0x00FC */ float           unk_0xFC;
    /* 0x0100 */ float           unk_0x100;
    /* 0x0104 */ float           unk_0x104;
    /* 0x0108 */ z2_xyzf_t       pos_4;
    /* 0x0114 */ uint16_t        unk_10_;
    /* 0x0116 */ uint16_t        text_id;
    /* 0x0118 */ int16_t         frozen;
    /* 0x011A */ char            unk_11_[0x0003];
    /* 0x011D */ uint8_t         active;
    /* 0x011E */ char            unk_0x11E;
    /* 0x011F */ uint8_t         tatl_enemy_text_id;
    /* 0x0120 */ z2_actor_t     *attached_a;
    /* 0x0124 */ z2_actor_t     *attached_b;
    /* 0x0128 */ z2_actor_t     *prev;
    /* 0x012C */ z2_actor_t     *next;
    /* 0x0130 */ void           *ctor;
    /* 0x0134 */ void           *dtor;
    /* 0x0138 */ void           *main_proc;
    /* 0x013C */ void           *draw_proc;
    /* 0x0140 */ void           *code_entry;
}; // size = 0x144

typedef struct {
    /* 0x0000 */
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
        /* 0x0000 */ uint16_t        pad;
    };
    /* 0x0002 */ int8_t              x;
    /* 0x0003 */ int8_t              y;
} z2_controller_t; // size = 0x04

typedef struct {
    /* 0x0000 */ z2_controller_t     raw;
    /* 0x0004 */ uint16_t            status;
    /* 0x0006 */ z2_controller_t     raw_prev;
    /* 0x000A */ uint16_t            status_prev;
    /* 0x000C */ uint16_t            pad_pressed;
    /* 0x000E */ int8_t              x_diff;
    /* 0x000F */ int8_t              y_diff;
    /* 0x0010 */ char                unk_0x10_[0x02];
    /* 0x0012 */ uint16_t            pad_released;
    /* 0x0014 */ int8_t              adjusted_x;
    /* 0x0016 */ int8_t              adjusted_y;
    /* 0x0018 */ char                unk_0x16[0x02];
} z2_input_t; // size = 0x18

typedef struct {
    /* 0x0000 */ uint32_t    size;
    /* 0x0004 */ Gfx         *buf;
    /* 0x0008 */ Gfx         *p;
    /* 0x000C */ Gfx         *d;
} z2_disp_buf_t; // size = 0x10

typedef struct {
    /* 0x0000 */ Gfx             *poly_opa_d_start;
    /* 0x0004 */ Gfx             *poly_xlu_d_start;
    /* 0x0008 */ char            unk_0x08_[0x08];
    /* 0x0010 */ Gfx             *overlay_d_start;
    /* 0x0014 */ char            unk_0x18_[0x24];
    /* 0x0038 */ OSMesg          task_mesg[8];
    /* 0x0058 */ char            unk_0x58_[0x04];
    /* 0x005C */ OSMesgQueue     task_queue;
    /* 0x0074 */ char            pad_0x74_[0x04];
    /* 0x0078 */ OSScTask        task;
    /* 0x00D0 */ char            unk_0xD0_[0xD0];
    /* 0x01A0 */ Gfx             *work_d_start;
    /* 0x01A4 */ z2_disp_buf_t   work;
    /* 0x01B4 */ char            unk_0x1B4_[0x04];
    /* 0x01B8 */ z2_disp_buf_t   unk_0x1B8_buf;
    /* 0x01C8 */ char            unk_0x1C8[0xD0];
    /* 0x0298 */ z2_disp_buf_t   overlay;
    /* 0x02A8 */ z2_disp_buf_t   poly_opa;
    /* 0x02B8 */ z2_disp_buf_t   poly_xlu;
    /* 0x02C8 */ uint32_t        frame_cnt_1;
    /* 0x02CC */ void            *frame_buffer;
    /* 0x02D0 */ char            unk_0x2D0_[0x0B];
    /* 0x02DB */ uint8_t         frame_cnt_2;
} z2_gfx_t; // size = 0x2DC

typedef struct z2_ctxt z2_ctxt_t;
typedef void (*game_update_t)(z2_ctxt_t* ctxt);

struct z2_ctxt {
    /* 0x0000 */ z2_gfx_t       *gfx;
    /* 0x0004 */ game_update_t   gamestate_update;
    /* 0x0008 */ void           *gamestate_dtor;
    /* 0x000C */ void           *gamestate_ctor;
    /* 0x0010 */ uint32_t        ctxt_size;
    /* 0x0014 */ z2_input_t      input[4];
    /* 0x0074 */ uint32_t        gamestate_heap_size;
    /* 0x0078 */ void           *gamestate_heap_ptr;
    /* 0x007C */ void           *heap_append_start;
    /* 0x0080 */ void           *heap_append_end;
    /* 0x0084 */ void           *gamestate_heap_node;
    /* 0x0088 */ char            unk_0x88_[0x10];
    /* 0x0098 */ int32_t         execute_state;
    /* 0x009C */ int32_t         gamestate_frames;
    /* 0x00A0 */ int32_t         unk_0xA0;
}; // size = 0xA4

typedef struct {
    /* 0x0000 */ char            view_magic[4];
    /* 0x0004 */ z2_gfx_t        *gfx;
    struct
    {
        /* 0x0008 */ uint32_t        top;
        /* 0x000C */ uint32_t        bottom;
        /* 0x0010 */ uint32_t        left;
        /* 0x0014 */ uint32_t        right;
    } screen;
    /* 0x0018 */ float           camera_distance;
    /* 0x001C */ float           fog_distance;
    /* 0x0020 */ float           z_distance;
    /* 0x0024 */ char            unk_0x24[0x004];
    /* 0x0028 */ z2_xyzf_t       unk_0x28;
    /* 0x0034 */ z2_xyzf_t       unk_0x34;
    /* 0x0040 */ z2_xyzf_t       unk_0x40;
    /* 0x0050 */ Vp              viewport_movemem;
    /* 0x0060 */ Mtx             unk_mtx_0x60;
    /* 0x00A0 */ Mtx             unk_mtx_0xA0;
    /* 0x00E0 */ char            unk_0x00E0[0x40];
    /* 0x0120 */ Mtx            *unk_mtx_0x60_task;
    /* 0x0124 */ Mtx            *unk_mtx_0xA0_task;
    /* 0x0128 */ float           unk_0x128[0x09];
    /* 0x014C */ char            unk_0x14C[0x10];
    /* 0x015C */ uint16_t        perspnorm_scale;
    /* 0x0160 */ uint32_t        unk_0x160;
    /* 0x0164 */ uint32_t        unk_0x164;
} z2_view_t; // size = 0x168

typedef struct {
    /* 0x0000 */ char            param_data[0x50];
    /* 0x0050 */ z2_xyzf_t       at;
    /* 0x005C */ z2_xyzf_t       eye;
    /* 0x0068 */ z2_xyzf_t       up;
    /* 0x0074 */ z2_xyzf_t       eye_next;
    /* 0x0080 */ z2_xyzf_t       sky_offset;
    /* 0x008C */ z2_ctxt_t      *game;
    /* 0x0090 */ z2_actor_t     *focus;
    /* 0x0094 */ z2_xyzf_t       focus_pos;
    /* 0x00A0 */ z2_xyz_t        focus_rot;
    /* 0x00A8 */ z2_actor_t     *target;
    /* 0x00AC */ z2_xyzf_t       target_pos;
    /* 0x00B8 */ z2_xyz_t        target_rot;
    /* 0x00C0 */ float           unk_0xC0;
    /* 0x00C4 */ float           unk_0xC4;
    /* 0x00C8 */ float           unk_0xC8;
    /* 0x00CC */ float           unk_0xCC;
    /* 0x00D0 */ float           unk_0xD0;
    /* 0x00D4 */ float           unk_0xD4;
    /* 0x00D8 */ float           speed_xz;
    /* 0x00DC */ float           dist;
    /* 0x00E0 */ float           speed_ratio;
    /* 0x00E4 */ z2_xyzf_t       at_offset;
    /* 0x00F0 */ z2_xyzf_t       focus_delta;
    /* 0x00FC */ float           fov;
    /* 0x0100 */ float           unk_0x100;
    /* 0x0104 */ float           focus_ground_y;
    /* 0x0108 */ z2_xyzf_t       floor_norm;
    /* 0x0114 */ float           water_y_pos;
    /* 0x0118 */ int             water_prev_idx;
    /* 0x011C */ int             water_prev_set;
    /* 0x0120 */ int16_t         water_quake_id;
    /* 0x0124 */ void           *data0;
    /* 0x0128 */ void           *data1;
    /* 0x012C */ int16_t         data2;
    /* 0x012E */ int16_t         data3;
    /* 0x0130 */ int16_t         uid;
    /* 0x0132 */ char            unk_0x132[2];
    /* 0x0134 */ z2_xyz_t        input_dir;
    /* 0x013A */ z2_xyz_t        cam_dir;
    /* 0x0140 */ int16_t         status;
    /* 0x0142 */ int16_t         set;
    /* 0x0144 */ int16_t         mode;
    /* 0x0146 */ int16_t         bg_chk_id;
    /* 0x0148 */ int16_t         data_idx;
    /* 0x014A */ char            unk_0x14A[0x14];
    /* 0x015E */ int16_t         param_flags;
    /* 0x0160 */ char            unk_0x160[0x18];
} z2_camera_t; // size = 0x178

typedef struct {
    /* 0x0000 */ int32_t count;
    /* 0x0004 */ z2_actor_t *first;
    /* 0x0008 */ int32_t unk_0x8;
} z2_actor_type_t; // size = 0xC

typedef struct {
    /* game_t 0x1CA0 */
    /* 0x0000 */ uint8_t         unk_0x0000;
    /* 0x0001 */ uint8_t         unk_0x0001;
    /* 0x0002 */ uint8_t         unk_0x0002;
    /* 0x0003 */ uint8_t         unk_0x0003;
    /* 0x0004 */ int8_t          unk_0x0004;
    /* 0x0005 */ uint8_t         unk_0x0005;
    /* 0x0006 */ char            unk_0x0006[5];
    /* 0x000B */ int8_t          unk_0x000B;
    /* 0x000C */ char            unk_0x000C[2];
    /* 0x000E */ uint8_t         n_actors_loaded;
    /* 0x000F */ uint8_t         unk_0x000F;
    /* 0x0010 */ z2_actor_type_t  actor_list[16];
    /* 0x00D0 */ char            unk_0x00D0[0x50];
    struct
    {
        /* 0x0120 */ MtxF            unk_0x0120;
        /* 0x0160 */ float           unk_0x0160;
        /* 0x0164 */ float           unk_0x0164;
        /* 0x0168 */ int16_t         unk_0x0168;
        /* 0x016A */ uint8_t         unk_0x016A;
        /* 0x016B */ uint8_t         unk_0x016B;
        /* 0x016C */ int8_t          unk_0x016C;
        /* 0x170 */
        struct
        {
            /* 0x0000 */ float           unk_0x00;
            /* 0x0004 */ float           unk_0x04;
            /* 0x0008 */ float           unk_0x08;
            /* 0x000C */ float           unk_0x0C;
            /* 0x0010 */ int32_t         unk_0x10;
            // size = 0x14
        }               unk_0x0170[3];
        /* 0x01AC */ int32_t         unk_0x01AC;
        /* 0x01B0 */ int32_t         unk_0x01B0;
        /* 0x01B4 */ int32_t         unk_0x01B4;
    };
    /* 0x01B8 */ int32_t         switch_1;
    /* 0x01BC */ int32_t         switch_2;
    /* 0x01C0 */ int32_t         switch_3;
    /* 0x01C4 */ int32_t         switch_4;
    /* 0x01C8 */ int32_t         chest;
    /* 0x01CC */ int32_t         clear;
    /* 0x01D0 */ int32_t         unk_0x01D0;
    /* 0x01D4 */ int32_t         collectible_1;
    /* 0x01D8 */ int32_t         collectible_2;
    /* 0x01DC */ int32_t         collectible_3;
    /* 0x01E0 */ int32_t         collectible_4;
    struct
    {
        /* 0x01E4 */ char        unk_0x00[0x0A];
        /* 0x01EE */ int8_t      unk_0x0A;
        /* 0x01EF */ int8_t      unk_0x0B;
        /* 0x01F0 */ int16_t     unk_0x0C;
        /* 0x01F2 */ int16_t     unk_0x0E;
    } unk_0x1E4;
    /* 0x01F4 */ int8_t          unk_0x01F4;
    /* 0x01F5 */ uint8_t         unk_0x01F5;
    /* 0x01F8 */ float           unk_0x01F8;
    /* 0x01FC */ char            unk_0x01FC[0x54];
    /* 0x0250 */ int32_t         unk_0x0250;
    /* 0x0254 */ char            unk_0x0254[0x14];
    /* 0x0268 */ uint8_t         unk_0x0268;
    /* 0x0269 */ char            unk_0x0269[3];
    /* 0x026C */ char            unk_0x026C[0x18];
} z2_actor_ctxt_t; // size = 0x284

typedef struct
{
    /* 0x0000 */ int16_t           poly_idx;
    /* 0x0002 */ uint16_t          list_next;
} z2_col_list_t; //size = 0x04

typedef struct
{
    /* 0x0000 */
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
    } flags_1;
    /* 0x0004 */
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
    } flags_2;
} z2_col_type_t; // size = 0x8

typedef struct
{
    /* 0x0000 */ z2_xyz_t      pos;
    /* 0x0006 */ int16_t       width;
    /* 0x0008 */ int16_t       depth;
    /* 0x000C */
    struct
    {
        uint32_t    unk_00_ : 12;
        uint32_t    active : 1;
        uint32_t    group : 6; /* ? */
        uint32_t    unk_01_ : 5;
        uint32_t    camera : 8;
    } flags;
} z2_col_water_t; // size = 0x10

typedef struct
{
    /* 0x0000 */ z2_xyz_t        min;
    /* 0x0006 */ z2_xyz_t        max;
    /* 0x000C */ uint16_t        n_vtx;
    /* 0x000E */ uint16_t        pad;
    /* 0x0010 */ z2_xyz_t       *vtx;
    /* 0x0014 */ uint16_t        n_poly;
    // /* 0x0016 */ uint16_t        pad_0x16;
    /* 0x0018 */ z2_col_poly_t  *poly;
    /* 0x001C */ z2_col_type_t  *type;
    /* 0x0020 */ z2_camera_t    *camera;
    /* 0x0024 */ uint16_t        n_water;
    ///* 0x0026 */ uint16_t        pad_0x26;
    /* 0x0028 */ z2_col_water_t *water;
} z2_col_hdr_t; // size = 0x2C

typedef struct
{
    /* 0x0000 */ z2_actor_t       *actor;
    /* 0x0004 */ z2_col_hdr_t     *col_hdr;
    /* 0x0008 */ uint16_t          poly_idx;
    /* 0x000A */ uint16_t          ceiling_list_idx;
    /* 0x000C */ uint16_t          wall_list_idx;
    /* 0x000E */ uint16_t          floor_list_idx;
    /* 0x0010 */ int16_t           unk_0x10;
    /* 0x0014 */ z2_xyzf_t         scale_1;
    /* 0x0020 */ z2_rot_t          rot_1;
    /* 0x0028 */ z2_xyzf_t         pos_1;
    /* 0x0034 */ z2_xyzf_t         scale_2;
    /* 0x0040 */ z2_rot_t          rot_2;
    /* 0x0048 */ z2_xyzf_t         pos_2;
    /* 0x0054 */ int16_t           unk_0x54;
    /* 0x0056 */ int16_t           unk_0x56;
    /* 0x0058 */ int16_t           unk_0x58;
    /* 0x005A */ int16_t           unk_0x5A;
    /* 0x005C */ char              unk_0x5C[0x8];
} z2_col_chk_actor_t; // size = 0x64

typedef struct
{
    /* 0x0000 */ uint16_t          floor_list_idx;
    /* 0x0002 */ uint16_t          wall_list_idx;
    /* 0x0004 */ uint16_t          ceil_list_idx;
} z2_col_lut_t; // size = 0x06

typedef struct /* z2_col_ctxt_t */
{
    /* static collision stuff */
    /* 0x0000 */ z2_col_hdr_t       *col_hdr;
    /* 0x0004 */ z2_xyzf_t           bbox_min;
    /* 0x0010 */ z2_xyzf_t           bbox_max;
    /* 0x001C */ int32_t             n_sect_x;
    /* 0x0020 */ int32_t             n_sect_y;
    /* 0x0024 */ int32_t             n_sect_z;
    /* 0x0028 */ z2_xyzf_t           sect_size;
    /* 0x0034 */ z2_xyzf_t           sect_inv;
    /* 0x0040 */ z2_col_lut_t       *stc_lut;
    /* 0x0044 */ uint16_t            stc_list_max;
    /* 0x0046 */ uint16_t            stc_list_pos;
    /* 0x0048 */ z2_col_list_t      *stc_list;
    /* 0x004C */ uint8_t            *stc_check;

                                                /* bg actor collision struct start */
    /* 0x0050 */ int8_t              unk_0x0050;
    /* 0x0054 */ z2_col_chk_actor_t  actors[50];
    /* 0x13DC */
    union {
        uint16_t        data;
        struct {
            uint16_t    unk_0x00    : 14;
            uint16_t    destroy     : 1;
            uint16_t    active      : 1;
        };
    }                   dynamic_flags[50];
                                                /* dynamic collision stuff */
    /* 0x1440 */ z2_col_poly_t      *dyn_poly;
    /* 0x1444 */ z2_xyz_t           *dyn_vtx;
    /* 0x1448 */ int32_t             unk_0x1448;
    /* 0x144C */ void               *unk_0x144C;
                                                /* struct */
    struct
    {
        /* 0x1450 */ z2_col_list_t    *list;
        /* 0x1454 */ int32_t           count;
        /* 0x1458 */ int32_t           max;
    } dyn;
                                                /* bg actor collision struct end */
    /* 0x145C */ uint32_t            dyn_list_max;
    /* 0x1460 */ uint32_t            dyn_poly_max;
    /* 0x1464 */ uint32_t            dyn_vtx_max;
    /* 0x1468 */ uint32_t            mem_size;
    /* 0x146C */ uint32_t            unk_0x146C;
} z2_col_ctxt_t; // size = 0x1470

typedef struct {
    /* 0x0000 */ uint32_t    vrom_start;
    /* 0x0004 */ uint32_t    vrom_end;
} z2_rom_file_t; // size = 0x08

typedef struct {
    /* 0x0000 */ uint32_t            vrom_addr;
    /* 0x0004 */ void               *dram;
    /* 0x0008 */ uint32_t            size;
    /* 0x000C */ char               *filename;
    /* 0x0010 */ int32_t             line;
    /* 0x0014 */ int32_t             unk_0x14;
    /* 0x0018 */ OSMesgQueue        *notify_mq;
    /* 0x001C */ OSMesg              notify_msg;
} z2_loadfile_t; // size = 0x20

typedef struct {
    /* 0x0000 */ int16_t             id;
    //char                pad_0x02[0x02];
    /* 0x0004 */ void               *data;
    /* 0x0008 */ z2_loadfile_t       loadfile;
    /* 0x0028 */ OSMesgQueue         load_mq;
    /* 0x0040 */ OSMesg              load_msg;
} z2_obj_t; // size = 0x44

typedef struct {
    /* 0x0000 */ void               *obj_space_start;
    /* 0x0004 */ void               *obj_space_end;
    /* 0x0008 */ uint8_t             obj_cnt;
    /* 0x0009 */ uint8_t             spec_cnt;
    /* 0x000A */ uint8_t             keep_idx;
    /* 0x000B */ uint8_t             skeep_idx;
    /* 0x000C */ z2_obj_t            obj[35];
} z2_obj_ctxt_t; // size = 0x958

typedef struct {
    /* 0x0000 */ int8_t      room_1;
    /* 0x0001 */ int8_t      effect_1;
    /* 0x0002 */ int8_t      room_2;
    /* 0x0003 */ int8_t      effect_2;
    /* 0x0004 */ int16_t     actor_id;
    /* 0x0006 */ z2_xyz_t    pos;
    /* 0x000C */ z2_angle_t  rot;
    /* 0x000E */ int16_t     variable;
} z2_room_trans_actor_t; // size = 0x10

typedef struct {
    /* 0x0000 */ uint8_t             idx;
    /* 0x0001 */ char                unk_0x01[0x03];
    /* 0x0004 */ uint8_t             echo;
    /* 0x0006 */ uint8_t             show_invisible_actor;
    //char                pad_0x06[0x02];
    /* 0x0008 */ void               *mesh_hdr;
    /* 0x000C */ void               *file;
    /* 0x0010 */ char                unk_0x10[0x04];
} z2_room_t; // size = 0x14

typedef struct {
    /* 0x0000 */ z2_room_t           rooms[2];
    /* 0x0028 */ void               *room_space_start;
    /* 0x002C */ void               *room_space_end;
    /* 0x0030 */ uint8_t             load_slot;
    /* 0x0031 */ uint8_t             load_active;
    /* 0x0034 */ void               *load_addr;
    /* 0x0038 */ z2_loadfile_t       loadfile;
    /* 0x0058 */ OSMesgQueue         load_mq;
    /* 0x0070 */ OSMesg              load_msg;
    /* 0x0074 */ void               *area_map_tex;
    /* 0x0078 */ char                unk_0x0078[0x08];
    /* 0x0080 */ uint8_t             transition_cnt;
    /* 0x0084 */ void               *transition_list;
} z2_room_ctxt_t; // size = 0x0088

typedef struct {
    /* 0x0000 */ char        unk_0x00[0x168];
    /* 0x0168 */ void       *icon_item_static;
    /* 0x016C */ void       *icon_item_24;
    /* 0x0170 */ void       *icon_item_map;
    /* 0x0174 */ void       *icon_text;
    /* 0x0178 */ void       *icon_name_text;
    /* 0x017C */ Gfx        *bg_dlist;
    /* 0x0180 */ char        unk_0x180[0x6C];
    /* 0x01EC */ uint16_t    state;
    /* 0x01EE */ uint16_t    debug_menu;
    /* 0x01F0 */ char        unk_0x1F0[0x10];
    /* 0x0200 */ uint16_t    switching_screen;
    /* 0x0202 */ uint16_t    unk_0x202;
    /* 0x0204 */ uint16_t    screen_idx;
    /* 0x0206 */ char        unk_0x206[0x32];
    /* 0x0238 */ int16_t     item_cell_offset;
    /* 0x023A */ char        unk_0x23A[4];
    /* 0x023E */ int16_t     mask_cell_offset;
    /* 0x0240 */ char        unk_0x240[2];
    /* 0x0242 */ uint16_t    item_x;
    /* 0x0244 */ char        unk_0x244[0x4];
    /* 0x0248 */ uint16_t    mask_x;
    /* 0x024A */ char        unk_0x24A[0x2];
    /* 0x024C */ uint16_t    item_y;
    /* 0x024E */ char        unk_0x24E[0x4];
    /* 0x0252 */ uint16_t    mask_y;
    /* 0x0254 */ char        unk_0x254[0x4];
    /* 0x0258 */ uint16_t    scroll_button;
    /* 0x025A */ char        unk_0x25A[0x2];
    /* 0x025C */ uint16_t    selected_item;
    /* 0x025E */ uint16_t    item_item;
    /* 0x0260 */ uint16_t    map_item;
    /* 0x0262 */ uint16_t    quest_item;
    /* 0x0264 */ uint16_t    mask_item;
    /* 0x0266 */ uint16_t    unk_0x266;
    /* 0x0268 */ uint16_t    item_cell;
    /* 0x026A */ uint16_t    map_cell;
    /* 0x026C */ uint16_t    quest_cell;
    /* 0x026E */ uint16_t    mask_cell;
} z2_pause_ctxt_t; // size = 0x270

typedef struct {
    /* 0x0000 */ char        unk_0x00[0x170];
    /* 0x0170 */ void       *parameter_static;
    /* 0x0174 */ void       *do_action_static;
    /* 0x0178 */ void       *icon_item_static;
    /* 0x017C */ void       *minimap_texture;
    /* 0x0180 */ char        unk_0x180[0x04];
    /* 0x0184 */ uint32_t    action_rom_addr;
    /* 0x0188 */ void       *action_ram;
    /* 0x018C */ uint32_t    action_size;
    /* 0x0190 */ char        unk_0x190[0x84];
    /* 0x0214 */ uint16_t    action;
    /* 0x0216 */ char        unk_0x216[0x4E];
    union{
        struct {
            /* 0x0264 */ uint16_t    fadeout_alpha;
            /* 0x0266 */ uint16_t    a_alpha;
            /* 0x0268 */ uint16_t    b_alpha;
            /* 0x026A */ uint16_t    c_left_alpha;
            /* 0x026C */ uint16_t    c_down_alpha;
            /* 0x026E */ uint16_t    c_right_alpha;
            /* 0x0270 */ uint16_t    hearts_alpha;
            /* 0x0272 */ uint16_t    rupees_alpha;
        };
        /* 0x0264 */ uint16_t        alphas[0x08];
    };
    /* 0x0274 */ char        unk_0x0274[0x9A];
    /* 0x030E */ uint8_t     restriction_flags[0xC];
    /* 0x031A */ char        unk_0x031A[0x2E];
} z2_hud_ctxt_t; // size = 0x348

enum hitbox_type {
    /* 0x00 */ Z2_HITBOX_SPHERE_LIST,
    /* 0x01 */ Z2_HITBOX_CYLINDER,
    /* 0x02 */ Z2_HITBOX_TRI_LIST,
    /* 0x03 */ Z2_HITBOX_QUAD,
    /* 0x04 */ Z2_HITBOX_SPHERE
};

typedef struct {
    /* 0x0000 */ z2_actor_t                 *actor;
    /* 0x0004 */ char                        unk_0x04[0x11];
    /* 0x0015 */ uint8_t                     type;
    /* 0x0016 */ char                        unk_0x16[0x2];
} z2_hitbox_t; // size = 0x0018

typedef struct {
    /* 0x0000 */ char                        unk_0x00[0x28];
    /* 0x0028 */ z2_xyz_t                    unk_0x28;
    /* 0x002E */ int16_t                     unk_0x2E;
    /* 0x0030 */ z2_xyz_t                    pos;
    /* 0x0036 */ int16_t                     radius;
    /* 0x0038 */ float                       unk_0x38;
    /* 0x003C */ char                        unk_0x3C[0x4];
} z2_hitbox_sphere_ent_t; // size = 0x40

typedef struct {
    /* 0x0000 */ z2_hitbox_t                 base;
    /* 0x0018 */ int32_t                     entry_cnt;
    /* 0x001C */ z2_hitbox_sphere_ent_t     *entries;
} z2_hitbox_sphere_list_t; // size = 0x20

typedef struct {
    /* 0x0000 */ z2_hitbox_t                 base;
    /* 0x0018 */ char                        unk_0x18[0x28];
    /* 0x0040 */ int16_t                     radius;
    /* 0x0042 */ int16_t                     height;
    /* 0x0044 */ int16_t                     y_offset;
    /* 0x0046 */ z2_xyz_t                    pos;
} z2_hitbox_cylinder_t; // size = 0x4C

typedef struct {
    /* 0x0000 */ z2_hitbox_t                 base;
    /* 0x0018 */ char                        unk_0x18[0X30];
    /* 0x0048 */ z2_xyz_t                    pos;
    /* 0x004E */ int16_t                     radius;
} z2_hitbox_sphere_t; // size = 0x50

typedef struct {
    /* 0x0000 */ char                        unk_0x00[0x28];
    /* 0x0028 */ z2_xyzf_t                   vertices[3];
    /* 0x004C */ char                        unk_0x4c[0x10];
} z2_hitbox_tri_ent_t; // size = 0x5C

typedef struct {
    /* 0x0000 */ z2_hitbox_t                 base;
    /* 0x0018 */ int32_t                     entry_cnt;
    /* 0x001C */ z2_hitbox_tri_ent_t        *entries;
} z2_hitbox_tri_list_t; // size = 0x20

typedef struct {
    /* 0x0000 */ z2_hitbox_t                 base;
    /* 0x0018 */ char                        unk_0x18[0x28];
    /* 0x0040 */ z2_xyzf_t                   vertices[4];
    /* 0x0070 */ char                        unk_0x70[0xC];
} z2_hitbox_quad_t; // size = 0x7C

typedef struct {
    /* 0x0000 */ int16_t                     attack_cnt;
    /* 0x0002 */ int16_t                     pad;
    /* 0x0004 */ z2_hitbox_t                *attack[50];
    /* 0x00CC */ int32_t                     ac_cnt;
    /* 0x00D0 */ z2_hitbox_t                *ac[60];
    /* 0x01C0 */ int32_t                     other_cnt;
    /* 0x01C4 */ z2_hitbox_t                *other[50];
} z2_hitbox_ctxt_t; // size = 0x28C

typedef struct {
    /* 0x0000 */ char                unk_0x00[0x168];
    /* 0x0168 */ void               *texture[2];
    /* 0x0170 */ char                unk_0x170[0x8];
    /* 0x0178 */ void               *unk_0x178;
    /* 0x017C */ Gfx                *disp_buf;
    /* 0x0180 */ Gfx                *disp_p;
    /* 0x0184 */ Vtx                *vtx;
    /* 0x0188 */ char                unk_0x188[0x98];
} z2_skybox_ctxt_t; // size = 0x220

typedef struct z2_light_node_s z2_light_node_t;
struct z2_light_node_s {
    /* 0x0000 */ void               *light;
    /* 0x0004 */ z2_light_node_t    *prev;
    /* 0x0008 */ z2_light_node_t    *next;
}; // size = 0x0C

typedef struct {
    /* 0x0000 */ uint32_t            node_cnt;
    /* 0x0004 */ uint32_t            node_cur;
    /* 0x0008 */ z2_light_node_t     nodes[32];
} z2_light_queue_t; // size = 0x0188

typedef struct {
    /* 0x0000 */ uint16_t unk_0x00;
    /* 0x0002 */ uint16_t unk_0x02;
    /* 0x0004 */ char unk_0x04[0x64];
} z2_night_sfx_t; // size = 0x0068

typedef struct {
    /* NZSE     NZSJ */
    /* 0x00000  0x00000 */ z2_ctxt_t           common;
    /* 0x000A4  0x000A4 */ uint16_t            scene_index;
    /* 0x000A6  0x000A6 */ uint8_t             scene_draw_id;
    /* 0x000A7  0x000A7 */ char                unk_0x000A7[9];
    /* 0x000B0  0x000B0 */ void               *scene_addr;
    /* 0x000B4  0x000B4 */ char                unk_0x00B4[4];
    /* 0x000B8  0x000B8 */ z2_view_t           view_scene;
    /* 0x00220  0x00220 */ z2_camera_t         cameras[4];
    /* 0x00800  0x00800 */ z2_camera_t        *active_cameras[4];
    /* 0x00810  0x00810 */ int16_t             camera_cur;
    /* 0x00812  0x00812 */ int16_t             camera_next;
    /* 0x00814  0x00814 */ uint8_t             seq_idx;
    /* 0x00815  0x00815 */ uint8_t             night_seq_idx;
    /* 0x00818  0x00818 */ z2_light_node_t    *light_append;
    /* 0x0081C  0x0081C */ char                unk_0x81C[0x14];
    /* 0x00830  0x00830 */ z2_col_ctxt_t       col_ctxt;
    /* 0x01CA0  0x01CA0 */ z2_actor_ctxt_t     actor_ctxt;
    /* 0x01F24  0x01F24 */ char                unk_0x1F24[0x04];
    /* 0x01F28  0x01F28 */ void               *cutscene_ptr;
    /* 0x01F2C  0x01F2C */ int8_t              cutscene_state;
    /* 0x01F2D  0x01F2D */ char                unk_0x1F2D[0x27B3];
    /* 0x046E0  0x046E0 */ z2_skybox_ctxt_t    skybox_ctx;
    /* 0x04900  0x04900 */ char                unk_0x4900[0x11F2A];
    /* 0x1682A  0x1682A */ uint8_t             message_state_1;
    /* 0x1682B  0x1682B */ char                unk_0x1682B[0xFD];
    /* 0x18928  0x18928 */ uint8_t             message_state_2;
    /* 0x16929  0x16929 */ char                unk_0x16829[0x02];
    /* 0X1692B  0X1692B */ uint8_t             message_state_3;
    /* 0x1692C  0x1692C */ char                unk_0x1692C[0x9C];
#if Z2_VERSION==NZSE
    /* 0x169C8          */ char                unk_0x169C8[0x20];
#endif
    /* 0x169E8   0x169C8 */ z2_hud_ctxt_t       hud_ctx;
    /* 0x16D30   0x16D10 */ z2_pause_ctxt_t     pause_ctx;
    /* 0x16FA0   0x16F80 */ char                unk_0x16FA0[0x148];
    /* 0x170E8   0x170c8 */ uint8_t             night_sfx_state;
    /* 0x170E9   0x170c9 */ char                unk_0x170E9[0xE];
    /* 0x170F7   0x170d7 */ uint8_t             rain_timer1;
    /* 0x170F8   0x170d8 */ char                unk_0x170F8[2];
    /* 0x170FA   0x170dA */ uint8_t             rain_timer2;
    /* 0x170FB   0x170dB */ char                unk_0x170FB[0xC8D];
    /* 0x17D88   0x17D68 */ z2_obj_ctxt_t       obj_ctx;
    /* 0x186E0   0x186c0 */ z2_room_ctxt_t      room_ctx;
    /* 0x18768   0x18748 */ char                unk_0x18768[0xE0];
    /* 0x18848   0x18828 */ uint8_t             room_cnt;
    /* 0x1884C   0x1882C */ z2_rom_file_t      *room_list;
    /* 0x18850   0x18820 */ char                unk_0x18850[0x24];
    /* 0x18874   0x18854 */ uint8_t             skybox_type;
    /* 0x18875   0x18855 */ uint8_t             scene_load_flag;
    /* 0x18876   0x18856 */ char                unk_0x18876[0x04];
    /* 0x1887A   0x1885A */ uint16_t            entrance_index;
    /* 0x1887C   0x1885C */ char                unk_0x1887C[0x08];
    /* 0x18884   0x18864 */ z2_hitbox_ctxt_t    hitbox_ctxt;
    /* 0x18B10   0x18B10 */ char                unk_0x18B10[0x748];
} z2_game_t; // size = 0x19258  0x19238

typedef struct {
    /* 0x0000 */ z2_actor_t          common;
    /* 0x0144 */ char                unk_0x144[0x250];
    /* 0x0394 */ uint8_t             action;
    /* 0x0395 */ char                unk_0x395[0x6D7];
    /* 0x0A6C */ uint32_t            state_flags_1;
    /* 0x0A70 */ uint32_t            state_flags_2;
    /* 0x0A74 */ char                unk_0xA74[0x5C];
    /* 0x0AD0 */ float               linear_velocity;
    /* 0x0AD4 */ uint16_t            movement_angle;
    /* 0x0AD6 */ char                unk_0xAD6[0x05];
    /* 0x0ADB */ uint8_t             sword_active;
    /* 0x0ADC */ char                unk_0xADC[0x84];
    /* 0x0B60 */ uint16_t            blast_mask_timer;
} z2_link_t; // size >= 0x0B60

typedef struct {
    /* 0x0000 */ z2_actor_t          common;
    /* 0x0144 */ uint8_t             daynight;
    /* 0x0145 */ char                unk_0x145;
    /* 0x0146 */ uint16_t            timer_boundaries[3];
} z2_timer_t; // size = 0x14C

typedef union{
    struct {
        /* 0x0000 */ uint32_t    direct_reference;
        /* 0x0004 */ uint32_t    nintendo_logo;
        /* 0x0008 */ uint32_t    current_scene;
        /* 0x000C */ uint32_t    current_room;
        /* 0x0010 */ uint32_t    gameplay_keep;
        /* 0x0014 */ uint32_t    gameplay_dungeon_field_keep;
        /* 0x0018 */ uint32_t    current_object;
        /* 0x001C */ uint32_t    link_animetion;
        /* 0x0020 */ uint32_t    unk_0x0020;
        /* 0x0024 */ uint32_t    unk_0x0024;
        /* 0x0028 */ uint32_t    current_mask;
        /* 0x002C */ uint32_t    unk_0x002C;
        /* 0x0030 */ uint32_t    unk_0x0030;
        /* 0x0034 */ uint32_t    unk_0x0034;
        /* 0x0038 */ uint32_t    z_buffer;
        /* 0x003C */ uint32_t    frame_buffer;
    };
    /* 0x0000 */ uint32_t        segments[0x10];
} z2_segment_t; // size = 0x40

typedef struct z2_arena_node    z2_arena_node_t;
typedef struct z2_arena         z2_arena_t;

/* english nodes are 0x10 bytes, jp 1.1 and 1.0 nodes are 0x30 bytes */

struct z2_arena_node {
    /* 0x0000 */ uint16_t            magic;                  /* 0x0000 */
    /* 0x0002 */ uint16_t            free;                   /* 0x0002 */
    /* 0x0004 */ uint32_t            size;                   /* 0x0004 */
    /* 0x0008 */ z2_arena_node_t    *next;                   /* 0x0008 */
    /* 0x000C */ z2_arena_node_t    *prev;                   /* 0x000C */
#if Z2_VERSION==NZSE
    /* 0x0010 */ char                data[];                 /* 0x0010 */
#else
    /* 0x0010 */ char               *filename;               /* 0x0010 */
    /* 0x0014 */ int32_t             line;                   /* 0x0014 */
    /* 0x0018 */ OSId                thread_id;              /* 0x0018 */
    /* 0x001C */ z2_arena_t         *arena;                  /* 0x001C */
    /* 0x0020 */ uint32_t            count_hi;               /* 0x0020 */
    /* 0x0024 */ uint32_t            count_lo;               /* 0x0024 */
    /* 0x0028 */ char                pad[0x8];               /* 0x0028 */
    /* 0x0030 */ char                data[];                 /* 0x0030 */
#endif
}; // size 0x10 0x30

struct z2_arena {
    /* 0x0000 */ z2_arena_node_t    *first;
    /* 0x0004 */ void               *start;
    /* 0x0008 */ void               *unk_0x08_;
    /* 0x000C */ void               *unk_0x0C_;
}; // size = 0x10

typedef struct {
    /* 0x0000 */ void               *ram;
    /* 0x0004 */ uint32_t            vrom_start;
    /* 0x0008 */ uint32_t            vrom_end;
    /* 0x000C */ uint32_t            vram_start;
    /* 0x0010 */ uint32_t            vram_end;
    /* 0x0014 */ char                unk_0x14[0x04];
    /* 0x0018 */ void               *vram_ctor;
    /* 0x001C */ void               *vram_dtor;
    /* 0x0020 */ char                unk_0x20[0xC];
    /* 0x002C */ uint32_t            alloc_size;
} z2_gamestate_table_t; // size = 0x30

typedef struct {
    /* 0x0000 */ uint32_t            vrom_start;
    /* 0x0004 */ uint32_t            vrom_end;
    /* 0x0008 */ uint32_t            vram_start;
    /* 0x000C */ uint32_t            vram_end;
    /* 0x0010 */ void               *ram;
    /* 0x0014 */ uint32_t            initialization;
    /* 0x0018 */ void               *filename;
    /* 0x001C */ uint16_t            alloc_type;
    /* 0x001E */ uint8_t             loaded_cnt;
    /* 0x001F */ char                pad_0x19;
} z2_actor_ovl_table_t; // size = 0x20

typedef struct {
    /* 0x0000 */ void               *ram;
    /* 0x0004 */ uint32_t            vrom_start;
    /* 0x0008 */ uint32_t            vrom_end;
    /* 0x000C */ uint32_t            vram_start;
    /* 0x0010 */ uint32_t            vram_end;
    /* 0x0014 */ int32_t             reloc_off;
    /* 0x0018 */ char               *filename;
} z2_player_ovl_table_t; // size = 0x1C

typedef struct {
    /* 0x0000 */ uint32_t            vrom_start;
    /* 0x0004 */ uint32_t            vrom_end;
    /* 0x0008 */ uint32_t            prom_start;
    /* 0x000C */ uint32_t            prom_end;
} z2_file_table_t; // size = 0x10

typedef union {
    struct {
        uint16_t    no_y_rot    : 1;
        uint16_t    no_x_rot    : 1;
        uint16_t    no_z_rot    : 1;
        uint16_t                : 1;
        uint16_t    actor_id    : 12;
    };
    uint16_t    param_1;
} z2_actorlist_param0_t;

typedef union{
    struct {
        uint16_t    x_rot               : 9;
        uint16_t                        : 4;
        uint16_t    spawn_time_flags_hi : 3;
    };
    uint16_t param_4;
} z2_actorlist_param4_t;

typedef union{
    struct {
        uint16_t    y_rot       : 9;
        uint16_t    camera_idx  : 7;
    };
    uint16_t param_5;
} z2_actorlist_param5_t;

typedef union{
    struct {
        uint16_t    z_rot               : 9;
        uint16_t    spawn_time_flags_lo : 7;
    };
    uint16_t param_6;
} z2_actorlist_param6_t;

typedef struct {
    uint32_t    text_size;              /* 0x0000 */
    uint32_t    data_size;              /* 0x0004 */
    uint32_t    rodata_size;            /* 0x0008 */
    uint32_t    bss_size;               /* 0x000C */
    uint32_t    reloc_cnt;              /* 0x0010 */
} z2_overlay_hdr_t;                     /* 0x0014 */

typedef struct {
    /* 0x0000 */ uint32_t    vrom_start;
    /* 0x0004 */ uint32_t    vrom_end;
    /* 0x0008 */ uint16_t    entrance_message;
    /* 0x000A */ char        unk_0xA;
    /* 0x000B */ uint8_t     scene_configuration;
    /* 0x000C */ char        pad[0x4];
} z2_scene_table_ent_t; // size = 0x10

typedef struct {
    /* 0x0000 */ uint32_t        vrom_start;
    /* 0x0004 */ uint32_t        vrom_end;
    /* 0x0008 */ uint32_t        vram_start;
    /* 0x000C */ uint32_t        vram_end;
    /* 0x0010 */ void           *ram;
    /* 0x0014 */ uint32_t        vram_ctor;
    /* 0x0018 */ uint32_t        unk_0x18;
} z2_particle_ovl_table_t; // size = 0x1C

typedef struct {
    /* 0x0000 */ z2_xyzf_t       pos;
    /* 0x000C */ z2_xyzf_t       speed;
    /* 0x0018 */ z2_xyzf_t       acceleration;
    /* 0x0024 */ void           *main;
    /* 0x0028 */ void           *draw;
    /* 0x002C */ z2_xyzf_t       data;
    /* 0x0038 */ Gfx            *dlist;
    /* 0x003C */ int32_t         data_32;
    /* 0x0040 */ int16_t         data_16[14];
    /* 0x005C */ int16_t         time;
    /* 0x005E */ uint8_t         priority;
    /* 0x005F */ uint8_t         id;
} z2_particle_t; // size = 0x6

typedef struct {
    /* 0x0000 */ z2_particle_t  *part_space;
    /* 0x0004 */ uint32_t        part_cnt;
    /* 0x0008 */ uint32_t        part_max;
} z2_particle_info_t; // size = 0x0C

typedef struct {
    /* 0x0000 */ uint8_t         active;
    /* 0x0001 */ char            data[0x4C7];
} z2_dot_t; // size = 0x4C8

typedef struct {
    /* 0x0000 */ uint8_t         active;
    /* 0x0001 */ char            data[0x1AF];
} z2_trail_t; // size = 0x1B0

typedef struct {
    /* 0x0000 */ uint8_t         active;
    /* 0x0001 */ char            data[0x1CB];
} z2_spark_t; // size = 0x1CC

typedef struct {
    /* 0x0000 */ uint8_t         active;
    /* 0x0001 */ char            data[0x60F];
} z2_unk_part_t; // size = 0x610

typedef struct {
    /* 0x0000 */ z2_game_t      *game;
    /* 0x0004 */ z2_dot_t        dots[3];
    /* 0x0E5C */ z2_trail_t      trails[25];
    /* 0x388C */ z2_spark_t      sparks[3];
    /* 0x3DF0 */ z2_unk_part_t   unks[15];
} z2_static_particle_ctxt_t; // size = 0x98E0

typedef struct {
    /* 0x0000 */ uint8_t     cmd;
    /* 0x0001 */ uint8_t     tex_idx;
    /* 0x0002 */ uint16_t    pad_0x02;
    /* 0x0004 */ uint8_t     sky_id;
    /* 0x0005 */ uint8_t                 : 4;
    /* 0x0005 */ uint8_t     sky_effect  : 4;
    /* 0x0006 */ uint8_t                 : 4;
    /* 0x0006 */ uint8_t     fog         : 4;
    /* 0x0007 */ uint8_t     pad_0x07;
} z2_sky_scene_cmd_t; //size = 0x08;

typedef struct {
    /* 0x0000 */ uint8_t     cmd;
    /* 0x0001 */ uint8_t     pad_0x01[3];
    /* 0x0004 */ uint32_t    addr;
} z2_alternate_scene_cmd_t; // size = 0x08

typedef struct {
    /* 0x0000 */ uint8_t     cmd;
    /* 0x0001 */ uint8_t     data[7];
} z2_scene_cmd_t; // size = 0x08

typedef struct {
    /* 0x0000 */ float unk_0x00;
    /* 0x0004 */ float unk_0x04;
    /* 0x0008 */ float unk_0x08;
    /* 0x000C */ float unk_0x0c;
    /* 0x0010 */ char unk_0x10[8];
    /* 0x0018 */ uint16_t unk_0x18;
    /* 0x001A */ uint16_t unk_0x1A;
} seq_ctl_unk_0x00_t; // size = 0x1C

typedef struct {
    /* 0x0000 */ seq_ctl_unk_0x00_t unk_0x00[16];
    /* 0x01C0 */ float vol_cur;
    /* 0x01C4 */ float vol_target;
    /* 0x01C8 */ char unk_0x1C8[0x34];
    /* 0x01FC */ uint32_t unk_1FC;
    /* 0x0200 */ uint16_t timer;
    /* 0x0202 */ char unk_0x202[0x6];
    /* 0x0208 */ uint16_t unk_0x208;
    /* 0x020A */ uint16_t seq_idx;
    /* 0x020C */ uint16_t prev_seq_idx;
    /* 0x020E */ char unk_0x20E[0x4];
    /* 0x0212 */ uint8_t vol_factor[4];
    /* 0x0216 */ char unk_0x216[5];
    /* 0x021B */ uint8_t unk_21B;
} z2_seq_ctl_t; // size = 0x21C

typedef struct {
    /* 0x0000 */ uint32_t    hi;
    /* 0x0004 */ void       *lo;
} z2_audio_cmd_t; // size = 0x08

typedef struct {
    union {
        struct {
            /* 0x0000 : 0 */ uint32_t enabled : 1;
            /* 0x0000 : 1 */ uint32_t : 2;
            /* 0x0000 : 5 */ uint32_t mute : 1;
            /* 0x0000 : 6 */ uint32_t : 28;
        };
        /* 0x0000 */ uint32_t flags;
    };
    /* 0x0004 */ char unk_0x04[0xE0];
} z2_aud_chan_t; // size = 0xE4

typedef struct {
    union {
        struct {
            /* 0x0000 : 0 */ uint32_t enabled : 1;
            /* 0x0000 : 1 */ uint32_t : 31;
        };
        /* 0x0000 */ uint32_t flags;
    };
    /* 0x0001 */ char unk_0x04[0xC];
    /* 0x0010 */ uint16_t delay;
    /* 0x0012 */ char unk_0x12[0x26];
    /* 0x0038 */ z2_aud_chan_t *channels[16];
    /* 0x0078 */ char unk_0x078[0xE8];
} z2_sequencer_t; // size = 0x160

typedef struct {
    /* 0x0000 */ char unk_0x00[0x28C0];
    /* 0x28C0 */ uint32_t unk_ctr;
    /* 0x1B9C */ char unk_0x28C4[0x1B9C];
    /* 0x4460 */ z2_sequencer_t sequencers[5];
    /* 0x4B40 */ char unk_0x4B40[0x2D00];
    /* 0x7840 */ z2_aud_chan_t chan_none;
    /* 0x7924 */ char unk_0x7924[0x54];
    /* 0x7978 */ uint8_t cmd_wr_pos;
    /* 0x7979 */ uint8_t cmd_rd_pos;
    /* 0x797A */ char unk_0x797A[0x7A];
    /* 0x79F4 */ z2_audio_cmd_t cmd_buf[0x100];
    /* 0x81F4 */ char unk_0x81F4[4];
} z2_audio_ctxt_t; // size = 0x81F8

typedef struct {
    /* 0x0000 */ OSThread thread;
    /* 0x01F4 */ uint8_t arg_cnt;
    /* 0x01F5 */ uint8_t flags;
    /* 0x01F8 */ void (*callback)();
    /* 0x01FC */ void *cbArg0;
    /* 0x0200 */ void *cbArg1;
} z2_bgtask_t; // size = 0x204

typedef struct {
    /* 0x0000 */ int8_t scene_id;
    /* 0x0001 */ int8_t spawn_id;
    /* 0x0002 */ int16_t variable;
} z2_entrance_rec_t; // size = 0x04

typedef struct {
    /* 0x0000 */ int8_t ent_cnt;
    /* 0x0004 */ z2_entrance_rec_t **entrances;
    /* 0x0008 */ char filename;
} z2_extern_ent_tab_t; // size = 0x0C

typedef struct {
    /* 0x0000 */ uint16_t map;
    /* 0x0002 */ int16_t x;
    /* 0x0004 */ int16_t y;
    /* 0x0006 */ int16_t z;
    /* 0x0008 */ int16_t unk_0x8;
} z2_minimap_room_info_t; // size = 0x0A

typedef struct {
    /* 0x0000 */ z2_minimap_room_info_t **room_info;
    /* 0x0004 */ int32_t room;
    /* 0x0008 */ int16_t target_x;
    /* 0x000A */ int16_t target_y;
    /* 0x000C */ int16_t cur_x;
    /* 0x000E */ int16_t cur_y;
    /* 0x0010 */ void *cur_tex;
    /* 0x0014 */ int32_t trnsn_room;
    /* 0x0018 */ void *prev_tex;
    /* 0x001C */ int16_t unk_0x1C;
    /* 0x001E */ int16_t unk_0x1E;
    /* 0x0020 */ int32_t unk_0x20;
    /* 0x0024 */ int32_t trnsn_timer;
    /* 0x0028 */ void *tex1;
    /* 0x002C */ void *tex2;
    /* 0x0030 */ int16_t unk_0x30;
    /* 0x0032 */ int16_t unk_0x32;
    /* 0x0034 */ int16_t unk_0x34;
    /* 0x0036 */ int16_t unk_0x36;
    /* 0x0038 */ int16_t unk_0x38;
    /* 0x003A */ int16_t unk_0x3A;
    /* 0x003C */ int32_t unk_0x3C;
    /* 0x0040 */ int16_t unk_0x40;
    /* 0x0042 */ int16_t unk_0x42;
    /* 0x0044 */ int16_t unk_0x44;
    /* 0x0048 */ int32_t unk_0x48;
    /* 0x004C */ int32_t unk_0x4C;
} z2_minimap_ctxt_t; // size = 0x50

#define Z2_DISP_SIZE 0x20310
#define Z2_CIMG_SIZE 0x25800

#define z2_extern       extern __attribute__ ((section(".data")))
/* functions */
z2_extern void          z2_LoadOverlay              (uint32_t vrom_start, uint32_t vrom_end, uint32_t vram_start, uint32_t vram_end, void *dest);
z2_extern int32_t       osEPiStartDma               (OSPiHandle *pihandle, OSIoMesg *mb, int32_t direction);
z2_extern int32_t       osEPiReadIo                 (OSPiHandle *pihandle, uint32_t devAddr, uint32_t *data);
z2_extern int32_t       osEPiWriteIo                (OSPiHandle *pihandle, uint32_t devAddr, uint32_t data);
z2_extern void          z2_DrawActors               (z2_game_t *game, z2_actor_ctxt_t *actor_ctx);
z2_extern z2_actor_t   *z2_SpawnActor               (z2_actor_ctxt_t *actor_ctx, z2_game_t *game, uint16_t actor_id,
                                                     float x, float y, float z, z2_angle_t rx, z2_angle_t ry, z2_angle_t rz,
                                                     int16_t actor_variable, int camera_cmd_idx, int spawn_time_flags, void *param_13);
z2_extern void          z2_DeleteActor              (z2_actor_ctxt_t *actor_ctx, z2_actor_t *actor, z2_game_t *game);
z2_extern void          z2_CreateStaticCollision    (z2_col_ctxt_t *col_ctx, z2_game_t *game, z2_col_lut_t *col_lut);
z2_extern void          z2_pause_persp              (z2_game_t *game);
z2_extern void          z2_load_pause_map           (z2_game_t *game, void *ptr);
z2_extern uint32_t      z2_get_mmap_tile_size       (uint16_t map_id);
z2_extern int           z2_get_map_type             (z2_game_t *game);
z2_extern void          z2_btnupdate                (z2_game_t *game, uint8_t btn_idx);
z2_extern void          z2_ActionLabelUpdate        (z2_hud_ctxt_t *hud_ctx, uint16_t action, int btn_idx);
z2_extern void          z2_LoadRoom                 (z2_game_t *game, z2_room_ctxt_t *room_ctx, uint8_t room_id);
z2_extern void          z2_DrawRoom                 (z2_game_t *game, z2_room_t *room);
z2_extern void          z2_UnloadRoom               (z2_game_t *game, z2_room_ctxt_t *room_ctx);
z2_extern void          z2_CreateSkyboxVtx          (z2_skybox_ctxt_t *skybox_ctx, int a1);
z2_extern void          z2_LoadSkyboxData           (z2_game_t  *game, z2_skybox_ctxt_t *sky_ctx, int16_t type);
z2_extern void          z2_MotionBlur               (z2_ctxt_t *ctx);
z2_extern void          z2_input_update             (z2_ctxt_t *ctx);
z2_extern void          z2_adjust_stick             (z2_input_t *inp);
z2_extern void          z2_DecodeArchiveFile        (uint32_t rom, uint8_t tile, void *ram, uint32_t size);
#if Z2_VERSION==NZSE
z2_extern void          z2_LoadArchiveFile          (uint32_t rom, void *ram, size_t size);
#endif
z2_extern void          z2_LoadArchiveFile2         (uint32_t rom, int arg1, void *ram, size_t size);
z2_extern void          z2_BgTaskStop               (z2_bgtask_t *bgtask);
z2_extern void          z2_dmaflashtoram            (void *ram, uint32_t block, uint32_t block_cnt);
z2_extern void          z2_dmaramtoflash            (void *ram, uint32_t block, uint32_t block_cnt);
z2_extern void          z2_AfxCmdFloat              (uint32_t cmd, float data);
z2_extern void          z2_AfxCmdWord               (uint32_t cmd, uint32_t data);
z2_extern void          z2_AfxCmdByte               (uint32_t cmd, int8_t data);
z2_extern void          z2_AfxCmdShort              (uint32_t cmd, uint16_t data);
z2_extern void          z2_CheckAfxCfg              (void);
z2_extern void          z2_AfxConfig                (uint8_t cfg);
z2_extern void          z2_AudioReset               (uint8_t cfg);
z2_extern void          z2_StopSfx                  (void);
z2_extern void          z2_AfxCmdFlush              (void);

/* data */
z2_extern int32_t                   z2_vi_counter;
z2_extern OSMesgQueue               z2_file_msgqueue;
z2_extern z2_arena_t                z2_arena;
z2_extern z2_file_table_t           z2_file_table[];
z2_extern z2_particle_info_t        z2_particle_info;
z2_extern z2_particle_ovl_table_t   z2_particle_ovl_table[38];
z2_extern z2_actor_ovl_table_t      z2_actor_ovl_table[689];
z2_extern char                      z2_hud_state[];
z2_extern uint32_t                  z2_event_state_1;
z2_extern char                      z2_letter_box_timer[];
z2_extern char                      z2_cutscene_state[];
z2_extern z2_gamestate_table_t      z2_gamestate_table[];
z2_extern z2_minimap_ctxt_t         z2_minimap_ctx;
z2_extern int                       z2_minimap_room_cnt;
z2_extern uint32_t                  z2_icon_item_static_table[0x65];
z2_extern char                      z2_restriction_table[0x23A];
z2_extern z2_rom_file_t             z2_area_tex_table[9];
z2_extern uint16_t                  z2_link_form_obj_idx[];
z2_extern z2_rom_file_t             z2_obj_table[];
z2_extern z2_scene_table_ent_t      z2_scene_table[113];
z2_extern z2_extern_ent_tab_t       z2_extern_scene_table[110];
z2_extern uint16_t                  z2_stored_song;
z2_extern uint16_t                  z2_link_spawn_obj;
z2_extern z2_player_ovl_table_t     z2_player_ovl_table[2];
z2_extern z2_player_ovl_table_t    *z2_player_ovl_cur;
z2_extern float                     z2_framerate_div;
z2_extern z2_static_particle_ctxt_t z2_static_particle_ctxt;
z2_extern z2_file_t                 z2_file;
z2_extern char                      z2_cs_info[];
z2_extern z2_light_queue_t          z2_light_queue;
z2_extern z2_arena_t                z2_game_arena;
z2_extern z2_segment_t              z2_segment;
z2_extern uint32_t                 *z2_cimg[];
z2_extern z2_input_t                z2_input_direct;
z2_extern MtxF                     *z2_mtx_stack;
z2_extern MtxF                     *z2_mtx_stack_top;
z2_extern OSPiHandle                z2_pi_io_handle;
z2_extern uint8_t                   z2_sfx_cmd_wr_pos;
z2_extern uint8_t                   z2_sfx_cmd_rd_pos;
z2_extern uint8_t                   z2_seq_cmd_wr_pos;
z2_extern uint8_t                   z2_seq_cmd_rd_pos;
z2_extern uint8_t                   z2_afx_cfg;
z2_extern uint8_t                   z2_afx_cfg_state;
z2_extern uint32_t                  z2_seq_cmd_buf[0x100];
z2_extern z2_seq_ctl_t              z2_seq_ctl[5];
z2_extern z2_audio_ctxt_t           z2_audio_ctxt;
z2_extern char                      z2_disp[];
z2_extern Gfx                       z2_seg_disp[18];
z2_extern z2_ctxt_t                 z2_ctxt;
z2_extern z2_game_t                 z2_game;
z2_extern z2_link_t                 z2_link;
z2_extern uint32_t                  z2_cs_bars;
z2_extern z2_bgtask_t               z2_prerender_bgtask;
z2_extern uint8_t                   z2_oca_note_pos;
z2_extern uint8_t                   z2_cur_oca_song[9];
z2_extern uint8_t                   z2_oca_state[];
z2_extern uint32_t                  z2_oca_unk_ctr;
z2_extern uint8_t                   z2_song_state[];
z2_extern uint8_t                   z2_staff_notes[];
z2_extern void                     *z2_song_ptr;
z2_extern uint32_t                  z2_song_ctr;
z2_extern uint16_t                  z2_unk_song_pos;
z2_extern uint32_t                  z2_random;
z2_extern z2_night_sfx_t            z2_night_sfx[];
z2_extern char                      z2_saturation[];
z2_extern char                      z2_hide_clock;

// pause menu hacks
#if Z2_VERSION==NZSE
#define item_right_offset               0x5F48
#define item_left_offset                0x5FF4
#define mask_right_offset               0xA7C4
#define mask_left_offset                0xA8B0
#elif Z2_VERSION==NZSJ
#define item_right_offset               0x5F48
#define item_left_offset                0x5FF4
#define mask_right_offset               0xA7C4
#define mask_left_offset                0xA8B0
#elif Z2_VERSION==NZSJ10
#define item_right_offset               0x5F48
#define item_left_offset                0x5FF4
#define mask_right_offset               0xA734
#define mask_left_offset                0xA820
#endif

// File indcies
#if Z2_VERSION==NZSE
#define z2_icon_item_static             9
#define z2_icon_item_field_static       10
#define z2_icon_item_dungeon_static     11
#define z2_map_name_static              13
#define z2_pause_assets                 14
#define z2_minimap_assets               16
#define z2_item_names                   17
#define z2_location_names               18
#define z2_item_icon_archive            19
#define z2_icon_item_24_static          20
#define z2_do_action_static             25
#define z2_parameter_static             1126
#define z2_clock_face_days              1127
#else
#define z2_icon_item_static             8
#define z2_icon_item_24_static          9
#define z2_icon_item_field_static       10
#define z2_icon_item_dungeon_static     11
#define z2_map_name_static              13
#define z2_minimap_assets               15
#define z2_item_names                   16
#define z2_item_icon_archive            18
#define z2_do_action_static             22
#define z2_parameter_static             1125
#define z2_clock_face_days              1126
#endif

#endif
