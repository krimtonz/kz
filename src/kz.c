#include <stdio.h>
#include <stdlib.h>
#include <startup.h>
#include <inttypes.h>
#include <math.h>
#include <libundermine/gfx.h>
#include <libundermine/input.h>
#include <libundermine/watches.h>
#include "kz.h"
#include "collision_view.h"
#include "scenes.h"
#include "commands.h"
#include "settings.h"
#include "kzresource.h"
#include "zu.h"
#include "state.h"

#ifdef WIIVC
#define CPU_COUNTER 46777500
#else
#define CPU_COUNTER 46875000
#endif

__attribute__((section(".data")))
kz_ctxt_t kz = {
    .ready = 0,
};

char restriction_table[0x23A];

static void cpu_counter(void){
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
            BUTTON_D_UP,    BUTTON_D_UP,
            BUTTON_D_DOWN,  BUTTON_D_DOWN,
            BUTTON_D_LEFT,  BUTTON_D_RIGHT,
            BUTTON_D_LEFT,  BUTTON_D_RIGHT,
            BUTTON_B,       BUTTON_A
        };
        static int reset_pos = 0;
        uint16_t pressed = input_pressed();
        if(pressed){
            if(pressed & settings_reset[reset_pos]){
                reset_pos++;
            }
            else{
                reset_pos = 0;
            }
        }
        if(reset_pos == sizeof(settings_reset) / sizeof(*settings_reset)){
            kz_log("settings reset");
            load_default_settings();
            kz_apply_settings();
            reset_pos = 0;
        }
    }

    zu_disp_ptr_save(&kz.disp_p);

    /* input display */
    {
        if(settings->input_display){
            int x = settings->id_x;
            int y = settings->id_y;
            gfx_printf(x, y, "%4i %4i", input_x(), input_y());
            gfx_texture *btn_tex = resource_get(R_KZ_BUTTONS);
            static const int8_t btns[] = { 15, 14, 12, 13, 3, 2, 0, 1, 5, 4, 11, 10, 8, 9};
            uint16_t pad = input_pressed_raw();
            for(int i = 0;i < sizeof(btns) / sizeof(*btns);i++){
                int8_t btn = btns[i];
                if(!(pad & (1 << btn))){
                    continue;
                }
                gfx_draw_sprite_color(btn_tex, x + 80 + (i*10), y, btn, 8, 8, button_colors[btn]);
            }
        }
    }

    /* draw floating watches */
    {
        for(watch_t *watch = kz.watches.first;watch;watch = list_next(watch)){
            if(watch->floating){
                watch_printf(watch, DEFAULT_COLOR);
            }
        }
    }

    if(settings->lag_counter){
        int32_t lag_frames = z2_vi_counter + kz.frames_offset - kz.frames;
        gfx_printf(settings->lag_x, settings->lag_y, "%d", lag_frames);
    }

    kz.frames += z2_file.static_ctx->update_rate;

    if(!kz.timer_running){
        kz.cpu_offset -= kz.cpu_cycle_counter - kz.cpu_prev;
    }
    kz.cpu_prev = kz.cpu_cycle_counter;
    if(settings->timer){
        int64_t count = kz.cpu_cycle_counter + kz.cpu_offset;
        int tenths = count / (CPU_COUNTER / 10);
        int seconds = tenths / 10;
        int minutes = seconds / 60;
        int hours = minutes / 60;
        tenths %= 10;
        seconds %= 60;
        minutes %= 60;
        gfx_printf(settings->timer_x, settings->timer_y, "%d:%02d:%02d.%d", hours, minutes, seconds, tenths);
    }

    /* activate cheats */
    {
        if(settings->cheats & (1 << CHEAT_BLAST_MASK))
            z2_link.blast_mask_timer = 0x00;
        if(settings->cheats & (1 << CHEAT_ISG))
            z2_link.sword_active = 0x01;
        if(settings->cheats & (1 << CHEAT_ARROWS)){
            static uint8_t arrow_cap[] = { 1, 30, 40, 50, 1, 20, 30, 40 };
            z2_file.ammo[Z2_SLOT_BOW] = arrow_cap[z2_file.quiver];
        }
        static uint8_t bomb_cap[] = { 1, 20, 30, 40, 1, 1, 1, 1 };
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
            static uint8_t nut_cap[] = { 1, 20, 30, 40, 1, 99, 1, 99 };
            z2_file.ammo[Z2_SLOT_NUT] = nut_cap[z2_file.nut_upgrade];
        }
        if(settings->cheats & (1 << CHEAT_STICKS)){
            static uint8_t stick_cap[] = { 1, 10, 20, 30, 1, 20, 30, 40 };
            z2_file.ammo[Z2_SLOT_STICK] = stick_cap[z2_file.stick_upgade];
        }
        if(settings->cheats & (1 << CHEAT_HEALTH)){
            z2_file.current_health = z2_file.max_health;
        }
        if(settings->cheats & (1 << CHEAT_MAGIC)){
            z2_file.current_magic = z2_file.magic_level * 0x30;
        }
        if(settings->cheats & (1 << CHEAT_RUPEES)){
            static uint16_t rupee_cap[] = { 99, 200, 500, 500 };
            z2_file.rupees = rupee_cap[z2_file.wallet_upgrade];
        }
        if(settings->cheats & (1 << CHEAT_TURBO)){
            z2_link.linear_velocity = 18.0f;
            gfx_printf_color(Z2_SCREEN_WIDTH - 60, Z2_SCREEN_HEIGHT - 60, COLOR_GREEN, "t");
        }
        if(settings->cheats & (1 << CHEAT_FREEZE_TIME) && z2_file.static_ctx){
            z2_file.timespeed = -z2_file.static_ctx->time_speed;
        }
        if(settings->cheats & (1 << CHEAT_RESTRICTION)){
            memset(&z2_game.hud_ctx.restriction_flags, 0, 0xC);
            for(int i = 0;i < 4;i++){
                for(int j = 0;j < 4;j++){
                    if(z2_file.form_button_item[i].button_item[j] == Z2_MASK_FIERCE_DEITY){
                        z2_file.restriction_flags[j] = 0;
                        //z2_game.hud_ctx.alphas[2 + j] = 0xFF;
                    }
                }
            }
        }
    }

    /* collision view / hitbox view */
    {
        kz_col_view();
        kz_hitbox_view();
    }

    /* handle menu */
    {
        void *event_data = NULL;
        if(kz.menu_active){
            menu_t *kz_menu = &kz.main_menu;
            if(input_bind_pressed_raw(KZ_CMD_TOGGLE_MENU)){
                kz.menu_active = 0;
                if(!kz.debug_active){
                    free_buttons(BUTTON_L | BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP);
                }
            }else if(input_bind_pressed_raw(KZ_CMD_RETURN)){
                menu_trigger_event(kz_menu, MENU_EVENT_RETURN, &event_data);
            }else{
                uint16_t pressed = input_pressed();
                if(pressed & BUTTON_D_DOWN){
                    menu_trigger_event(kz_menu, MENU_EVENT_NAVIGATE, (void*)MENU_NAV_DOWN);
                }
                if(pressed & BUTTON_D_UP){
                    menu_trigger_event(kz_menu, MENU_EVENT_NAVIGATE, (void*)MENU_NAV_UP);
                }
                if(pressed & BUTTON_D_LEFT){
                    menu_trigger_event(kz_menu, MENU_EVENT_NAVIGATE, (void*)MENU_NAV_LEFT);
                }
                if(pressed & BUTTON_D_RIGHT){
                    menu_trigger_event(kz_menu, MENU_EVENT_NAVIGATE, (void*)MENU_NAV_RIGHT);
                }
                if(pressed & BUTTON_L){
                    menu_trigger_event(kz_menu, MENU_EVENT_ACTIVATE, &event_data);
                }
            }
            menu_trigger_event(kz_menu, MENU_EVENT_UPDATE, &event_data);
            menu_draw(kz_menu);
        }else if(input_bind_pressed_raw(KZ_CMD_TOGGLE_MENU)){
            kz.menu_active = 1;
            reserve_buttons(BUTTON_L | BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP);
        }
    }
    /* handle command bindings */
    {
        for(int i = 0;i < KZ_CMD_MAX;i++){
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
        gfx_draw_sprite(resource_get(R_KZ_ICON), Z2_SCREEN_WIDTH - 40, 20, 3, 20, 20);
    }else if(kz.pending_frames>0){
        gfx_draw_sprite(resource_get(R_KZ_ICON), Z2_SCREEN_WIDTH - 40, 20, 4, 20, 20);
    }
    if(!kz.z2_input_enabled){
        gfx_printf_color(Z2_SCREEN_WIDTH - 68, Z2_SCREEN_HEIGHT - 60, COLOR_RED, "i");
    }

    /* print logo */
#define MAKESTRING(S) MAKESTRING_(S)
#define MAKESTRING_(S) #S
    {
        static uint16_t logo_time = 0x100;
        if(logo_time > 0){
            const char *name = MAKESTRING(PACKAGE);
            const char *url = MAKESTRING(URL);
            gfx_printf_color(10, Z2_SCREEN_HEIGHT - 32 - kfont->c_height, COLOR_GREEN, name);
            gfx_printf_color(Z2_SCREEN_WIDTH - 10 - (kfont->c_width * strlen(url)), Z2_SCREEN_HEIGHT - 32 - kfont->c_height, COLOR_GREEN, url);
            logo_time--;
        }

    }
#undef MAKESTRING_
#undef MAKESTRING

    if(kz.tooltip && kz.menu_active){
        gfx_printf(kz.main_menu.x_offset, Z2_SCREEN_HEIGHT - 40, "%s", kz.tooltip);
    }

    for(int i = KZ_LOG_MAX - 1;i >= 0;i--){
        const int fade_start = 20;
        const int fade_len = 20;
        struct log *log_entry = &kz.log[i];
        uint8_t alpha;
        if(!log_entry->mesg){
            continue;
        }
        log_entry->time++;
        if(log_entry->time > (fade_start + fade_len)){
            free(log_entry->mesg);
            log_entry->mesg = NULL;
            continue;
        }
        else if(log_entry->time > fade_start){
            alpha = 0xFF - (log_entry->time - fade_start) * 0xFF / fade_len;
        }else{
            alpha = 0xFF;
        }
        int x = Z2_SCREEN_WIDTH - 10 - strlen(log_entry->mesg) * 8;
        int y = Z2_SCREEN_HEIGHT - 40;
        gfx_printf_color(x, y - (i * 10), GPACK_RGB24A8(0xFFFFFF, alpha), "%s", log_entry->mesg);
    }

    // Release Debug Menu Bindings
    if(z2_game.pause_ctx.state == 0 && !kz.menu_active && kz.debug_active){
        free_buttons(BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP | BUTTON_L);
        kz.debug_active = 0;
    }

#ifdef LITE
    struct item_texture *textures = resource_get(R_Z2_ITEMS);
    for(int i = 0;i < Z2_ITEM_END;i++){
        if(!textures[i].texture || !textures[i].release){
            continue;
        }
        textures[i].last_access_counter++;
        if(textures[i].last_access_counter >= 60){
            gfx_destroy_texture(textures[i].texture);
            textures[i].texture = NULL;
        }
    }
#endif
gfx_printf(100,100,"%08x",sbrk(0));
    gfx_finish();
}

