#ifndef LITE
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "fat.h"
#include "sys.h"

enum sfn_case{
    SFN_CASE_ANY,
    SFN_CASE_LOWER,
    SFN_CASE_UPPER,
};

static _Bool name_compare(const char *a, const char *b){
    while(*a && *b){
        char ca = *a++;
        char cb = *b++;
        if(ca >= 'a' && ca <= 'z'){
            ca += 'A' - 'a';
        }
        if(cb >= 'a' && cb <= 'z'){
            cb += 'A' - 'a';
        }
        if(ca != cb){
            return 0;
        }
    }
    return !*a && !*b;
}

static size_t name_trim(const char *s, size_t len){
    while(len > 0 && (s[len - 1] == ' ' || s[len - 1] == '.')){
        len--;
    }
    return len;
}

static void name_split(const char *s, const char **name, const char **ext, int *name_len, int *ext_len){
    const char *end = s + name_trim(s,strlen(s));
    const char *dot = NULL;
    for(const char *c = end - 1;c >= s;c--){
        if(*c == '.'){
            dot = c;
            break;
        }
    }
    *name = s;
    if(dot){
        *name_len = dot - *name;
        *ext = dot + 1;
        *ext_len = end - *ext;
    }
    else{
        *name_len = end - *name;
        *ext = NULL;
        *ext_len = 0;
    }
}

static _Bool char_is_short_file_name(char c, enum sfn_case *cse){
    if(c >= 'A' && c <= 'Z'){
        if(cse){
            if(*cse == SFN_CASE_LOWER){
                return 0;
            }else if(*cse == SFN_CASE_ANY){
                *cse = SFN_CASE_UPPER;
            }
        }
    }
    if(c >= 'a' && c <= 'z'){
        if(cse){
            if(*cse == SFN_CASE_UPPER){
                return 0;
            }else if(*cse == SFN_CASE_ANY){
                *cse = SFN_CASE_LOWER;
            }
        }
        c += 'A' - 'a';
    }
    return (c >= '@' && c <= 'Z') || (c >= '0' && c <= '9') ||
           (c >= '#' && c <= ')') || (c >= '^' && c <= '`') ||
           (c >= '\x80' && c <= '\xFF') ||
           c == ' ' || c == '!' || c == '-' || c == '{' || c == '}' || c == '~';
}

static int convert_long_name_to_short_name(const char *s, int s_len, char *buf, int buf_len){
    int p = 0;
    for(int i = 0;i < s_len && p < buf_len;i++){
        char c = s[i];
        if(c >= 'a' && c <= 'z'){
            c+='A' - 'a';
        }else if(c == '.' || c == ' '){
            continue;
        }else if(!char_is_short_file_name(c,NULL)){
            c='_';
        }
        buf[p++] = c;
    }
    int len = p;
    while(p<buf_len){
        buf[p++] = ' ';
    }
    return len;
}

static void convert_upper(char *s, int len){
    while(len-->0){
        if(*s >= 'a' && *s <= 'z'){
            *s += 'A' -'a';
        }
        s++;
    }
}

static void convert_lower(char *s, int len){
    while(len-->0){
        if(*s >= 'A' && *s<='Z'){
            *s += 'a' - 'A';
        }
        s++;
    }
}

static void convert_83(const char *s, char *short_file_name){
    memset(short_file_name, ' ', 11);
    if(strcmp(s,".") == 0){
        short_file_name[0] = '.';
    }else if(strcmp(s,"..") == 0){
        short_file_name[0] = '.';
        short_file_name[1] = '.';
    }else{
        const char *name;
        const char *ext;
        int name_len;
        int ext_len;
        name_split(s,&name, &ext, &name_len,&ext_len);
        memcpy(&short_file_name[0],name,name_len);
        memcpy(&short_file_name[8],ext,ext_len);
        convert_upper(short_file_name,11);
        if(short_file_name[0] == '\xE5'){
            short_file_name[0] = '\x05';
        }
    }
}

static uint8_t compute_long_checksum(const char *short_file_name){
    uint8_t *p = (void*)short_file_name;
    uint8_t checksum = 0;
    for(int i = 0;i < 11;i++){
        checksum = ((checksum & 1) << 7) + (checksum >> 1) + p[i];
    }
    return checksum;
}

static void unix2dos(time_t time, uint16_t *dos_date, uint16_t *dos_time){
    int sec = time % 60;
    time /= 60;
    int min = time % 60;
    time /= 60;
    int hr = time % 24;
    time /= 24;
    time += 719468;
    int era = (time > 0 ? time : time - 146096) / 146097;
    int doe = time - era * 146097;
    int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int y = yoe + era * 400;
    int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    int mp = (5 * doy + 2) / 153;
    int d = doy - (153 * mp + 2 ) / 5 + 1;
    int m = mp + (mp < 10 ? 3 : -9);
    y += (m <= 2);
    if(y < 1980 || y > 2107){
        if(dos_date){
            *dos_date = 0;
        }
        if(dos_time){
            *dos_time = 0;
        }
    }
    else{
        if(dos_date){
            *dos_date = ((uint16_t)(y - 1980) << 9) | ((uint16_t)m << 5) | ((uint16_t)d << 0);
        }
        if(dos_time){
            *dos_time = ((uint16_t)hr << 11) | ((uint16_t)min << 5) | ((uint16_t)(sec / 2) << 0);
        }
    }
}

static time_t dos2unix(uint16_t dos_date, uint16_t dos_time){
    int y = 1980 + ((dos_date >> 9) & 0x7F);
    int m = (int)((dos_date >> 5) & 0xF);
    int d = (int)((dos_date >> 0) & 0x1F);
    int hr = (dos_time >> 11) & 0x1F;
    int min = (dos_time >> 5) & 0x3F;
    int sec = ((dos_time >> 0) & 0x1F) * 2;
    if(m < 1 || m > 12 || d < 1 || d > 31 || hr > 23 || min > 59 || sec > 59){
        return 0;
    }
    y -= (m <= 2);
    int era = (y >=0 ? y : y - 399) / 400;
    int yoe = y - era * 400;
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) /5 + d - 1;
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return (era * 146097 + doe - 719468) * 86400 + hr * 3600 + min * 60 + sec;
}

static _Bool validate_short_file_name(const char *short_file_name){
    if(short_file_name[0] == '.'){
        for(int i = 2;i < 11;i++){
            if(short_file_name[i] != ' '){
                return 0;
            }
        }
        return short_file_name[1] == '.' || short_file_name[1] == ' ';
    }
    enum sfn_case cse = SFN_CASE_UPPER;
    for(int i = 0;i < 11;i++){
        if(!char_is_short_file_name(short_file_name[i],&cse)){
            return 0;
        }
    }
    return 1;
}

