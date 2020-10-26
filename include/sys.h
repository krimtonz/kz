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
};

typedef struct {
    uint32_t command;
    void *n64_buffer;
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
            DIR *dir;
        } close_dir;
        struct {
            DIR *dir;
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
    };
} hb_fat_regs_t;

#define hb_fat ((volatile hb_fat_regs_t*)0xA8058000)

#define open(__ret, __path, __oflags) { \
    hb_fat->open.path = (__path); \
    hb_fat->open.open_flags = (__oflags); \
    hb_fat->open.has_mode = 0; \
    hb_fat->n64_buffer = (__ret); \
    hb_fat->command = SYS_OPEN; \
}(void)0

#define opendir(__ret, __dir) { \
    hb_fat->opendir.dir = (__dir); \
    hb_fat->n64_buffer = (__ret); \
    hb_fat->command = SYS_OPEN_DIR; \
}(void)0

#define closedir(__ret, __dir) { \
    hb_fat->close_dir.dir = (__dir); \
    hb_fat->n64_buffer = (__ret); \
    hb_fat->command = SYS_CLOSE_DIR; \
}(void)0

#define readdir(__ret, __buf, __dir) { \
    hb_fat->read_dir.dir = (__dir); \
    hb_fat->read_dir.buf = (__buf); \
    hb_fat->n64_buffer = (__ret); \
    hb_fat->command = SYS_READ_DIR; \
}(void)0

#define creat(__ret, __path, __mode) { \
    hb_fat->creat.path = (__path); \
    hb_fat->creat.mode = (__mode); \
    hb_fat->n64_buffer = (__ret); \
    hb_fat->command = SYS_CREAT; \
}(void)0

#define write(__ret, __fd, __buf, __cnt) { \
    hb_fat->write.fd = (__fd); \
    hb_fat->write.buf = (__buf); \
    hb_fat->write.byte_cnt = (__cnt); \
    hb_fat->n64_buffer = (__ret); \
    hb_fat->command = SYS_WRITE; \
}(void)0

#define read(__ret, __fd, __buf, __cnt) { \
    hb_fat->read.fd = (__fd); \
    hb_fat->read.buf = (__buf); \
    hb_fat->read.byte_cnt = (__cnt); \
    hb_fat->n64_buffer = (__ret); \
    hb_fat->command = SYS_READ; \
}(void)0

#define close(__ret, __fd) { \
    hb_fat->close.fd = (__fd); \
    hb_fat->n64_buffer = (__ret); \
    hb_fat->command = SYS_CLOSE; \
}(void)0

#define getcwd(__buf, __size) { \
    hb_fat->getcwd.buf = (__buf); \
    hb_fat->getcwd.size = (__size);\
    hb_fat->command = SYS_GET_CWD; \
}(void)0

#endif