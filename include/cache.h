/*
* cache.h
*
* definitions for vr4300 cache handling
*/


#ifndef _LIBUM_CACHE_H
#define _LIBUM_CACHE_H

void    osInvalICache       (void *ptr, int size);
void    osWritebackDCache   (void *ptr, int size);
void    osInvalDCache       (void *ptr, int size);

#endif