static int generate_short_file_name(fat_ctxt_t *fat, uint32_t cluster, const char *long_file_name, char *short_file_name){
    const char *name;
    const char *ext;
    int name_len;
    int ext_len;
    name_split(long_file_name,&name, &ext, &name_len, &ext_len);
    int short_file_name_len = convert_long_name_to_short_name(name,name_len,&short_file_name[0],8);
    int short_file_name_ext_len = convert_long_name_to_short_name(ext,ext_len,&short_file_name[8],3);
    for(int i = 1;i<1000000;i++){
        char short_file_name_disc[8];
        sprintf(short_file_name_disc,"%i",i);
        int short_file_name_disc_len = strlen(short_file_name_disc);
        int short_file_name_nd_len = 7 - short_file_name_len;
        if(short_file_name_nd_len > short_file_name_len){
            short_file_name_nd_len = short_file_name_len;
        }
        char name_buf[13];
        sprintf(name_buf,"%.*s~%s.%.*s",short_file_name_nd_len,&short_file_name[0],short_file_name_disc,short_file_name_ext_len,&short_file_name[8]);
        int e = errno;
        if(dir_find(fat,cluster,name_buf,NULL) == 0){
            continue;
        }
        else if(errno != ENOENT){
            return -1;
        }
        errno = e;
        short_file_name[short_file_name_nd_len] = '~';
        memcpy(&short_file_name[short_file_name_nd_len+1],short_file_name_disc,short_file_name_disc_len);
        return 0;
    }
    errno = EEXIST;
    return -1;
}

static int get_short_file_name(const char *short_name, char *buf, int *name_len, int *ext_len){
    int nl = 8;
    int el = 3;
    while(nl > 0 && short_name[nl - 1] == ' '){
        nl--;
    }
    while(el > 0 && short_name[8 + el - 1] == ' '){
        el--;
    }
    memcpy(&buf[0],&short_name[0],nl);
    int l = nl;
    if(el > 0){
        buf[nl] = '.';
        memcpy(&buf[nl + 1], &short_name[8], el);
        l += el + 1;
    }
    buf[l] = 0;
    if(name_len){
        *name_len = nl;
    }
    if(ext_len){
        *ext_len = el;
    }
    return l;
}

static _Bool name_is_short(const char *s, _Bool *lower_name, _Bool *lower_extension){
    if(strcmp(s,".") == 0 || strcmp(s,"..") == 0){
        if(lower_name){
            *lower_name = 0;
        }
        if(lower_extension){
            *lower_extension = 0;
        }
        return 1;
    }
    const char *name;
    const char *extension;
    int name_len;
    int extension_len;
    name_split(s,&name,&extension,&name_len,&extension_len);
    if(name_len == 0 || name_len > 8 || (extension && extension_len > 3)){
        return 0;
    }
    if(name[name_len - 1] == ' ' || (extension && extension[extension_len - 1] == ' ')){
        return 0;
    }
    enum sfn_case name_cse = SFN_CASE_ANY;
    enum sfn_case ext_cse = SFN_CASE_ANY;
    for(int i=0;i<name_len;i++){
        if(!char_is_short_file_name(name[i],&name_cse)){
            return 0;
        }
    }
    for(int i=0;i<extension_len;i++){
        if(!char_is_short_file_name(extension[i],&ext_cse)){
            return 0;
        }
    }
    if(lower_name){
        *lower_name = (name_cse == SFN_CASE_LOWER);
    }
    if(lower_extension){
        *lower_extension = (ext_cse == SFN_CASE_LOWER);
    }
    return 1;
}

static uint32_t get_word(const void *buf, uint32_t offset, size_t len){
    const uint8_t *p = buf;
    uint32_t ret = 0;
    for(int i = 0;i < len;i++){
        ret |= (uint32_t)p[offset + i] << (i * 8);
    }
    return ret;
}

static void set_word(void *buf, uint32_t offset, size_t len, uint32_t value){
    uint8_t *p = buf;
    for(int i = 0;i < len;i++){
        p[offset + i] = value >> (i * 8);
    }
}

static void cache_dirty(fat_ctxt_t *fat, enum fat_cache_type type){
    fat->cache[type].dirty = 1;
}

static int cache_flush(fat_ctxt_t *fat, enum fat_cache_type type){
    fat_cache_t *cache = &fat->cache[type];
    if(!cache->valid || !cache->dirty){
        return 0;
    }
    if(fat->write(cache->data,cache->load_lba,cache->sector_cnt)){
        errno = EIO;
        return -1;
    }
    cache->dirty = 0;
    return 0;
}

static void *cache_prep(fat_ctxt_t *fat, enum fat_cache_type type, uint32_t lba, _Bool load){
    fat_cache_t *cache = &fat->cache[type];
    if(cache->valid && cache->load_lba <= lba && cache->load_lba + cache->sector_cnt > lba){
        cache->prep_lba = lba;
        uint32_t offset = fat->bytes_per_sector * (cache->prep_lba - cache->load_lba);
        return &cache->data[offset];
    }
    if(cache_flush(fat,type)){
        return NULL;
    }
    if(load){
        cache->sector_cnt = FAT_MAX_CACHE_SECTOR;
        if(lba + cache->sector_cnt > cache->max_lba){
            cache->sector_cnt = cache->max_lba - lba;
        }
        if(fat->read(cache->data,lba,cache->sector_cnt)){
            cache->valid = 0;
            errno = EIO;
            return NULL;
        }
    }else{
        cache->sector_cnt = 1;
    }
    cache->valid = 1;
    cache->dirty = 0;
    cache->load_lba = lba;
    cache->prep_lba = lba;
    return cache->data;
}

static void *cache_load(fat_ctxt_t *fat, enum fat_cache_type type, uint32_t lba){
    fat_cache_t *cache = &fat->cache[type];
    
    // loading from data in the cache already, and cache is valid
    if(cache->valid && cache->load_lba <= lba && cache->load_lba + cache->sector_cnt > lba){
        cache->prep_lba = lba;
        uint32_t offset = fat->bytes_per_sector * (cache->prep_lba - cache->load_lba);
        return &cache->data[offset];
    }

    cache_flush(fat,type);
    cache->sector_cnt = 4;
    if(lba + cache->sector_cnt > cache->max_lba){
        cache->sector_cnt = cache->max_lba - lba;
    }
    fat->read(cache->data,lba,cache->sector_cnt);
    cache->valid = 1;
    cache->dirty = 0;
    cache->load_lba = lba;
    cache->prep_lba = lba;
    return cache->data;
}

static void cache_read(fat_ctxt_t *fat, int index, uint32_t offset, void *dst, uint32_t len){
    fat_cache_t *cache = &fat->cache[index];
    offset += fat->bytes_per_sector * (cache->prep_lba - cache->load_lba);
    if(dst){
        memcpy(dst,&cache->data[offset],len);
    }
}

static void cache_write(fat_ctxt_t *fat, int index, uint32_t offset, const void *src, uint32_t len){
    fat_cache_t *cache = &fat->cache[index];
    offset += fat->bytes_per_sector * (cache->prep_lba - cache->load_lba);
    if(src){
        memcpy(&cache->data[offset],src,len);
    }
    else{
        memset(&cache->data[offset],0,len);
    }
    cache->dirty = 1;
}

static void cache_invalidate(fat_ctxt_t *fat, int index){
    fat->cache[index].valid = 0;
}

