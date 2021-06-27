#include <stdio.h>
#include <stdlib.h>
#include <startup.h>
#include <inttypes.h>
#include <math.h>
#include "kz.h"
#include "collision_view.h"
#include "scenes.h"
#include "commands.h"
#include "settings.h"
#include "kzresource.h"
#include "zu.h"
#include "gfx.h"
#include "start.h"
#include "input.h"
#include "camera.h"
#include "hb_heap.h"
#include "sys.h"
#include "state.h"
#include "mem.h"
#include "printf.h"
#include "vc.h"
#include "inventory_map.h"
#include "cache.h"

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
static int8_t item_val;
static struct item_map_row *pause_row;

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
        if(pressed) {
            if(pressed & settings_reset[reset_pos]) {
                reset_pos++;
            } else {
                reset_pos = 0;
            }
        }

        if(reset_pos == sizeof(settings_reset) / sizeof(*settings_reset)) {
            load_default_settings();
            kz_apply_settings();
            kz.main_menu.x_offset = settings->menu_x;
            kz.main_menu.y_offset = settings->menu_y;
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
            gfx_texture *btn_tex = resource_get(resource_handles[R_KZ_BUTTONS]);
            static const int8_t btns[] = { 15, 14, 12, 13, 3, 2, 0, 1, 5, 4, 11, 10, 8, 9};
            uint16_t pad = input_pressed_raw();
            for(int i = 0; i < sizeof(btns) / sizeof(*btns); i++) {
                int8_t btn = btns[i];
                if(!(pad & (1 << btn))) {
                    continue;
                }

                gfx_draw_sprite_color(btn_tex, x + 80 + (i*10), y, btn, 8, 8, button_colors[btn]);
            }
        }
    }

    /* draw floating watches */
    {
        for(watch_t *watch = kz.watches.first; watch != NULL; watch = list_next(watch)) {
            if(watch->floating) {
                watch_printf(watch, DEFAULT_COLOR);
            }
        }
    }

    if(settings->lag_counter) {
        int32_t lag_frames = z2_vi_counter + kz.frames_offset - kz.frames;
        gfx_printf(settings->lag_x, settings->lag_y, "%d", lag_frames);
    }

    kz.frames += z2_file.static_ctx->update_rate;

    if(!kz.timer_running) {
        kz.cpu_offset -= kz.cpu_cycle_counter - kz.cpu_prev;
    }

    kz.cpu_prev = kz.cpu_cycle_counter;
    if(settings->timer) {
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
        if(settings->cheats & (1 << CHEAT_BLAST_MASK)) {
            z2_link.blast_mask_timer = 0x00;
        }

        if(settings->cheats & (1 << CHEAT_ISG)) {
            z2_link.sword_active = 0x01;
        }

        if(settings->cheats & (1 << CHEAT_ARROWS)) {
            static uint8_t arrow_cap[] = { 1, 30, 40, 50, 1, 20, 30, 40 };
            z2_file.ammo[Z2_SLOT_BOW] = arrow_cap[z2_file.quiver];
        }

        static uint8_t bomb_cap[] = { 1, 20, 30, 40, 1, 1, 1, 1 };
        if(settings->cheats & (1 << CHEAT_BOMBS)) {
            z2_file.ammo[Z2_SLOT_BOMB] = bomb_cap[z2_file.bomb_bag];
        }

        if(settings->cheats & (1 << CHEAT_BOMBCHUS)) {
            z2_file.ammo[Z2_SLOT_BOMBCHU] = bomb_cap[z2_file.bomb_bag];
        }

        if(settings->cheats & (1 << CHEAT_POWDER_KEG)) {
            z2_file.ammo[Z2_SLOT_POWDER_KEG] = 0x01;
        }

        if(settings->cheats & (1 << CHEAT_NUTS)) {
            static uint8_t nut_cap[] = { 1, 20, 30, 40, 1, 99, 1, 99 };
            z2_file.ammo[Z2_SLOT_NUT] = nut_cap[z2_file.nut_upgrade];
        }

        if(settings->cheats & (1 << CHEAT_STICKS)) {
            static uint8_t stick_cap[] = { 1, 10, 20, 30, 1, 20, 30, 40 };
            z2_file.ammo[Z2_SLOT_STICK] = stick_cap[z2_file.stick_upgade];
        }

        if(settings->cheats & (1 << CHEAT_HEALTH)) {
            z2_file.current_health = z2_file.max_health;
        }

        if(settings->cheats & (1 << CHEAT_MAGIC)) {
            z2_file.current_magic = z2_file.magic_level * 0x30;
        }

        if(settings->cheats & (1 << CHEAT_RUPEES)) {
            static uint16_t rupee_cap[] = { 99, 200, 500, 500 };
            z2_file.rupees = rupee_cap[z2_file.wallet_upgrade];
        }

        if(settings->cheats & (1 << CHEAT_TURBO)) {
            z2_link.linear_velocity = 18.0f;
            gfx_printf_color(Z2_SCREEN_WIDTH - 60, Z2_SCREEN_HEIGHT - 60, COLOR_GREEN, "t");
        }

        if(settings->cheats & (1 << CHEAT_FREEZE_TIME) && z2_file.static_ctx) {
            z2_file.timespeed = -z2_file.static_ctx->time_speed;
        }

        if(settings->cheats & (1 << CHEAT_RESTRICTION)) {
            memset(&z2_game.hud_ctx.restriction_flags, 0, 0xC);
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 4; j++) {
                    if(z2_file.form_button_item[i].button_item[j] == Z2_MASK_FIERCE_DEITY) {
                        z2_file.restriction_flags[j] = 0;
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
        menu_t *kz_menu = &kz.main_menu;
        if(kz.menu_active) {
            if(input_bind_pressed_raw(KZ_CMD_TOGGLE_MENU)) {
                kz.menu_active = 0;
                free_buttons(BUTTON_L | BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP);
            } else if(input_bind_pressed_raw(KZ_CMD_RETURN)) {
                menu_trigger_event(kz_menu, MENU_EVENT_RETURN, &event_data);
            } else {
                menu_input(kz_menu, &event_data);
            }
            menu_trigger_event(kz_menu, MENU_EVENT_UPDATE, &event_data);
            menu_draw(kz_menu);
        } else if(input_bind_pressed_raw(KZ_CMD_TOGGLE_MENU)) {
            kz.menu_active = 1;
            reserve_buttons(BUTTON_L | BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP);
        }
    }
    /* handle command bindings */
    {
        z2_pause_ctxt_t *p_ctx = &z2_game.pause_ctx;
        _Bool is_pause = z2_player_ovl_cur == &z2_player_ovl_table[0] && p_ctx->state == 6;
        static _Bool item_update = 0;
        static _Bool switched = 0;
        static _Bool entered_pause = 0;
        static uint32_t prev_inst[4];
        if(is_pause && !kz.menu_active) {
            if(button_time(button_get_index(BUTTON_L)) >= INPUT_REPEAT && !switched) {
                switched = 1;
                item_update = !item_update;
                if(item_update) {
                    if(p_ctx->screen_idx == 0 || p_ctx->screen_idx == 3) {
                        int8_t *slot = NULL;
                        int16_t *offset = NULL;
                        uint16_t *cell = NULL;
                        uint16_t *x = NULL;
                        uint16_t *y = NULL;
                        _Bool has_item = 0;
                        if(p_ctx->screen_idx == Z2_PAUSE_ITEM) {
                            slot = z2_file.items;
                            offset = &p_ctx->item_cell_offset;
                            cell = &p_ctx->item_cell;
                            x = &p_ctx->item_x;
                            y = &p_ctx->item_y;
                        } else {
                            slot = z2_file.masks;
                            offset = &p_ctx->mask_cell_offset;
                            cell = &p_ctx->mask_cell;
                            x = &p_ctx->mask_x;
                            y = &p_ctx->mask_y;
                        }

                        for(int i = 0; i < 24; i++) {
                            if(slot[i] != Z2_ITEM_NULL) {
                                has_item = 1;
                                break;
                            }
                        }

                        if(!has_item) {
                            *offset = 0;
                            *cell = 0;
                            *x = 0;
                            *y = 0;
                        }
                    }
                    reserve_buttons(BUTTON_L | BUTTON_D_UP | BUTTON_D_DOWN);
                    prev_inst[0] = *(uint32_t*)(z2_player_ovl_cur->ram + mask_right_offset);
                    prev_inst[1] = *(uint32_t*)(z2_player_ovl_cur->ram + mask_left_offset);
                    prev_inst[2] = *(uint32_t*)(z2_player_ovl_cur->ram + item_right_offset);
                    prev_inst[3] = *(uint32_t*)(z2_player_ovl_cur->ram + item_left_offset);
                    *(uint32_t*)(z2_player_ovl_cur->ram + mask_right_offset) = 0xA600023E;
                    *(uint32_t*)(z2_player_ovl_cur->ram + mask_left_offset) = 0xA600023E;
                    *(uint32_t*)(z2_player_ovl_cur->ram + item_right_offset) = 0xA6000238;
                    *(uint32_t*)(z2_player_ovl_cur->ram + item_left_offset) = 0xA6000238;
                    osInvalICache(z2_player_ovl_cur->ram + mask_right_offset, 4);
                    osInvalICache(z2_player_ovl_cur->ram + mask_left_offset, 4);
                    osInvalICache(z2_player_ovl_cur->ram + item_right_offset, 4);
                    osInvalICache(z2_player_ovl_cur->ram + item_left_offset, 4);
                    kz_log("hold L to stop inv edit");
                } else {
                    free_buttons(BUTTON_L | BUTTON_D_UP | BUTTON_D_DOWN);
                    input_mask_clear(0x0000, 0xFF, 0xFF);
                    *(uint32_t*)(z2_player_ovl_cur->ram + mask_right_offset) = prev_inst[0];
                    *(uint32_t*)(z2_player_ovl_cur->ram + mask_left_offset) = prev_inst[1];
                    *(uint32_t*)(z2_player_ovl_cur->ram + item_right_offset) = prev_inst[2];
                    *(uint32_t*)(z2_player_ovl_cur->ram + item_left_offset) = prev_inst[3];
                    osInvalICache(z2_player_ovl_cur->ram + mask_right_offset, 4);
                    osInvalICache(z2_player_ovl_cur->ram + mask_left_offset, 4);
                    osInvalICache(z2_player_ovl_cur->ram + item_right_offset, 4);
                    osInvalICache(z2_player_ovl_cur->ram + item_left_offset, 4);
                    menu_item_list_active_set(kz.pause_item_list, 0);
                    kz_log("hold L to start inv edit");
                }
            }
        } else {
            if(item_update) {
                item_update = 0;
                switched = 0;
                if(!kz.menu_active) {
                    free_buttons(BUTTON_L | BUTTON_D_UP | BUTTON_D_DOWN);
                }
            }
        }
        if(is_pause && !entered_pause) {
            entered_pause = 1;
            kz_log("hold L to start inv edit");
        } else if(!is_pause && entered_pause) {
            entered_pause = 0;
        }

        if(!item_update) {
            for(int i = 0;i < KZ_CMD_MAX;i++) {
                _Bool activate = 0;
                switch(kz_commands[i].type) {
                    case COMMAND_HOLD:
                        activate = input_bind_held(i);
                        break;
                    case COMMAND_PRESS:
                        activate = input_bind_pressed(i);
                        break;
                }
                if(activate && kz_commands[i].proc) {
                    kz_commands[i].proc();
                }
            }
        } else {
            gfx_printf_color(280, 200, GPACK_RGBA8888(0xFF, 0x00, 0x00, 0xFF), "i");
            static int prev_page = 0;
            if(p_ctx->screen_idx != prev_page) {
                if(kz.pause_item_list != NULL) {
                    if(menu_item_list_active_get(kz.pause_item_list) == 1) {
                        input_mask_clear(0x0000, 0xFF, 0xFF);
                        menu_item_list_active_set(kz.pause_item_list, 0);
                    }
                }
                prev_page = p_ctx->screen_idx;
            }
            switch(p_ctx->screen_idx) {
                case Z2_PAUSE_ITEM:
                case Z2_PAUSE_MASK:
                    if(!switched) {
                        void *event_data;
                        menu_input(&kz.pause_menu, &event_data);
                        menu_trigger_event(&kz.pause_menu, MENU_EVENT_UPDATE, NULL);
                        menu_draw(&kz.pause_menu);
                    }
                    break;
                case Z2_PAUSE_QUEST:
                    if(!switched && input_pressed() & BUTTON_L) {
                        switch(p_ctx->quest_cell) {
                            case Z2_QUEST_ODOWLA:
                                z2_file.odolwas_remains = !z2_file.odolwas_remains;
                                break;
                            case Z2_QUEST_GOHT:
                                z2_file.gohts_remains = !z2_file.gohts_remains;
                                break;
                            case Z2_QUEST_GYORG:
                                z2_file.gyorgs_remains = !z2_file.gyorgs_remains;
                                break;
                            case Z2_QUEST_TWINMOLD:
                                z2_file.twinmolds_remains = !z2_file.twinmolds_remains;
                                break;
                            case Z2_QUEST_SHIELD:
                                z2_file.shield++;
                                z2_file.shield %= 3;
                                break;
                            case Z2_QUEST_SWORD:
                                z2_file.sword++;
                                z2_file.sword %= 4;
                                if(z2_file.sword == 0) {
                                    z2_file.form_button_item[0].b = Z2_ITEM_NULL;
                                } else {
                                    z2_file.form_button_item[0].b = Z2_ITEM_KOKIRI_SWORD + z2_file.sword - 1;
                                }
                                z2_btnupdate(&z2_game, 0);
                                break;
                            case Z2_QUEST_SONATA:
                                z2_file.sonata_of_awakening = !z2_file.sonata_of_awakening;
                                break;
                            case Z2_QUEST_LULLABY:
                                z2_file.goron_lullaby = !z2_file.goron_lullaby;
                                break;
                            case Z2_QUEST_NWBN:
                                z2_file.new_wave_bossa_nova = !z2_file.new_wave_bossa_nova;
                                break;
                            case Z2_QUEST_ELEGY:
                                z2_file.elegy_of_emptiness = !z2_file.elegy_of_emptiness;
                                break;
                            case Z2_QUEST_OATH:
                                z2_file.oath_to_order = !z2_file.oath_to_order;
                                break;
                            case Z2_QUEST_SOT:
                                z2_file.song_of_time = !z2_file.song_of_time;
                                break;
                            case Z2_QUEST_HEALTING:
                                z2_file.song_of_healing = !z2_file.song_of_healing;
                                break;
                            case Z2_QUEST_EPONA:
                                z2_file.eponas_song = !z2_file.eponas_song;
                                break;
                            case Z2_QUEST_SOARING:
                                z2_file.song_of_soaring = !z2_file.song_of_soaring;
                                break;
                            case Z2_QUEST_STORMS:
                                z2_file.song_of_storms = !z2_file.song_of_storms;
                                break;
                            case Z2_QUEST_BOMBERS:
                                z2_file.bombers_notebook = !z2_file.bombers_notebook;
                                break;
                            case Z2_QUEST_QUIVER:
                                z2_file.quiver++;
                                z2_file.quiver %= 4;
                                break;
                            case Z2_QUEST_BOMB:
                                z2_file.bomb_bag++;
                                z2_file.bomb_bag %= 4;
                                break;
                            case Z2_QUEST_HEART:
                                z2_file.heart_piece++;
                                z2_file.heart_piece %= 4;
                                break;
                        }
                    }
                    break;
            }
        }

        if(switched && !(input_pressed() & BUTTON_L)) {
            switched = 0;
        }
    }

    /* print frame advance status */
    if(kz.pending_frames == 0) {
        gfx_draw_sprite(resource_get(R_Z2_ICON), Z2_SCREEN_WIDTH - 40, 20, 3, 20, 20);
    } else if(kz.pending_frames>0) {
        gfx_draw_sprite(resource_get(R_Z2_ICON), Z2_SCREEN_WIDTH - 40, 20, 4, 20, 20);
    }

    /* print logo */
#define MAKESTRING(S) MAKESTRING_(S)
#define MAKESTRING_(S) #S
    {
        static uint16_t logo_time = 0x100;
        if(logo_time > 0){
            const char *name = MAKESTRING(PACKAGE);
            const char *url = MAKESTRING(PACKAGE_URL);
            const char *version = MAKESTRING(PACKAGE_VERSION);

            gfx_printf_color(10, Z2_SCREEN_HEIGHT - 32 - kfont->c_height, COLOR_GREEN, name);
            gfx_printf_color(Z2_SCREEN_WIDTH - 10 - (kfont->c_width * strlen(url)), Z2_SCREEN_HEIGHT - 32 - kfont->c_height, COLOR_GREEN, url);
            gfx_printf_color(10, Z2_SCREEN_HEIGHT - 20 - kfont->c_height, COLOR_GREEN, version);

            gfx_draw_sprite(resource_get(R_KZ_KZFLEX), Z2_SCREEN_WIDTH - 61, Z2_SCREEN_HEIGHT - 106, 0, 51, 32);
            gfx_draw_sprite(resource_get(R_KZ_KZFLEX), Z2_SCREEN_WIDTH - 61, Z2_SCREEN_HEIGHT - 74, 1, 51, 32);

            logo_time--;
        }

    }
#undef MAKESTRING_
#undef MAKESTRING

    if(kz.tooltip != NULL && kz.menu_active) {
        gfx_printf(kz.main_menu.x_offset, Z2_SCREEN_HEIGHT - 40, "%s", kz.tooltip);
    }

    for(int i = KZ_LOG_MAX - 1; i >= 0; i--) {
        const int fade_start = 20;
        const int fade_len = 20;
        struct log *log_entry = &kz.log[i];
        uint8_t alpha;
        
        if(!log_entry->active){
            continue;
        }

        log_entry->time++;
        
        if(log_entry->time > (fade_start + fade_len)) {
            log_entry->active = 0;
            continue;
        } else if(log_entry->time > fade_start) {
            alpha = 0xFF - (log_entry->time - fade_start) * 0xFF / fade_len;
        } else {
            alpha = 0xFF;
        }

        int x = Z2_SCREEN_WIDTH - 30 - (strlen(log_entry->mesg) * 8);
        int y = Z2_SCREEN_HEIGHT - 40;
        gfx_printf_color(x, y - (i * 10), GPACK_RGB24A8(0xFFFFFF, alpha), "%s", log_entry->mesg);
    }

#ifdef LITE
    struct item_texture *textures = resource_get(resource_handles[R_Z2_ITEMS]);
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

    /*
    if(kz.draw_camera) {
        camera_draw();
    }
    */

    gfx_finish();
}

static int main_menu_return_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    free_buttons(BUTTON_L | BUTTON_D_DOWN | BUTTON_D_LEFT | BUTTON_D_RIGHT | BUTTON_D_UP);
    kz.menu_active = 0;
    return 1;
}

