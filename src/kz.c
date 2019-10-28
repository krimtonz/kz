#include <stdio.h>
#include <stdlib.h>
#include <startup.h>
#include <inttypes.h>
#include <math.h>
#include "kz.h"
#include "gfx.h"
#include "watches.h"
#include "input.h"
#include "collision_view.h"
#include "scenes.h"
#include "commands.h"
#include "settings.h"
#include "resource.h"
#include "keyboard.h"

__attribute__((section(".data")))
kz_ctxt_t kz = {
    .ready = 0,
};

char restriction_table[0x23A];

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

void gfx_reloc(int src_disp_idx, int src_cimg_idx){
    z2_gfx_t *gfx = z2_game.common.gfx;
    uint32_t src_gfx = z2_disp_addr + src_disp_idx * Z2_DISP_SIZE;
    uint32_t dst_gfx = z2_disp_addr + (gfx->frame_cnt_1 & 1) * Z2_DISP_SIZE;
    uint32_t src_cimg = z2_cimg[src_cimg_idx];
    uint32_t dst_cimg = z2_cimg[gfx->frame_cnt_2 & 1];
    
    z2_disp_buf_t segment_setup;
    segment_setup.buf = (Gfx*)(dst_gfx + 0x140);
    segment_setup.p = segment_setup.buf + 21;

    z2_disp_buf_t primary;
    primary.buf = (Gfx*)(dst_gfx + 0x02A8);
    primary.p = primary.buf + 12;
    
    z2_disp_buf_t *new_disp[6] = {
        &gfx->work,
        &gfx->poly_opa,
        &gfx->poly_xlu,
        &gfx->overlay,
        &primary,
        &segment_setup,
    };
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
                case G_BG_1CYC: {
                    p->lo = 0;
                    p->hi = 0;
                    continue;
                }
                case G_BG_COPY: break;
                default: continue;
            }
            if(p->lo >=src_gfx && p->lo<src_gfx + Z2_DISP_SIZE){
                p->lo += dst_gfx - src_gfx;
            }
            if(p->lo >=src_cimg && p->lo<src_cimg + Z2_CIMG_SIZE){
                p->lo = dst_cimg;
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
    // Emergency settings reset, konami code
    {
        static uint16_t settings_reset[] = {
            BUTTON_D_UP, BUTTON_D_UP,
            BUTTON_D_DOWN, BUTTON_D_DOWN,
            BUTTON_D_LEFT, BUTTON_D_RIGHT,
            BUTTON_D_LEFT, BUTTON_D_RIGHT,
            BUTTON_B, BUTTON_A
        };
        static int reset_pos = 0;
        uint16_t pressed = input_pressed();
        if(pressed){
            if(pressed & settings_reset[reset_pos]) reset_pos++;
            else reset_pos = 0;
        }
        if(reset_pos == sizeof(settings_reset)/sizeof(*settings_reset)){
            kz_log("settings reset");
            load_default_settings();
            kz_apply_settings();
            reset_pos = 0;
        }
    }

    save_disp_p(&kz.disp_p);
    
    /* Disable DPad on Pause Screen */
    if(z2_player_ovl_table[0].ram!=NULL){
        *((uint32_t*)z2_player_ovl_table[0].ram + z2_dpad_disable_offset) = 0x00001025; // or v0, r0, r0
    }

    /* input display */
    {
        if(settings->input_display){
            int x = settings->id_x;
            int y = settings->id_y;
            gfx_printf(x,y,"%4i %4i",input_x(),input_y());
            gfx_texture *t_btn = resource_get(R_KZ_BUTTONS);
            static const int8_t btns[] = { 15, 14, 12, 13, 3, 2, 0, 1, 5, 4, 11, 10, 8, 9}; 
            uint16_t pad = input_pressed_raw();
            for(int i = 0;i<sizeof(btns)/sizeof(*btns);i++){
                int8_t btn = btns[i];
                if(!(pad & (1 << btn))) continue;
                gfx_draw_sprite_color(t_btn,x + 80 + (i*10), y, btn,8,8,button_colors[btn]);
            }
        }
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
        if(settings->cheats & (1 << CHEAT_BLAST_MASK))
            z2_link.blast_mask_timer = 0x00;
        if(settings->cheats & (1 << CHEAT_ISG))
            z2_link.sword_active = 0x01;
        if(settings->cheats & (1 << CHEAT_ARROWS)){
            uint8_t arrow_cap[] = { 1, 30, 40, 50, 1, 20, 30, 40 };
            z2_file.ammo[Z2_SLOT_BOW] = arrow_cap[z2_file.quiver];
        }
        uint8_t bomb_cap[] = { 1, 20, 30, 40, 1, 1, 1, 1 };
        if(settings->cheats & (1 << CHEAT_BOMBS)){
            z2_file.ammo[Z2_SLOT_BOMB] = bomb_cap[z2_file.bomb_bag];
        }
        if(settings->cheats & (1 << CHEAT_BOMBCHUS)){
            z2_file.ammo[Z2_SLOT_BOMBCHU] = bomb_cap[z2_file.bomb_bag];
        }
        if(settings->cheats & (1 << CHEAT_POWDER_KEG)){
            z2_file.ammo[Z2_SLOT_POWDER_KEG] = 0x01;
        }
        if(settings->cheats & (1 << CHEAT_NUTS)){
            uint8_t nut_cap[] = { 1, 20, 30, 40, 1, 99, 1, 99};
            z2_file.ammo[Z2_SLOT_NUT] = nut_cap[z2_file.nut_upgrade];
        }
        if(settings->cheats & (1 << CHEAT_STICKS)){
            uint8_t stick_cap[] = { 1, 10, 20, 30, 1, 20, 30, 40};
            z2_file.ammo[Z2_SLOT_STICK] = stick_cap[z2_file.stick_upgade];
        }
        if(settings->cheats & (1 << CHEAT_HEALTH)){
            z2_file.current_health = z2_file.max_health;
        }
        if(settings->cheats & (1 << CHEAT_MAGIC)){
            z2_file.current_magic = z2_file.magic_level * 0x30;
        }
        if(settings->cheats & (1 << CHEAT_RUPEES)){
            uint16_t rupee_cap[] = { 99, 200, 500, 500 };
            z2_file.rupees = rupee_cap[z2_file.wallet_upgrade];
        }
        if(settings->cheats & (1 << CHEAT_TURBO)){
            z2_link.linear_velocity=18.0f;
            gfx_printf_color(Z2_SCREEN_WIDTH-60, Z2_SCREEN_HEIGHT-60,0x00FF00FF,"t");
        }
        if(settings->cheats & (1 << CHEAT_RESTRICTION)){
            memset(&z2_game.hud_ctx.restriction_flags,0,0xC);
            for(int i=0;i<4;i++){
                for(int j=0;j<4;j++){
                    if(z2_file.form_button_item[i].button_item[j] == Z2_MASK_FIERCE_DEITY){
                        z2_file.restriction_flags[j] = 0;
                        //z2_game.hud_ctx.alphas[2 + j] = 0xFF;
                    }
                }
            }
        }
    }

    /* collision view */
    {
        if(z2_game.pause_ctx.state==0){
            kz_col_view();
        }
    }

    /* handle menu */
    {
        if(kz.menu_active){
            struct menu *kz_menu = &kz.main_menu;
            if(input_bind_pressed(Z2_CMD_TOGGLE_MENU)){
                kz.menu_active=0;
                if(!kz.debug_active){
                    free_buttons(BUTTON_L | BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP);
                }
            }else if(input_bind_pressed(Z2_CMD_RETURN)){
                menu_callback(kz_menu,MENU_CALLBACK_RETURN);
            }else{
                uint16_t pressed = input_pressed();
                if(pressed & BUTTON_D_DOWN){
                    menu_navigate(kz_menu,MENU_NAV_DOWN);
                }
                if(pressed & BUTTON_D_UP){
                    menu_navigate(kz_menu,MENU_NAV_UP);
                }
                if(pressed & BUTTON_D_LEFT){
                    menu_navigate(kz_menu,MENU_NAV_LEFT);
                }
                if(pressed & BUTTON_D_RIGHT){
                    menu_navigate(kz_menu,MENU_NAV_RIGHT);
                }
                if(pressed & BUTTON_L){
                    menu_callback(kz_menu,MENU_CALLBACK_ACTIVATE);
                }
            }
            menu_draw(kz_menu);
        }else if(input_bind_pressed(Z2_CMD_TOGGLE_MENU)){
            kz.menu_active=1;
            reserve_buttons(BUTTON_L | BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP);
        }
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

    /* print frame advance status */
    if(kz.pending_frames == 0){
        gfx_draw_sprite(resource_get(R_KZ_ICON),Z2_SCREEN_WIDTH-40,20,3,20,20);
    }else if(kz.pending_frames>0){
        gfx_draw_sprite(resource_get(R_KZ_ICON),Z2_SCREEN_WIDTH-40,20,4,20,20);
    }

    /* print logo */
#define MAKESTRING(S) MAKESTRING_(S)
#define MAKESTRING_(S) #S
    {
        static uint16_t logo_time = 0x100;
        if(logo_time>0){
            const char *name = MAKESTRING(PACKAGE);
            const char *url = MAKESTRING(URL);
            gfx_printf_color(10, Z2_SCREEN_HEIGHT - 32 - kfont->c_height, COLOR_GREEN, name);
            gfx_printf_color(Z2_SCREEN_WIDTH - 10 - (kfont->c_width * strlen(url)), Z2_SCREEN_HEIGHT - 32 - kfont->c_height,COLOR_GREEN,url);
            logo_time--;
        }

    }
#undef MAKESTRING_
#undef MAKESTRING

    if(kz.tooltip && kz.menu_active){
        gfx_printf(10, Z2_SCREEN_HEIGHT - 40,"%s",kz.tooltip);
    }

    for(int i = 9;i>=0;i--){
        const int fade_start = 20;
        const int fade_len = 20;
        struct log *log_entry = &kz.log[i];
        uint8_t alpha;
        if(!log_entry->mesg) continue;
        log_entry->time++;
        if(log_entry->time > (fade_start + fade_len)){
            free(log_entry->mesg);
            log_entry->mesg=NULL;
            continue;
        }
        else if(log_entry->time > fade_start){
            alpha = 0xFF - (log_entry->time - fade_start) * 0xFF / fade_len;
        }else{
            alpha = 0xFF;
        }
        int x = Z2_SCREEN_WIDTH - 10 - strlen(log_entry->mesg) * 8;
        int y = Z2_SCREEN_HEIGHT - 40;
        gfx_printf_color(x,y,GPACK_RGB24A8(0xFFFFFF,alpha),"%s",log_entry->mesg);
    }

    // Release Debug Menu Bindings
    if(z2_game.pause_ctx.state==0 && !kz.menu_active && kz.debug_active){
        free_buttons(BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP | BUTTON_L);
        kz.debug_active = 0;
    }
    
#ifdef LITE
    struct item_texture *textures = resource_get(R_Z2_ITEMS);
    for(int i=0;i<Z2_ITEM_END;i++){
        if(!textures[i].texture) continue;
        textures[i].last_access_counter++;
        if(textures[i].last_access_counter>=60){
            gfx_destroy_texture(textures[i].texture);
            textures[i].texture = NULL;
        }
    }
#endif

    gfx_finish();
}

void init() {
    clear_bss();
    do_global_ctors();
    gfx_init();

    init_resources();

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

    memset(&kz.log,0,sizeof(kz.log));

    kz.menu_active = 0;
    menu_init(&kz.main_menu, settings->menu_x, settings->menu_y);
    kz.main_menu.selected_item = menu_add_button(&kz.main_menu,0,0,"return",menu_return,NULL);

    menu_add_submenu(&kz.main_menu,0,1,create_warps_menu(),"warps");
    menu_add_submenu(&kz.main_menu,0,2,create_cheats_menu(),"cheats");
    menu_add_submenu(&kz.main_menu,0,3,create_scene_menu(),"scene");
    menu_add_submenu(&kz.main_menu,0,4,create_watches_menu(),"watches");
    menu_add_submenu(&kz.main_menu,0,5,create_inventory_menu(),"inventory");
    menu_add_submenu(&kz.main_menu,0,6,create_equips_menu(),"equips");
    menu_add_submenu(&kz.main_menu,0,7,create_file_menu(),"file");
    menu_add_submenu(&kz.main_menu,0,8,create_settings_menu(),"settings");

    init_kz_keyboard();

    kz.memfile = malloc(sizeof(*kz.memfile) * KZ_MEMFILE_MAX);
    memset(kz.memfile,0,sizeof(*kz.memfile) * KZ_MEMFILE_MAX);
    kz.memfile_slot = 0;

    kz.position_save = malloc(sizeof(*kz.position_save) * KZ_POSITION_MAX);
    memset(kz.position_save,0,sizeof(*kz.position_save) * KZ_POSITION_MAX);
    kz.pos_slot = 0;
    
    //memcpy(restriction_table,(void*)z2_restriction_table_addr,sizeof(restriction_table));

    kz.ready = 1;
}

void input_hook(void){
    if(kz.pending_frames!=0){
        void (*z2_input_update)(z2_game_t *game);
        z2_input_update = (void*)z2_input_update_addr;
        z2_input_update(&z2_game);
    }
}

void blur_hook(void){
    if(kz.pending_frames!=0){
        z2_blur(&z2_ctxt);
    }
}

void kz_log(const char *format, ...){
    struct log *log_entry = &kz.log[9];
    if(log_entry->mesg){
        free(log_entry->mesg);
    }
    for(int i=9;i>0;i--){
        kz.log[i] = kz.log[i-1];
    }
    va_list va;
    va_start(va,format);
    int l = vsnprintf(NULL,0,format,va);
    va_end(va);

    log_entry = &kz.log[0];
    log_entry->mesg = malloc(l + 1);
    if(!log_entry->mesg) return;
    va_start(va,format);
    vsprintf(log_entry->mesg,format,va);
    va_end(va);
    log_entry->time = 0;
}

void game_state_main(){
    if(kz.pending_frames!=0){
        if(kz.pending_frames>0){
            kz.pending_frames--;
        }
        z2_ctxt.gamestate_update(&z2_ctxt);
    }else{
        z2_gfx_t *gfx = z2_game.common.gfx;
        if(z2_ctxt.gamestate_frames!=0){
            if(gfx->frame_cnt_1 & 1){
                memcpy(((void*)z2_disp_addr + Z2_DISP_SIZE),(void*)z2_disp_addr,Z2_DISP_SIZE);
            }else{
                memcpy((void*)z2_disp_addr,(void*)(z2_disp_addr + Z2_DISP_SIZE),Z2_DISP_SIZE);
            }
            load_disp_p(&kz.disp_p);
            gfx_reloc(1-(gfx->frame_cnt_1 & 1),1-(gfx->frame_cnt_2 & 1));
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
ENTRY void _start() {
    init_gp();
    if(!kz.ready){
        kz_stack(init);
    }
    game_state_main();
    kz_stack(kz_main);
}

#include <startup.c>
#include <vector/vector.c>
#include <list/list.c>
