#include <stdlib.h>
#include "settings.h"
#include "io.h"
#include "kz.h"

static _Alignas(128) struct settings settings_info;
struct settings_data *settings = &settings_info.data;

void load_default_settings(){
    settings_info.header.magic[0] = 'k';
    settings_info.header.magic[1] = 'z';
    settings_info.header.magic[2] = 'k';
    settings_info.header.magic[3] = 'z';
    settings_info.header.version = SETTINGS_VER;
    settings->watch_cnt = 0;
}

void save_settings_to_flashram(int profile){
    settings->watch_cnt = kz.watch_cnt;
    for(int i=0;i<kz.watch_cnt;i++){
        watch_t *watch = vector_at(&kz.watches,i);
        settings->watch_addresses[i] = watch->address;
        settings->watch_x[i] = watch->x;
        settings->watch_y[i] = watch->y;
        settings->watch_info[i].floating = watch->floating;
        settings->watch_info[i].type = watch->type;
    }
    kz_io(&settings_info,SETTINGS_ADDR + (profile * sizeof(settings_info)),sizeof(*settings),OS_WRITE);
}

void load_settings_from_flashram(int profile){
    struct settings settings_temp;
    kz_io(&settings_temp,SETTINGS_ADDR + (profile * sizeof(settings_temp)),sizeof(settings_temp),OS_READ);
    if(settings_temp.header.magic[0] == 'k' && settings_temp.header.magic[1]=='z' && settings_temp.header.magic[2] == 'k' && settings_temp.header.magic[3] == 'z'){
        memcpy((void*)&settings_info,(void*)&settings_temp,sizeof(settings_temp));
    }else{
        load_default_settings();
    }
}

void kz_apply_settings(){
    kz.watch_cnt = settings->watch_cnt;
    for(int i=0;i<kz.watch_cnt;i++){
        watch_t *watch = vector_push_back(&kz.watches,1,NULL);
        watch->address = (void*)settings->watch_addresses[i];
        watch->x = settings->watch_x[i];
        watch->y = settings->watch_y[i];
        watch->floating = settings->watch_info[i].floating;
        watch->type = settings->watch_info[i].type;
    }
}
