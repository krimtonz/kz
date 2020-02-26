/*
* start.h
*
* defintions for kz entering and exiting
*/

#ifndef __START_H
#define __START_H

#define kz_exit(func, ...) \
    exit_func = func; \
    _kz_exit(__VA_ARGS__);

void    _start();
void   *_kz_exit();

__attribute__((section(".data")))
extern void *exit_func;

#endif