static int main_menu_return_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    free_buttons(BUTTON_L | BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP);
    kz.menu_active = 0;
    return 1;
}

static void init(void) {
    clear_bss();
    do_global_ctors();

    input_init(&z2_input_direct.raw.pad, &z2_input_direct.raw.x, &z2_input_direct.raw.y, settings->binds, KZ_CMD_MAX);
    bind_override(KZ_CMD_TOGGLE_MENU);
    bind_override(KZ_CMD_RETURN);

    gfx_init(0x32800, resource_get(R_KZ_FONT), &z2_ctxt.gfx->overlay.p);
    static_sprites_init();

    kz.cpu_cycle_counter = 0;
    cpu_counter();
    kz.cpu_offset = -kz.cpu_cycle_counter;
    kz.cpu_prev = kz.cpu_cycle_counter;
    kz.timer_running = 0;

    kz.frames = 0;
    kz.frames_offset = -(int32_t)z2_vi_counter;
    kz.lag_counter = 1;

    kz.collision_view_status = COL_VIEW_NONE;
    kz.hitbox_view_status = COL_VIEW_NONE;
    kz.hide_actors = 0;
    kz.hide_room = 0;

    list_init(&kz.watches, sizeof(watch_t));

    kz.pending_frames = -1;
    kz.z2_input_enabled = 1;
    kz.prev_timespeed = 0x80000000;

    kz.settings_profile = 0;
    load_settings_from_flashram(kz.settings_profile);
    kz_apply_settings();

    memset(&kz.log, 0, sizeof(kz.log));

    kz.menu_active = 0;
    menu_ctx_init(&kz.main_menu, &kz.tooltip);
    menu_init(&kz.main_menu, settings->menu_x, settings->menu_y);
    kz.main_menu.selected_item = menu_button_add(&kz.main_menu, 0, 0, "return", main_menu_return_onactivate, NULL);

    menu_submenu_add(&kz.main_menu, 0, 1, "warps", create_warps_menu());
    menu_submenu_add(&kz.main_menu, 0, 2, "cheats", create_cheats_menu());
    menu_submenu_add(&kz.main_menu, 0, 3, "scene", create_scene_menu());
    menu_submenu_add(&kz.main_menu, 0, 4, "watches", create_watches_menu());
    menu_submenu_add(&kz.main_menu, 0, 5, "inventory", create_inventory_menu());
    menu_submenu_add(&kz.main_menu, 0, 6, "equips", create_equips_menu());
    menu_submenu_add(&kz.main_menu, 0, 7, "file", create_file_menu());
    #ifndef LITE
    menu_submenu_add(&kz.main_menu, 0, 8, "debug", create_debug_menu());
    menu_submenu_add(&kz.main_menu, 0, 9, "settings", create_settings_menu());
    #else
    menu_submenu_add(&kz.main_menu, 0, 8, "settings", create_settings_menu());
    #endif

    kz.memfile = malloc(sizeof(*kz.memfile) * KZ_MEMFILE_MAX);
    memset(kz.memfile, 0, sizeof(*kz.memfile) * KZ_MEMFILE_MAX);
    kz.memfile_slot = 0;

    kz.position_save = malloc(sizeof(*kz.position_save) * KZ_POSITION_MAX);
    memset(kz.position_save, 0, sizeof(*kz.position_save) * KZ_POSITION_MAX);
    kz.pos_slot = 0;

    memcpy(restriction_table, &z2_restriction_table, sizeof(z2_restriction_table));

    kz.ready = 1;
}