static void *file_data(fat_file_t *file){
    fat_ctxt_t *fat = file->fat_ctxt;
    fat_cache_t *cache = &fat->cache[FAT_CACHE_DATA];
    uint32_t offset = file->p_sector_offset;
    offset += fat->bytes_per_sector * (cache->prep_lba - cache->load_lba);
    return &cache->data[offset];
}

static int file_sector(fat_file_t *file, _Bool load){
    fat_ctxt_t *fat = file->fat_ctxt;
    uint32_t cluster_lba;
    if(file->p_cluster < 2){
        if(fat->type == FAT32){
            cluster_lba = fat->data_lba + (fat->root_cluster - 2) * fat->sectors_per_cluster;
        }
        else{
            cluster_lba = fat->root_lba;
        }
    }
    else{
        cluster_lba = fat->data_lba + (file->p_cluster - 2) * fat->sectors_per_cluster;
    }
    if(!cache_prep(fat,FAT_CACHE_DATA,cluster_lba + file->p_cluster_sector,load)){
        return -1;
    }
    return 0;
}

static int find_partition(fat_ctxt_t *fat){
    void *mbr = cache_load(fat,FAT_CACHE_DATA,0);
    if(get_word(mbr,0x1FE,2)!=0xAA55){
        errno = ENOENT;
        return -1;
    }
    uint8_t partition_type = get_word(mbr,0x1C2,1);
    if(partition_type != 0x01 && partition_type != 0x04 && partition_type != 0x06 &&
       partition_type != 0x0E && partition_type != 0x0B && partition_type != 0x0C){
           errno = ENOENT;
           return -1;
       }
    fat->partition_lba = get_word(mbr,0x1C6,4);
    fat->partition_sectors = get_word(mbr,0x1CA,4);
    if(fat->partition_lba == 0 || fat->partition_lba == 0 || fat->partition_sectors == 0){
        errno = ENOENT;
        return -1;
    }
    return 0;
}

static void make_root(fat_ctxt_t *fat, fat_entry_t *entry){
    memset(entry,0,sizeof(*entry));
    entry->fat_ctxt = fat;
    entry->attributes = FAT_ATTRIBUTE_DIRECTORY;
    if(fat->type!=FAT32){
        entry->size = fat->entry_cnt * 0x20;
    }
}

static int cluster_get(fat_ctxt_t *fat, uint32_t cluster, uint32_t *value){
    if(cluster>=fat->max_cluster){
        errno = EOVERFLOW;
        return -1;
    }
    if(fat->type == FAT12){
        // TODO
        return 0;
    }
    uint32_t entry_size = fat->type == FAT16 ? 2 : 4;
    uint32_t lba = cluster / (fat->bytes_per_sector / entry_size);
    uint32_t offset = cluster % (fat->bytes_per_sector / entry_size) * entry_size;
    void *block = cache_prep(fat,FAT_CACHE_FAT,fat->fat_lba + lba,1);
    if(!block){
        return -1;
    }
    if(fat->type == FAT16){
        *value = get_word(block,offset,2);
        if(*value >= 0xFFF7){
            *value |= 0x0FFF0000;
        }
    }else{
        *value = get_word(block,offset,4);
        *value &= 0x0FFFFFFF;
    }
    if(cluster == fat->blocks_free && value != 0){
        fat->blocks_free++;
    }
    return 0;
}

static int cluster_set(fat_ctxt_t *fat, uint32_t cluster, uint32_t value){
    if(cluster >= fat->max_cluster){
        errno = EOVERFLOW;
        return -1;
    }
    if(fat->type==FAT12){
        //TODO
        return 0;
    }
    uint32_t entry_size = fat->type == FAT16 ? 2 : 4;
    uint32_t lba = cluster / (fat->bytes_per_sector / entry_size);
    uint32_t offset = cluster % (fat->bytes_per_sector / entry_size) * entry_size;
    void *block = cache_prep(fat,FAT_CACHE_FAT,fat->fat_lba + lba,1);
    if(!block){
        return -1;
    }
    if(fat->type == FAT16){
        value &= 0x0000FFFF;
        set_word(block,offset,2,value);
    }else{
        value &= 0x0FFFFFFF;
        set_word(block,offset,4,value);
    }
    cache_dirty(fat,FAT_CACHE_FAT);
    if(cluster < fat->blocks_free && value == 0){
        fat->blocks_free = cluster;
    }
    else if(cluster == fat->blocks_free && value!=0){
        fat->blocks_free++;
    }
    return 0;
}

static uint32_t cluster_free_length(fat_ctxt_t *fat, uint32_t start, uint32_t preferred_len){
    if(start < 2){
        if(fat->type == FAT32){
            start = fat->root_cluster;
        }
        else{
            return 0;
        }
    }
    uint32_t len = 0;
    while(len < preferred_len && start < fat->max_cluster){
        uint32_t value = 0;
        if(cluster_get(fat, start, &value)){
            return 0;
        }
        if(value!=0){
            break;
        }
        len++;
        start++;
    }
    return len;
}

static uint32_t cluster_find_free(fat_ctxt_t *fat, uint32_t start, uint32_t preferred_len, uint32_t *len){
    if(start < fat->blocks_free){
        start = fat->blocks_free;
    }
    uint32_t max_cluster = 0;
    uint32_t max_len = 0;
    while(max_len < preferred_len && start < fat->max_cluster){
        uint32_t chunk_len = cluster_free_length(fat, start, preferred_len);
        if(chunk_len > max_len){
            max_cluster = start;
            max_len = chunk_len;
        }
        if(chunk_len > 0){
            start += chunk_len;
        }
        else{
            start++;
        }
    }
    if(max_len == 0){
        errno = ENOSPC;
    }
    if(len){
        *len = max_len;
    }
    return max_cluster;
}

static int cluster_check_free_space(fat_ctxt_t *fat, uint32_t needed){
    uint32_t free_cnt = 0;
    for(uint32_t i = fat->blocks_free;i < fat->max_cluster && free_cnt < needed;i++){
        uint32_t value = 0;
        if(cluster_get(fat,i,&value)){
            return -1;
        }
        if(value == 0x00000000){
            free_cnt++;
        }
    }
    if(free_cnt < needed){
        errno = ENOSPC;
        return -1;
    }
    return 0;
}

static int cluster_link(fat_ctxt_t *fat,uint32_t cluster,uint32_t next,_Bool end){
    if(cluster < 2){
        if(fat->type == FAT32){
            cluster = fat->root_cluster;
        }else{
            errno = EINVAL;
            return -1;
        }
    }
    if(cluster_set(fat,cluster,next)){
        return -1;
    }
    if(end){
        if(cluster_set(fat,next,0x0FFFFFFF)){
            return -1;
        }
    }
    return 0;
}

