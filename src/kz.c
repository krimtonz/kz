#include <stdio.h>
#include <stdlib.h>
#include <startup.h>
#include <inttypes.h>
#include "kz.h"
#include "gfx.h"
#include "watches.h"
#include "input.h"
#include "collision_view.h"
#include "scenes.h"
#include "commands.h"

extern void game_update_start(z2_game_t *game);
typedef void (*game_update_start_t)(z2_game_t *game);

__attribute__((section(".data")))
kz_ctxt_t kz = { 
    .ready = 0, 
};
static void kz_main(void) {
    gfx_begin();

    input_update();

    /* :relaxed: */
    {
        uint16_t pressed = get_pad_pressed();
        static uint16_t timer;
        if(pressed){
            static const uint16_t konami_code[] = {
                BUTTON_D_UP,
                BUTTON_D_UP,
                BUTTON_D_DOWN,
                BUTTON_D_DOWN,
                BUTTON_D_LEFT,
                BUTTON_D_RIGHT,
                BUTTON_D_LEFT,
                BUTTON_D_RIGHT,
                BUTTON_B,
                BUTTON_A,
                BUTTON_START,
            };
            static int kc_pos = 0;
            size_t kc_code_size = sizeof(konami_code)/sizeof(*konami_code);
            
            if(pressed == konami_code[kc_pos]){
                kc_pos++;
                if(kc_pos == kc_code_size){
                    kc_pos = 0;
                    timer = 0x100;
                }
            }else{
                kc_pos = 0;
            }
        }
        if(timer>0){
            timer--;
            gfx_printf_color(100,100,GPACK_RGBA8888(0xFF,0,0,0xFF),"Congratulations!");
            gfx_printf_color(35,108,GPACK_RGBA8888(0xFF,0,0,0xFF),"You have undermined Jimmie1717");
        }
    }

    /* input display */
    {
        z2_controller_t inp = z2_game.common.input[0].raw;
        gfx_printf(16,240-16,"%4i %4i",inp.x,inp.y);
        gfx_printf_color(106,240-16,GPACK_RGBA8888(0x00,0x00,0xFF,0xFF),"%s", inp.a?"A":" ");
        gfx_printf_color(116,240-16,GPACK_RGBA8888(0x00,0xFF,0x00,0xFF),"%s", inp.b?"B":" ");
        gfx_printf_color(126,240-16,GPACK_RGBA8888(0xFF,0x00,0x00,0xFF),"%s", inp.s?"S":" ");
        gfx_printf_color(136,240-16,GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF),"%s%s%s", inp.z?"Z":" ",inp.l?"L":" ",inp.r?"R":" ");
        gfx_printf_color(166,240-16,GPACK_RGBA8888(0xFF,0xFF,0x00,0xFF),"%s%s%s%s", inp.cl?"<":" ",inp.cu?"^":" ", inp.cr?">":" ",inp.cd?"v":" ");
        gfx_printf_color(206,240-16,GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF),"%s%s%s%s", inp.dl?"<":" ",inp.du?"^":" ", inp.dr?">":" ",inp.dd?"v":" ");
    }

    /* draw floating watches */
    {
        static char watch_buf[16];
        for(int i=0;i<kz.watch_cnt;i++){
            watch_t *watch = vector_at(&kz.watches,i);
            if(watch->floating){
                watch_printf(watch,watch_buf);
                gfx_printf(watch->x,watch->y,"%s",watch_buf);
            }
        }
    }

    /* activate cheats */
    {
        if(kz.cheat_blast_mask)
            z2_link.blast_mask_timer = 0x00;
        if(kz.cheat_isg)
            z2_link.sword_active = 0x01;
        if(kz.cheat_infinite_arrows)
            z2_file.ammo[Z2_SLOT_BOW] = 0x10;
        if(kz.cheat_infinite_bombs)
            z2_file.ammo[Z2_SLOT_BOMB] = 0x10;
        if(kz.cheat_infinite_bombchu)
            z2_file.ammo[Z2_SLOT_BOMBCHU] = 0x10;
        if(kz.cheat_infinite_powder_keg)
            z2_file.ammo[Z2_SLOT_POWDER_KEG] = 0x01;
        if(kz.cheat_infinite_health){
            z2_file.current_health = z2_file.max_health;
        }
        if(kz.cheat_infinite_magic){
            z2_file.current_magic = z2_file.magic_level * 0x30;
        }
    }

    z2_input_t input = z2_game.common.input[0];

    /* collision view */
    {
        kz_col_view();
    }   

    static _Bool skip_menu = 0;

    if((get_pad_pressed_unrestricted() & kz_commands[0].bind) == kz_commands[0].bind){
        skip_menu = 1;
        kz.menu_active = !kz.menu_active;
        if(kz.menu_active){
            reserve_buttons(BUTTON_L | BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP);
        }else{
            free_buttons(BUTTON_L | BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP);
        }
    }

    if(kz.menu_active && !skip_menu){
        struct menu *kz_menu = &kz.main_menu;
        enum menu_nav navdir = MENU_NAV_NONE;
        enum menu_callback callback = MENU_CALLBACK_NONE;
        if(input.pad_pressed & BUTTON_D_DOWN){
            navdir=MENU_NAV_DOWN;
        }else if(input.pad_pressed & BUTTON_D_UP){
            navdir=MENU_NAV_UP;
        }else if(input.pad_pressed & BUTTON_D_LEFT){
            navdir=MENU_NAV_LEFT;
        }else if(input.pad_pressed & BUTTON_D_RIGHT){
            navdir=MENU_NAV_RIGHT;
        }else if(input.pad_pressed & BUTTON_L){
            callback = MENU_CALLBACK_ACTIVATE;
        }

        menu_callback(kz_menu,callback);
        menu_navigate(kz_menu,navdir);
        menu_draw(kz_menu);
    }

    skip_menu = 0;

    for(int i=0;i<COMMAND_CNT;i++){
        _Bool activate = 0;
        switch(kz_commands[i].type){
            case COMMAND_HOLD:
                activate = (get_pad_held() & kz_commands[i].bind) == kz_commands[i].bind;
                break;
            case COMMAND_PRESS:
                activate = (get_pad_pressed() & kz_commands[i].bind) == kz_commands[i].bind;
                break;
        }
        if(activate && kz_commands[i].proc){
            kz_commands[i].proc();
        }
    }

    /* print logo */
