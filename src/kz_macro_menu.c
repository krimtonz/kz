#include <stdint.h>
#include <sys/stat.h>
#include "kz.h"
#include "menu.h"
#include "menu_file.h"
#include "sys.h"
#include "kzresource.h"
#include "macro.h"
#include "printf.h"

static char macro_frame_text[40];

static int record_onupdate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    if(event == MENU_EVENT_ACTIVATE) {
        macro_record();
    } else if(event == MENU_EVENT_UPDATE) {
        item->text = macro_mode() == MACRO_RECORDING ? "stop recording" : "record";
    }
    return 1;
}

static int play_onupdate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *item = handler->subscriber;
    if(event == MENU_EVENT_ACTIVATE) {
        macro_play();
    } else if(event == MENU_EVENT_UPDATE) {
        item->text = macro_mode() == MACRO_PLAYING ? "stop playing" : "play";
    }
    return 1;
}

static int rewind_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    macro_rewind();
    return 1;
}

static int trim_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    macro_trim();
    kz_log("macro trimmed to %d frames", macro_length());
    return 1;
}

static int clear_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    macro_clear();
    kz_log("macro cleared");
    return 1;
}

static int frame_onupdate(event_handler_t *handler, menu_event_t event, void **event_data){
    static const char *mode_text[] = {"", " rec", " play"};
    snprintf(macro_frame_text, sizeof(macro_frame_text), "frame %d / %d%s",
             (int)macro_frame(), (int)macro_length(), mode_text[macro_mode()]);
    return 1;
}

static int file_write(void *ctx, const void *buf, size_t len){
    return write(*(int*)ctx, (void*)buf, len) == len;
}

static int file_read(void *ctx, void *buf, size_t len){
    return read(*(int*)ctx, buf, len) == len;
}

static void do_export_macro(char *path, void *data){
    int file = creat(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(file == -1) {
        kz_log("could not create file");
        return;
    }
    if(macro_store_write(macro_get_store(), Z2_VERSION, file_write, &file)) {
        kz_log("exported macro (%d frames)", macro_length());
    } else {
        kz_log("could not write macro");
    }
    close(file);
}

static void do_import_macro(char *path, void *data){
    int file = open(path, O_RDONLY);
    if(file == -1) {
        kz_log("could not open file");
        return;
    }

    struct stat sbuf;
    int error = MACRO_READ_IO;
    macro_clear();
    if(stat(path, &sbuf) == 0) {
        error = macro_store_read(macro_get_store(), Z2_VERSION, sbuf.st_size, file_read, &file);
    }
    close(file);

    switch(error) {
        case 0:                     kz_log("imported macro (%d frames)", macro_length()); break;
        case MACRO_READ_MEMORY:     kz_log("not enough memory for macro"); break;
        case MACRO_READ_FORMAT:     kz_log("not a macro file"); break;
        case MACRO_READ_VERSION:    kz_log("wrong mm version"); break;
        default:                    kz_log("could not read macro"); break;
    }
}

static int export_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    if(macro_length() == 0) {
        kz_log("no macro");
        return 1;
    }
    menu_file_get(FILE_MODE_SAVE, "macro", ".kzmc", do_export_macro, NULL);
    return 1;
}

static int import_onactivate(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_file_get(FILE_MODE_LOAD, "macro", ".kzmc", do_import_macro, NULL);
    return 1;
}

menu_t *create_macro_menu(void){
    static menu_t macro_menu;
    menu_init(&macro_menu, 0, 0);
    macro_menu.selected_item = menu_button_add(&macro_menu, 0, 0, "return", menu_return, NULL);

    menu_item_t *item = menu_button_add(&macro_menu, 0, 1, "record", NULL, NULL);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, record_onupdate, NULL);
    item = menu_button_add(&macro_menu, 0, 2, "play", NULL, NULL);
    menu_item_register_event(item, MENU_EVENT_ACTIVATE | MENU_EVENT_UPDATE, play_onupdate, NULL);

    menu_button_add(&macro_menu, 0, 3, "rewind", rewind_onactivate, NULL);
    menu_button_add(&macro_menu, 8, 3, "trim", trim_onactivate, NULL);
    menu_button_add(&macro_menu, 14, 3, "clear", clear_onactivate, NULL);

    item = menu_label_add(&macro_menu, 0, 4, macro_frame_text);
    item->color = COLOR_FADED;
    menu_item_register_event(item, MENU_EVENT_UPDATE, frame_onupdate, NULL);

    menu_gfx_button_add(&macro_menu, 0, 5, pexport_sprite, export_onactivate, NULL)->tooltip = "export macro";
    menu_gfx_button_add(&macro_menu, 4, 5, pimport_sprite, import_onactivate, NULL)->tooltip = "import macro";
    return &macro_menu;
}
