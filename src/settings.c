#include "settings.h"
#include "io.h"
#include "kz.h"

void load_default_settings(struct settings *settings){
    settings->magic[0] = 'k';
    settings->magic[1] = 'z';
    settings->magic[2] = 'k';
    settings->magic[3] = 'z';
    settings->version = SETTINGS_VER;
    settings->watch_cnt = 0;
}

void save_settings_to_flashram(struct settings *settings){
    settings->watch_cnt = kz.watch_cnt;
    for(int i=0;i<kz.watch_cnt;i++){
        watch_t *watch = vector_at(&kz.watches,i);
        settings->watch_addresses[i] = (uint32_t)watch->address;
        settings->watch_x[i] = watch->x;
        settings->watch_y[i] = watch->y;
        settings->watch_info[i].floating = watch->floating;
        settings->watch_info[i].type = watch->type;
    }
    kz_io(settings,KZ_START_BLOCK,sizeof(*settings),OS_WRITE);
}

void load_settings_from_flashram(struct settings *settings){
    struct settings settings_temp;
    kz_io(&settings_temp,KZ_START_BLOCK,sizeof(settings_temp),OS_READ);
    if(settings_temp.magic[0] == 'k' && settings_temp.magic[1]=='z' && settings_temp.magic[2] == 'k' && settings_temp.magic[3] == 'z'){
        memcpy((void*)settings,(void*)&settings_temp,sizeof(settings_temp));
    }else{
        load_default_settings(settings);
        save_settings_to_flashram(settings);
    }
}

void kz_apply_settings(){
    if(kz.settings){
        kz.watch_cnt = kz.settings->watch_cnt;
        for(int i=0;i<kz.watch_cnt;i++){
            watch_t *watch = vector_push_back(&kz.watches,1,NULL);
            watch->address = (void*)kz.settings->watch_addresses[i];
            watch->x = kz.settings->watch_x[i];
            watch->y = kz.settings->watch_y[i];
            watch->floating = kz.settings->watch_info[i].floating;
            watch->type = kz.settings->watch_info[i].type;
        }
    }
}