#define MAKESTRING(S) MAKESTRING_(S)
#define MAKESTRING_(S) #S
    {
        static uint16_t logo_time = 0x100;
        if(logo_time>0){
            const char *name = MAKESTRING(PACKAGE);
            const char *url = MAKESTRING(URL);
            gfx_printf_color(10, Z2_SCREEN_HEIGHT - 20 - kfont->c_height, COLOR_GREEN, name);
            gfx_printf_color(Z2_SCREEN_WIDTH - 10 - (kfont->c_width * strlen(url)), Z2_SCREEN_HEIGHT - 20 - kfont->c_height,COLOR_GREEN,url);
            logo_time--;
        }
    }
#undef MAKESTRING_
#undef MAKESTRING

    gfx_finish();
}

void init() {
    clear_bss();
    do_global_ctors();
    gfx_init();
    vector_init(&kz.watches, sizeof(watch_t));
    vector_reserve(&kz.watches,10);
    kz.watch_cnt = 0;
    kz.menu_active = 0;
    menu_init(&kz.main_menu, 10, 10);
    kz.main_menu.selected_item = menu_add_button(&kz.main_menu,0,0,"return",menu_return,NULL);

    menu_add_submenu(&kz.main_menu,0,1,create_warps_menu(),"warps");
    menu_add_submenu(&kz.main_menu,0,2,create_scene_menu(),"scene");
    menu_add_submenu(&kz.main_menu,0,3,create_watches_menu(),"watches");
    menu_add_submenu(&kz.main_menu,0,4,create_inventory_menu(),"inventory");

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
ENTRY void _start(z2_game_t *game, game_update_start_t game_update_start) {
    init_gp();
    if(!kz.ready){
        kz_stack(init);
    }
    game_update_start(game);
    kz_stack(kz_main);
}

#include <startup.c>
#include <vector/vector.c>
#include <list/list.c>
