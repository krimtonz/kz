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
#include "settings.h"
#include "io.h"
#include "resource.h"

__attribute__((section(".data")))
kz_ctxt_t kz = { 
    .ready = 0, 
};

void save_disp_p(struct disp_p *disp_p){
    z2_gfx_t *gfx = z2_game.common.gfx;
    disp_p->work_p = gfx->work.p - gfx->work.buf;
    disp_p->work_d = gfx->work.d - gfx->work.buf;
    disp_p->poly_opa_p = gfx->poly_opa.p - gfx->poly_opa.buf;
    disp_p->poly_opa_d = gfx->poly_opa.d - gfx->poly_opa.buf;
    disp_p->poly_xlu_p = gfx->poly_xlu.p - gfx->poly_xlu.buf;
    disp_p->poly_xlu_d = gfx->poly_xlu.d - gfx->poly_xlu.buf;
    disp_p->overlay_p = gfx->overlay.p - gfx->overlay.buf;
    disp_p->overlay_d = gfx->overlay.d - gfx->overlay.buf;
}

void load_disp_p(struct disp_p *disp_p){
    z2_gfx_t *gfx = z2_game.common.gfx;
    gfx->work.p = disp_p->work_p + gfx->work.buf;
    gfx->work.d = disp_p->work_d + gfx->work.buf;
    gfx->poly_opa.p = disp_p->poly_opa_p + gfx->poly_opa.buf;
    gfx->poly_opa.d = disp_p->poly_opa_d + gfx->poly_opa.buf;
    gfx->poly_xlu.p = disp_p->poly_xlu_p + gfx->poly_xlu.buf;
    gfx->poly_xlu.d = disp_p->poly_xlu_d + gfx->poly_xlu.buf;
    gfx->overlay.p = disp_p->overlay_p + gfx->overlay.buf;
    gfx->overlay.d = disp_p->overlay_d + gfx->overlay.buf;
}

void gfx_reloc(int src){
    z2_gfx_t *gfx = z2_game.common.gfx;
    z2_disp_buf_t *new_disp[4] = {
        &gfx->work,
        &gfx->poly_opa,
        &gfx->poly_xlu,
        &gfx->overlay
    };
    uint32_t src_gfx = z2_disp_addr + src * Z2_DISP_SIZE;
    uint32_t dst_gfx = z2_disp_addr + (gfx->frame_cnt_1 & 1) * Z2_DISP_SIZE;
    for(int i=0;i<sizeof(new_disp)/sizeof(*new_disp);i++){
        z2_disp_buf_t *dbuf = new_disp[i];
        for(Gfx *p = dbuf->buf;p!=dbuf->p;p++){
            switch(p->hi >> 24){
                case G_VTX: break;
                case G_DMA_IO: break;
                case G_MTX: break;
                case G_MOVEWORD:
                switch((p->hi>>16) & 0xFF){
                    case G_MW_SEGMENT: break;
                    default: continue;
                }
                case G_MOVEMEM: break;
                case G_LOAD_UCODE: break;
                case G_DL: break;
                case G_RDPHALF_1: 
                switch(p[1].hi>>24){
                    case G_BRANCH_Z: break;
                    case G_LOAD_UCODE: break;
                    default: continue;
                }
                case G_SETTIMG: break;
                case G_SETZIMG: break;
                case G_SETCIMG: break;
                case G_BG_1CYC: break;
                case G_BG_COPY: break;
                default: continue;
            }
            if(p->lo >=src_gfx && p->lo<src_gfx + Z2_DISP_SIZE){
                p->lo += dst_gfx - src_gfx;
            }
        }
    }
}

static void cpu_counter(){
    static uint32_t count = 0;
    uint32_t new_count;
    __asm__ volatile("mfc0 $t0, $9;"
                     "nop;"
                     "sw $t0, %0" : "=m"(new_count) :: "t0");
    kz.cpu_cycle_counter += new_count - count;
    count = new_count;
}

