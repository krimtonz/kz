#include <n64.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <libundermine.h>
#include "kzgfx.h"
#include "kzresource.h"
#include "zu.h"

#define TILE_SIZE(x,y,z)    ((x * y * G_SIZ_BITS(z) + 63) / 64 * 8)

gfx_texture *gfx_load_item_texture(uint8_t item_id){
    size_t size = TILE_SIZE(32, 32, G_IM_SIZ_32b);
    gfx_texture *texture = malloc(sizeof(*texture));
    if(texture){
        texture->tile_size = size;
        texture->x_tiles = 1;
        texture->y_tiles = 2;
        texture->tile_width = 32;
        texture->tile_height = 32;
        texture->img_fmt = G_IM_FMT_RGBA;
        texture->img_size = G_IM_SIZ_32b;
        texture->data = malloc(size * 2);
        z2_DecodeArchiveFile(z2_file_table[z2_item_icon_archive].prom_start, item_id, texture->data, size);
        memcpy((char*)texture->data + size, (char*)texture->data,size);
        gfx_texture_desaturate((char*)texture->data + size, size);
    }
    return texture;
}

static gfx_texture *gfx_load_archive(texture_loader *loader){
    gfx_texture *texture = malloc(sizeof(*texture));
    if(texture){
        int tiles_cnt = loader->end_item - loader->start_item + 1;
        size_t tile_size = TILE_SIZE(loader->width, loader->height, loader->size);
        texture->tile_size = tile_size;
        texture->img_fmt = loader->format;
        texture->img_size = loader->size;
        texture->tile_width = loader->width;
        texture->tile_height = loader->height;
        texture->x_tiles = 1;
        texture->y_tiles = tiles_cnt * (loader->desaturate ? 2 : 1);

        texture->data = malloc((tile_size * tiles_cnt) * (loader->desaturate ? 2 : 1));
        int i;
        uint8_t j;
        for(i = 0, j = loader->start_item;i < tiles_cnt;i++, j++){
            z2_DecodeArchiveFile(z2_file_table[loader->file].prom_start, j, (char*)texture->data + (tile_size * i), tile_size);
            if(loader->desaturate){
                // Copy newly decoded file and desaturate
                memcpy((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i), (char*)texture->data + (tile_size * i), tile_size);
                gfx_texture_desaturate((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i), tile_size);
            }
        }
    }
    return texture;
}

static gfx_texture *gfx_load_from_rom(texture_loader *loader){
    gfx_texture *texture = malloc(sizeof(*texture));
    if(texture){
        size_t tile_size = TILE_SIZE(loader->width, loader->height, loader->size);
        texture->tile_size = tile_size;
        texture->data = malloc((texture->tile_size * loader->num_tiles) * (loader->desaturate ? 2 : 1));
        texture->img_fmt = loader->format;
        texture->img_size = loader->size;
        texture->tile_width = loader->width;
        texture->tile_height = loader->height;
        texture->x_tiles = 1;
        texture->y_tiles = loader->num_tiles * (loader->desaturate ? 2 : 1);
        uint16_t file = loader->file;
        void *tempdata = malloc(z2_file_table[file].vrom_end - z2_file_table[file].vrom_start);
        zu_file_idx_load(file, tempdata);
        memcpy(texture->data, (char*)tempdata + loader->offset,tile_size * loader->num_tiles);
        free(tempdata);
        if(loader->desaturate){
            int tiles_cnt = loader->num_tiles;
            for(int i = 0;i < tiles_cnt;i++){
                memcpy((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i), (char*)texture->data + (tile_size * i), tile_size);
                gfx_texture_desaturate((char*)texture->data + (tile_size * tiles_cnt) + (tile_size * i),tile_size);
            }
        }
    }
    return texture;
}

gfx_texture *gfx_load(texture_loader *loader){
    switch(loader->source){
        case SOURCE_ARCHIVE:
            return gfx_load_archive(loader);
        case SOURCE_FILE:
            return gfx_load_from_rom(loader);
        default:
            return NULL;
    }
}
