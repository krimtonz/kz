#include "scenes.h"

struct kz_scene scenes[] = {
    {                                                   /* 0000 */
        0, 3, 
        "mayor's residence",
        (const char*[]){
            "east clock town",
            "after couples mask",
            "unknown (crash)",
        },
    },
    {                                                   /* 0001 */
        2, 1,
        "majora's lair",
        (const char*[]){
            "moon",
        },
    },
    {                                                   /* 0002 */
        4, 1,
        "hags potion shop",
        (const char*[]){
            "southern swamp",
        },
    },
    {                                                   /* 0003 */
        6, 3,
        "romani ranch buildings",
        (const char*[]){
            "barn from ranch",
            "house from ranch",
            "unknown (crash)"
        },
    },
    {                                                   /* 0004 */
        8, 1,
        "honey & darling",
        (const char*[]){
            "east clock town",
        },
    },
    {                                                   /* 0005 */
        10, 2, 
        "beneath graveyard",
        (const char*[]){
            "day 2 start",
            "day 1 start",
        },
    },
    {                                                   /* 0006 */
        12, 11,
        "southern swamp (clear)",
        (const char*[]){
            "swamp road",
            "boat house",
            "woodfall",
            "lower deku palace",
            "upper deku palace",
            "hags potion shop",
            "boat cruise",
            "woods of mystery",
            "swamp spider house",
            "ikana canyon",
            "owl statue",
        },
    },
    {                                                   /* 0007 */
        14, 4,
        "curiosity shop",
        (const char*[]){
            "west clock town",
            "kafei's hideout from laundry pool",
            "spying start",
            "spying end",
        },
    },
    {                                                   /* 0008 */
        16, 1,
        "test map",
        (const char*[]){
            "unknown",
        },
    },
    {                                                   /* 0009 */
        20, 17, 
        "grottos",
        (const char*[]){
            "ocean gossip stones",
            "swamp gossip stones",
            "canyon gossip stones",
            "mountain gossip stones",
            "genetic grotto",
            "hot spring",
            "maze straight (a)",
            "dodongo",
            "maze vines (lower)",
            "business scrub",
            "cows",
            "ocean heart piece",
            "magic bean seller",
            "peahat",
            "maze straight (b)",
            "maze grotto (upper)",
            "lens of truth",
        },
    },
    {                                                   /* 0010 */
        28, 10,
        "cutscene map",
        (const char*[]){
            "unknown",
            "unknown (crash)",
            "unknown (crash)",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown (crash)",
        },
    },
    {                                                   /* 0011 */
        32, 15,
        "ikana canyon",
        (const char*[]){
            "ikana road",
            "ghost hut",
            "music box house",
            "stone tower",
            "owl statue",
            "beneath the well",
            "sakon's hideout",
            "after stone tower",
            "ikana castle",
            "after house opens",
            "song of storms cave from ikana after house opened",
            "fairy fountain",
            "secret shrine",
            "from song of storms cave",
            "song of storms cave from ikana before house opened",
        },
    },
    {                                                   /* 0012 */
        34, 15,
        "pirates fortress",
        (const char*[]){
            "exterior pirates fortress",
            "lower hookshot room",
            "upper hookshot room",
            "silver rupee room",
            "silver rupee room exit",
            "barrel room",
            "barrel room exit",
            "twin barrel room",
            "twin barrel room exit",
            "oob near twin barrel",
            "telescope",
            "oob hookshot room",
            "balcony",
            "uuper hookshot room",
            "unknown (crash)",
        },
    },
    {                                                   /* 0013 */
        36, 1, 
        "milk bar",
        (const char*[]){
            "east clock town",
        },
    },
    {                                                   /* 0014 */
        38, 2,
        "stone tower temple",
        (const char*[]){
            "intro",
            "no intro",
        },
    },
    {                                                   /* 0015 */
        40, 2,
        "treasure chest shop",
        (const char*[]){
            "east clock town",
            "after game",
        },
    },
    {                                                   /* 0016 */
        42, 3,
        "stone tower temple (inverted)",
        (const char*[]){
            "main entrance",
            "boss room entrance",
            "unknown (crash)",
        },
    },
    {                                                   /* 0017 */
        44, 3,
        "clock tower",
        (const char*[]){
            "first encounter",
            "after song of time",
            "unknown (crash)",
        },
    },
    {                                                   /* 0018 */
        46, 5,
        "before clock town",
        (const char*[]){
            "falling from cliff",
            "inside clock tower",
            "transformed to deku",
            "void respawn",
            "song of time flashback",
        },
    },
    {                                                   /* 0019 */
        48, 3,
        "woodfall temple",
        (const char*[]){
            "main entrance",
            "prison after odolwa",
            "prision (unknown)",
        },
    },
    {                                                   /* 0020 */
        50, 2,
        "path to mountain village",
        (const char*[]){
            "termina field",
            "mountain village",
        },
    },
    {                                                   /* 0021 */
        52, 7, 
        "ikana castle",
        (const char*[]){
            "beneath the well",
            "ikana canyon",
            "exterior from interior",
            "interior from exterior",
            "powder keg hole",
            "block hole",
            "throne room"
        },
    },
    {                                                   /* 0022 */
        54, 2,
        "deku playground",
        (const char*[]){
            "north clock town",
            "after game",
        },
    },
    {                                                   /* 0023 */
        56, 1,
        "odolwa",
        (const char*[]){
            "woodfall temple",
        },
    },
    {                                                   /* 0024 */
        58, 2,
        "town shooting gallery",
        (const char*[]){
            "east clock town (intro)",
            "east clock town (no intro)",
        },
    },
    {                                                   /* 0025 */
        60, 2,
        "snowhead temple",
        (const char*[]){
            "snowhead (intro)",
            "snowhead (no intro)",
        },
    },
    {                                                   /* 0026 */
        62, 7,
        "milk road",
        (const char*[]){
            "termina field",
            "romani ranch",
            "gorman track (track exit)",
            "gorman track (main exit)",
            "owl statue",
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0027 */
        64, 16, 
        "pirates fortress interior",
        (const char*[]){
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown (crash)",
            "unknown (crash)",
            "unknown (crash)",
            "unknown (crash)",
        },
    },
    {                                                   /* 0028 */
        66, 1,
        "swamp shooting gallery",
        (const char*[]){
            "road to southern swamp",
        },
    },
    {                                                   /* 0029 */
        68, 2,
        "pinnacle rock",
        (const char*[]){
            "great bay coast",
            "void respawn",
        },
    },
    {                                                   /* 0030 */
        70, 10,
        "fairy fountain",
        (const char*[]){
            "clock town",
            "woodfall",
            "snowhead",
            "zora cape",
            "ikana canyon",
            "after magic",
            "after spin attack",
            "after double magic",
            "after double defense",
            "after great fairy sword",
        },
    },
    {                                                   /* 0031 */
        72, 1,
        "swamp spider house",
        (const char*[]){
            "soutern swamp",
        },
    },
    {                                                   /* 0032 */
        74, 1,
        "oceanside spider house",
        (const char*[]){
            "great bay coast",
        },
    },
    {                                                   /* 0033 */
        76, 3,
        "observatory",
        (const char*[]){
            "east clock town",
            "termina field",
            "after telescope",
        },
    },
    {                                                   /* 0034 */
        78, 1,
        "deku trial",
        (const char*[]){
            "moon",
        },
    },
    {                                                   /* 0035 */
        80, 11,
        "deku palace",
        (const char*[]){
            "soutern swamp",
            "thrown out",
            "deku king chamber",
            "deku king chamber (upper)",
            "deku shrine",
            "southern swamp (shortcut)",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0036 */
        82, 1,
        "mountain smithy",
        (const char*[]){
            "moutain village",
        },
    },
    {                                                   /* 0037 */
        84, 18,
        "termina field",
        (const char*[]){
            "west clock town",
            "road to soutern swamp",
            "great bay coast",
            "path to mountain village",
            "road to ikana",
            "milk road",
            "south clock town",
            "east clock town",
            "north clock town",
            "observatory",
            "observatory (telescope)",
            "unknown",
            "moon crash",
            "cremia hug",
            "skullkid cutscene",
            "west clock town",
            "unknown (crash)",
            "unknown (crash)",
        },
    },
    {                                                   /* 0038 */
        86, 1,
        "post office",
        (const char*[]){
            "west clock town",
        },
    },
    {                                                   /* 0039 */
        88, 1,
        "marine lab",
        (const char*[]){
            "great bay coast",
        },
    },
    {                                                   /* 0040 */
        90, 2,
        "dampes house",
        (const char*[]){
            "beneath the graveyard",
            "graveyard",
        },
    },
    {                                                   /* 0041 */
        94, 4,
        "goron shrine",
        (const char*[]){
            "goron village",
            "goron shop",
            "after lullaby",
            "goron village (no intro)",
        },
    },
    {                                                   /* 0042 */
        96, 9,
        "zora hall",
        (const char*[]){
            "zora cape",
            "zora cape (turtle)",
            "zora shop",
            "lulu's room",
            "evan's room",
            "japa's room",
            "mikau & tijo's room",
            "stage",
            "after rehearsal",
        },
    },
    {                                                   /* 0043 */
        98, 2,
        "trading post",
        (const char*[]){
            "west clock town (intro)",
            "west clock town (no intro)",
        },
    },
    {                                                   /* 0044 */
        100, 12,
        "romani ranch",
        (const char*[]){
            "milk road",
            "after target practice",
            "barn",
            "house",
            "cucco shack",
            "doggy racetrack",
            "after aliens",
            "after romani leaves",
            "failing cremia game",
            "failing aliens",
            "after aliens intro",
            "leaving with cremia",
        },
    },
    {                                                   /* 0045 */
        102, 5,
        "twinmold",
        (const char*[]){
            "inverted stone tower",
            "inverted stone tower",
            "unknown (crash)",
            "unknown (crash)",
            "unknown (crash)",
        },
    },
    {                                                   /* 0046 */
        104, 14,
        "great bay coast",
        (const char*[]){
            "termina field",
            "zora cape",
            "void respawn",
            "pinnacle rock",
            "fisherman hut",
            "pirates fortress",
            "void resapwn (murky water)",
            "marine lab",
            "oceanside spider house",
            "during zora mask",
            "after zora mask",
            "owl statue",
            "thrown out",
            "after jumping game",
        },
    },
    {                                                   /* 0047 */
        106, 10,
        "zora cape",
        (const char*[]){
            "great bay coast",
            "zora hall",
            "zora hall (turtle)",
            "void respawn",
            "waterfall",
            "fairy fountain",
            "owl statue",
            "great bay temple",
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0048 */
        108, 1,
        "lottery shop",
        (const char*[]){
            "west clock town",
        },
    },
    {                                                   /* 0049 */
        112, 7,
        "pirates fortress exterior",
        (const char*[]){
            "great bay coast",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0050 */
        114, 1,
        "fisherman's hut",
        (const char*[]){
            "great bay coast",
        },
    },
    {                                                   /* 0051 */
        116, 1, 
        "goron shop",
        (const char*[]){
            "goron shrine",
        },
    },
    {                                                   /* 0052 */
        118, 4,
        "deku king's chamber",
        (const char*[]){
            "deku palace",
            "deku palace (upper)",
            "monkey released",
            "unknown",
        },
    },
    {                                                   /* 0053 */
        120, 1,
        "goron trial",
        (const char*[]){
            "moon",
        },
    },
    {                                                   /* 0054 */
        122, 3,
        "road to southern swamp",
        (const char*[]){
            "termina field",
            "southern swamp",
            "swamp shooting gallery",
        },
    },
    {                                                   /* 00555 */
        124, 2,
        "doggy racetrack",
        (const char*[]){
            "romani ranch",
            "after race",
        },
    },
    {                                                   /* 0056 */
        126, 2,
        "cucco shack",
        (const char*[]){
            "romani ranch",
            "after bunny hood",
        },
    },
    {                                                   /* 0057 */
        128, 6,
        "ikana graveyard",
        (const char*[]){
            "road to ikana",
            "unknown",
            "unknown",
            "unknown",
            "dampe's house",
            "after keeta defeated",
        },
    },
    {                                                   /* 0058 */
        130, 1,
        "goht",
        (const char*[]){
            "snowhead temple",
        },
    },
    {                                                   /* 0059 */
        132, 11,
        "southern swamp (poison)",
        (const char*[]){
            "road to soutern swamp",
            "boat house",
            "woodfall",
            "deku palace",
            "deku palace (shortcut)",
            "hags potion shop",
            "boat ride",
            "woods of mystery",
            "swamp spider house",
            "ikana canyon",
            "owl statue",
        },
    },
    {                                                   /* 0060 */
        134, 5,
        "woodfall",
        (const char*[]){
            "southern swamp",
            "unknown",
            "fairy fountain",
            "unknown",
            "owl statue",
        },
    },
    {                                                   /* 0061 */
        136, 2,
        "zora trial",
        (const char*[]){
            "moon",
            "void respawn",
        },
    },
    {                                                   /* 0062 */
        138, 5,
        "goron village (spring)",
        (const char*[]){
            "path to goron village (spring)",
            "unknown",
            "goron shrine",
            "lens of truth",
            "unknown",
        },
    },
    {                                                   /* 0063 */
        140, 3,
        "great bay temple",
        (const char*[]){
            "zora cape (waving)",
            "zora cape",
            "unknown (crash)",
        },
    },
    {                                                   /* 0064 */
        142, 4,
        "waterfall",
        (const char*[]){
            "zora cape",
            "race start",
            "race end",
            "unknown",
        },
    },
    {                                                   /* 0065 */
        144, 2,
        "beneath the well",
        (const char*[]){
            "ikana canyon",
            "ikana castle",
        },
    },
    {                                                   /* 0066 */
        146, 7,
        "zora hall rooms",
        (const char*[]){
            "mikau from zora hall",
            "japas from zora hall",
            "lulu from zora hall",
            "evan from zora hall",
            "japa after jam session",
            "zora shop from zora hall",
            "evan after composing song",
        },
    },
    {                                                   /* 0067 */
        148, 5,
        "goron village (winter)",
        (const char*[]){
            "path to goron village (winter)",
            "unknown",
            "goron shrine",
            "lens of truth",
            "unknown",
        },
    },
    {                                                   /* 0068 */
        150, 2,
        "goron graveyard",
        (const char*[]){
            "mountain village",
            "receiving goron mask",
        },
    },
    {                                                   /* 0069 */
        152, 1,
        "sakon's hideout",
        (const char*[]){
            "ikana canyon",
        },
    },
    {                                                   /* 0070 */
        154, 9,
        "mountain village (winter)",
        (const char*[]){
            "unknown",
            "mountain smithy",
            "path to goron village (winter)",
            "goron graveyard",
            "path to snowhead",
            "unknown",
            "path to mountain village",
            "unknown",
            "owl statue",
        },
    },
    {                                                   /* 0071 */
        156, 3,
        "ghost hut",
        (const char*[]){
            "ikana canyon",
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0072 */
        158, 3,
        "deku shrine",
        (const char*[]){
            "deku palace",
            "deku palace",
            "unknown (crash)",
        },
    },
    {                                                   /* 0073 */
        160, 3,
        "road to ikana",
        (const char*[]){
            "termina field",
            "ikana canyon",
            "ikana graveyard",
        },
    },
    {                                                   /* 0074 */
        162, 1,
        "swordsman school",
        (const char*[]){
            "west clock town",
        },
    },
    {                                                   /* 0075 */
        164, 1, 
        "music box house",
        (const char*[]){
            "ikana canyon",
        },
    },
    {                                                   /* 0076 */
        166, 1, 
        "igos du ikana",
        (const char*[]){
            "ikana castle",
        },
    },
    {                                                   /* 0077 */
        168, 3,
        "boat house",
        (const char*[]){
            "southern swamp",
            "koume",
            "tingle's dad",
        },
    },
    {                                                   /* 0078 */
        170, 4,
        "stone tower",
        (const char*[]){
            "ikana canyon",
            "unknown",
            "stone tower temple",
            "owl statue",
        },
    },
    {                                                   /* 0079 */
        172, 2,
        "stone tower (inverted)",
        (const char*[]){
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0080 */
        174, 9,
        "mountain village (spring)",
        (const char*[]){
            "unknown",
            "mountain smithy",
            "path to goron village (spring)",
            "goron graveyard",
            "path to snowhead",
            "unknown",
            "path to mountain village",
            "unknown",
            "owl statue",
        },
    },
    {                                                   /* 0081 */
        176, 4,
        "path to snowhead",
        (const char*[]){
            "mountain village",
            "unknown",
            "snowhead",
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0082 */
        178, 4,
        "snowhead",
        (const char*[]){
            "path to snowhead",
            "snowhead temple",
            "fairy fountain",
            "owl statue",
        },
    },
    {                                                   /* 0083 */
        180, 3,
        "path to goron village (winter)",
        (const char*[]){
            "mountain village (winter)",
            "goron village (winter)",
            "goron racetrack",
        },
    },
    {                                                   /* 0084 */
        182, 3,
        "path to goron village (spring)",
        (const char*[]){
            "mountain village (spring)",
            "goron village (spring)",
            "goron racetrack",
        },
    },
    {                                                   /* 0085 */
        184, 2,
        "gyorg",
        (const char*[]){
            "great bay temple",
            "unknown",
        },
    },
    {                                                   /* 0086 */
        186, 1,
        "secret shrine",
        (const char*[]){
            "ikana canyon",
        },
    },
    {                                                   /* 0087 */
        188, 6,
        "stock pot inn",
        (const char*[]){
            "east clock town (main)",
            "east clock town (balcony)",
            "granny's story",
            "unknown",
            "eavesdropping anju",
            "after eavesdropping",
        },
    },
    {                                                   /* 0088 */
        190, 1,
        "great bay (cutscene)",
        (const char*[]){
            "zora cape",
        },
    },
    {                                                   /* 0089 */
        192, 7,
        "clock tower interior",
        (const char*[]){
            "before portal to termina",
            "south clock town",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0090 */
        194, 1,
        "woods of mystery",
        (const char*[]){
            "southern swamp",
        },
    },
    {                                                   /* 0091 */
        196, 3,
        "lost woods",
        (const char*[]){
            "unknown",
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0092 */
        198, 1,
        "link trial",
        (const char*[]){
            "moon",
        },
    },
    {                                                   /* 0093 */
        200, 1,
        "moon",
        (const char*[]){
            "clock tower",
        },
    },
    {                                                   /* 0094 */
        202,2,
        "bomb shop",
        (const char*[]){
            "west clock town",
            "west clock town",
        },
    },
    {                                                   /* 0095 */
        204, 1, 
        "giants chamber",
        (const char*[]){
            "oath to order",
        },
    },
    {                                                   /* 0096 */
        206, 6,
        "gorman race track",
        (const char*[]){
            "milk road",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0097 */
        208, 3, 
        "goron racetrack",
        (const char*[]){
            "path to mountain village",
            "race start",
            "race end",
        },
    },
    {                                                   /* 0098 */
        210, 13,
        "east clock town",
        (const char*[]){
            "termina field (freeze)",
            "south clock town (freeze)",
            "observatory (freeze)",
            "south clock town",
            "treasure chest shop",
            "north clock town",
            "honey & darling",
            "mayor's residence",
            "shooting gallery",
            "stock pot inn (main)",
            "stock pot inn (upper)",
            "milk bar",
            "unknown (crash)",
        },
    },
    {                                                   /* 0099 */
        212, 10,
        "west clock town",
        (const char*[]){
            "termina field",
            "south clock town (lower)",
            "south clock town (upper)",
            "swordsman school",
            "curiosity shop",
            "trading post",
            "bomb shop",
            "post office",
            "lottery shop",
            "termina field",
        },
    },
    {                                                   /* 0100 */
        214, 8,
        "north clock town",
        (const char*[]){
            "termina field",
            "east clock town",
            "south clock town",
            "fairy fountain",
            "deku playground",
            "unknown",
            "unknown",
            "unknown",
        },
    },
    {                                                   /* 0101 */
        216, 11,
        "south clock town",
        (const char*[]){
            "clock tower interior",
            "termina field",
            "east clock town (upper)",
            "west clock town (upper)",
            "north clock town",
            "west clock town (lower)",
            "laundry pool",
            "east clock town (lower)",
            "clock tower",
            "owl statue",
            "clock tower (after song of time)",
        },
    },
    {                                                   /* 0102 */
        218, 3,
        "laundry pool",
        (const char*[]){
            "south clock town",
            "kafei's hideout",
            "unknown (crash)",
        },
    },
};

struct kz_scene_category scene_categories[] = {
    {
        "dungeons",
        5, (uint8_t[]){
            19, 25, 63, 14, 16,
        },
    },
    {
        "bosses",
        6, (uint8_t[]){
            23, 58, 85, 45, 01,
            76,
        },
    },
    {
        "trials",
        4, (uint8_t[]){
            34, 53, 61, 92,
        }
    },
    {
        "overworld",
        15, (uint8_t[]){
            37, 54, 06, 59, 60,
            20, 83, 84, 81, 82,
            46, 47, 73, 11, 26,
        },
    },
    {
        "towns",
        14, (uint8_t[]){
            101, 100, 98, 99, 102, 
            44, 35, 52, 70, 80, 
            67, 62, 41, 42, 
        },
    },
    {
        "shops",
        7, (uint8_t[]){
            2, 7, 13, 43, 48, 
            51, 94
        }
    },
    {
        "minigames",
        11, (uint8_t[]){
            4, 22, 24, 28, 38, 
            55, 56, 74, 86, 71,
            72, 
        }
    },
    {
        "houses",
        10, (uint8_t[]){
            0, 3, 39, 40, 50,
            69, 75, 77, 87, 36
        }
    }
};