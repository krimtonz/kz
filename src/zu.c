#include <mips.h>
#include "zu.h"
#include "z2.h"

int zu_ingame(){
    return z2_ctxt.gamestate_dtor == z2_gamestate_table[3].vram_dtor;
}

void zu_getfile(uint32_t vrom_addr, void *ram, size_t size){
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

void *zu_segment_reloc(void *phy_seg_addr){
    uint32_t *phy_32 = phy_seg_addr;
    uint32_t addr = *phy_32;
    uint8_t seg = (addr >> 24) & 0xF;
    uint32_t off = addr & 0x00FFFFFF;
    return (void*)(*phy_32 = MIPS_PHYS_TO_KSEG0(z2_segment.segments[seg] + off));
}

void *zu_segment_loc(uint32_t seg_addr){
    uint8_t segment_id = (seg_addr >> 24) & 0xF;
    uint32_t offset = seg_addr & 0x00FFFFFF;
    uint32_t physical = z2_segment.segments[segment_id] + offset;
    if(!physical){
        return NULL;
    }
    return (void*)MIPS_PHYS_TO_KSEG0(physical);
}