static uint32_t cluster_resize_chain(fat_ctxt_t *fat,uint32_t cluster,uint32_t cluster_cnt,uint32_t chunk_len){
    if(cluster < 2){
        if(fat->type == FAT32){
            cluster = fat->root_cluster;
        }else{
            errno = ENOSPC;
            return -1;
        }
    }
    _Bool end = 0;
    uint32_t alloc_cnt = 0;
    uint32_t new_cluster = 0;
    for(uint32_t i = 0;i < cluster_cnt || !end; i++){
        uint32_t value = 0;
        if(cluster_get(fat,cluster,&value)){
            return -1;
        }
        if((!end && value >= 0x0FFFFFF8) || value < 2){
            if(cluster_set(fat,cluster,0x0FFFFFFF)){
                return -1;
            }
            if(cluster_cnt > i+1){
                alloc_cnt = cluster_cnt - (i + 1);
                if(cluster_check_free_space(fat,alloc_cnt)){
                    return -1;
                }
            }
            end = 1;
        }
        if(i >= cluster_cnt){
            if(cluster_set(fat,cluster,0x00000000)){
                return -1;
            }
            cluster = value;
        }
        else if (i == cluster_cnt - 1){
            if(cluster_set(fat,cluster,0x0FFFFFFF)){
                return -1;
            }
            cluster = value;
        }
        else if(end){
            if(chunk_len == 0){
                new_cluster = cluster_find_free(fat,new_cluster,alloc_cnt,&chunk_len);
                if(new_cluster == cluster){
                    new_cluster = cluster_find_free(fat, cluster + 1,alloc_cnt,&chunk_len);
                }
                if(new_cluster == 0){
                    return -1;
                }
                alloc_cnt -= chunk_len;
            }else{
                new_cluster = cluster + 1;
            }
            if(cluster_link(fat,cluster,new_cluster,0)){
                return -1;
            }
            chunk_len--;
            cluster = new_cluster;
        }else{
            cluster = value;
        }
    }
    return 0;
}

static int cluster_advance(fat_ctxt_t *fat, uint32_t *cluster){
    uint32_t current = *cluster;
    if(current < 2){
        if(fat->type == FAT32){
            current = fat->root_cluster;
        }
        else{
            return 0;
        }
    }
    uint32_t next = 0;
    if(cluster_get(fat,current,&next)){
        return -1;
    }
    if(next < 2 || next >= 0x0FFFFFF8 || next == current){
        return 0;
    }
    *cluster = next;
    return 1;
}

static void dir_begin(fat_ctxt_t *fat, fat_file_t *file, uint32_t cluster){
    if(cluster < 2){
        fat_root(fat,file);
    }
    else{
        file->fat_ctxt = fat;
        file->cluster = cluster;
        file->size = 0;
        file->is_dir = 1;
        fat_rewind(file);
    }
}

int dir_find(fat_ctxt_t *fat, uint32_t cluster, const char *name, fat_entry_t *entry){
    fat_file_t pos;
    dir_begin(fat,&pos,cluster);
    _Bool is_short = name_is_short(name,NULL,NULL);
    fat_entry_t ent;
    int e = errno;
    errno = 0;
    while(fat_dir(&pos,&ent) == 0){
        if(ent.attributes & FAT_ATTRIBUTE_LABEL){
            continue;
        }
        _Bool is_match;
        if(is_short){
            is_match = name_compare(name,ent.short_name);
        }else{
            is_match = name_compare(name,ent.long_name);
        }
        if(is_match){
            if(entry){
                *entry = ent;
            }
            errno = e;
            return 0;
        }
    }
    if(errno == 0){
        errno = ENOENT;
    }
    return -1;
}

static int dir_insert(fat_ctxt_t *fat, uint32_t dir_cluster, const char *name, time_t create_time, int cms, time_t access_time, time_t modify_time, uint8_t attributes, uint32_t cluster, uint32_t size, fat_entry_t *entry){
    char long_file_name_buf[0x20];
    char short_file_name_buf[0x20];
    int name_len = strlen(name);
    int pent_cnt = 1;
    _Bool lower_name;
    _Bool lower_ext;
    if(name_is_short(name,&lower_name,&lower_ext)){
        convert_83(name,&short_file_name_buf[0x00]);
        uint8_t cse = 0x00;
        if(lower_name){
            cse |= 0x08;
        }
        if(lower_ext){
            cse |= 0x10;
        }
        set_word(short_file_name_buf,0x0C,1,cse);
    }
    else{
        pent_cnt += (name_len + 12) / 13;
        if(generate_short_file_name(fat,dir_cluster,name,&short_file_name_buf[0x00])){
            return -1;
        }
        set_word(short_file_name_buf,0x0C,1,0x00);
        set_word(long_file_name_buf,0x0B, 1,0x0F);
        set_word(long_file_name_buf,0x0C,1,0x00);
        set_word(long_file_name_buf,0x0D,1,compute_long_checksum(&short_file_name_buf[0x00]));
        set_word(long_file_name_buf,0x1A,2,0x0000);
    }
    fat_file_t start;
    fat_file_t pos;
    dir_begin(fat,&pos,dir_cluster);
    for(int free_cnt = 0;free_cnt < pent_cnt;){
        fat_file_t ent_p = pos;
        _Bool ate;
        if(fat_advance(&pos,0x20,&ate) != 0x20){
            if(!ate){
                return -1;  
            }
            pos = ent_p;
            uint32_t new_cluster = cluster_find_free(fat,0,1,NULL);
            if(new_cluster == 0){
                return -1;
            }
            if(cluster_link(fat,pos.p_cluster,new_cluster,1)){
                return -1;
            }
            fat_file_t cluster_pos;
            dir_begin(fat,&cluster_pos,new_cluster);
            if(fat_rw(&cluster_pos,FAT_WRITE,NULL,fat->bytes_per_cluster,NULL,NULL)!=fat->bytes_per_cluster){
                return -1;
            }
            continue;
        }
        uint8_t mark;
        if(fat_rw(&ent_p,FAT_READ,&mark,1,NULL,&ate) != 1){
            if(ate){
                errno = EINVAL;
            }
            return -1;
        }
        if(mark == 0x00 || mark == 0xE5){
            if(free_cnt == 0){
                start = ent_p;
            }
            free_cnt++;
        }else{
            free_cnt = 0;
        }
    }
    pos = start;
    for(int i = 0;i < pent_cnt - 1;i++){
        uint8_t seq = pent_cnt - 1 -i;
        if(i == 0){
            seq |= 0x40;
        }
        set_word(long_file_name_buf,0x00,1,seq);
        for(int j = 0;j < 13;j++){
            uint32_t p = 1 + j * 2;
            if(j >= 5){
                p += 3;
            }
            if(j >= 11){
                p += 2;
            }
            int n = 13 * (pent_cnt - 2 - i) + j;
            uint16_t c;
            if(n > name_len){
                c = 0xFFFF;
            }else if(n == name_len){
                c = 0x0000;
            }else{
                c = (uint8_t)name[n];
            }
            set_word(long_file_name_buf,p,2,c);
        }
        _Bool ate;
        if(fat_rw(&pos, FAT_WRITE, long_file_name_buf,0x20, &pos, &ate)!=0x20){
            if(ate){
                errno = EINVAL;
            }
            return -1;
        }    
    }
    set_word(short_file_name_buf,0x0B,1,attributes);
    cms += (create_time % 2) * 1000;
    create_time -= create_time % 2;
    create_time += cms / 2000;
    cms %= 2000;
    uint16_t dos_create_date;
    uint16_t dos_create_time;
    unix2dos(create_time,&dos_create_date,&dos_create_time);
    set_word(short_file_name_buf,0x0D,1,cms/10);
    set_word(short_file_name_buf,0x0E,2,dos_create_time);
    set_word(short_file_name_buf, 0x10,2,dos_create_time);
    uint16_t dos_access_date;
    unix2dos(access_time,&dos_access_date,NULL);
    set_word(short_file_name_buf,0x12,2,dos_access_date);
    set_word(short_file_name_buf,0x14,2, cluster >> 16);
    set_word(short_file_name_buf,0x1A,2,cluster);
    uint16_t dos_modify_date;
    uint16_t dos_modify_time;
    unix2dos(create_time,&dos_modify_date,&dos_modify_time);
    set_word(short_file_name_buf,0x16,2,dos_modify_time);
    set_word(short_file_name_buf,0x18,2,dos_modify_date);
    set_word(short_file_name_buf,0x1C,4,size);
    fat_file_t eot_pos;
    _Bool ate;
    if(fat_rw(&pos, FAT_WRITE, short_file_name_buf,0x20, &eot_pos, &ate)!=0x20){
        if(ate){
            errno = EINVAL;
        }
        return -1;
    }
    if(entry){
        get_short_file_name(&short_file_name_buf[0],entry->short_name,NULL,NULL);
        memcpy(entry->long_name,name,name_len);
        entry->long_name[name_len] = 0;
        entry->fat_ctxt = fat;
        entry->first = start;
        entry->last = pos;
        entry->create = dos2unix(dos_create_date,dos_create_time);
        entry->cms = cms;
        entry->access_time = dos2unix(dos_access_date,0);
        entry->modify_time = dos2unix(dos_modify_date,dos_modify_time);
        entry->attributes = attributes;
        entry->cluster = cluster;
        entry->size = size;
    }
    return 0;
}

