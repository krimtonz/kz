/*
* sys.h
*
* definitions for i/o using homeboy devices
*/

#ifndef _SYS_H
#define _SYS_H

#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef void *DIR;

typedef struct {
    ino_t   dir_ino;
    char    dir_name[256];
    mode_t  mode;
    time_t  create_time;
    time_t  modify_time;
    off_t   size;
} dirent_t;

int open(const char *path, int open_flags, ...);
int creat(const char *path, mode_t mode);
int mkdir(const char *path, mode_t mode);
int chdir(const char *path);
int stat(const char *path, struct stat *buf);
char *getcwd(char *buf, size_t size);
int close(int file);
int read(int file, void *buf, uint32_t byte_cnt);
int write(int file, void *buf, uint32_t byte_cnt);
dirent_t *readdir(DIR *dir);
int closedir(DIR *dir);
DIR *opendir(const char *dir);
int reset_disk(void);

#endif
