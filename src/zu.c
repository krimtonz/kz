#include "zu.h"
#include "z2.h"

int zu_ingame(){
    return z2_ctxt.gamestate_dtor == z2_gamestate_table[3].vram_dtor;
}