static int pause_menu_event(event_handler_t *handler, menu_event_t event, void **event_data) {
    menu_item_t *item = handler->subscriber;
    z2_pause_ctxt_t *pause_ctx = &z2_game.pause_ctx;
    if(pause_ctx->screen_idx != 0 && pause_ctx->screen_idx != 3) {
        menu_item_list_active_set(item, 0);
        return 0;
    }

    if(pause_ctx->scroll_button != 0) {
        menu_item_list_active_set(handler->subscriber, 0);
        return 0;
    }

    int active = (int)*event_data;
    if(active) {
        input_mask_set(0x0000, 0xFF, 0xFF);
        if(pause_ctx->screen_idx == 0) {
            pause_row = &item_map_table[pause_ctx->item_cell];
        } else {
            pause_row = &mask_map_table[pause_ctx->mask_cell];
        }
        
        if(*pause_row->slot == Z2_ITEM_NULL) {
            item_val = pause_row->item;
        } else {
            item_val = *pause_row->slot;
        }
        menu_item_list_set(handler->subscriber);
    } else {
        *pause_row->slot = item_val;
        input_mask_clear(0x0000, 0xFF, 0xFF);
    }
    return 1;
}

static void init(void) {
    clear_bss();
    do_global_ctors();
    kz.cpu_cycle_counter = 0;
    cpu_counter();

    // force a word and byte load/store in hb_heap memory.
#ifdef WIIVC
    char *vc_hack = halloc(0x10);
    hmemcpy(vc_hack, vc_hack, 5);
    hfree(vc_hack);
#endif

    kz.cpu_offset = -kz.cpu_cycle_counter;
    kz.cpu_prev = kz.cpu_cycle_counter;
    kz.timer_running = 0;

    kz.frames = 0;
    kz.frames_offset = -(int32_t)z2_vi_counter;
    kz.lag_counter = 1;

    kz.collision_view_status = COL_VIEW_INACTIVE;
    kz.hitbox_view_status = COL_VIEW_INACTIVE;
    kz.hide_actors = 0;
    kz.hide_room = 0;

    list_init(&kz.watches, sizeof(watch_t));

    kz.pending_frames = -1;
    kz.prev_timespeed = 0x80000000;

    kz.settings_profile = 0;
    load_settings_from_flashram(kz.settings_profile);
    kz_apply_settings();

    bind_override(KZ_CMD_TOGGLE_MENU);
    bind_override(KZ_CMD_RETURN);

    kz_resource_init();
    gfx_init();
    rdp_mode_set(RDP_MODE_COMBINE, G_CC_MODE(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM));

    kz.menu_active = 0;
    menu_init(&kz.main_menu, settings->menu_x, settings->menu_y);
    kz.main_menu.selected_item = menu_button_add(&kz.main_menu, 0, 0, "return", main_menu_return_onactivate, NULL);

    menu_submenu_add(&kz.main_menu, 0, 1, "warps", create_warps_menu());
    menu_submenu_add(&kz.main_menu, 0, 2, "cheats", create_cheats_menu());
    menu_submenu_add(&kz.main_menu, 0, 3, "scene", create_scene_menu());

#ifndef LITE
#define Y 5
    menu_submenu_add(&kz.main_menu, 0, 4, "states", create_states_menu());
#else
#define Y 4
#endif
    menu_submenu_add(&kz.main_menu, 0, Y, "watches", create_watches_menu());
    menu_submenu_add(&kz.main_menu, 0, Y + 1, "inventory", create_inventory_menu());
    menu_submenu_add(&kz.main_menu, 0, Y + 2, "equips", create_equips_menu());
    menu_submenu_add(&kz.main_menu, 0, Y + 3, "file", create_file_menu());
#ifndef LITE
    menu_submenu_add(&kz.main_menu, 0, Y + 4, "debug", create_debug_menu());
    menu_submenu_add(&kz.main_menu, 0, Y + 5, "settings", create_settings_menu());
#else
    menu_submenu_add(&kz.main_menu, 0, Y + 4, "settings", create_settings_menu());
#endif
#undef Y

    menu_init(&kz.pause_menu, 0, 0);
    static menu_sprite_t sprite = {
        NULL,   0,      0,      COLOR_GREEN,    0x64FF78FF,
        22,     22,     NULL,   0x64FF78FF,     0,
        0,      NULL
    };
    
    kz.pause_item_list = menu_item_list_add(&kz.pause_menu, 0, 0, 0, NULL, Z2_ITEM_END - 1, &item_val, NULL, Z2_ITEM_END - 1, &sprite, NULL);
    kz.pause_menu.selected_item = kz.pause_item_list;
    kz.pause_item_list->draw_proc = NULL;
    menu_item_register_event(kz.pause_item_list, MENU_EVENT_ACTIVATE, pause_menu_event, NULL);

    kz.memfile = malloc(sizeof(*kz.memfile) * KZ_MEMFILE_MAX);
    memset(kz.memfile, 0, sizeof(*kz.memfile) * KZ_MEMFILE_MAX);
    kz.memfile_slot = 0;

    kz.position_save = malloc(sizeof(*kz.position_save) * KZ_POSITION_MAX);
    memset(kz.position_save, 0, sizeof(*kz.position_save) * KZ_POSITION_MAX);
    kz.pos_slot = 0;

    memcpy(restriction_table, &z2_restriction_table, sizeof(z2_restriction_table));

    memset(&kz.log, 0, sizeof(kz.log));

#ifndef LITE
    for(int i = 0; i < STATE_MAX; i++) {
        kz.states[i] = NULL;
    }

    kz.state_slot = 0;
#endif
    
    kz.input_mask = BUTTON_DPAD;
    kz.stick_x_mask = 0x00;
    kz.stick_y_mask = 0x00;

    kz.ready = 1;
}

