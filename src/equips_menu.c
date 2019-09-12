#include <stdlib.h>

#include "kz.h"
#include "resource.h"

static int8_t b_val = Z2_ITEM_NULL;

static int b_button_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_file.form_button_item[z2_file.current_form==4?0:z2_file.current_form].b = b_val;
        z2_btnupdate(&z2_game,0);
        return 1;
    }
    if(callback == MENU_CALLBACK_UPDATE){
        b_val = z2_file.form_button_item[z2_file.current_form==4?0:z2_file.current_form].b;
        return 1;
    }
    return 0;
}

static int c_button_callback(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        z2_btnupdate(&z2_game,(uint32_t)data);
        return 1;
    }
    return 0;
}

struct menu *create_equips_menu(){
    static struct menu equips;

    static struct tilebg_info b_bg;
    gfx_texture *buttons_texture = resource_get(R_Z2_BUTTONS);
    b_bg.texture = buttons_texture;
    b_bg.tile=0;
    b_bg.on_color.color=0xFF0000FF;
    b_bg.off_color.color=0x64FF78FF;

    static struct tilebg_info c_bg;
    c_bg.texture = buttons_texture;
    c_bg.tile=0;
    c_bg.on_color.color=0xFF0000FF;
    c_bg.off_color.color=0xFFF000FF;

    gfx_texture *items_texture = resource_get(R_Z2_ITEMS);
    static draw_info_t b_draw_info;
    b_draw_info.x_scale=1.4f;
    b_draw_info.y_scale=1.4f;
    b_draw_info.texture = items_texture;
    b_draw_info.background = &b_bg;

    static draw_info_t c_draw_info;
    c_draw_info.x_scale=1.0f;
    c_draw_info.y_scale=1.0f;
    c_draw_info.texture = items_texture;
    c_draw_info.background = &c_bg;

    static struct tilebg_info c_left;
    c_left.texture=buttons_texture;
    c_left.tile=2;
    c_left.on_color.color = 0xFF0000FF;
    c_left.off_color.color = 0xFFF000FF;

    static struct tilebg_info c_down;
    c_down.texture=buttons_texture;
    c_down.on_color.color = 0xFF0000FF;
    c_down.off_color.color = 0xFFF000FF;
    c_down.tile=3;

    static struct tilebg_info c_right;
    c_right.texture=buttons_texture;
    c_right.on_color.color = 0xFF0000FF;
    c_right.off_color.color = 0xFFF000FF;
    c_right.tile=4;

    menu_init(&equips,0,0);
    menu_set_cell(&equips,16,16);

    equips.selected_item = menu_add_button(&equips,0,0,"return",menu_return,NULL);
    menu_add_item_list(&equips,0,1,b_button_callback,&b_val,0,NULL,
                        Z2_ITEM_ODOLWAS_REMAINS-1,&b_val,NULL,Z2_ITEM_ODOLWAS_REMAINS-1,&b_draw_info,NULL);

    struct menu_item *item = menu_add_item_list(&equips,1,1,c_button_callback,(void*)1,0,NULL,
                        Z2_ITEM_ODOLWAS_REMAINS-1,&z2_file.form_button_item[0].cleft,NULL,Z2_ITEM_ODOLWAS_REMAINS-1,&c_draw_info,&c_left);
    set_item_offset(item,6,8);

    item = menu_add_item_list(&equips,2,1,c_button_callback,(void*)2,0,NULL,
                        Z2_ITEM_ODOLWAS_REMAINS-1,&z2_file.form_button_item[0].cdown,NULL,Z2_ITEM_ODOLWAS_REMAINS-1,&c_draw_info,&c_down);
    set_item_offset(item,4,18);

    item = menu_add_item_list(&equips,3,1,c_button_callback,(void*)3,0,NULL,
                        Z2_ITEM_ODOLWAS_REMAINS-1,&z2_file.form_button_item[0].cright,NULL,Z2_ITEM_ODOLWAS_REMAINS-1,&c_draw_info,&c_right);
    set_item_offset(item,2,8);
    return &equips;
}