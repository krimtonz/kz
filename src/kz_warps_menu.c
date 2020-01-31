#include <stdlib.h>
#include <libundermine.h>
#include "scenes.h"
#include "kz.h"

static int warp_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    uint16_t entrance = (uint16_t)((uint32_t)handler->callback_data);
    z2_game.entrance_index = entrance;
    z2_file.exit = z2_game.entrance_index;
    z2_game.common.execute_state = 0;
    z2_game.common.gamestate_ctor = z2_gamestate_table[3].vram_ctor;
    z2_game.common.ctxt_size = z2_gamestate_table[3].alloc_size;
    int scene = ((uint32_t)entrance >> 8) & 0xFF;
    if(scenes[scene].entrance_cnt > 0){
        menu_trigger_event(&kz.main_menu, MENU_EVENT_RETURN, NULL);
    }
    menu_trigger_event(&kz.main_menu, MENU_EVENT_RETURN, NULL);
    return 1;
}

menu_t *create_warps_menu(void){
    static menu_t warps;
    menu_init(&warps, 0, 0);
    menu_padding_set(&warps, 0, 1);
    warps.selected_item = menu_button_add(&warps, 0, 0, "return", menu_return, NULL);
    for(int i = 0;i < sizeof(scene_categories) / sizeof(struct kz_scene_category);i++){
        struct kz_scene_category cat = scene_categories[i];
        menu_t *cat_menu = malloc(sizeof(*cat_menu));
        menu_init(cat_menu, 0, 0);
        menu_padding_set(cat_menu, 0, 1);
        cat_menu->selected_item = menu_button_add(cat_menu, 0, 0, "return", menu_return, NULL);
        menu_submenu_add(&warps, 0, i + 1, cat.name, cat_menu);
        for(int j = 0; j < cat.scene_cnt;j++){
            struct kz_scene scene = scenes[cat.scenes[j]];
            if(scene.entrance_cnt > 1){
                menu_t *entrance_menu = malloc(sizeof(*entrance_menu));
                menu_init(entrance_menu, 0, 0);
                menu_padding_set(entrance_menu, 0, 1);
                entrance_menu->selected_item = menu_button_add(entrance_menu, 0, 0, "return", menu_return, NULL);
                for(int k = 0;k < scene.entrance_cnt;k++){
                    menu_button_add(entrance_menu, 0, k + 1, scene.entrances[k], warp_onactivate, (void*)(((scene.scene & 0xFF) << 8) | ((k & 0xFF) << 4)));
                }
                menu_submenu_add(cat_menu, 0, j + 1, scene.scene_name, entrance_menu);
            }else{
                menu_button_add(cat_menu, 0, j + 1, scene.scene_name, warp_onactivate, (void*)(((scene.scene & 0xFF) << 8) | ((0 & 0xFF) << 4)));
            }
        }
    }
    return &warps;
}