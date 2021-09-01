#include "scenes.h"
#include "z2.h"

struct kz_scene scenes[] = {
    SCENE_INIT(0, "mayor's residence",
            "east clock town",
            "after couples mask",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
#endif
    ),                                                      /* 000 */
    SCENE_INIT(2, "majora's lair",
            "moon",
    ),                                                      /* 001 */
    SCENE_INIT(4, "hags potion shop",
            "southern swamp",
    ),                                                      /* 002 */
    SCENE_INIT(6, "romani ranch buildings",
            "barn from ranch",
            "house from ranch",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)"
#endif
    ),                                                      /* 003 */
    SCENE_INIT(8, "honey & darling",
            "east clock town",
    ),                                                      /* 004 */
    SCENE_INIT(10, "beneath graveyard",
            "day 2 start",
            "day 1 start",
    ),                                                      /* 005 */
    SCENE_INIT(12, "southern swamp (clear)",
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
    ),                                                      /* 006 */
    SCENE_INIT(14, "curiosity shop",
            "west clock town",
            "kafei's hideout",
            "spying start",
            "spying end",
    ),                                                      /* 007 */
    SCENE_INIT(16, "test map",
            "unknown",
    ),                                                      /* 008 */
    SCENE_INIT(20, "grottos",
            "ocean gossip stones",
            "swamp gossip stones",
            "canyon gossip stones",
            "mountain gossip stones",
            "generic grotto",
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
    ),                                                      /* 009 */
    SCENE_INIT(28, "cutscene map",
            "unknown",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
            "unknown (crash)",
#endif
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
            "unknown",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
#endif
    ),                                                      /* 010 */
    SCENE_INIT(32, "ikana canyon",
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
            "song of storms cave (house open)",
            "fairy fountain",
            "secret shrine",
            "from song of storms cave",
            "song of storms cave (house closed) ",
    ),                                                      /* 011 */
    SCENE_INIT(34, "pirates fortress",
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
            "upper hookshot room",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
#endif
    ),                                                      /* 012 */
    SCENE_INIT(36, "milk bar",
            "east clock town",
    ),                                                      /* 013 */
    SCENE_INIT(38, "stone tower temple",
            "intro",
            "no intro",
    ),                                                      /* 014 */
    SCENE_INIT(40, "treasure chest shop",
            "east clock town",
            "after game",
    ),                                                      /* 015 */
    SCENE_INIT(42, "stone tower temple (inverted)",
            "main entrance",
            "boss room entrance",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
#endif
    ),                                                      /* 016 */
    SCENE_INIT(44, "clock tower",
            "first encounter",
            "after song of time",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
#endif
    ),                                                      /* 017 */
    SCENE_INIT(46, "before clock town",
            "falling from cliff",
            "inside clock tower",
            "transformed to deku",
            "void respawn",
            "song of time flashback",
    ),                                                      /* 018 */
    SCENE_INIT(48, "woodfall temple",
            "main entrance",
            "prison after odolwa",
            "deku princess room",
    ),                                                      /* 019 */
    SCENE_INIT(50, "path to mountain village",
            "termina field",
            "mountain village",
    ),                                                      /* 020*/
    SCENE_INIT(52, "ikana castle",
            "beneath the well",
            "ikana canyon",
            "exterior from interior",
            "interior from exterior",
            "powder keg hole",
            "block hole",
            "throne room"
    ),                                                      /* 021 */
    SCENE_INIT(54, "deku playground",
            "north clock town",
            "after game",
    ),                                                      /* 022 */
    SCENE_INIT(56, "odolwa",
            "woodfall temple",
    ),                                                      /* 023 */
    SCENE_INIT(58, "town shooting gallery",
            "east clock town (intro)",
            "east clock town (no intro)",
    ),                                                      /* 024 */
    SCENE_INIT(60, "snowhead temple",
            "snowhead (intro)",
            "snowhead (no intro)",
    ),                                                      /* 025 */
    SCENE_INIT(62, "milk road",
            "termina field",
            "romani ranch",
            "gorman track (track exit)",
            "gorman track (main exit)",
            "owl statue",
            "unknown",
            "unknown",
    ),                                                      /* 026 */
    SCENE_INIT(64, "pirates fortress interior",
            "hookshot room",
            "hookshot room upper",
            "100 rupee room",
            "100 rupee room (egg)",
            "barrel room",
            "barrel room (egg)",
            "twin barrel room",
            "twin barrel room (egg)",
            "telescope",
            "outside, underwater",
            "outside, telescope",
            "unknown",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
            "unknown (crash)",
            "unknown (crash)",
            "unknown (crash)",
#endif
    ),                                                      /* 027 */
    SCENE_INIT(66, "swamp shooting gallery",
            "road to southern swamp",
    ),                                                      /* 028 */
    SCENE_INIT(68, "pinnacle rock",
            "great bay coast",
            "void respawn",
    ),                                                      /* 029 */
    SCENE_INIT(70, "fairy fountain",
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
    ),                                                      /* 030 */
    SCENE_INIT(72, "swamp spider house",
            "southern swamp",
    ),                                                      /* 031 */
    SCENE_INIT(74, "oceanside spider house",
            "great bay coast",
    ),                                                      /* 032 */
    SCENE_INIT(76, "observatory",
            "east clock town",
            "termina field",
            "after telescope",
    ),                                                      /* 033 */
    SCENE_INIT(78, "deku trial",
            "moon",
    ),                                                      /* 034 */
    SCENE_INIT(80, "deku palace",
            "southern swamp",
            "thrown out",
            "deku king chamber",
            "deku king chamber (upper)",
            "deku shrine",
            "southern swamp (shortcut)",
            "jp grotto left, first room",
            "jp grotto left, second room",
            "jp grotto right, second room",
            "bean seller",
            "jp grotto right, first room",
    ),                                                      /* 035 */
    SCENE_INIT(82, "mountain smithy",
            "mountain village",
    ),                                                      /* 036 */
    SCENE_INIT(84, "termina field",
            "west clock town",
            "road to southern swamp",
            "great bay coast",
            "path to mountain village",
            "road to ikana",
            "milk road",
            "south clock town",
            "east clock town",
            "north clock town",
            "observatory",
            "observatory (telescope)",
            "near ikana",
            "moon crash",
            "cremia hug",
            "skullkid cutscene",
            "west clock town",
    ),                                                      /* 037 */
    SCENE_INIT(86, "post office",
            "west clock town",
    ),                                                      /* 038 */
    SCENE_INIT(88, "marine lab",
            "great bay coast",
    ),                                                      /* 039 */
    SCENE_INIT(90, "dampes house",
            "beneath the graveyard",
            "graveyard",
    ),                                                      /* 040 */
    SCENE_INIT(94, "goron shrine",
            "goron village",
            "goron shop",
            "after lullaby",
            "goron village (no intro)",
    ),                                                      /* 041 */
    SCENE_INIT(96, "zora hall",
            "zora cape",
            "zora cape (turtle)",
            "zora shop",
            "lulu's room",
            "evan's room",
            "japa's room",
            "mikau & tijo's room",
            "stage",
            "after rehearsal",
    ),                                                      /* 042 */
    SCENE_INIT(98, "trading post",
            "west clock town (intro)",
            "west clock town (no intro)",
    ),                                                      /* 043 */
    SCENE_INIT(100, "romani ranch",
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
    ),                                                      /* 044 */
    SCENE_INIT(102, "twinmold",
            "inverted stone tower",
            "inverted stone tower",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
            "unknown (crash)",
            "unknown (crash)",
#endif
    ),                                                      /* 045 */
    SCENE_INIT(104, "great bay coast",
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
    ),                                                      /* 046 */
    SCENE_INIT(106, "zora cape",
            "great bay coast",
            "zora hall",
            "zora hall (turtle)",
            "void respawn",
            "waterfall",
            "fairy fountain",
            "owl statue",
            "great bay temple",
            "after great bay temple",
            "unknown",
    ),                                                      /* 047 */
    SCENE_INIT(108, "lottery shop",
            "west clock town",
    ),                                                      /* 048 */
    SCENE_INIT(112, "pirates fortress exterior",
            "great bay coast",
            "pirates fortress",
            "underwater passage",
            "underwater jet",
            "kicked out",
            "hookshot platform",
            "passage door",
    ),                                                      /* 049 */
    SCENE_INIT(114, "fisherman's hut",
            "great bay coast",
    ),                                                      /* 050 */
    SCENE_INIT(116, "goron shop",
            "goron shrine",
    ),                                                      /* 051 */
    SCENE_INIT(118, "deku king's chamber",
            "deku palace",
            "deku palace (upper)",
            "monkey released",
            "front of king",
    ),                                                      /* 052 */
    SCENE_INIT(120, "goron trial",
            "moon",
    ),                                                      /* 053 */
    SCENE_INIT(122, "road to southern swamp",
            "termina field",
            "southern swamp",
            "swamp shooting gallery",
    ),                                                      /* 054 */
    SCENE_INIT(124, "doggy racetrack",
            "romani ranch",
            "after race",
    ),                                                      /* 055 */
    SCENE_INIT(126, "cucco shack",
            "romani ranch",
            "after bunny hood",
    ),                                                      /* 056 */
    SCENE_INIT(128, "ikana graveyard",
            "road to ikana",
            "grave 1",
            "grave 2",
            "grave 3",
            "dampe's house",
            "after keeta defeated",
    ),                                                      /* 057 */
    SCENE_INIT(130, "goht",
            "snowhead temple",
    ),                                                      /* 058 */
    SCENE_INIT(132, "southern swamp (poison)",
            "road to southern swamp",
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
    ),                                                      /* 059 */
    SCENE_INIT(134, "woodfall",
            "southern swamp",
            "unknown",
            "fairy fountain",
            "unknown",
            "owl statue",
    ),                                                      /* 060 */
    SCENE_INIT(136, "zora trial",
            "moon",
            "void respawn",
    ),                                                      /* 061 */
    SCENE_INIT(138, "goron village (spring)",
            "path to goron village (spring)",
            "unknown",
            "goron shrine",
            "lens of truth",
            "void out",
    ),                                                      /* 062 */
    SCENE_INIT(140, "great bay temple",
            "zora cape (waving)",
            "zora cape",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
#endif
    ),                                                      /* 063 */
    SCENE_INIT(142, "waterfall",
            "zora cape",
            "race start",
            "race end",
            "game won",
    ),                                                      /* 064 */
    SCENE_INIT(144, "beneath the well",
            "ikana canyon",
            "ikana castle",
    ),                                                      /* 065 */
    SCENE_INIT(146, "zora hall rooms",
            "mikau from zora hall",
            "japas from zora hall",
            "lulu from zora hall",
            "evan from zora hall",
            "japa after jam session",
            "zora shop from zora hall",
            "evan after composing song",
    ),                                                      /* 066 */
    SCENE_INIT(148, "goron village (winter)",
            "path to goron village (winter)",
            "deku flower",
            "goron shrine",
            "lens of truth",
            "void out",
    ),                                                      /* 067 */
    SCENE_INIT(150, "goron graveyard",
            "mountain village",
            "receiving goron mask",
    ),                                                      /* 068 */
    SCENE_INIT(152, "sakon's hideout",
            "ikana canyon",
    ),                                                      /* 069 */
    SCENE_INIT(154, "mountain village (winter)",
            "after snowhead",
            "mountain smithy",
            "path to goron village (winter)",
            "goron graveyard",
            "path to snowhead",
            "on ice",
            "path to mountain village",
            "unknown",
            "owl statue",
    ),                                                      /* 070 */
    SCENE_INIT(156, "ghost hut",
            "ikana canyon",
            "after minigame",
            "beat minigame",
    ),                                                      /* 071 */
    SCENE_INIT(158, "deku shrine",
            "deku palace",
            "deku palace",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
#endif
    ),                                                      /* 072 */
    SCENE_INIT(160, "road to ikana",
            "termina field",
            "ikana canyon",
            "ikana graveyard",
    ),                                                      /* 073 */
    SCENE_INIT(162, "swordsman school",
            "west clock town",
    ),                                                      /* 074 */
    SCENE_INIT(164, "music box house",
            "ikana canyon",
    ),                                                      /* 075 */
    SCENE_INIT(166, "igos du ikana",
            "ikana castle",
    ),                                                      /* 076 */
    SCENE_INIT(168, "boat house",
            "southern swamp",
            "koume",
            "tingle's dad",
    ),                                                      /* 077 */
    SCENE_INIT(170, "stone tower",
            "ikana canyon",
            "unknown",
            "stone tower temple",
            "owl statue",
    ),                                                      /* 078 */
    SCENE_INIT(172, "stone tower (inverted)",
            "after inverting",
            "stone tower temple",
    ),                                                      /* 079 */
    SCENE_INIT(174, "mountain village (spring)",
            "after snowhead",
            "mountain smithy",
            "path to goron village (spring)",
            "goron graveyard",
            "path to snowhead",
            "behind waterfall",
            "path to mountain village",
            "after snowhead (cutscene)",
            "owl statue",
    ),                                                      /* 080 */
    SCENE_INIT(176, "path to snowhead",
            "mountain village",
            "unknown",
            "snowhead",
            "unknown",
            "unknown",
    ),                                                      /* 081 */
    SCENE_INIT(178, "snowhead",
            "path to snowhead",
            "snowhead temple",
            "fairy fountain",
            "owl statue",
    ),                                                      /* 082 */
    SCENE_INIT(180, "path to goron village (winter)",
            "mountain village (winter)",
            "goron village (winter)",
            "goron racetrack",
    ),                                                      /* 083 */
    SCENE_INIT(182, "path to goron village (spring)",
            "mountain village (spring)",
            "goron village (spring)",
            "goron racetrack",
    ),                                                      /* 084 */
    SCENE_INIT(184, "gyorg",
            "great bay temple",
            "falling cutscene",
    ),                                                      /* 085 */
    SCENE_INIT(186, "secret shrine",
            "ikana canyon",
    ),                                                      /* 086 */
    SCENE_INIT(188, "stock pot inn",
            "east clock town (main)",
            "east clock town (balcony)",
            "granny's story",
            "anju meeting",
            "eavesdropping anju",
            "after eavesdropping",
    ),                                                      /* 087 */
    SCENE_INIT(190, "great bay (cutscene)",
            "zora cape",
    ),                                                      /* 088 */
    SCENE_INIT(192, "clock tower interior",
            "twisted hallway",
            "south clock town",
            "deku mask cutscene",
            "moon crash",
            "song of time",
            "twisted hallway",
            "majora's mask cutscene",
    ),                                                      /* 089 */
    SCENE_INIT(194, "woods of mystery",
            "southern swamp",
    ),                                                      /* 090 */
    SCENE_INIT(196, "lost woods",
            "kicked off epona",
            "song of time cutscene",
            "unknown",
    ),                                                      /* 091 */
    SCENE_INIT(198, "link trial",
            "moon",
    ),                                                      /* 092 */
    SCENE_INIT(200, "moon",
            "clock tower",
    ),                                                      /* 093 */
    SCENE_INIT(202, "bomb shop",
            "west clock town",
            "west clock town",
    ),                                                      /* 094 */
    SCENE_INIT(204, "giants chamber",
            "oath to order",
    ),                                                      /* 095 */
    SCENE_INIT(206, "gorman race track",
            "milk road",
            "unknown",
            "beat minigame",
            "milk road behind fence",
            "milk road fence cutscene",
            "unknown",
            "start minigame",
    ),                                                      /* 096 */
    SCENE_INIT(208, "goron racetrack",
            "path to mountain village",
            "race start",
            "race end",
    ),                                                      /* 097 */
    SCENE_INIT(210, "east clock town",
            "termina field",
            "south clock town",
            "observatory",
            "south clock town",
            "treasure chest shop",
            "north clock town",
            "honey & darling",
            "mayor's residence",
            "shooting gallery",
            "stock pot inn (main)",
            "stock pot inn (upper)",
            "milk bar",
#ifdef ENABLE_CRASH_WARP
            "unknown (crash)",
#endif
    ),                                                      /* 098 */
    SCENE_INIT(212, "west clock town",
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
    ),                                                      /* 099 */
    SCENE_INIT(214, "north clock town",
            "termina field",
            "east clock town",
            "south clock town",
            "fairy fountain",
            "deku playground",
            "bombers code",
            "after bomberrs",
            "after sakon",
    ),                                                      /* 100 */
    SCENE_INIT(216, "south clock town",
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
            "clock tower (after song of time)"
    ),                                                      /* 101 */
    SCENE_INIT(218, "laundry pool",
            "south clock town",
            "kafei's hideout"
#ifdef ENABLE_CRASH_WARP
            , "unknown (crash)"
#endif
    )                                                       /* 102 */
};

