/*
* fat.h
*
* definitions for the fat file system
*/

#ifndef LITE
#ifndef _FAT_H
#define _FAT_H

#include <stdint.h>
#include <list/list.h>

#define FAT_MAX_CACHE_SECTOR    4

#define FAT_ATTRIBUTE_DEFAULT    0x00
#define FAT_ATTRIBUTE_READONLY   0x01
#define FAT_ATTRIBUTE_HIDDEN     0x02
#define FAT_ATTRIBUTE_SYSTEM     0x04
#define FAT_ATTRIBUTE_LABEL      0x08
#define FAT_ATTRIBUTE_DIRECTORY  0x10
#define FAT_ATTRIBUTE_ARCHIVE    0x20
#define FAT_ATTRIBUTE_DEVICE     0x40

enum fat_io {
    FAT_READ,
    FAT_WRITE,
};

enum fat_type {
    FAT12,
    FAT16,
    FAT32
};

enum fat_cache_type {
    FAT_CACHE_FAT,
    FAT_CACHE_DATA,
    FAT_CACHE_MAX
};

typedef struct {
    _Bool       valid;
    _Bool       dirty;
    uint32_t    max_lba;
    uint32_t    load_lba;
    uint32_t    prep_lba;
    int         sector_cnt;
    _Alignas(0x10)
    char        data[0x800];
} fat_cache_t;

typedef struct {
    enum fat_type       type;
    uint32_t            partition_lba;
    uint32_t            partition_sectors;
    uint32_t            bytes_per_sector;
    uint32_t            sectors_per_cluster;
    uint32_t            reserved_sectors;
    uint32_t            number_of_fat;
    uint16_t            entry_cnt;
    uint32_t            fs_sector_cnt;
    uint32_t            fat_sector_cnt;
    uint32_t            root_cluster;
    uint32_t            fsis_lba;
    uint32_t            fat_lba;
    uint32_t            root_lba;
    uint32_t            data_lba;
    uint32_t            bytes_per_cluster;
    uint32_t            max_cluster;
    uint32_t            blocks_free;
    fat_cache_t         cache[FAT_CACHE_MAX];
} fat_ctxt_t;

typedef struct {
    fat_ctxt_t *fat_ctxt;
    uint32_t    cluster;
    uint32_t    size;
    _Bool       is_dir;
    uint32_t    p_offset;
    uint32_t    p_cluster;
    uint32_t    p_cluster_seq;
    uint32_t    p_cluster_sector;
    uint32_t    p_sector_offset;
} fat_file_t;

typedef struct {
    struct list entry_list;
} fat_path_t;

typedef struct {
    fat_ctxt_t *fat_ctxt;
    fat_file_t  first;
    fat_file_t  last;
    char        short_name[13];
    char        long_name[256];
    time_t      create;
    int         cms;
    time_t      access_time;
    time_t      modify_time;
    uint8_t     attributes;
    uint32_t    cluster;
    uint32_t    size;
} fat_entry_t;

int             fat_init        (fat_ctxt_t *fat);
fat_entry_t    *fat_path_target (fat_path_t *fat_path);
fat_path_t     *fat_path        (fat_ctxt_t *fat, fat_path_t *dir, const char *path, const char **tail);
void            fat_rewind      (fat_file_t *file);
void            fat_begin       (fat_entry_t *entry, fat_file_t *file);
fat_path_t     *fat_create_path (fat_ctxt_t *fat, fat_path_t *dir, const char *path, uint8_t attributes);
int             fat_create      (fat_ctxt_t *fat, fat_entry_t *dir, const char *path, uint8_t attributes, fat_entry_t *entry);
void            fat_free        (fat_path_t *fp);
int             fat_resize      (fat_entry_t *entry, uint32_t size, fat_file_t *file);
void            fat_root        (fat_ctxt_t *fat, fat_file_t *file);
int             fat_dir         (fat_file_t *dir, fat_entry_t *entry);
uint32_t        fat_advance     (fat_file_t *file, uint32_t byte_cnt, _Bool *eof);
uint32_t        fat_rw          (fat_file_t * file, enum fat_io rw, void *buf, uint32_t byte_cnt, fat_file_t *new_file, _Bool *eof);
int             fat_find        (fat_ctxt_t *fat, fat_entry_t *dir, const char *path, fat_entry_t *entry);
int             fat_flush       (fat_ctxt_t *fat);
int             fat_remove      (fat_entry_t *entry);
int             fat_rename      (fat_ctxt_t *fat, fat_path_t *ent_fp, fat_path_t *dir_fp, const char *path, fat_entry_t *new_ent);
int             fat_attribute   (fat_entry_t *ent, uint8_t attribute);
int             dir_find        (fat_ctxt_t *fat, uint32_t cluster, const char *name, fat_entry_t *entry);

#endif
#endif