int fat_flush(fat_ctxt_t *fat){
    for(int i = 0;i < FAT_CACHE_MAX;i++){
        if(cache_flush(fat,i)){
            return -1;
        }
    }
    return 0;
}

uint32_t fat_advance(fat_file_t *file, uint32_t byte_cnt, _Bool *eof){
    fat_ctxt_t *fat = file->fat_ctxt;
    _Bool ate = 0;
    uint32_t p_offset = file->p_offset;
    uint32_t old_offset = p_offset;
    uint32_t new_offset = p_offset + byte_cnt;
    if(!(file->is_dir && file->size == 0)){
        if(new_offset > file->size || new_offset < p_offset){
            new_offset = file->size;
            byte_cnt = new_offset - p_offset;
            ate = 1;
        }
    }
    uint32_t p_cluster_offset = file->p_cluster_sector * fat->bytes_per_sector;
    p_cluster_offset += file->p_sector_offset;
    p_offset -= p_cluster_offset;
    byte_cnt += p_cluster_offset;
    _Bool no_cluster = fat->type !=FAT32 && file->cluster < 2;
    if(!no_cluster){
        uint32_t cluster = file->p_cluster;
        uint32_t cluster_seq = file->p_cluster_seq;
        uint32_t new_cluster_seq = new_offset / fat->bytes_per_cluster;
        while(cluster_seq < new_cluster_seq){
            int e = cluster_advance(fat,&cluster);
            if(e == -1){
                if(eof){
                    *eof = 0;
                }
                return 0;
            }
            if(e == 0){
                byte_cnt = fat->bytes_per_cluster;
                ate = 1;
                break;
            }
            p_offset += fat->bytes_per_cluster;
            byte_cnt -= fat->bytes_per_cluster;
            cluster_seq++;
        }
        file->p_cluster = cluster;
        file->p_cluster_seq = cluster_seq;
    }
    p_offset += byte_cnt;
    file->p_offset = p_offset;
    file->p_cluster_sector = byte_cnt / fat->bytes_per_sector;
    file->p_sector_offset = byte_cnt % fat->bytes_per_sector;
    if(!no_cluster){
        if(file->p_cluster_sector == fat->sectors_per_cluster){
            file->p_cluster_sector--;
            file->p_sector_offset += fat->bytes_per_sector;
        }
    }
    if(eof){
        *eof = ate;
    }
    return p_offset - old_offset;
}

static uint32_t cluster_rw(fat_file_t *file, enum fat_io rw, void *buf, uint32_t cluster_cnt, _Bool *eof){
    fat_ctxt_t *fat = file->fat_ctxt;
    char *p = buf;
    if(cache_flush(fat,FAT_CACHE_DATA)){
        return 0;
    }
    cache_invalidate(fat,FAT_CACHE_DATA);
    uint32_t cluster = file->p_cluster;
    if(cluster < 2){
        cluster = fat->root_cluster;
    }
    uint32_t copy_cnt = 0;
    while(cluster_cnt > 0){
        uint32_t chunk_start = cluster;
        uint32_t chunk_len = 1;
        while(1){
            uint32_t p_cluster = cluster;
            if(cluster_get(fat,cluster,&cluster)){
                return copy_cnt;
            }
            if(cluster >= 0x0FFFFFF7 || cluster != p_cluster + 1 || chunk_len >= cluster_cnt){
                break;
            }
            chunk_len++;
        }
        uint32_t lba = fat->data_lba + fat->sectors_per_cluster * (chunk_start - 2);
        uint32_t block_cnt = fat->sectors_per_cluster * chunk_len;
        uint32_t byte_cnt = block_cnt * fat->bytes_per_sector;
        int e;
        if(rw == FAT_READ){
            e = fat->read(p,lba,block_cnt);
        }else{
            e = fat->write(p,lba,block_cnt);
        }
        if(e){
            break;
        }
        cluster_cnt -= chunk_len;
        copy_cnt += chunk_len;
        file->p_offset += byte_cnt;
        file->p_cluster_seq += chunk_len;
        if(cluster < 2 || cluster >= 0x0FFFFFF7){
            file->p_cluster_sector = fat->sectors_per_cluster - 1;
            file->p_sector_offset = fat->bytes_per_sector;
            if(eof){
                *eof = 1;
            }
            break;
        }
        else{
            file->p_cluster = cluster;
            p+= byte_cnt;
        }
    }
    return copy_cnt;
}