void kz_log(const char *format, ...) {
    struct log *log_entry = &kz.log[KZ_LOG_MAX - 1];
    
    for(int i = KZ_LOG_MAX - 1; i > 0; i--) {
        kz.log[i] = kz.log[i - 1];
    }
    
    log_entry = &kz.log[0];

    va_list va;
    va_start(va, format);
    vsnprintf(log_entry->mesg, 255, format, va);
    va_end(va);

    log_entry->active = 1;
    log_entry->time = 0;
}

static void game_state_main(void) {
    if(kz.pending_frames != 0) {
        if(kz.pending_frames > 0) {
            kz.pending_frames--;
        }
        kz_exit(z2_ctxt.gamestate_update, &z2_ctxt);
    } else {
        z2_gfx_t *gfx = z2_game.common.gfx;
        if(z2_ctxt.gamestate_frames != 1) {
            if(gfx->frame_cnt_1 & 1) {
                memcpy(&z2_disp[Z2_DISP_SIZE], z2_disp, Z2_DISP_SIZE);
            } else {
                memcpy(z2_disp, &z2_disp[Z2_DISP_SIZE], Z2_DISP_SIZE);
            }

            zu_disp_ptr_load(&kz.disp_p);
            zu_gfx_reloc(1 - (gfx->frame_cnt_1 & 1), 1 - (gfx->frame_cnt_2 & 1));
        } else {
            gDPSetColorImage(gfx->poly_opa.p++, G_IM_FMT_RGBA, G_IM_SIZ_16b, Z2_SCREEN_WIDTH, 0x0F000000);
            gDPSetCycleType(gfx->poly_opa.p++, G_CYC_FILL);
            gDPSetRenderMode(gfx->poly_opa.p++, G_RM_NOOP, G_RM_NOOP2);
            gDPSetFillColor(gfx->poly_opa.p++, 0);
            gDPFillRectangle(gfx->poly_opa.p++, 0, 0, Z2_SCREEN_WIDTH -1, Z2_SCREEN_HEIGHT - 1);
            gDPPipeSync(gfx->poly_opa.p++);
        }

        z2_game.common.gamestate_frames--;
    }
}

