#include <stddef.h>
#include "vc_dev.h"

typedef struct {
    uint32_t func_to_clean;
    uint32_t heap_stats; 
    uint32_t recomp_stats;
} vc_dev_t;

static vc_dev_t *vc_dev = NULL;

bool vc_dev_event(void *heap_p, int event, void *arg);

static class_type_t vc_dev_class = {
    "VC-DEV",
    sizeof(vc_dev_t),
    0,
    vc_dev_event
};

static bool force_clean(void) {
    func_tree_node_t *node = NULL;
    if(!kz_treeSearchNode(gSystem->cpu->recomp_ctx->code_tree_root, vc_dev->func_to_clean, &node) &&
        !kz_treeSearchNode(gSystem->cpu->recomp_ctx->other_tree_root, vc_dev->func_to_clean, &node) &&
        !kz_treeSearchNode(kz_tree_root, vc_dev->func_to_clean, &node)) {

        return false;
    }

    treeCallerCheck(gSystem->cpu, node, 1, 0x80000000, 0x80800000);
}

static void heap_stats(void) {
    int *free = (int*)(gSystem->ram->dram_ptr + vc_dev->heap_stats);

    xlHeapGetFreeArena1(&free[0]);
    xlHeapGetFreeArena2(&free[1]);
}

static void recomp_stats(void) {
    int *free = (int*)(gSystem->ram->dram_ptr + vc_dev->recomp_stats);
    free[0] = 0;
    free[1] = 0;

    for(int i = 0; i < sizeof(gSystem->cpu->sm_blk_status) / sizeof(*gSystem->cpu->sm_blk_status); i++) {
        uint32_t blk = gSystem->cpu->sm_blk_status[i];
        if(blk == 0xFFFFFFFF) {
            continue;
        }

        for(int j = 0; j < 32; j++) {
            if(!(blk & (1 << j))) {
                free[0]++;
            }
        }
    }

    for(int i = 0; i < sizeof(gSystem->cpu->lg_blk_status) / sizeof(*gSystem->cpu->lg_blk_status); i++) {
        uint32_t blk = gSystem->cpu->lg_blk_status[i];
        if(blk == 0xFFFFFFFF) {
            continue;
        }

        for(int j = 0; j < 32; j++) {
            if(!(blk & (1 << j))) {
                free[1]++;
            }
        }
    }
}

static bool sb(void *callback, uint32_t addr, uint8_t *src) {
    return false;
}

static bool sh(void *callback, uint32_t addr, uint16_t *src) {
    return false;
}

static bool sw(void *callback, uint32_t addr, uint32_t *src) {
    addr -= 0x118e0000;

    if(addr == 0x00) {
        vc_dev->func_to_clean = *src;
        return force_clean();
    } else if(addr == 0x04) {
        vc_dev->heap_stats = *src & ~0xFC000000;
        heap_stats();
    } else if(addr == 0x08) {
        vc_dev->recomp_stats = *src & ~0xFC000000;
        recomp_stats();
    }

    return true;
}

static bool sd(void *callback, uint32_t addr, uint64_t *src) {
    return false;
}

static bool lb(void *callback, uint32_t addr, uint8_t *dst) {
    return false;
}

static bool lh(void *callback, uint32_t addr, uint16_t *dst) {
    return false;
}

static bool lw(void *callback, uint32_t addr, uint32_t *dst) {
    return false;
}

static bool ld(void *callback, uint32_t addr, uint64_t *dst ){
    return false;
}

bool vc_dev_event(void *heap_p, int event, void *arg) {
    vc_dev_t *vc_dev = (vc_dev_t*)heap_p;
    
    if(event == 0x1002) {
        if(!cpuSetDevicePut(gSystem->cpu, arg, sb, sh, sw, sd)) {
            return false;
        }
        if(!cpuSetDeviceGet(gSystem->cpu, arg, lb, lh, lw, ld)) {
            return false;
        }
    }
    
    return true;
}

bool init_vc_dev(void){
    if(!xlObjectMake((void**)&vc_dev, NULL, &vc_dev_class)) {
        return false;
    }

    if(!cpuMapObject(gSystem->cpu, vc_dev, 0x8C70000, 0x8C80000, 0)){
        return false;
    }

    return true;
}
