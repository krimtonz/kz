#ifndef _ZU_H
#define _ZU_H

#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    uint32_t    work_p;
    uint32_t    work_d;
    uint32_t    poly_opa_p;
    uint32_t    poly_opa_d;
    uint32_t    poly_xlu_p;
    uint32_t    poly_xlu_d;
    uint32_t    overlay_p;
    uint32_t    overlay_d;
} zu_disp_ptr_t;

int zu_is_ingame();
void zu_file_get(uint32_t vrom_addr, void *ram, size_t size);
void *zu_segment_reloc(void *phy_seg_addr);
void *zu_segment_find(uint32_t seg_addr);
void zu_disp_ptr_save(zu_disp_ptr_t *disp_ptr);
void zu_disp_ptr_load(zu_disp_ptr_t *disp_ptr);
void zu_gfx_reloc(int src_disp_idx, int src_cimg_idx);

#endif