void kz_log(const char *format, ...){
    struct log *log_entry = &kz.log[KZ_LOG_MAX-1];
    if(log_entry->mesg){
        free(log_entry->mesg);
    }
    for(int i = KZ_LOG_MAX - 1;i > 0;i--){
        kz.log[i] = kz.log[i - 1];
    }
    va_list va;
    va_start(va, format);
    int l = vsnprintf(NULL, 0, format, va);
    va_end(va);

    log_entry = &kz.log[0];
    log_entry->mesg = malloc(l + 1);
    if(!log_entry->mesg){
        return;
    }
    va_start(va, format);
    vsprintf(log_entry->mesg, format, va);
    va_end(va);
    log_entry->time = 0;
}

static void game_state_main(void){
    if(kz.pending_frames != 0){
        if(kz.pending_frames > 0){
            kz.pending_frames--;
        }
        z2_ctxt.gamestate_update(&z2_ctxt);
    }else{
        z2_gfx_t *gfx = z2_game.common.gfx;
        if(z2_ctxt.gamestate_frames != 0){
            if(gfx->frame_cnt_1 & 1){
                memcpy(&z2_disp[Z2_DISP_SIZE], z2_disp, Z2_DISP_SIZE);
            }else{
                memcpy(z2_disp, &z2_disp[Z2_DISP_SIZE], Z2_DISP_SIZE);
            }
            zu_disp_ptr_load(&kz.disp_p);
            zu_gfx_reloc(1 - (gfx->frame_cnt_1 & 1), 1 - (gfx->frame_cnt_2 & 1));
        }
        z2_game.common.gamestate_frames--;
    }
}

