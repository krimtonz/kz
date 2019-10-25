#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "menu.h"

void init_kz_keyboard();
void show_keyboard(struct menu_item *item, char **dest, menu_generic_callback callback);

#endif