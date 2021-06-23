#include <stdarg.h>
#include "mem.h"
#include "sys.h"

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
    SYS_RMDIR,
    SYS_MKDIR,
    SYS_STAT,
    SYS_LSTAT
};

typedef struct {
    uint32_t command;
    void *n64_buffer;
    union {
        struct {
            const char *path;
            uint32_t open_flags;
            uint32_t has_mode;
            uint32_t mode;
        } open;
        struct {
            const char *dir;
        } opendir;
        struct {
            void *dir;
        } close_dir;
        struct {
            void *dir;
            dirent_t *buf;
        } read_dir;
        struct {
            const char *path;
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
            const char *path;
            struct stat *stat;
        } stat;
        struct {
            const char *path;
        } chdir;
        struct {
            const char *path;
            mode_t mode;
        } mkdir;
    };
} hb_fat_regs_t;

#define hb_fat ((volatile hb_fat_regs_t*)0xA9058000)

typedef struct {
    void *dir;
    dirent_t dirent;
} opendir_t;

__attribute__((used))
__attribute__((noinline))
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
}

int open(const char *path, int open_flags, ...) {
    int ret;

    hb_fat->open.path = path;
    hb_fat->open.open_flags = open_flags;
    va_list va;
    va_start(va, open_flags);
    mode_t mode = va_arg(va, mode_t);
    hb_fat->open.has_mode = 1;
    hb_fat->open.mode = mode;
    va_end(va);
    hb_fat->n64_buffer = &ret;
    hb_fat->command = SYS_OPEN;

    return ret;
}

int creat(const char *path, mode_t mode) {
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

DIR *opendir(const char *dir) {
    opendir_t *dir_desc = malloc(sizeof(*dir_desc));

    if(dir_desc == NULL) {
        return NULL;
    }

    hb_fat->opendir.dir = dir;
    hb_fat->n64_buffer = &dir_desc->dir;
    hb_fat->command = SYS_OPEN_DIR;

    return (DIR*)dir_desc;
}

int closedir(DIR *dir) {
    int ret;
    opendir_t *dir_desc = (opendir_t*)dir;

    hb_fat->close_dir.dir = dir_desc->dir;
    hb_fat->n64_buffer = &ret;
    hb_fat->command = SYS_CLOSE_DIR;

    free(dir_desc);

    return ret;
}

dirent_t *readdir(DIR *dir) {
    int ret;
    opendir_t *dir_desc = (opendir_t*)dir;

    hb_fat->read_dir.dir = dir_desc->dir;
    hb_fat->read_dir.buf = &dir_desc->dirent;
    hb_fat->n64_buffer = &ret;
    hb_fat->command = SYS_READ_DIR;

    if(ret == 0) {
        return NULL;
    }

    return &dir_desc->dirent;
}

int write(int file, void *buf, uint32_t byte_cnt) {
    int ret;

    hb_fat->write.fd = file;
    hb_fat->write.buf = buf;
    hb_fat->write.byte_cnt = byte_cnt;
    hb_fat->n64_buffer = &ret;
    hb_fat->command = SYS_WRITE;

    return ret;
}

int read(int file, void *buf, uint32_t byte_cnt) {
    int ret;

    hb_fat->read.fd = file;
    hb_fat->read.buf = buf;
    hb_fat->read.byte_cnt = byte_cnt;
    hb_fat->n64_buffer = &ret;
    hb_fat->command = SYS_READ;

    return ret;
}

int close(int file) {
    int ret;

    hb_fat->close.fd = file;
    hb_fat->n64_buffer = &ret;
    hb_fat->command = SYS_CLOSE;

    return ret;
}

char *getcwd(char *buf, size_t size) {
    hb_fat->getcwd.buf = buf;
    hb_fat->getcwd.size = size;
    hb_fat->command = SYS_GET_CWD;

    return buf;
}

int stat(const char *path, struct stat *buf) {
    int ret;

    hb_fat->stat.path = path;
    hb_fat->stat.stat = buf;
    hb_fat->n64_buffer = &ret;
    hb_fat->command = SYS_STAT;

    return ret;
}

int chdir(const char *path) {
    int ret;

    hb_fat->chdir.path =path;
    hb_fat->n64_buffer = &ret;
    hb_fat->command = SYS_CHDIR;

    return ret;
}

int mkdir(const char *path, mode_t mode) {
    int ret;

    hb_fat->mkdir.path = path;
    hb_fat->mkdir.mode = mode;
    hb_fat->n64_buffer = &ret;
    hb_fat->command = SYS_MKDIR;

    return ret;
}
