#include <stdlib.h>
#include "menu.h"
#include "kz.h"

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

struct menu *create_scene_menu(){
    static struct menu scene;
    menu_init(&scene,0,0);
    scene.selected_item = menu_add_button(&scene,0,0,"return",menu_return,NULL);
    menu_add(&scene,0,1,"collision viewer");
    menu_add_button(&scene,0,2,"show", collison_show, NULL);
    menu_add_button(&scene,0,3,"hide", collison_hide, NULL);
    menu_add_button(&scene,0,4,"gen", collison_gen, NULL);
    menu_add_button(&scene,0,5,"reduce", collision_reduced, NULL);
    menu_add_button(&scene,0,6,"opaque",collision_opaque, NULL);
    return &scene;
}