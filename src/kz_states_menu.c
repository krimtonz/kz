#include "kz.h"
#include "menu.h"
#include "kzresource.h"
#include <sys/stat.h>
#include "menu_file.h"
#include "sys.h"
#ifdef LITE
#include "mem.h"
#include "printf.h"
#else
#include "hb_heap.h"
#endif

static const char *no_state = "no state";
static char *state_name = NULL;

static int prev_state_onactivate(event_handler_t *handler, menu_event_t event, void **event_data) {
    kz.state_slot += STATE_MAX - 1;
    kz.state_slot %= STATE_MAX;
    kz_state_hdr_t *state = kz.states[kz.state_slot];
    if(kz.states[kz.state_slot] != NULL) {
#ifdef WIIVC
        hmemcpy(state_name, state->name, 64);
#else
        strcpy(state_name, state->name);
#endif
    } else {
        strcpy(state_name, no_state);
    }
    return 1;
}

static int next_state_onactivate(event_handler_t *handler, menu_event_t event, void **event_data) {
    kz.state_slot++;
    kz.state_slot %= STATE_MAX;
    kz_state_hdr_t *state = kz.states[kz.state_slot];
    if(kz.states[kz.state_slot] != NULL) {
#ifdef WIIVC
        hmemcpy(state_name, state->name, 64);
#else
        strcpy(state_name, state->name);
#endif
    } else {
        strcpy(state_name, no_state);
    }

    return 1;
}

static int name_onupdate(event_handler_t *handler, menu_event_t event, void **event_data) {
    kz_state_hdr_t *state = kz.states[kz.state_slot];
    menu_item_t *item = handler->subscriber;
    if(event == MENU_EVENT_UPDATE) {
        if(kz.states[kz.state_slot] != NULL) {
#ifdef WIIVC
            hmemcpy(state_name, state->name, 64);
#else
            strcpy(state_name, state->name);
#endif
            item->interactive = 1;
            item->color = 0xFFFFFFFF;
        } else {
            strcpy(state_name, no_state);
            item->interactive = 0;
            item->color = COLOR_FADED;
        }
    } else if (event == MENU_EVENT_ACTIVATE) {
        menu_keyboard_get(item, &state_name);
    } else if(event == MENU_EVENT_KEYBOARD) {
#ifdef WIIVC
        hmemcpy(state->name, state_name, 64);
#else
        strcpy(state->name, state_name);
#endif
    }

    return 1;
}

#ifdef LITE
static char state_mem_text[40];

static int state_mem_onupdate(event_handler_t *handler, menu_event_t event, void **event_data) {
    size_t total;
    mem_largest_free(&total);
    kz_state_hdr_t *state = kz.states[kz.state_slot];
    int state_kb = state != NULL ? (state->size + 1023) / 1024 : 0;
    snprintf(state_mem_text, sizeof(state_mem_text), "size %dkb free %dkb", state_kb, (int)(total / 1024));
    return 1;
}

static int file_write(void *ctx, const void *buf, size_t len){
    return write(*(int*)ctx, (void*)buf, len) == len;
}

static int file_read(void *ctx, void *buf, size_t len){
    return read(*(int*)ctx, buf, len) == len;
}

