/*
* sys.h
*
* definitions for i/o using homeboy devices
*/

#ifndef LITE
#ifndef _SYS_H
#define _SYS_H

#include <sys/stat.h>
#include <sys/param.h>
#include <fcntl.h>
#include <time.h>

typedef void *DIR;

typedef struct{
    ino_t   dir_ino;
    char    dir_name[256];
    mode_t  mode;
    time_t  create_time;
    time_t  modify_time;
    off_t   size;
} dirent_t;

int         open(const char *path, int open_flags, ...);
DIR        *opendir(const char *dir);
int         closedir(DIR *dir);
dirent_t   *readdir(DIR *dir);
int         creat(const char *path, mode_t mode);
int         write(int file, void *buf, uint32_t byte_cnt);
int         read(int file, void *buf, uint32_t byte_cnt);
int         close(int file);
time_t      time(time_t *tloc);
char       *getcwd(char *buf, size_t size);
int         chdir(const char *path);
void        reset_disk(void);

#endif
#endif