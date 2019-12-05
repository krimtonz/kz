#include <stdlib.h>
#include "menu.h"
#include "scenes.h"
#include "kz.h"

static int activate_warp_proc(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_game.entrance_index = (uint16_t)((uint32_t)data);
        z2_file.exit = z2_game.entrance_index;
        z2_game.common.execute_state = 0;
        z2_game.common.gamestate_ctor = z2_gamestate_table[3].vram_ctor;
        z2_game.common.ctxt_size = z2_gamestate_table[3].alloc_size;
        int scene = ((uint32_t)data >> 8) & 0xFF;
        if(scenes[scene].entrance_cnt > 0){
            menu_callback(&kz.main_menu,MENU_CALLBACK_RETURN);
        }
        menu_callback(&kz.main_menu,MENU_CALLBACK_RETURN);
        return 1;
    }
    return 0;
}

struct menu *create_warps_menu(){
    static struct menu warps;
    menu_init(&warps,kz.main_menu.x,kz.main_menu.y);
    menu_set_padding(&warps,0,1);
    warps.selected_item = menu_add_button(&warps,0,0,"return",menu_return,NULL);
    for(int i=0;i<sizeof(scene_categories)/sizeof(struct kz_scene_category);i++){
        struct kz_scene_category cat = scene_categories[i];
        struct menu *cat_menu = malloc(sizeof(*cat_menu));
        menu_init(cat_menu, 0, 0);
        menu_set_padding(cat_menu,0,1);
        cat_menu->selected_item = menu_add_button(cat_menu,0,0,"return",menu_return,NULL);
        menu_add_submenu(&warps,0,i+1,cat_menu,cat.name);
        for(int j = 0; j<cat.scene_cnt;j++){
            struct kz_scene scene = scenes[cat.scenes[j]];
            if(scene.entrance_cnt>1){
                struct menu *entrance_menu = malloc(sizeof(*entrance_menu));
                menu_init(entrance_menu, 0, 0);
                entrance_menu->selected_item = menu_add_button(entrance_menu,0,0,"return",menu_return,NULL);
                for(int k=0;k<scene.entrance_cnt;k++){
                    menu_add_button(entrance_menu,0,k+1,scene.entrances[k],activate_warp_proc,(void*)(((scene.scene & 0xFF) << 8) | ((k & 0xFF) <<4)));
                }
                menu_add_submenu(cat_menu,0,j+1,entrance_menu,scene.scene_name);
            }else{
                menu_add_button(cat_menu,0,j+1,scene.scene_name,activate_warp_proc,(void*)(((scene.scene & 0xFF) << 8) | ((0 & 0xFF) << 4)));
            }
        }
    }
    return &warps;
}