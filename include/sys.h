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

enum {
    SYS_OPEN,
    SYS_OPEN_DIR,
    SYS_CLOSE_DIR,
    SYS_READ_DIR,
    SYS_CREAT,
    SYS_WRITE,
    SYS_READ,
    SYS_CLOSE,
    SYS_TIME,
    SYS_GET_CWD,
    SYS_CHDIR,
    SYS_FSTAT,
    SYS_FSTAT_AT,
    SYS_ISATTY,
    SYS_LSEEK,
    SYS_TRUNC,
    SYS_RENAME,
    SYS_CHMOD,
    SYS_UNLINK,
    SYS_SEEK_DIR,
    SYS_TELL_DIR,
    SYS_REWIND_DIR,
    SYS_RM_DIR,
    SYS_MK_DIR,
    SYS_STAT,
    SYS_LSTAT
};

typedef struct {
    uint32_t command;
    uint32_t n64_buffer;
    union {
        struct {
            char *path;
            uint32_t open_flags;
            uint32_t has_mode;
            uint32_t mode;
        } open;
        struct {
            char *dir;
        } opendir;
        struct {
            void *dir;
        } close_dir;
        struct {
            void *dir;
            dirent_t *buf;
        } read_dir;
        struct {
            char *path;
            uint32_t mode;
        } creat;
        struct {
            int fd;
            void *buf;
            uint32_t byte_cnt;
        } write;
        struct {
            int fd;
            void *buf;
            uint32_t byte_cnt;
        } read;
        struct {
            int fd;
        } close;
        struct {
            void *buf;
            size_t size;
        } getcwd;
        struct {
            char *path;
            struct stat *stat;
        } stat;
        struct {
            char *path;
        } chdir;
    };
} hb_fat_regs_t;

#define hb_fat ((volatile hb_fat_regs_t*)0xA9058000)

#define open(__ret, __path, __oflags) { \
    hb_fat->open.path = (__path); \
    hb_fat->open.open_flags = (__oflags); \
    hb_fat->open.has_mode = 0; \
    hb_fat->n64_buffer = (uint32_t)(__ret); \
    hb_fat->command = SYS_OPEN; \
}(void)0

#define opendir(__ret, __dir) { \
    hb_fat->opendir.dir = (__dir); \
    hb_fat->n64_buffer = (uint32_t)(__ret); \
    hb_fat->command = SYS_OPEN_DIR; \
}(void)0

#define closedir(__ret, __dir) { \
    hb_fat->close_dir.dir = (__dir); \
    hb_fat->n64_buffer = (uint32_t)(__ret); \
    hb_fat->command = SYS_CLOSE_DIR; \
}(void)0

#define readdir(__ret, __buf, __dir) { \
    hb_fat->read_dir.dir = (__dir); \
    hb_fat->read_dir.buf = (__buf); \
    hb_fat->n64_buffer = (uint32_t)(__ret); \
    hb_fat->command = SYS_READ_DIR; \
}(void)0

#define creat(__ret, __path, __mode) { \
    hb_fat->creat.path = (__path); \
    hb_fat->creat.mode = (__mode); \
    hb_fat->n64_buffer = (uint32_t)(__ret); \
    hb_fat->command = SYS_CREAT; \
}(void)0

#define write(__ret, __fd, __buf, __cnt) { \
    hb_fat->write.fd = (__fd); \
    hb_fat->write.buf = (__buf); \
    hb_fat->write.byte_cnt = (__cnt); \
    hb_fat->n64_buffer = (uint32_t)(__ret); \
    hb_fat->command = SYS_WRITE; \
}(void)0

#define read(__ret, __fd, __buf, __cnt) { \
    hb_fat->read.fd = (__fd); \
    hb_fat->read.buf = (__buf); \
    hb_fat->read.byte_cnt = (__cnt); \
    hb_fat->n64_buffer = (uint32_t)(__ret); \
    hb_fat->command = SYS_READ; \
}(void)0

#define close(__ret, __fd) { \
    hb_fat->close.fd = (__fd); \
    hb_fat->n64_buffer = (uint32_t)(__ret); \
    hb_fat->command = SYS_CLOSE; \
}(void)0

#define getcwd(__buf, __size) { \
    hb_fat->getcwd.buf = (__buf); \
    hb_fat->getcwd.size = (__size);\
    hb_fat->command = SYS_GET_CWD; \
}(void)0

#define stat(__ret, __path, __sbuf) { \
    hb_fat->stat.path = (__path); \
    hb_fat->stat.stat = (__sbuf); \
    hb_fat->n64_buffer = (uint32_t)(__ret); \
    hb_fat->command = SYS_STAT; \
}(void)0

#define chdir(__ret, __path) { \
    hb_fat->chdir.path = (__path); \
    hb_fat->n64_buffer = (uint32_t)(__ret); \
    hb_fat->command = SYS_CHDIR; \
}(void)0

#endif