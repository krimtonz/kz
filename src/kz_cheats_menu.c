#include "menu.h"
#include "kz.h"
#include "resource.h"

struct cheat_item{
    enum cheats mask;
    char       *name;
};

static struct cheat_item cheat_table[] = {
    { CHEAT_ARROWS, "arrows" },
    { CHEAT_BOMBS, "bombs" },
    { CHEAT_BOMBCHUS, "bombchus" },
    { CHEAT_POWDER_KEG, "powder kegs" },
    { CHEAT_STICKS, "sticks" },
    { CHEAT_NUTS, "nuts" },
    { CHEAT_HEALTH, "health" },
    { CHEAT_MAGIC, "magic" },
    { CHEAT_RUPEES, "rupees" },
    { CHEAT_BLAST_MASK, "no blast mask cooldown" },
    { CHEAT_RESTRICTION, "no item restriction" },
    { CHEAT_ISG, "isg" },
    { CHEAT_FREEZE_TIME, "freeze time"},
};

static int cheat_callback(struct menu_item *item, enum menu_callback callback, void *data){
    enum cheats cheat = (enum cheats)data;
    if(callback == MENU_CALLBACK_ACTIVATE){
        settings->cheats = settings->cheats ^ (1 << cheat);
        if(cheat == CHEAT_RESTRICTION){
            if(settings->cheats & (1 << CHEAT_RESTRICTION)){
                memset((void*)z2_restriction_table_addr,1,sizeof(restriction_table));
            }else{
                memcpy((void*)z2_restriction_table_addr,restriction_table,sizeof(restriction_table));
            }
        }
        return 1;
    }else if(callback == MENU_CALLBACK_UPDATE){
        menu_checkbox_set(item,((settings->cheats & (1 << cheat)) > 0));
    }
    return 0;
}

struct menu *create_cheats_menu(){
    static struct menu cheats;
    menu_init(&cheats,0,0);
    menu_set_padding(&cheats,0,2);
    cheats.selected_item = menu_add_button(&cheats,0,0,"return",menu_return,NULL);
    struct menu_item *item = NULL;
    for(int i=0;i<sizeof(cheat_table)/sizeof(*cheat_table);i++){
        item = menu_add_checkbox(&cheats,0,i+1,cheat_callback,(void*)cheat_table[i].mask);
        cheat_callback(item,MENU_CALLBACK_UPDATE,(void*)cheat_table[i].mask);
        menu_add(&cheats,2,i+1,cheat_table[i].name);
    }
    return &cheats;
}