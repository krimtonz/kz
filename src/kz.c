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

extern void game_update_start(z2_game_t *game);
typedef void (*game_update_start_t)(z2_game_t *game);

__attribute__((section(".data")))
kz_ctxt_t kz = { 
    .ready = 0, 
};

extern void *end;

static void kz_main(void) {
    gfx_begin();

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

    /* print watches */
    {
        for(int i=0;i<kz.watchescnt;i++){
            watch_t *watch = vector_at(&kz.watches,i);
            char buf[16];
            switch(watch->type){
                case WATCH_TYPE_U8:
                snprintf(buf,16,"%" PRIu8,*(uint8_t*)watch->address);
                break;
                case WATCH_TYPE_S8:
                snprintf(buf,16,"%" PRIi8,*(int8_t*)watch->address);
                break;
                case WATCH_TYPE_X8:
                snprintf(buf,16,"0x%" PRIx8,*(uint8_t*)watch->address);
                case WATCH_TYPE_U16:
                snprintf(buf,16,"%" PRIu16,*(uint16_t*)watch->address);
                break;
                case WATCH_TYPE_S16:
                snprintf(buf,16,"%" PRIi16,*(int16_t*)watch->address);
                break;
                case WATCH_TYPE_X16:
                snprintf(buf,16,"0x%" PRIx16,*(uint16_t*)watch->address);
                break;
                case WATCH_TYPE_U32:
                snprintf(buf,16,"%" PRIu32,*(uint32_t*)watch->address);
                break;
                case WATCH_TYPE_S32:
                snprintf(buf,16,"%" PRIi32,*(int32_t*)watch->address);
                break;
                case WATCH_TYPE_X32:
                snprintf(buf,16,"0x%" PRIx32,*(uint32_t*)watch->address);
                break;
                case WATCH_TYPE_FLOAT:
                snprintf(buf,16,"%f", *(float*)watch->address);
                break;
            }
            gfx_printf(watch->x,watch->y,"%s",buf);
            
        }
    }

    /* activate cheats */
    {
        if(kz.cheat_blast_mask)
            z2_link.blast_mask_timer = 0x00;
        if(kz.cheat_isg)
            z2_link.sword_active = 0x01;
        if(kz.cheat_infinite_arrows)
            z2_file.ammo[Z64_SLOT_BOW] = 0x10;
        if(kz.cheat_infinite_bombs)
            z2_file.ammo[Z64_SLOT_BOMB] = 0x10;
        if(kz.cheat_infinite_bombchu)
            z2_file.ammo[Z64_SLOT_BOMBCHU] = 0x10;
        if(kz.cheat_infinite_powder_keg)
            z2_file.ammo[Z64_SLOT_POWDER_KEG] = 0x01;
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

    gfx_finish();
}

void test(struct menu_item *item){
    z2_game.entrance_index = (uint16_t)((uint32_t)item->data);
    z2_game.scene_load_flag = 0x14;
}

void collison_show(struct menu_item *item){
    kz.col_enable = 1;
}

void collison_hide(struct menu_item *item){
    kz.col_enable = 0;
}

void collison_gen(struct menu_item *item){
    kz.col_gen = 1;
}

void collision_reduced(struct menu_item *item){
    kz.col_redux = !kz.col_redux;
}

void collision_opaque(struct menu_item *item){
    kz.col_opaque = !kz.col_opaque;
}

void init() {
    gfx_init();
    vector_init(&kz.watches, sizeof(watch_t));

    menu_init(&kz.main_menu);
    static struct menu warps;
    static struct menu collision;

    menu_init(&warps);
    menu_add_submenu(&kz.main_menu,&warps,"warps");
    menu_add_submenu(&kz.main_menu,&collision,"collision");

    menu_init(&collision);
    collision.selected_item = menu_add_button(&collision,"return",menu_return,NULL);
    menu_add_button(&collision, "show", collison_show, NULL);
    menu_add_button(&collision, "hide", collison_hide, NULL);
    menu_add_button(&collision, "gen", collison_gen, NULL);
    menu_add_button(&collision, "reduce", collision_reduced, NULL);
    menu_add_button(&collision, "opaque",collision_opaque, NULL);

    warps.selected_item = menu_add_button(&warps,"return",menu_return,NULL);
    for(int i=0;i<sizeof(scene_categories)/sizeof(struct kz_scene_category);i++){
        struct kz_scene_category cat = scene_categories[i];
        struct menu *cat_menu = malloc(sizeof(*cat_menu));
        menu_init(cat_menu);
        menu_add_button(cat_menu,"return",menu_return,NULL);
        for(int j = 0; j<cat.scene_cnt;j++){
            struct kz_scene scene = scenes[cat.scenes[j]];
            if(scene.entrance_cnt>1){
                struct menu *entrance_menu = malloc(sizeof(*entrance_menu));
                menu_init(entrance_menu);
                menu_add_button(entrance_menu,"return",menu_return,NULL);
                for(int k=0;k<scene.entrance_cnt;k++){
                    menu_add_button(entrance_menu,scene.entrances[k],test,(void*)(((scene.scene & 0xFF) << 8) | ((k & 0xFF) <<4)));
                }
                menu_add_submenu(cat_menu,entrance_menu,scene.scene_name);
                entrance_menu->selected_item = entrance_menu->items.first;
            }else{
                menu_add_button(cat_menu,scene.scene_name,test,(void*)(((scene.scene & 0xFF) << 8) | ((0 & 0xFF) << 4)));
            }
        }
        menu_add_submenu(&warps,cat_menu,cat.name);
        cat_menu->selected_item=cat_menu->items.first;
    }
    
    kz.main_menu.selected_item= kz.main_menu.items.first;
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
