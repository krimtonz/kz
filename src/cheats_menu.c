#include "menu.h"
#include "kz.h"

struct cheat_item{
    enum cheats mask;
    const char *name;
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
    { CHEAT_RAZOR_SWORD, "unbreakable razor sword" },
    { CHEAT_RESTRICTION, "no item restriction" },
    { CHEAT_ISG, "isg" }
};

struct menu *create_cheats_menu(){
    static struct menu cheats;
    menu_init(&cheats,0,0);
    cheats.selected_item = menu_add_button(&cheats,0,0,"return",menu_return,NULL);
    for(int i=0;i<sizeof(cheat_table)/sizeof(*cheat_table);i++){
        menu_add_switch(&cheats,0,i+1,&kz.cheats,2,(1 << cheat_table[i].mask),NULL,cheat_table[i].name);
    }
    return &cheats;
}