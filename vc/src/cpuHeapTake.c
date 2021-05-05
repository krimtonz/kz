#include <stdint.h>
#include <stdbool.h>
#include "vc.h"

#define SM_BLK_SIZE 0x200
#define LG_BLK_SIZE 0xA00

HOOK bool kz_cpuHeapTake(uint32_t **code, n64_cpu_t *cpu, func_tree_node_t *node, size_t size)
{
    bool swapped_type = false;
    bool chunk_found = false;
    uint32_t *blk_status;
    uint32_t sm_blk_needed = (size + (SM_BLK_SIZE - 1)) / SM_BLK_SIZE;
    uint32_t lg_blk_needed = (size + (LG_BLK_SIZE - 1)) / LG_BLK_SIZE;
    uint32_t blks_needed;
    int status_cnt;

    while(!swapped_type) {
        switch(node->alloc_type) {
            case -1:
                if(node >= &kz_tree[0] && node < &kz_tree[0x80]){
                    node->alloc_type = 3;
                }
                else if (size < 0x3c01) {
                    node->alloc_type = 1;
                }
                else {
                    node->alloc_type = 2;
                }
                break;
            case 1:
                node->alloc_type = 2;
                swapped_type = true;
            case 2:
                node->alloc_type = 1;
                swapped_type = true;
                break;
        }

        if (node->alloc_type == 1) {
            node->alloc_type = 1;
            status_cnt = sizeof(cpu->sm_blk_status) / sizeof(*cpu->sm_blk_status);
            blk_status = cpu->sm_blk_status;
            blks_needed = sm_blk_needed;
            if (swapped_type && sm_blk_needed > 31) {
                // We tried swapping from large block to small block, but can't fulfill
                // the request in a single small block chain.  Try taking from sys memory.
                node->alloc_type = 3;
                node->block_pos = -1;
                return xlHeapTake((void**)code, size);
            }
        }
        else if (node->alloc_type == 2) {
            node->alloc_type = 2;
            status_cnt = sizeof(cpu->lg_blk_status) / sizeof(*cpu->lg_blk_status);
            blk_status = cpu->lg_blk_status;
            blks_needed = lg_blk_needed;
        }

        if (node->alloc_type == 3 || blks_needed > 31) {
            // We tried swapping from small blocks to large blocks, but can't fulfill
            // the request in a single large block chain.  Try taking from sys memory.
            node->alloc_type = 3;
            node->block_pos = -1;
            return xlHeapTake((void**)code, size);
        }

        for(int i = 0; i < status_cnt; i++) {
            uint32_t needed_mask = (1 << blks_needed) - 1;
            if(blk_status[i] == 0xFFFFFFFF) {
                continue;
            }

            for(int j = 0; j < 32 - blks_needed; j++) {
                if((blk_status[i] & needed_mask) == 0) {
                    chunk_found = true;
                    blk_status[i] |= needed_mask;
                    node->block_pos = (blks_needed << 16) | ((i * 32) + j);
                    break;
                }

                needed_mask <<= 1;
            }

            if(chunk_found) {
                break;
            }
        }

        if (chunk_found) {
            if (node->alloc_type == 1) {
                *code = cpu->sm_blk_code + ((node->block_pos & 0xFFFF) * SM_BLK_SIZE);
            }
            else {
                *code = cpu->lg_blk_code + ((node->block_pos & 0xFFFF) * LG_BLK_SIZE);
            }
            return true;
        }
    }

    node->alloc_type = -1;
    node->block_pos = -1;
    return false;
}

