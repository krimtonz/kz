#ifndef LITE
#ifndef _MENU_FILE_H
#define _MENU_FILE_H

enum file_mode{
    FILE_MODE_LOAD,
    FILE_MODE_SAVE,
};

typedef void (*get_file_callback_t)(char *filename, void *data);

void menu_get_file(enum file_mode mode, const char *default_fn, const char *extension, get_file_callback_t callback, void *callback_data);

#endif
#endif