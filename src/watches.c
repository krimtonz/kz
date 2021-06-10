#include <stdio.h>
#include "gfx.h"
#include "watches.h"
#include "printf.h"

void watch_printf(watch_t *watch, uint32_t color)
{
    char buf[16] = { 0 };
    void *address = watch->address;
    switch(watch->type)
    {
        case WATCH_TYPE_U8:
            sprintf(buf, "%u", *(uint8_t*)address);
            break;
        case WATCH_TYPE_S8:
            sprintf(buf, "%d", *(int8_t*)address);
            break;
        case WATCH_TYPE_X8:
            sprintf(buf, "%1X", *(uint8_t*)address);
            break;
        case WATCH_TYPE_U16:
            sprintf(buf, "%u", *(uint16_t*)address);
            break;
        case WATCH_TYPE_S16:
            sprintf(buf, "%d", *(int16_t*)address);
            break;
        case WATCH_TYPE_X16:
            sprintf(buf, "%2X", *(uint16_t*)address);
            break;
        case WATCH_TYPE_U32:
            sprintf(buf, "%lu", *(uint32_t*)address);
            break;
        case WATCH_TYPE_S32:
            sprintf(buf, "%ld", *(int32_t*)address);
            break;
        case WATCH_TYPE_X32:
            sprintf(buf, "%8lX", *(uint32_t*)address);
            break;
        case WATCH_TYPE_FLOAT:
        {
            union {
                uint32_t uv;
                float fv;
            } comp;

            comp.uv = *(uint32_t*)address;

            /* moderately annoying workaround for double rounding issues in _etoa and _ftoa, 
             * this doesn't solve the rounding issues, but rather fixes a common case of 0.0 */
            if(comp.uv == 0) {
                sprintf(buf, "%f", comp.fv);
            } else {
                sprintf(buf, "%g", comp.fv);
            }
        }
        break;
    }

    gfx_printf_color(watch->x, watch->y, color, "%s", buf);
    if(watch->label && watch->floating)
    {
        gfx_printf_color(watch->x + ((strlen(buf) + 1) * 8), watch->y, color, "%s", watch->label);
    }
}