static void kz_main(void) {
    cpu_counter();
    gfx_begin();

    input_update();

    save_disp_p(&kz.disp_p);

    /* input display */
    {
        int x = settings->id_x;
        int y = settings->id_y;
        z2_controller_t inp = z2_game.common.input[0].raw;
        gfx_printf(x,y,"%4i %4i",inp.x,inp.y);
        gfx_printf_color(x+90,y,GPACK_RGBA8888(0x00,0x00,0xFF,0xFF),"%s", inp.a?"A":" ");
        gfx_printf_color(x+100,y,GPACK_RGBA8888(0x00,0xFF,0x00,0xFF),"%s", inp.b?"B":" ");
        gfx_printf_color(x+110,y,GPACK_RGBA8888(0xFF,0x00,0x00,0xFF),"%s", inp.s?"S":" ");
        gfx_printf_color(x+120,y,GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF),"%s%s%s", inp.z?"Z":" ",inp.l?"L":" ",inp.r?"R":" ");
        gfx_printf_color(x+150,y,GPACK_RGBA8888(0xFF,0xFF,0x00,0xFF),"%s%s%s%s", inp.cl?"<":" ",inp.cu?"^":" ", inp.cr?">":" ",inp.cd?"v":" ");
        gfx_printf_color(x+190,y,GPACK_RGBA8888(0xFF,0xFF,0xFF,0xFF),"%s%s%s%s", inp.dl?"<":" ",inp.du?"^":" ", inp.dr?">":" ",inp.dd?"v":" ");
    }

    /* draw floating watches */
    {
        for(watch_t *watch=kz.watches.first;watch;watch = list_next(watch)){
            if(watch->floating){
                watch_printf(watch);
            }
        }
    }

    if(settings->lag_counter){
        int32_t lag_frames = z2_vi_counter + kz.frames_offset - kz.frames;
        gfx_printf(settings->lag_x,settings->lag_y,"%d",lag_frames);
    }

    kz.frames += z2_static_ctxt.update_rate;

    if(!kz.timer_running){
        kz.cpu_offset -= kz.cpu_cycle_counter - kz.cpu_prev;
    }
    kz.cpu_prev = kz.cpu_cycle_counter;
    if(settings->timer){
        int64_t count = kz.cpu_cycle_counter + kz.cpu_offset;
        int tenths = count / 4677750;
        int seconds = tenths / 10;
        int minutes = seconds / 60;
        int hours = minutes / 60;
        tenths %= 10;
        seconds %= 60;
        minutes %= 60;
        gfx_printf(settings->timer_x,settings->timer_y,"%d:%02d:%02d.%d",hours,minutes,seconds,tenths);
    }

    /* activate cheats */
    {
        if(kz.cheats & (1 << CHEAT_BLAST_MASK))
            z2_link.blast_mask_timer = 0x00;
        if(kz.cheats & (1 << CHEAT_ISG))
            z2_link.sword_active = 0x01;
        if(kz.cheats & (1 << CHEAT_ARROWS)){
            uint8_t arrow_cap[] = { 1, 30, 40, 50, 1, 20, 30, 40 };
            z2_file.ammo[Z2_SLOT_BOW] = arrow_cap[z2_file.quiver];
        }
        uint8_t bomb_cap[] = { 1, 20, 30, 40, 1, 1, 1, 1 };
        if(kz.cheats & (1 << CHEAT_BOMBS)){
            z2_file.ammo[Z2_SLOT_BOMB] = bomb_cap[z2_file.bomb_bag];
        }
        if(kz.cheats & (1 << CHEAT_BOMBCHUS)){
            z2_file.ammo[Z2_SLOT_BOMBCHU] = bomb_cap[z2_file.bomb_bag];
        }
        if(kz.cheats & (1 << CHEAT_POWDER_KEG)){
            z2_file.ammo[Z2_SLOT_POWDER_KEG] = 0x01;
        }
        if(kz.cheats & (1 << CHEAT_NUTS)){
            uint8_t nut_cap[] = { 1, 20, 30, 40, 1, 99, 1, 99};
            z2_file.ammo[Z2_SLOT_NUT] = nut_cap[z2_file.nut_upgrade];
        }
        if(kz.cheats & (1 << CHEAT_STICKS)){
            uint8_t stick_cap[] = { 1, 10, 20, 30, 1, 20, 30, 40};
            z2_file.ammo[Z2_SLOT_STICK] = stick_cap[z2_file.stick_upgade];
        }
        if(kz.cheats & (1 << CHEAT_HEALTH)){
            z2_file.current_health = z2_file.max_health;
        }
        if(kz.cheats & (1 << CHEAT_MAGIC)){
            z2_file.current_magic = z2_file.magic_level * 0x30;
        }
        if(kz.cheats & (1 << CHEAT_RUPEES)){
            uint16_t rupee_cap[] = { 99, 200, 500, 500 };
            z2_file.rupees = rupee_cap[z2_file.wallet_upgrade];
        }
        if(kz.cheats & (1 << CHEAT_TURBO)){
            z2_link.linear_velocity=18.0f;
            gfx_printf_color(Z2_SCREEN_WIDTH-60, Z2_SCREEN_HEIGHT-60,0x00FF00FF,"t");
        }
    }

    /* collision view */
    {
        kz_col_view();
    }   

    /* handle menu */
    {
        static _Bool skip_menu = 0;
        z2_input_t input = z2_game.common.input[0];
        if(input_bind_pressed(0)){
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

            menu_navigate(kz_menu,navdir);
            menu_callback(kz_menu,callback);
            menu_draw(kz_menu);
        }

        skip_menu = 0;
    }
    menu_update(&kz.main_menu);
    /* handle command bindings */
    {
        for(int i=0;i<Z2_CMD_MAX;i++){
            _Bool activate = 0;
            switch(kz_commands[i].type){
                case COMMAND_HOLD:
                    activate = input_bind_held(i);
                    break;
                case COMMAND_PRESS:
                    activate = input_bind_pressed(i);
                    break;
            }
            if(activate && kz_commands[i].proc){
                kz_commands[i].proc();
            }
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

int save_memfile(struct menu_item *item, void *data){
    if(!kz.memfile){
        kz.memfile=malloc(sizeof(*kz.memfile));
    }
    memcpy(&kz.memfile->file,&z2_file,sizeof(kz.memfile->file));
    memcpy(&kz.memfile->link_pos,&z2_link.common.pos_2,sizeof(kz.memfile->link_pos));
    return 1;
}

int load_memfile(struct menu_item *item, void *data){
    if(kz.memfile){
        memcpy(&z2_file,&kz.memfile->file,sizeof(kz.memfile->file));
        memcpy(&z2_link.common.pos_2,&kz.memfile->link_pos,sizeof(kz.memfile->link_pos));
    }
    return 1;
}

void init() {
    clear_bss();
    do_global_ctors();
    gfx_init();

    init_textures();

    kz.cpu_cycle_counter = 0;
    cpu_counter();
    kz.cpu_offset = -kz.cpu_cycle_counter;
    kz.cpu_prev = kz.cpu_cycle_counter;
    kz.timer_running = 0;

    kz.frames = 0;
    kz.frames_offset = -(int32_t)z2_vi_counter;
    kz.lag_counter = 1;

    kz.collision_view_status = COL_VIEW_NONE;

    list_init(&kz.watches,sizeof(watch_t));

    kz.pending_frames=-1;

    kz.settings_profile = 0;
    load_settings_from_flashram(kz.settings_profile);
    kz_apply_settings();

    kz.menu_active = 0;
    menu_init(&kz.main_menu, 10, 10);
    kz.main_menu.selected_item = menu_add_button(&kz.main_menu,0,0,"return",menu_return,NULL);

    menu_add_submenu(&kz.main_menu,0,1,create_warps_menu(),"warps");
    menu_add_submenu(&kz.main_menu,0,2,create_cheats_menu(),"cheats");
    menu_add_submenu(&kz.main_menu,0,3,create_scene_menu(),"scene");
    menu_add_submenu(&kz.main_menu,0,4,create_watches_menu(),"watches");
    menu_add_submenu(&kz.main_menu,0,5,create_inventory_menu(),"inventory");
    menu_add_submenu(&kz.main_menu,0,6,create_equips_menu(),"equips");
    menu_add_submenu(&kz.main_menu,0,7,create_file_menu(),"file");
    menu_add_submenu(&kz.main_menu,0,8,create_settings_menu(),"settings");

    kz.memfile=NULL;
    kz.ready = 1;
}

void game_state_main(z2_gamesate_update_t game_update_start){
    if(kz.pending_frames!=0){
        kz.pending_frames--;
        game_update_start(&z2_game);
    }else{
        z2_gfx_t *gfx = z2_game.common.gfx;
        if(z2_game.common.gamestate_frames!=0){
            if(gfx->frame_cnt_1 & 1){
                memcpy(((void*)z2_disp_addr + Z2_DISP_SIZE),(void*)z2_disp_addr,Z2_DISP_SIZE);
            }else{
                memcpy((void*)z2_disp_addr,(void*)(z2_disp_addr + Z2_DISP_SIZE),Z2_DISP_SIZE);
            }
            load_disp_p(&kz.disp_p);
            gfx_reloc(1-(gfx->frame_cnt_1 & 1));
        }
        z2_game.common.gamestate_frames--;
    }
}

// Uses kz's stack instead of graph stack. 
static void kz_stack(void (*kzfunc)(void)) {
    static _Alignas(8) __attribute__((section(".stack"))) 
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
ENTRY void _start(z2_game_t *game, z2_gamesate_update_t game_update_start) {
    init_gp();
    if(!kz.ready){
        kz_stack(init);
    }
    game_state_main(game_update_start);
    kz_stack(kz_main);
}

#include <startup.c>
#include <vector/vector.c>
#include <list/list.c>
