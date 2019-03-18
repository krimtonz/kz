#include <stdio.h>
#include "watches.h"

void watch_printf(watch_t *watch, char *buf){
    if(!buf){
        return;
    }
    switch(watch->type){
        case WATCH_TYPE_U8:
            sprintf(buf,"%u",*(uint8_t*)watch->address);
            break;
        case WATCH_TYPE_S8:
            sprintf(buf,"%d",*(int8_t*)watch->address);
            break;
        case WATCH_TYPE_X8:
            sprintf(buf,"%1X",*(uint8_t*)watch->address);
            break;
        case WATCH_TYPE_U16:
            sprintf(buf,"%u",*(uint16_t*)watch->address);
            break;
        case WATCH_TYPE_S16:
            sprintf(buf,"%d",*(int16_t*)watch->address);
            break;
        case WATCH_TYPE_X16:
            sprintf(buf,"%2X",*(uint16_t*)watch->address);
            break;
        case WATCH_TYPE_U32:
            sprintf(buf,"%lu",*(uint32_t*)watch->address);
            break;
        case WATCH_TYPE_S32:
            sprintf(buf,"%ld",*(int32_t*)watch->address);
            break;
        case WATCH_TYPE_X32:
            sprintf(buf,"%8lX",*(uint32_t*)watch->address);
            break;
        case WATCH_TYPE_FLOAT:
            sprintf(buf,"%f",*(float*)watch->address);
            break;
    }
}