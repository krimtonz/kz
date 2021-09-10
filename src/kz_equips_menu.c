#include <stdlib.h>

#include "kz.h"
#include "kzresource.h"
#include "menu.h"

static int8_t b_val = Z2_ITEM_NULL;

struct equip_option {
    uint8_t     shift;
    int8_t      val;
    int8_t      item_id;
};

static struct equip_option sword_options[] = {
    { 0,    1,  Z2_ITEM_KOKIRI_SWORD},
    { 0,    2,  Z2_ITEM_RAZOR_SWORD},
    { 0,    3,  Z2_ITEM_GILDED_SWORD}
};

static struct equip_option shield_options[] = {
    { 4,    1,  Z2_ITEM_HERO_SHIELD},
    { 4,    2,  Z2_ITEM_MIRROR_SHIELD}
};

static uint8_t get_equip_value(struct equip_option *data){
    return (z2_file.equipment >> data->shift) & 0b11;
}

static int b_button_event(event_handler_t *handler, menu_event_t event, void **event_data){
    if(event == MENU_EVENT_ACTIVATE){
        if((int)*event_data == 0){
            z2_file.form_button_item[0].b = b_val;
            z2_btnupdate(&z2_game, 0);
        }
    }
    if(event == MENU_EVENT_UPDATE){
        b_val = z2_file.form_button_item[0].b;
    }
    return 1;
}

static int c_button_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    z2_btnupdate(&z2_game, (uint32_t)handler->callback_data);
    return 1;
}

static void equip_option_draw(menu_item_t *item){
    struct equip_option *data = item->data;
    int x = menu_item_x(item);
    int y = menu_item_y(item);
    if(item->owner->selected_item == item){
        gfx_draw_rectangle(x, y, 16, 16, SELECTED_COLOR);
    }
    gfx_draw_sprite(get_item_texture(data->item_id, 0), x, y, get_equip_value(data) == data->val ? 0 : 1, 16, 16);
}

static int equip_option_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    struct equip_option *data = handler->callback_data;
    int8_t equip_val = get_equip_value(data);
    z2_file.equipment = z2_file.equipment & ~(0b11 << data->shift);
    int8_t bval = Z2_ITEM_NULL;
    if(equip_val != data->val){
        z2_file.equipment |= (data->val << data->shift);
        bval = data->item_id;
    }
    if(data->shift == 0){
        z2_file.form_button_item[0].b = bval;
        z2_btnupdate(&z2_game, 0);
    }
    return 1;
}

static menu_item_t *equip_option_add(menu_t *menu, uint16_t x_cell, uint16_t y_cell, struct equip_option *option){
    menu_item_t *item = menu_add(menu, x_cell, y_cell);
    if(item){
        item->interactive = 1;
        item->draw_proc = equip_option_draw;
        item->data = option;
        menu_item_register_event(item, MENU_EVENT_ACTIVATE, equip_option_onactivate, option);
    }
    return item;
}

menu_t *create_equips_menu(void){
    static menu_t equips;

    menu_init(&equips, 0, 0);
    menu_cell_set(&equips, 16, 16);

    equips.selected_item = menu_button_add(&equips, 0, 0, "return", menu_return, NULL);

    menu_item_t *item;

    for(int i = 0;i < sizeof(sword_options) / sizeof(*sword_options);i++){
        item = equip_option_add(&equips, i, 1, &sword_options[i]);
        menu_item_offset_set(item, 5 * i, 0);
    }

    for(int i = 0;i < sizeof(shield_options) / sizeof(*shield_options);i++){
        item = equip_option_add(&equips, i, 2, &shield_options[i]);
        menu_item_offset_set(item, i * 5, 5);
    }

    static menu_sprite_t b_sprite = {
        NULL,   0,      0,      COLOR_GREEN,    0x64FF78FF,
        22,     22,     NULL,   0x64FF78FF,     0,
        0,      NULL
    };

    static menu_sprite_t c_left_sprite = {
        NULL,   0,      0,      COLOR_GREEN,    0xFFF000FF,
        16,     16,     NULL,   0xFFF000FF,     0,
        2,      NULL
    };

    static menu_sprite_t c_down_sprite = {
        NULL,   0,      0,      COLOR_GREEN,    0xFFF000FF,
        16,     16,     NULL,   0xFFF000FF,     0,
        3,      NULL
    };

    static menu_sprite_t c_right_sprite = {
        NULL,   0,      0,      COLOR_GREEN,    0xFFF000FF,
        16,     16,     NULL,   0xFFF000FF,     0,
        4,      NULL
    };

    gfx_texture *buttons_texture = resource_get(R_Z2_BUTTONS);
    b_sprite.background = buttons_texture;
    c_left_sprite.background = buttons_texture;
    c_left_sprite.null_texture = buttons_texture;
    c_down_sprite.background = buttons_texture;
    c_down_sprite.null_texture = buttons_texture;
    c_right_sprite.background = buttons_texture;
    c_right_sprite.null_texture = buttons_texture;

    item = menu_item_list_add(&equips, 0, 3, 0, NULL, Z2_ITEM_ODOLWAS_REMAINS - 1, &b_val, NULL, Z2_ITEM_ODOLWAS_REMAINS - 1, &b_sprite, NULL);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, b_button_event, NULL);
    menu_item_offset_set(item, 0, 8);

    item = menu_item_list_add(&equips, 1, 3, 0, NULL, Z2_ITEM_ODOLWAS_REMAINS - 1, &z2_file.form_button_item[0].cleft, NULL, Z2_ITEM_ODOLWAS_REMAINS - 1, &c_left_sprite, NULL);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE, c_button_onactivate, (void*)1);
    menu_item_offset_set(item, 6, 11);

    item = menu_item_list_add(&equips, 2, 3, 0, NULL, Z2_ITEM_ODOLWAS_REMAINS - 1, &z2_file.form_button_item[0].cdown, NULL, Z2_ITEM_ODOLWAS_REMAINS - 1, &c_down_sprite, NULL);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE, c_button_onactivate, (void*)2);
    menu_item_offset_set(item, 4, 21);

    item = menu_item_list_add(&equips, 3, 3, 0, NULL, Z2_ITEM_ODOLWAS_REMAINS - 1, &z2_file.form_button_item[0].cright, NULL, Z2_ITEM_ODOLWAS_REMAINS - 1, &c_right_sprite, NULL);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE, c_button_onactivate, (void*)3);
    menu_item_offset_set(item, 2, 11);

    return &equips;
}
