#include <mips.h>
#include "zu.h"
#include "z2.h"

int zu_is_ingame()
{
    return z2_ctxt.gamestate_dtor == z2_gamestate_table[3].vram_dtor;
}

void zu_file_load(uint32_t vrom_addr, void *ram, size_t size)
{
    OSMesgQueue queue;
    OSMesg mesg;
    osCreateMesgQueue(&queue, &mesg, 1);
    z2_loadfile_t file = {
        vrom_addr,  ram,    size,
        NULL,       0,      0,
        &queue,     0
    };

    osSendMesg(&z2_file_msgqueue, &file, OS_MESG_NOBLOCK);
    osRecvMesg(&queue, NULL, OS_MESG_BLOCK);
}

void zu_file_idx_load(int idx, void *ram)
{
    zu_file_load(z2_file_table[idx].vrom_start, ram, z2_file_table[idx].vrom_end - z2_file_table[idx].vrom_start);
}

void *zu_segment_reloc(void *phy_seg_addr)
{
    uint32_t *phy_32 = phy_seg_addr;
    uint32_t addr = *phy_32;
    uint8_t seg = (addr >> 24) & 0xF;
    uint32_t off = addr & 0x00FFFFFF;
    return (void*)(*phy_32 = MIPS_PHYS_TO_KSEG0(z2_segment.segments[seg] + off));
}

void *zu_segment_find(uint32_t seg_addr)
{
    uint8_t segment_id = (seg_addr >> 24) & 0xF;
    uint32_t offset = seg_addr & 0x00FFFFFF;
    uint32_t physical = z2_segment.segments[segment_id] + offset;
    if(!physical)
    {
        return NULL;
    }

    return (void*)MIPS_PHYS_TO_KSEG0(physical);
}

void zu_disp_ptr_save(zu_disp_ptr_t *disp_ptr)
{
    z2_gfx_t *gfx = z2_game.common.gfx;
    disp_ptr->work_p = gfx->work.p - gfx->work.buf;
    disp_ptr->work_d = gfx->work.d - gfx->work.buf;
    disp_ptr->poly_opa_p = gfx->poly_opa.p - gfx->poly_opa.buf;
    disp_ptr->poly_opa_d = gfx->poly_opa.d - gfx->poly_opa.buf;
    disp_ptr->poly_xlu_p = gfx->poly_xlu.p - gfx->poly_xlu.buf;
    disp_ptr->poly_xlu_d = gfx->poly_xlu.d - gfx->poly_xlu.buf;
    disp_ptr->overlay_p = gfx->overlay.p - gfx->overlay.buf;
    disp_ptr->overlay_d = gfx->overlay.d - gfx->overlay.buf;
}

void zu_disp_ptr_load(zu_disp_ptr_t *disp_ptr)
{
    z2_gfx_t *gfx = z2_game.common.gfx;
    gfx->work.p = disp_ptr->work_p + gfx->work.buf;
    gfx->work.d = disp_ptr->work_d + gfx->work.buf;
    gfx->poly_opa.p = disp_ptr->poly_opa_p + gfx->poly_opa.buf;
    gfx->poly_opa.d = disp_ptr->poly_opa_d + gfx->poly_opa.buf;
    gfx->poly_xlu.p = disp_ptr->poly_xlu_p + gfx->poly_xlu.buf;
    gfx->poly_xlu.d = disp_ptr->poly_xlu_d + gfx->poly_xlu.buf;
    gfx->overlay.p = disp_ptr->overlay_p + gfx->overlay.buf;
    gfx->overlay.d = disp_ptr->overlay_d + gfx->overlay.buf;
}

void zu_gfx_reloc(int src_disp_idx, int src_cimg_idx)
{
    z2_gfx_t *gfx = z2_game.common.gfx;
    uint32_t src_gfx = (uint32_t)&z2_disp[src_disp_idx * Z2_DISP_SIZE];
    uint32_t dst_gfx = (uint32_t)&z2_disp[(gfx->frame_cnt_1 & 1) * Z2_DISP_SIZE];
    uint32_t src_cimg = (uint32_t)z2_cimg[src_cimg_idx];
    uint32_t dst_cimg = (uint32_t)z2_cimg[gfx->frame_cnt_2 & 1];
    
    z2_disp_buf_t segment_setup;
    segment_setup.buf = (Gfx*)(dst_gfx + 0x140);
    segment_setup.p = segment_setup.buf + 21;

    z2_disp_buf_t primary;
    primary.buf = (Gfx*)(dst_gfx + 0x02A8);
    primary.p = primary.buf + 12;
    
    z2_disp_buf_t *new_disp[6] = {
        &gfx->work,
        &gfx->poly_opa,
        &gfx->poly_xlu,
        &gfx->overlay,
        &primary,
        &segment_setup,
    };

    for(int i = 0; i < sizeof(new_disp) / sizeof(*new_disp); i++)
    {
        z2_disp_buf_t *dbuf = new_disp[i];
        for(Gfx *p = dbuf->buf; p != dbuf->p; p++)
        {
            switch(p->hi >> 24){
                case G_VTX: break;
                case G_DMA_IO: break;
                case G_MTX: break;
                case G_MOVEWORD:
                    switch((p->hi >> 16) & 0xFF){
                        case G_MW_SEGMENT: break;
                        default: continue;
                    }
                case G_MOVEMEM: break;
                case G_LOAD_UCODE: break;
                case G_DL: break;
                case G_RDPHALF_1:
                    switch(p[1].hi >> 24)
                    {
                        case G_BRANCH_Z: break;
                        case G_LOAD_UCODE: break;
                        default: continue;
                    }
                case G_SETTIMG: break;
                case G_SETZIMG: break;
                case G_SETCIMG: break;
                case G_BG_1CYC: break;
                case G_BG_COPY: break;
                default: continue;
            }

            if(p->lo >=src_gfx && p->lo < src_gfx + Z2_DISP_SIZE)
            {
                p->lo += dst_gfx - src_gfx;
            }

            if(p->lo >= src_cimg && p->lo < src_cimg + Z2_CIMG_SIZE)
            {
                p->lo = dst_cimg;
            }
        }
    }
}
