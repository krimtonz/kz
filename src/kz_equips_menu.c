#include <stdlib.h>

#include "kz.h"
#include "resource.h"

static int8_t b_val = Z2_ITEM_NULL;

struct equip_option {
    uint8_t     shift;
    uint8_t     val;
    uint8_t     item_id;
};

static struct equip_option sword_options[] = {
    { 0, 1, Z2_ITEM_KOKIRI_SWORD},
    { 0, 2, Z2_ITEM_RAZOR_SWORD},
    { 0, 3, Z2_ITEM_GILDED_SWORD}
};

static struct equip_option shield_options[] = {
    {4, 1, Z2_ITEM_HERO_SHIELD},
    {4, 2, Z2_ITEM_MIRROR_SHIELD}
};

static int8_t get_equip_value(struct equip_option *data){
    return (z2_file.equipment >> data->shift) & 0b11;
}

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

static void draw_equip_option(struct menu_item *item){
    struct equip_option *data = item->data;
    if(item->owner->selected_item == item){
        gfx_draw_rectangle(get_item_x_pos(item),get_item_y_pos(item),16,16,MENU_SELECTED_COLOR.color);
    }
    gfx_push(gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM));
    gfx_push(gsDPSetPrimColor(0,0,0xFF,0xFF,0xFF,0xFF));
    gfx_push(gsDPPipeSync());
    gfx_draw_sprite(get_item_texture(data->item_id),get_item_x_pos(item),get_item_y_pos(item),get_equip_value(data) == data->val?0:1,16,16);
    gfx_begin();
}

static void equip_option_activate(struct menu_item *item){
    struct equip_option *data = item->data;
    int8_t equip_val = get_equip_value(data);
    z2_file.equipment = z2_file.equipment & ~(0b11 << data->shift);
    int8_t bval = Z2_ITEM_NULL;
    if(equip_val != data->val){
        z2_file.equipment |= (data->val << data->shift);
        bval = data->item_id;
    }
    if(data->shift==0){
        z2_file.form_button_item[0].b = bval;
        z2_btnupdate(&z2_game,0);
    }
}

static struct menu_item *add_equip_option(struct menu *menu, int x, int y, struct equip_option *option){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        item->interactive = 1;
        item->data = option;
        item->draw_proc = draw_equip_option;
        item->activate_proc = equip_option_activate;
    }
    return item;
}

struct menu *create_equips_menu(){
    static struct menu equips;

    menu_init(&equips,0,0);
    menu_set_cell(&equips,16,16);

    equips.selected_item = menu_add_button(&equips,0,0,"return",menu_return,NULL);
    
    for(int i=0;i<sizeof(sword_options)/sizeof(*sword_options);i++){
        set_item_offset(add_equip_option(&equips,i,1,&sword_options[i]),5*i,0);
    }
    
    for(int i=0;i<sizeof(shield_options)/sizeof(*shield_options);i++){
        set_item_offset(add_equip_option(&equips,i,2,&shield_options[i]),i*5,5);
    }
    
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

    static draw_info_t b_draw_info;
    b_draw_info.x_scale=1.4f;
    b_draw_info.y_scale=1.4f;
    b_draw_info.texture = NULL;
    b_draw_info.background = &b_bg;

    static draw_info_t c_draw_info;
    c_draw_info.x_scale=1.0f;
    c_draw_info.y_scale=1.0f;
    c_draw_info.texture = NULL;
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

    struct menu_item *item = menu_add_item_list(&equips,0,3,b_button_callback,&b_val,0,NULL,
                        Z2_ITEM_ODOLWAS_REMAINS-1,&b_val,NULL,Z2_ITEM_ODOLWAS_REMAINS-1,&b_draw_info,NULL);
    set_item_offset(item,0,8);

    item = menu_add_item_list(&equips,1,3,c_button_callback,(void*)1,0,NULL,
                        Z2_ITEM_ODOLWAS_REMAINS-1,&z2_file.form_button_item[0].cleft,NULL,-1,&c_draw_info,&c_left);
    set_item_offset(item,6,11);

    item = menu_add_item_list(&equips,2,3,c_button_callback,(void*)2,0,NULL,
                        Z2_ITEM_ODOLWAS_REMAINS-1,&z2_file.form_button_item[0].cdown,NULL,-1,&c_draw_info,&c_down);
    set_item_offset(item,4,21);

    item = menu_add_item_list(&equips,3,3,c_button_callback,(void*)3,0,NULL,
                        Z2_ITEM_ODOLWAS_REMAINS-1,&z2_file.form_button_item[0].cright,NULL,Z2_ITEM_ODOLWAS_REMAINS-1,&c_draw_info,&c_right);
    set_item_offset(item,2,11);
    return &equips;
}