#include "kz.h"
#include "z64.h"
#include "watches.h"
#include <stdlib.h>
#include <startup.h>

__attribute__((section(".data")))
kz_ctxt_t kz = { 
    .ready = 0, 
};

static void kz_main(void) {
    watch_t *watch = vector_at(&kz.watches, 0);
    watch->address++;
}

void init() {
    vector_init(&kz.watches, sizeof(watch_t));
    watch_t watch;
    watch.address= (void*)0x80000000;
    watch.type=WATCH_TYPE_X32;
    watch.x = 10;
    watch.y = 10;
    vector_insert(&kz.watches, 0, 1, &watch);
    kz.ptr = malloc(8*1024*1024);
    kz.ready = 1;
}


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

extern void game_update_start(z64_game_t *game);

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