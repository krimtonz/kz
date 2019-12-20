#ifndef _ZU_H
#define _ZU_H

#include <stdint.h>
#include <stdlib.h>

int zu_ingame();
void zu_getfile(uint32_t vrom_addr, void *ram, size_t size);
void *zu_segment_reloc(void *phy_seg_addr);
void *zu_segment_loc(uint32_t seg_addr);

#endif