struct kz_scene_category scene_categories[] = {
    SCENE_CATEGORY_INIT("clock town",
        101, 100, 99, 98, 102, 22, 94, 7, 43, 38, 48, 0, 4, 13, 15, 24, 74, 87
    ),
    SCENE_CATEGORY_INIT("swamp",
            54, 28, 59, 6, 77, 2, 90, 31, 35, 52, 72, 60, 19, 23
    ),
    SCENE_CATEGORY_INIT("snowhead",
            20, 70, 80, 36, 68, 83, 84, 97, 67, 62, 41, 51, 81, 82, 25, 58
    ),
    SCENE_CATEGORY_INIT("great bay",
            46, 39, 32, 50, 29, 12, 49, 27, 47, 64, 42, 66, 63, 85
    ),
    SCENE_CATEGORY_INIT("ikana",
            73, 57, 5, 40, 11, 69, 86, 71, 75, 65, 21, 76, 78, 79, 14, 16, 45
    ),
    SCENE_CATEGORY_INIT("overworld",
            37, 33, 9, 30
    ),
    SCENE_CATEGORY_INIT("milk road",
            26, 96, 44, 3, 55, 56
    ),
    SCENE_CATEGORY_INIT("moon",
            93, 34, 53, 61, 92, 1
    ),
    SCENE_CATEGORY_INIT("other",
            91, 18, 17, 89, 95, 88, 10
    ),
#if Z2_VERSION==NZSJ10
    SCENE_CATEGORY_INIT("beta",
            8
    ),
#endif
};
