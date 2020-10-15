/*
* menu_file.h
*
* definitions for the file list menu items
*/

#ifndef LITE
#ifndef _MENU_FILE_H
#define _MENU_FILE_H

typedef enum {
    FILE_MODE_LOAD,
    FILE_MODE_SAVE,
} file_mode_t;

typedef void (*get_file_callback_t)(char *filename, void *data);

void menu_file_get(file_mode_t mode, const char *default_fn, const char *extension, get_file_callback_t callback, void *callback_data);

#endif
#endif