HOOK void input_hook(void) {
    if(kz.pending_frames != 0) {
        z2_input_update(&z2_ctxt);
        z2_input_t *inp = &z2_ctxt.input[0];
        inp->pad_pressed &= ~kz.input_mask;
        inp->pad_released |= (inp->raw.pad & kz.input_mask);
        inp->raw.pad &= ~kz.input_mask;
        inp->raw.x &= ~kz.stick_x_mask;
        inp->raw.y &= ~kz.stick_y_mask;
        z2_adjust_stick(inp);
    }
}

HOOK void motion_blur_hook(void) {
    if(kz.pending_frames != 0) {
        z2_MotionBlur(&z2_ctxt);
    }
}

HOOK void draw_actors_hook(void) { 
    if(kz.hide_actors) {
        zu_disp_ptr_t disp_p;
        zu_disp_ptr_save(&disp_p);
        z2_DrawActors(&z2_game, &z2_game.actor_ctxt);
        zu_disp_ptr_load(&disp_p);
    } else {
        z2_DrawActors(&z2_game, &z2_game.actor_ctxt);
    }
}

HOOK void draw_room_hook(z2_game_t *game, z2_room_t *room) {
    if(kz.hide_room) {
        zu_disp_ptr_t disp_p;
        zu_disp_ptr_save(&disp_p);
        z2_DrawRoom(game, room);
        zu_disp_ptr_load(&disp_p);
    } else {
        z2_DrawRoom(game, room);
    }
}

HOOK int camera_hook(z2_camera_t* camera) {
    int (*camera_update_func)(z2_camera_t*);
    __asm__ volatile("sw $t9, %0" : "=m"(camera_update_func) :: "t9");
    if(!kz.free_cam_active) {
        return camera_update_func(camera);
    }

    if(!kz.free_cam_locked) {
        kz_camera_calc(camera);
    }

    camera->at = kz.kz_at;
    camera->eye = kz.kz_eye;
    return 1;
}

int main() {
    init_gp();
    if(!kz.ready){
        init();
    }
    game_state_main();
    kz_main();
    return 0;
}

#include <grc.c>
#include <startup.c>
#include <vector/vector.c>
#include <list/list.c>
#include <set/set.c>