static int fat_find(fat_ctxt_t *fat, fat_entry_t *dir, const char *path, fat_entry_t *entry){
    fat_entry_t ent;
    if(dir){
        ent = *dir;
    }
    else{
        make_root(fat,&ent);
    }
    if(!path){
        if(entry){
            *entry = ent;
        }
        return 0;
    }
    const char *p = path;
    while(*p){
        if(!(ent.attributes & FAT_ATTRIBUTE_DIRECTORY)){
            errno = ENOTDIR;
            return -1;
        }
        const char *s = p;
        const char *e = p;
        while(*p){
            char c = *p++;
            if(c == '/' || c == '\\'){
                break;
            }
            e++;
        }
        size_t name_len = name_trim(s, e-s);
        if(name_len == 0){
            while(s[name_len] == '.'){
                name_len++;
            }
        }
        if(name_len > 255){
            errno = ENAMETOOLONG;
            return -1;
        }
        if(name_len == 0){
            continue;
        }
        char name[256];
        memcpy(name,s,name_len);
        name[name_len] = 0;
        if(strcmp(name,".") == 0){
            continue;
        }
        if(dir_find(fat,ent.cluster,name,&ent)){
            return -1;
        }
    }
    if(entry){
        *entry = ent;
    }
    return 0;
}

uint32_t fat_rw(fat_file_t * file, enum fat_io rw, void *buf, uint32_t byte_cnt, fat_file_t *new_file, _Bool *eof){
    if(byte_cnt == 0){
        if(eof){
            *eof = 0;
        }
        return 0;
    }
    fat_ctxt_t *fat = file->fat_ctxt;
    _Bool ate = 0;
    if(!(file->is_dir && file->size == 0)){
        if(file->p_offset >= file->size){
            if(eof){
                *eof = 1;
            }
            return 0;
        }
        uint32_t new_offset = file->p_offset + byte_cnt;
        if(new_offset > file->size || new_offset < file->p_offset){
            byte_cnt = file->size - file->p_offset;
        }
    }
    _Bool no_cluster = fat->type !=FAT32 && file->cluster < 2;
    fat_file_t pos = *file;
    char *p = buf;
    uint32_t copy_cnt = 0;
    while(byte_cnt > 0){
        if(!no_cluster && byte_cnt >= fat->bytes_per_cluster && pos.p_cluster_sector == 0 && pos.p_sector_offset == 0){
            uint32_t cluster_cnt = byte_cnt / fat->bytes_per_cluster;
            uint32_t copy_cluster_cnt = cluster_rw(&pos,rw,p,cluster_cnt,&ate);
            uint32_t byte_cluster_cnt = copy_cluster_cnt * fat->bytes_per_cluster;
            if(p){
                p += byte_cluster_cnt;
            }
            byte_cnt -= byte_cluster_cnt;
            copy_cnt += byte_cluster_cnt;
            if(copy_cluster_cnt != cluster_cnt || ate){
                break;
            }else{
                continue;
            }
        }
        uint32_t chunk_size = fat->bytes_per_sector - pos.p_sector_offset;
        if(chunk_size > byte_cnt){
            chunk_size = byte_cnt;
        }
        uint32_t p_sector_offset = pos.p_sector_offset;
        if(chunk_size > 0){
            if(file_sector(&pos,rw == FAT_READ || chunk_size != fat->bytes_per_sector)){
                break;
            }
        }
        int e = errno;
        errno = 0;
        uint32_t advance = fat_advance(&pos, chunk_size, &ate);
        if(advance > 0){
            if(rw == FAT_READ){
                cache_read(fat, FAT_CACHE_DATA, p_sector_offset,p,advance);
            }else{
                cache_write(fat,FAT_CACHE_DATA,p_sector_offset,p,advance);
            }
            if(p){
                p += advance;
            }
            byte_cnt -= advance;
            copy_cnt += advance;
        }
        if(errno == 0){
            errno = e;
        }
        else{
            break;
        }
        if(advance != chunk_size || ate){
            break;
        }
    }
    if(new_file){
        *new_file = pos;
    }
    if(eof){
        *eof = ate;
    }
    return copy_cnt;
}

int fat_dir(fat_file_t *dir, fat_entry_t *entry){
    if(!dir->is_dir){
        errno = ENOTDIR;
        return -1;
    }
    fat_ctxt_t *fat = dir->fat_ctxt;
    int long_file_name_seq = -1;
    fat_file_t long_p;
    uint8_t long_checksum = 0;
    char long_file_name_buf[256];
    char ent_buf[0x20];
    fat_file_t dir_next;
    while(fat_rw(dir, FAT_READ, ent_buf, 0x20, &dir_next, NULL) == 0x20){
        uint8_t mark = get_word(ent_buf, 0x00,1);
        fat_file_t ent_p = *dir;
        *dir = dir_next;
        if(mark == 0x00 || mark == 0xe5){
            long_file_name_seq = -1;
            continue;
        }
        uint8_t attribute = get_word(ent_buf,0x0B, 1);
        if(attribute == 0x0F){
            uint8_t seq = mark & 0x1F;
            if(seq < 0x01 || seq > 0x14){
                long_file_name_seq = -1;
                continue;
            }
            uint8_t checksum = get_word(ent_buf, 0x0D, 1);
            if(mark & 0x40){
                long_file_name_seq = seq;
                long_p = ent_p;
                long_checksum = checksum;
                memset(long_file_name_buf,0,sizeof(long_file_name_buf));
            }
            else{
                if(seq!=long_file_name_seq - 1 || checksum != long_checksum){
                    long_file_name_seq = -1;
                    continue;
                }
                long_file_name_seq = seq;
            }
            int n = (long_file_name_seq - 1) * 13;
            for(int j = 0;j < 13 && n < 255;j++){
                uint32_t p = 1 + j * 2;
                if(j >= 5){
                    p += 3;
                }
                if(j >= 11){
                    p += 2;
                }
                uint16_t c = get_word(ent_buf,p,2);
                if(c > 0xFF){
                    c = 0x7F;
                }
                long_file_name_buf[n++] = c;
            }
        }
        else{
            _Bool have_long_file_name = 0;
            if(long_file_name_seq == 1 && long_checksum == compute_long_checksum(ent_buf)){
                have_long_file_name = 1;
            }
            long_file_name_seq = -1;
            if((attribute & FAT_ATTRIBUTE_DIRECTORY) && (attribute & FAT_ATTRIBUTE_LABEL)){
                continue;
            }
            if(!validate_short_file_name(&ent_buf[0])){
                continue;
            }
            int name_len;
            int ext_len;
            if(get_short_file_name(&ent_buf[0],entry->short_name, &name_len, &ext_len)==0){
                continue;
            }
            if(entry->short_name[0] == '\x05'){
                entry->short_name[0] = '\xE5';
            }
            if(have_long_file_name){
                strcpy(entry->long_name,long_file_name_buf);
                entry->first = long_p;
            }else{
                strcpy(entry->long_name, entry->short_name);
                entry->first = ent_p;
                uint8_t cse = get_word(ent_buf, 0x0C,1);
                if(cse & 0x08){
                    convert_lower(&entry->long_name[0], name_len);
                }
                if(cse & 0x10){
                    convert_lower(&entry->long_name[name_len + 1], ext_len);
                }
            }
            entry->last = ent_p;
            entry->create = dos2unix(get_word(ent_buf,0x10,2), get_word(ent_buf, 0x0E,2));
            entry->cms = get_word(ent_buf,0x0D,1) * 10;
            entry->create += entry->cms / 1000;
            entry->cms %= 1000;
            entry->access_time = dos2unix(get_word(ent_buf,0x12,2),0);
            entry->modify_time = dos2unix(get_word(ent_buf,0x18,2),get_word(ent_buf,0x16,2));
            entry->attributes = attribute;
            if(entry->attributes & FAT_ATTRIBUTE_LABEL){
                entry->cluster = 0;
            }else{
                entry->cluster = get_word(ent_buf,0x1A,2);
                if(fat->type == FAT32){
                    entry->cluster |= get_word(ent_buf,0x14,2) << 16;
                
                    if(entry->cluster == fat->root_cluster){
                        entry->cluster = 0;
                    }
                }
                if(entry->cluster == 1){
                    entry->cluster = 0;
                }
            }
            if(fat->type != FAT32 && (entry->attributes & FAT_ATTRIBUTE_DIRECTORY) && entry->cluster < 2){
                entry->size = fat->entry_cnt * 0x20;
            }else if(entry->attributes & (FAT_ATTRIBUTE_LABEL | FAT_ATTRIBUTE_DIRECTORY)){
                entry->size = 0;
            }else{
                entry->size = get_word(ent_buf,0x1C, 4);
            }
            entry->fat_ctxt = dir->fat_ctxt;
            return 0;
        }
    }
    return -1;
}

