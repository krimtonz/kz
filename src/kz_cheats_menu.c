#include <libundermine/menu.h>
#include "kz.h"
#include "kzresource.h"

struct cheat_item {
    enum cheats mask;
    char       *name;
};

static struct cheat_item cheat_table[] = {
    { CHEAT_ARROWS,         "arrows" },
    { CHEAT_BOMBS,          "bombs" },
    { CHEAT_BOMBCHUS,       "bombchus" },
    { CHEAT_POWDER_KEG,     "powder kegs" },
    { CHEAT_STICKS,         "sticks" },
    { CHEAT_NUTS,           "nuts" },
    { CHEAT_HEALTH,         "health" },
    { CHEAT_MAGIC,          "magic" },
    { CHEAT_RUPEES,         "rupees" },
    { CHEAT_BLAST_MASK,     "no blast mask cooldown" },
    { CHEAT_RESTRICTION,    "no item restriction" },
    { CHEAT_ISG,            "isg" },
    { CHEAT_FREEZE_TIME,    "freeze time"},
};

static int cheat_event(event_handler_t *handler, menu_event_t event, void **event_data){
    enum cheats cheat = (enum cheats)handler->callback_data;
    if(event == MENU_EVENT_ACTIVATE){
        settings->cheats = settings->cheats ^ (1 << cheat);
        if(cheat == CHEAT_RESTRICTION){
            if(settings->cheats & (1 << CHEAT_RESTRICTION)){
                memset(z2_restriction_table, 1, sizeof(z2_restriction_table));
            }else{
                memcpy(z2_restriction_table, restriction_table, sizeof(z2_restriction_table));
            }
        }else if(cheat == CHEAT_FREEZE_TIME){
            if(settings->cheats & (1 << CHEAT_FREEZE_TIME)){
                kz.prev_timespeed = z2_file.timespeed;
            }else{
                if(kz.prev_timespeed == 0x80000000){
                    z2_file.timespeed = 0;
                }else{
                    z2_file.timespeed = kz.prev_timespeed;
                }
                kz.prev_timespeed = 0x80000000;
            }
        }
    }else if(event == MENU_EVENT_UPDATE){
        menu_checkbox_set((menu_item_t*)handler->subscriber, (settings->cheats & (1 << cheat)) > 0);
    }
    return 1;
}

menu_t *create_cheats_menu(void){
    static menu_t cheats;
    menu_init(&cheats, 0, 0);
    menu_padding_set(&cheats, 0, 2);
    cheats.selected_item = menu_button_add(&cheats, 0, 0, "return", menu_return, NULL);
    menu_item_t *item = NULL;
    for(int i = 0;i < sizeof(cheat_table) / sizeof(*cheat_table);i++){
        item = menu_checkbox_add(&cheats, 0, i + 1);
        menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, cheat_event, (void*)cheat_table[i].mask);
        menu_label_add(&cheats, 2, i + 1, cheat_table[i].name);
    }
    return &cheats;
}