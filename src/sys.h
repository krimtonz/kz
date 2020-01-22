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

typedef struct {
    ino_t   dir_ino;
    char    dir_name[256];
    mode_t  mode;
    time_t  create_time;
    time_t  modify_time;
    off_t   size;
} dirent_t;

int         open            (const char *path, int open_flags, ...);
DIR        *opendir         (const char *dir);
int         closedir        (DIR *dir);
dirent_t   *readdir         (DIR *dir);
int         creat           (const char *path, mode_t mode);
int         write           (int file, void *buf, uint32_t byte_cnt);
int         read            (int file, void *buf, uint32_t byte_cnt);
int         close           (int file);
time_t      time            (time_t *tloc);
char       *getcwd          (char *buf, size_t size);
int         chdir           (const char *path);
int         fstat           (int file, struct stat *buf);
int         fstatat         (int file, const char *path, struct stat *buf, int flag);
int         isatty          (int file);
off_t       lseek           (int file, off_t offset, int whence);
int         truncate        (const char *path, off_t len);
int         rename          (const char *old_path, const char *new_path);
int         chmod           (const char *path, mode_t mode);
int         unlink          (const char *path);
void        seekdir         (DIR *dirp, long loc);
long        telldir         (DIR *dirp);
void        rewinddir       (DIR *dirp);
int         rmdir           (const char *path);
int         mkdir           (const char *path, mode_t mode);
int         stat            (const char *path, struct stat *buf);
int         lstat           (const char *path, struct stat *buf);
void        __assert_func   (const char *file, int line, const char *func, const char *failedexpr);
void        reset_disk      (void);

#endif
#endif