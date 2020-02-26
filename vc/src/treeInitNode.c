#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vc.h"

HOOK bool kz_treeInitNode(func_tree_node_t **node,func_tree_node_t *parent,int n64_start,int n64_end)

{
    bool bVar1;
    int out_pos;
    func_tree_node_t *out_node;

    bVar1 = false;
    if(n64_start > 0x80800000){
        bVar1 = kz_cpuTreeTake(&out_node, &out_pos, 0x48);
    }else{
        bVar1 = cpuTreeTake(&out_node,&out_pos,0x48);
    }
    if (bVar1 != false) {
        out_node->n64_start = n64_start;
        out_node->n64_end = n64_end;
        out_node->code_ref_table = 0;
        out_node->ref_cnt = 0;
        out_node->field_0x20 = 0x21;
        out_node->field_0x0 = 0;
        out_node->code = NULL;
        out_node->field_0x8 = 0;
        out_node->field_0xc = NULL;
        out_node->checksum = 0;
        out_node->field_0x28 = 1;
        out_node->field_0x2c = 0;
        out_node->alloc_type = -1;
        out_node->block_pos = -1;
        out_node->code_pos = out_pos;
        out_node->parent_node = parent;
        out_node->left_node = NULL;
        out_node->right_node = NULL;
        *node = out_node;
    }
    return bVar1 != false;
}

