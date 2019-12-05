#include <stdlib.h>
#include "menu.h"
#include "input.h"
#include "kz.h"

z2_rgba32_t MENU_SELECTED_COLOR = {{0x80,0x80,0xFF,0xFF}};
z2_rgba32_t MENU_DEFAULT_COLOR = {{0xFF,0xFF,0xFF,0xFF}};

static void item_enter(struct menu_item *item){
    kz.tooltip = item->tooltip;
}

static void item_exit(struct menu_item *item){
    kz.tooltip = NULL;
}

int get_item_x_pos(struct menu_item *item){
    return item->owner->x + (item->x * item->owner->cell_width) + (item->x * item->owner->x_padding) + item->x_offset;
}

int get_item_y_pos(struct menu_item *item){
    return item->owner->y + item->y * item->owner->cell_height + item->y * item->owner->y_padding + item->y_offset;
}

void set_item_offset(struct menu_item *item, int16_t x, int16_t y){
    item->x_offset = x;
    item->y_offset = y;
}

void menu_init(struct menu *menu, uint16_t x, uint16_t y){
    list_init(&menu->items, sizeof(struct menu_item));
    menu->child=NULL;
    menu->parent=NULL;
    menu->selected_item=NULL;
    menu->callback_proc = NULL;
    menu->x = x;
    menu->y = y;
    menu->cell_height = kfont->c_height;
    menu->cell_width = kfont->c_width;
    menu->x_padding = 0;
    menu->y_padding = 0;
}

void menu_set_cell(struct menu *menu, uint16_t width, uint16_t height){
    menu->cell_width = width;
    menu->cell_height = height;
}

void menu_set_padding(struct menu *menu, uint16_t x, uint16_t y){
    menu->x_padding = x;
    menu->y_padding = y;
}

void menu_set_pos(struct menu *menu, uint16_t x, uint16_t y){
    menu->x = x;
    menu->y = y;
}

void menu_draw(struct menu *menu){
    if(menu->child){
        menu_draw(menu->child);
        return;
    }

    for(struct menu_item *item = menu->items.first; item; item = list_next(item)){
        if(!item->enabled) continue;
        if(item->draw_proc){
            item->draw_proc(item);
            continue;
        }
        z2_rgba32_t color = MENU_DEFAULT_COLOR;
        if(item == menu->selected_item){
            color = MENU_SELECTED_COLOR;
        }
        gfx_printf_color(get_item_x_pos(item),get_item_y_pos(item),color.color,"%s",item->text);
    }
}

void menu_update(struct menu *menu){
    if(menu->child){
        menu_update(menu->child);
        return;
    }
    for(struct menu_item *item = menu->items.first;item;item=list_next(item)){
        if(item->update_proc){
            item->update_proc(item);
        }
    }
}

struct menu_item *menu_add(struct menu *menu, uint16_t x, uint16_t y, char *text){
    struct menu_item *item = list_push_back(&menu->items,NULL);
    if(item){
        item->text = text;
        item->owner = menu;
        item->interactive = 0;
        item->enabled = 1;
        item->x = x;
        item->y = y;
        item->x_offset = 0;
        item->y_offset = 0;
        item->draw_proc=NULL;
        item->activate_proc=NULL;
        item->navigate_proc=NULL;
        item->update_proc=NULL;
        item->tooltip=NULL;
        item->enter_proc=item_enter;
        item->exit_proc=item_exit;
    }
    return item;
}

void menu_item_remove(struct menu_item *item){
    list_erase(&item->owner->items, item);
}

void menu_navigate(struct menu *menu, enum menu_nav nav){
    if(nav==MENU_NAV_NONE) return;

    if(menu->child){
        menu_navigate(menu->child,nav);
        return;
    }

    if(menu->selected_item && menu->selected_item->navigate_proc && menu->selected_item->navigate_proc(menu->selected_item,nav))
        return;

    int dir_horiz = nav == MENU_NAV_LEFT?-1:(nav == MENU_NAV_RIGHT?1:0);
    int dir_vert = nav == MENU_NAV_UP?-1:(nav == MENU_NAV_DOWN?1:0);

    int ndist_pa = 0;
    int ndist_pe = 0;
    int fdist_pa = 0;
    int fdist_pe = 0;
    int cur_x_pos = get_item_x_pos(menu->selected_item);
    int cur_y_pos = get_item_y_pos(menu->selected_item);

    struct menu_item *near = NULL;
    struct menu_item *far = NULL;

    for(struct menu_item *item = menu->items.first;item!=NULL;item=list_next(item)){
        if(!item->interactive || !item->enabled) continue;
        int distance_x = get_item_x_pos(item) - cur_x_pos;
        int distance_y = get_item_y_pos(item) - cur_y_pos;

        int pa = dir_horiz?dir_horiz*distance_x:(dir_vert?dir_vert*distance_y:0);
        int pe = dir_vert?distance_x:distance_y;

        if(pe<0) pe = -pe;

        if(pa > 0 && (near == NULL || pe<ndist_pe || (pe==ndist_pe && pa<ndist_pa))){
            ndist_pa=pa;
            ndist_pe=pe;
            near=item;
        }

        if(pa<0 && (far == NULL || -pa>fdist_pa || (-pa==fdist_pa && pe<fdist_pe))){
            far = item;
            fdist_pa = -pa;
            fdist_pe = pe;
        }
    }

    if((near || far) && menu->selected_item && menu->selected_item->exit_proc){
        menu->selected_item->exit_proc(menu->selected_item);
    }

    if(near){
        if(near->enter_proc){
            near->enter_proc(near);
        }
        menu->selected_item = near;
    }else if(far){
        if(far->enter_proc){
            far->enter_proc(far);
        }
        menu->selected_item = far;
    }
}

void menu_callback(struct menu *menu, enum menu_callback callback){
    if(callback == MENU_CALLBACK_NONE){
        return;
    }
    if(menu->child){
        menu_callback(menu->child,callback);
        return;
    }
    if(callback == MENU_CALLBACK_RETURN){
        if(menu->parent){
            if(menu->callback_proc){
                menu->callback_proc(MENU_CALLBACK_EXIT);
            }
            if(menu->selected_item->exit_proc){
                menu->selected_item->exit_proc(menu->selected_item);
            }
            if(menu->parent->callback_proc){
                menu->parent->callback_proc(MENU_CALLBACK_ENTER);
            }
            if(menu->parent->selected_item->enter_proc){
                menu->parent->selected_item->enter_proc(menu->parent->selected_item);
            }
            menu->parent->child = NULL;
        }
    }
    else if(menu->selected_item->activate_proc){
        menu->selected_item->activate_proc(menu->selected_item);
    }
}

int menu_return(struct menu_item *item, enum menu_callback callback, void *data){
    if(callback == MENU_CALLBACK_ACTIVATE){
        menu_callback(item->owner,MENU_CALLBACK_RETURN);
        return 1;
    }
    return 0;
}

void menu_enter(struct menu *menu, struct menu *submenu){
    if(menu->child){
        menu_enter(menu->child,submenu);
        return;
    }
    menu->child = submenu;
    submenu->parent = menu;
    if(menu->callback_proc){
        menu->callback_proc(MENU_CALLBACK_EXIT);
    }
    if(menu->selected_item->exit_proc){
        menu->selected_item->exit_proc(menu->selected_item);
    }
    if(submenu->callback_proc){
        submenu->callback_proc(MENU_CALLBACK_ENTER);
    }
    if(submenu->selected_item->enter_proc){
        submenu->selected_item->enter_proc(submenu->selected_item);
    }
}