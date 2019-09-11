#include "kz.h"
#include "resource.h"

static uint8_t cur_form;

const char *form_options_text[] = {
    "fierce diety","goron","zora","deku","human"
};
uint8_t form_options_values[] = {
    0,1,2,3,4
};

static void change_selected_form(struct menu_item *item, uint16_t selected_idx){
    z2_file.current_form = form_options_values[selected_idx];
}

struct menu *create_file_menu(){
    static struct menu file;
    menu_init(&file,0,0);
    file.selected_item = menu_add_button(&file,0,0,"return",menu_return,NULL);
    draw_info_t draw_info = {
        resource_get(R_KZ_CHECKBOX), 1,-1, 1.f, 1.f, 8,8,{{0xFF,0xFF,0xFF,0xFF}}, {{0xFF,0xFF,0xFF,0xFF}},0,checkbox_bg
    };
    menu_add_gfx_switch(&file,0,1,&z2_file.intro_flag,1,0x01,NULL,NULL,&draw_info);
    menu_add(&file,2,1,"intro watched");
    menu_add(&file,0,2,"current form");
    menu_add_list(&file,13,2,form_options_text,form_options_values,1,sizeof(form_options_values),&cur_form,change_selected_form);
    return &file;
}