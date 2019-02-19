#ifndef _Z64_H
#define _Z64_H

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
} z64_game_t;                               // 0x00A4

#define z64_game (*(z64_game_t*) 0x803E6B20)

#endif