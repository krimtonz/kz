#include <libundermine.h>

menu_item_t    *menu_item_list_add  (menu_t *menu, uint16_t cell_x, uint16_t cell_y, uint16_t start_tile, int8_t *options,
                                     uint8_t option_cnt, int8_t *value_ptr, int8_t *ovl_values, int tiles_cnt,
                                     menu_sprite_t *sprite, char *tooltip);