static void do_export_state(char *path, void *data){
    int file = creat(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(file == -1) {
        kz_log("could not create file");
        return;
    }

    /* the compressed state, header included */
    if(!state_write_file(kz.states[kz.state_slot], file_write, &file)) {
        kz_log("could not write state");
    } else {
        kz_log("exported state %d", kz.state_slot);
    }
    close(file);
}

static void do_import_state(char *path, void *data){
    int file = open(path, O_RDONLY);
    if(file == -1) {
        kz_log("could not open file");
        return;
    }

    struct stat sbuf;
    kz_state_hdr_t hdr;
    if(stat(path, &sbuf) != 0 || sbuf.st_size < sizeof(hdr) || !file_read(&file, &hdr, sizeof(hdr))) {
        kz_log("could not read state");
        close(file);
        return;
    }

    int state_res = state_is_valid(&hdr);
    if(state_res != 0 || hdr.size != sbuf.st_size) {
        kz_log(state_res == -1 ? "wrong mm version" : "wrong state version");
        close(file);
        return;
    }

    /* keep the state in this slot unless its memory is needed */
    int error;
    kz_state_hdr_t *state = state_read_file(&hdr, file_read, &file, &error);
    if(state == NULL && error == STATE_READ_MEMORY && kz.states[kz.state_slot] != NULL) {
        state_free(kz.states[kz.state_slot]);
        kz.states[kz.state_slot] = NULL;
        /* nothing was read yet, so the file is still positioned after the header */
        state = state_read_file(&hdr, file_read, &file, &error);
    }
    close(file);

    if(state == NULL) {
        if(error == STATE_READ_MEMORY) {
            kz_log("not enough memory for state %d", kz.state_slot);
        } else {
            kz_log("could not read state");
        }
        return;
    }

    state_free(kz.states[kz.state_slot]);
    kz.states[kz.state_slot] = state;
    kz.state_macro_frame[kz.state_slot] = -1;
    kz_log("imported to state %d", kz.state_slot);
}
#else
static void do_export_state(char *path, void *data){
    int file = creat(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(file != -1){
        kz_state_hdr_t *state = kz.states[kz.state_slot];

        write(file, state, sizeof(*state));
        write(file, (char*)state + sizeof(*state), state->size);
        kz_log("export from state %d", kz.state_slot);
        close(file);
    }

}

static void do_import_state(char *path, void *data){
    int file = open(path, O_RDONLY);
    if(file != -1){
        kz_state_hdr_t *state = kz.states[kz.state_slot];
        if(state != NULL) {
#ifdef WIIVC
            hfree(state);
#else
            free(state);
#endif
        }

        struct stat sbuf;
        stat(path, &sbuf);
#ifdef WIIVC
        state = halloc(sbuf.st_size);
#else
        state = malloc(sbuf.st_size);
#endif
        read(file, (char*)state, sizeof(*state));
        int state_res = state_is_valid(state);
        if(state_res == 0) {
            read(file, (char*)state + sizeof(*state), sbuf.st_size - sizeof(*state));
            kz.states[kz.state_slot] = state;
            kz.state_macro_frame[kz.state_slot] = -1;
            kz_log("imported to state %d", kz.state_slot);
        } else {
#ifdef WIIVC
            hfree(state);
#else
            free(state);
#endif
            kz_log(state_res == -1 ? "wrong mm version" : "wrong state version");
        }
        close(file);
    }

}

#endif

static int export_state_onactivate(event_handler_t *handler, menu_event_t event, void **event_data)
{
    if(!kz.states[kz.state_slot])
    {
        kz_log("no state %d", kz.state_slot);
        return 1;
    }

    menu_file_get(FILE_MODE_SAVE, state_name, ".kzs", do_export_state, NULL);
    return 1;
}

static int import_state_onactivate(event_handler_t *handler, menu_event_t event, void **event_data)
{
    menu_file_get(FILE_MODE_LOAD, "state", ".kzs", do_import_state, NULL);
    return 1;
}

static int delete_state_onactivate(event_handler_t *handler, menu_event_t event, void **event_data) {
    if(kz.states[kz.state_slot]) {
#ifdef WIIVC
        hfree(kz.states[kz.state_slot]);
#elif defined(LITE)
        state_free(kz.states[kz.state_slot]);
#else
        free(kz.states[kz.state_slot]);
#endif
        kz.states[kz.state_slot] = NULL;
    } else {
        kz_log("no state %d", kz.state_slot);
    }

    return 1;
}

menu_t *create_states_menu(void) {
    static menu_t states_menu;
    if(state_name == NULL) {
        state_name = malloc(64);
    }
    memset(state_name, 0, 64);

    strcpy(state_name, no_state);

    menu_init(&states_menu, 0, 0);
    states_menu.selected_item = menu_button_add(&states_menu, 0, 0, "return", menu_return, NULL);
    menu_label_add(&states_menu, 0, 1, "cur state");
    menu_button_add(&states_menu, 10, 1, "<", prev_state_onactivate, NULL);
    static watch_t state_watch = {
        &kz.state_slot, WATCH_TYPE_S32, 0, 0, 0, NULL
    };
    menu_watch_add(&states_menu, 12, 1, &state_watch, 1);
    menu_button_add(&states_menu, 14, 1, ">", next_state_onactivate, NULL);
    menu_item_t *name_item = menu_label_add(&states_menu, 0, 2, state_name);
    name_item->color = COLOR_FADED;
    menu_item_register_event(name_item, MENU_EVENT_UPDATE | MENU_EVENT_ACTIVATE | MENU_EVENT_KEYBOARD, name_onupdate, NULL);

#ifdef LITE
    menu_item_t *mem_item = menu_label_add(&states_menu, 0, 3, state_mem_text);
    mem_item->color = COLOR_FADED;
    menu_item_register_event(mem_item, MENU_EVENT_UPDATE, state_mem_onupdate, NULL);
#endif
    menu_gfx_button_add(&states_menu, 0, 4, pexport_sprite, export_state_onactivate, NULL)->tooltip = "export state";
    menu_gfx_button_add(&states_menu, 4, 4, pimport_sprite, import_state_onactivate, NULL)->tooltip = "import state";

    static menu_sprite_t clear_state_sprite = {
        NULL,   2,  2,  DEFAULT_COLOR,  DEFAULT_COLOR,
        12,      12,      NULL,           0,
        0,      0,      NULL
    };

    clear_state_sprite.texture = resource_get(R_KZ_SD);
    menu_gfx_button_add(&states_menu, 8, 4, &clear_state_sprite, delete_state_onactivate, NULL)->tooltip = "clear state";
    return &states_menu;
}