void fat_root(fat_ctxt_t *fat, fat_file_t *file){
    file->fat_ctxt = fat;
    file->cluster = 0;
    if(fat->type == FAT32){
        file->size = 0;
    }else{
        file->size = fat->entry_cnt * 0x20;
    }
    file->is_dir = 1;
    fat_rewind(file);
}

void fat_free(fat_path_t *fp){
    list_destroy(&fp->entry_list);
    free(fp);
}

void fat_rewind(fat_file_t *file){
    file->p_offset = 0;
    file->p_cluster = file->cluster;
    file->p_sector_offset = 0;
    file->p_cluster_seq = 0;
    file->p_cluster_sector = 0;
}

void fat_begin(fat_entry_t *entry, fat_file_t *file){
    if((entry->attributes & FAT_ATTRIBUTE_DIRECTORY) && entry->cluster < 2){
        fat_root(entry->fat_ctxt, file);
    }
    else{
        file->fat_ctxt = entry->fat_ctxt;
        file->cluster = entry->cluster;
        file->size = entry->size;
        file->is_dir = entry->attributes & FAT_ATTRIBUTE_DIRECTORY;
        fat_rewind(file);
    }
}

fat_entry_t *fat_path_target(fat_path_t *fat_path){
    if(fat_path){
        return fat_path->entry_list.last;
    }else{
        return NULL;
    }
}

fat_path_t *fat_path(fat_ctxt_t *fat, fat_path_t *dir, const char *path, const char **tail){
    fat_path_t *ret = malloc(sizeof(*ret));
    if(!ret){
        errno = ENOMEM;
        return NULL;
    }
    list_init(&ret->entry_list,sizeof(fat_entry_t));
    if(dir){
        for(fat_entry_t *ent = dir->entry_list.first;ent;ent=list_next(ent)){
            if(!list_push_back(&ret->entry_list,ent)){
                errno = ENOMEM;
                fat_free(ret);
                return NULL;
            }
        }
    }
    else{
        fat_entry_t *entry = list_push_back(&ret->entry_list,NULL);
        if(!entry){
            errno = ENOMEM;
            fat_free(ret);
            return NULL;
        }
        make_root(fat,entry);
    }
    fat_entry_t *ent = ret->entry_list.last;
    const char *p_path = path;
    while(*p_path){
        const char *s = p_path;
        const char *e = p_path;
        while(*p_path){
            const char c = *p_path++;
            if(c == '/' || c == '\\'){
                break;
            }
            e++;
        }
        size_t namelen = name_trim(s, e - s);
        if(namelen == 0){
            while(s[namelen] == '.') namelen++;
        }
        if(namelen>255){
            errno = ENAMETOOLONG;
            break;
        }
        if(namelen == 0) continue;
        char name[256];
        memcpy(name,s,namelen);
        name[namelen] = 0;
        if(strcmp(name,".") == 0){
            continue;
        }
        fat_entry_t directory_entry;
        if(dir_find(fat,ent->cluster,name,&directory_entry)){
            if(errno == ENOENT && tail){
                *tail = s;
            }
            break;
        }
        if(directory_entry.attributes & FAT_ATTRIBUTE_DIRECTORY){
            _Bool exist = 0;
            for(fat_entry_t *entry = ret->entry_list.first;entry;entry = list_next(entry)){
                if(exist){
                    fat_entry_t *t = entry;
                    entry = list_prev(entry);
                    list_erase(&ret->entry_list,t);
                }else if(entry->attributes & FAT_ATTRIBUTE_DIRECTORY && entry->cluster==directory_entry.cluster){
                    ent = entry;
                    exist = 1;
                }
            }
            if(exist){
                continue;
            }
        }
        ent = list_push_back(&ret->entry_list,&directory_entry);
        if(!ent){
            errno = ENOMEM;
            break;
        }
    }
    return ret;
}

fat_path_t *fat_create_path(fat_ctxt_t *fat, fat_path_t *dir, const char *path, uint8_t attributes){
    int e = errno;
    errno = 0;
    const char *tail;
    fat_path_t *dest_fp = fat_path(fat,dir, path, &tail);
    if(errno == 0){
        errno = EEXIST;
        goto error;
    }
    else{
        if(errno == ENOENT && strlen(tail) > 0 && !strchr(tail,'/') && !strchr(tail, '\\')){
            errno = e;
        }else{
            goto error;
        }
    }
    fat_entry_t entry;
    if(fat_create(fat, fat_path_target(dest_fp), tail, attributes,&entry)){
        goto error;
    }
    if(!list_push_back(&dest_fp->entry_list, &entry)){
        errno = ENOMEM;
        goto error;
    }
    return dest_fp;
error:
    if(dest_fp){
        fat_free(dest_fp);
    }
    return NULL;
}

