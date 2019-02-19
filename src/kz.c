#include "kz.h"
#include "z64.h"
#include "watches.h"
#include <stdlib.h>
#include <startup.h>

extern void game_update_start(z64_game_t *game);

__attribute__((section(".data")))
kz_ctxt_t kz = { 
    .ready = 0, 
};

static void kz_main(void) {
}

void init() {
    vector_init(&kz.watches, sizeof(watch_t));
    kz.ready = 1;
}

// Uses kz's stack instead of graph stack. 
static void kz_stack(void (*kzfunc)(void)) {
    static __attribute__((section(".stack"))) _Alignas(8)
    char stack[0x2000];
    __asm__ volatile(   "la     $t0, %1;"
                        "sw     $sp, -0x04($t0);"
                        "sw     $ra, -0x08($t0);"
                        "addiu  $sp, $t0, -0x08;"
                        "jalr   %0;"
                        "nop;"
                        "lw     $ra, 0($sp);"
                        "lw     $sp, 4($sp);"
                        ::
                        "r"(kzfunc),
                        "i"(&stack[sizeof(stack)]));
}

/* Entry Point of KZ executable */
ENTRY void _start() {
    init_gp();
    if(!kz.ready){
        kz_stack(init);
    }
    kz_stack(kz_main);
    game_update_start(&z64_game);
}

#include <startup.c>
#include <vector/vector.c>