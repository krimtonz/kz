#ifndef _MENU_BIND_H
#define _MENU_BIND_H
#include <libundermine/menu.h>

void            menu_bind_set   (menu_item_t *item, int cmd);
menu_item_t    *menu_bind_add   (menu_t *menu, int x, int y, int cmd);

#endif