int fat_create(fat_ctxt_t *fat, fat_entry_t *dir, const char *path, uint8_t attributes, fat_entry_t *entry){
    _Bool is_dir = attributes & FAT_ATTRIBUTE_DIRECTORY;
    _Bool is_label = attributes & FAT_ATTRIBUTE_LABEL;
    if(is_dir && is_label){
        errno = EINVAL;
        return -1;
    }
    const char *dir_s;
    size_t dir_len;
    const char *file_s;
    size_t file_len;
    const char *end = path + name_trim(path,strlen(path));
    const char *slash = NULL;
    for(const char *p = end;p >= path;p--){
        if(*p == '\\' || *p == '/'){
            slash = p;
            break;
        }
    }
    if(slash){
        dir_s = path;
        dir_len = slash - dir_s;
        file_s = slash + 1;
    }else{
        dir_s = NULL;
        dir_len = 0;
        file_s = path;
    }
    file_len = end - file_s;
    if(file_len > 255){
        errno =ENAMETOOLONG;
        return -1;
    }
    if(file_len == 0){
        errno = EINVAL;
        return -1;
    }
    fat_entry_t dir_ent;
    if(dir_len > 0){
        char *dir_path = malloc(dir_len + 1);
        if(!dir_path){
            errno = ENOMEM;
            return -1;
        }
        memcpy(dir_path,dir_s,dir_len);
        dir_path[dir_len] = 0;
        int e = fat_find(fat,dir,dir_path,&dir_ent);
        free(dir_path);
        if(e){
            return -1;
        }
    }else{
        if(fat_find(fat,dir,NULL,&dir_ent)){
            return -1;
        }
    }
    int e = errno;
    if(fat_find(fat,&dir_ent,file_s,NULL) == 0){
        errno = EEXIST;
        return -1;
    }
    if(errno != ENOENT){
        return -1;
    }
    errno = e;

    uint32_t cluster = 0;
    if(is_dir){
        cluster = cluster_find_free(fat,0,1,NULL);
        if(cluster < 2){
            return -1;
        }
        if(cluster_set(fat,cluster,0x0FFFFFFF)){
            return -1;
        }
    }
    uint32_t dir_cluster = dir_ent.cluster;
    time_t t = time(NULL);
    char name[256];
    memcpy(name,file_s,file_len);
    name[file_len] = 0;
    if(dir_insert(fat,dir_cluster,name,t,0,t,t,attributes,cluster, 0, entry)){
        return -1;
    }
    if(is_dir){
        fat_file_t cluster_pos;
        dir_begin(fat,&cluster_pos,cluster);
        if(fat_rw(&cluster_pos,FAT_WRITE,NULL,fat->bytes_per_cluster,NULL,NULL)!=fat->bytes_per_cluster){
            return -1;
        }
        int d = dir_insert(fat,cluster, ".",t,0,t,t,FAT_ATTRIBUTE_DIRECTORY,cluster,0,NULL);
        int dd = dir_insert(fat,cluster,"..",t,0,t,t,FAT_ATTRIBUTE_DIRECTORY,dir_cluster,0,NULL);
        if(d || dd){
            return -1;
        }
    }
    return 0;
}

int fat_resize(fat_entry_t *entry, uint32_t size, fat_file_t *file){
    fat_ctxt_t *fat = entry->fat_ctxt;
    if(entry->attributes & FAT_ATTRIBUTE_DIRECTORY){
        errno = EISDIR;
        return -1;
    }
    if(entry->attributes & FAT_ATTRIBUTE_LABEL){
        errno = ENOENT;
        return -1;
    }
    if(size == entry->size){
        return 0;
    }
    uint32_t cluster_cnt = (size + fat->bytes_per_cluster - 1) / fat->bytes_per_cluster;
    uint32_t cluster = entry->cluster;
    uint32_t chunk_len = 0;
    if(size > 0 && cluster < 2){
        cluster = cluster_find_free(fat, 0, cluster_cnt, &chunk_len);
        if(cluster == 0){
            return -1;
        }
        chunk_len--;
    }
    if(cluster>=2){
        if(cluster_resize_chain(fat,cluster,cluster_cnt,chunk_len)){
            return -1;
        }
    }
    if(cluster_cnt == 0){
        cluster = 0;
    }
    entry->cluster = cluster;
    entry->size = size;
    if(file_sector(&entry->last,1)){
        return -1;
    }
    void *data = file_data(&entry->last);
    if(fat->type == FAT32){
        set_word(data, 0x14, 2, entry->cluster >> 16);
    }
    set_word(data,0x1A,2,entry->cluster);
    set_word(data,0x1C,4,entry->size);
    cache_dirty(fat, FAT_CACHE_DATA);
    if(file){
        file->size = entry->size;
        if(file->size == 0 || file->cluster < 2){
            file->cluster = entry->cluster;
            fat_rewind(file);
        }
        else if(file->p_offset > size || file->p_cluster_seq >= cluster_cnt){
            fat_rewind(file);
        }
    }
    return 0;
}

int fat_init(fat_ctxt_t *fat, fat_io_proc read, fat_io_proc write){
    fat->read = read;
    fat->write = write;
    for(int i = 0;i < FAT_CACHE_MAX;i++){
        fat->cache[i].valid = 0;
        fat->cache[i].max_lba = -1;
    }
    if(find_partition(fat)){
        fat->partition_lba = 0;
        fat->partition_sectors = 0;
    }
    void *partition = cache_load(fat,FAT_CACHE_DATA,fat->partition_lba);
    fat->bytes_per_sector = get_word(partition,0xB,2);
    fat->sectors_per_cluster = get_word(partition, 0xD,1);
    fat->reserved_sectors = get_word(partition,0xE,2);
    fat->number_of_fat = get_word(partition,0x10,1);
    fat->entry_cnt = get_word(partition,0x11,2);
    fat->fs_sector_cnt = get_word(partition,0x13,2);
    if(fat->fs_sector_cnt == 0){
        fat->fs_sector_cnt = get_word(partition,0x20,4);
    }
    fat->fat_sector_cnt = get_word(partition,0x16,2);
    if(fat->fat_sector_cnt == 0){
        fat->fat_sector_cnt = get_word(partition,0x24,4);
    }
    fat->fat_lba = fat->partition_lba + fat->reserved_sectors;
    fat->root_lba = fat->fat_lba + fat->number_of_fat * fat->fat_sector_cnt;
    fat->data_lba = fat->root_lba + (fat->entry_cnt * 0x20 + fat->bytes_per_sector - 1) / fat->bytes_per_sector;
    fat->cache[FAT_CACHE_FAT].max_lba = fat->fat_lba + fat->fat_sector_cnt;
    fat->cache[FAT_CACHE_DATA].max_lba = fat->partition_lba + fat->fs_sector_cnt;
    fat->bytes_per_cluster = fat->bytes_per_sector * fat->sectors_per_cluster;
    fat->max_cluster = 2 + (fat->fs_sector_cnt - fat->data_lba) / fat->sectors_per_cluster;
    fat->blocks_free = 2;
    if(fat->max_cluster < 0xFF7){
        fat->type = FAT12;
    }
    else if(fat->max_cluster < 0xFFF7){
        fat->type = FAT16;
    }else{
        fat->type = FAT32;
        if(fat->max_cluster > 0xFFFFFFF7) fat->max_cluster = 0xFFFFFFF7;
    }
    uint32_t fat_clusters = fat->fat_sector_cnt * fat->bytes_per_sector;
    if(fat->type == FAT12){
        fat_clusters = fat_clusters / 3 * 2;
    }else if(fat->type == FAT16){
        fat_clusters = fat_clusters / 2;
    }else{
        fat_clusters = fat_clusters / 4;
    }
    if(fat->max_cluster > fat_clusters){
        fat->max_cluster = fat_clusters;
    }
    if(fat->type == FAT32){
        fat->root_cluster = get_word(partition,0x2C,4);
        fat->fsis_lba = get_word(partition,0x30,2);
    }else{
        fat->root_cluster = 0;
        fat->fsis_lba = 0;
    }
    return 0;
}
#endif