HOOK void input_hook(void){
    if(kz.pending_frames != 0 && kz.z2_input_enabled){
        z2_input_update(&z2_ctxt);
        if(!kz.debug_active){
            z2_input_t *input = &z2_ctxt.input[0];
            uint16_t mask = ~(BUTTON_D_DOWN | BUTTON_D_UP | BUTTON_D_RIGHT | BUTTON_D_LEFT);
            input->raw.pad &=  mask;
            input->pad_pressed &= mask;
            input->pad_released &= mask;
        }
    }
}

HOOK void motion_blur_hook(void){
    if(kz.pending_frames != 0){
        z2_MotionBlur(&z2_ctxt);
    }
}

HOOK void draw_actors_hook(void){
    if(kz.hide_actors){
        zu_disp_ptr_t disp_p;
        zu_disp_ptr_save(&disp_p);
        z2_DrawActors(&z2_game, &z2_game.actor_ctxt);
        zu_disp_ptr_load(&disp_p);
    }else{
        z2_DrawActors(&z2_game, &z2_game.actor_ctxt);
    }
}

HOOK void draw_room_hook(z2_game_t *game, z2_room_t *room, int a2){
    if(kz.hide_room){
        zu_disp_ptr_t disp_p;
        zu_disp_ptr_save(&disp_p);
        z2_DrawRoom(game, room, a2);
        zu_disp_ptr_load(&disp_p);
    }else{
        z2_DrawRoom(game, room, a2);
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
                        "jalr   %0;\n"
                        "lw     $ra, 0($sp);"
                        "lw     $sp, 4($sp);"
                        ::
                        "r"(kzfunc),
                        "i"(&stack[sizeof(stack)])
                        :
                        "$v0", "$v1",
                        "$a0", "$a1", "$a2", "$a3",
                        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9");
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
