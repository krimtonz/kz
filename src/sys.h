#ifndef _SYS_H
#define _SYS_H

#include <sys/stat.h>
#include <fcntl.h>

int open(const char *path, int open_flags, ...);
int creat(const char *path, mode_t mode);
int write(int file, void *buf, uint32_t byte_cnt);
int close(int file);
time_t time(time_t *tloc);

#endif