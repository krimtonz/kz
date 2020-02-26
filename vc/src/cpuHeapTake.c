#include <stdint.h>
#include <stdbool.h>
#include "vc.h"

HOOK bool kz_cpuHeapTake(int **code,int cpu,func_tree_node_t *node,int size)

{
    bool bVar1;
    int iVar2;
    int iVar3;
    bool bVar4;
    bool ret;
    uint32_t uVar6;
    uint32_t *puVar7;
    int iVar8;
    int iVar9;
    int unaff_r25;
    int unaff_r26;
    uint32_t *unaff_r27;
    int iVar10;
    
    uVar6 = size + 0x1ff;
    bVar4 = false;
    bVar1 = false;
    iVar10 = ((int)uVar6 >> 9) + (uint32_t)((int)uVar6 < 0 && (uVar6 & 0x1ff) != 0);
    iVar2 = (size + 0x9ff) / 0xa00 + (size + 0x9ff >> 0x1f);
    do {
        iVar3 = node->alloc_type;
        if (iVar3 == -1) {
            if(node->n64_start > 0x80800000){
                node->alloc_type = 3;
            }
            else if (size < 0x3c01) {
                node->alloc_type = 1;
            }
            else {
                node->alloc_type = 2;
            }
        }
        else {
            if (iVar3 == 1) {
                node->alloc_type = 2;
                bVar1 = true;
            }
            else {
                if (iVar3 == 2) {
                    node->alloc_type = 1;
                    bVar1 = true;
                }
            }
        }
        if (node->alloc_type == 1) {
            node->alloc_type = 1;
            unaff_r26 = 0x100;
            unaff_r27 = (uint32_t *)(cpu + 0x10f88);
            unaff_r25 = iVar10;
            if ((bVar1) && (0x1f < iVar10)) {
                node->alloc_type = 3;
                node->block_pos = -1;
                return xlHeapTake((void**)code,size);
            }
        }
        else if (node->alloc_type == 2) {
            node->alloc_type = 2;
            unaff_r26 = 0xd;
            unaff_r27 = (uint32_t *)(cpu + 0x11388);
            unaff_r25 = iVar2 - (iVar2 >> 0x1f);
        }
        if (node->alloc_type == 3 || 0x1f < unaff_r25) {
            node->alloc_type = 3;
            node->block_pos = -1;
            return xlHeapTake((void**)code,size);
        }
        iVar8 = 0;
        puVar7 = unaff_r27;
        iVar3 = unaff_r26;
        if (0 < unaff_r26) {
            do {
                uVar6 = (1 << unaff_r25) - 1;
                iVar9 = 0x21 - unaff_r25;
                if (*puVar7 != 0xffffffff) {
                    do {
                        if ((*puVar7 & uVar6) == 0) {
                            bVar4 = true;
                            *puVar7 = *puVar7 | uVar6;
                            node->block_pos = unaff_r25 << 0x10 | iVar8 + ((0x21 - unaff_r25) - iVar9);
                            break;
                        }
                        iVar9 = iVar9 + -1;
                        uVar6 = uVar6 << 1;
                    } while (iVar9 != 0);
                }
                if (bVar4) break;
                puVar7 = puVar7 + 1;
                iVar8 = iVar8 + 0x20;
                iVar3 = iVar3 + -1;
            } while (iVar3 != 0);
        }
        if (bVar4) {
            if (node->alloc_type == 1) {
                *code = (int *)(*(int *)(cpu + 0x10f80) + (node->block_pos & 0xffffU) * 0x200);
            }
            else {
                *code = (int *)(*(int *)(cpu + 0x10f84) + (node->block_pos & 0xffffU) * 0xa00);
            }
            ret = 1;
            goto LAB_8004c7ec;
        }
        if (bVar1) {
            ret = 0;
            node->alloc_type = -1;
            node->block_pos = -1;
LAB_8004c7ec:
            return ret;
        }
    } while( true );
}

