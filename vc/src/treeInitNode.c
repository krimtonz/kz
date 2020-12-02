#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vc.h"

HOOK bool kz_treeInitNode(func_tree_node_t **out_node, func_tree_node_t *parent, int n64_start, int n64_end) {
    func_tree_node_t *new_node;
    int out_pos;
    bool (*tree_take)(func_tree_node_t**, int*, size_t);

    if(n64_start > 0x8003DF00 && n64_start < 0x80080000) {
        tree_take = kz_cpuTreeTake;
    } else {
        tree_take = cpuTreeTake;
    }

    if(!tree_take(&new_node, &out_pos, sizeof(func_tree_node_t))) {
        return false;
    }

    new_node->n64_start = n64_start;
    new_node->n64_end = n64_end;
    new_node->code_ref_table = NULL;
    new_node->ref_cnt = 0;
    new_node->state = 0x21;
    new_node->unk_0x00 = 0;
    new_node->code = NULL;
    new_node->unk_0x08 = 0;
    new_node->unk_0x0C = NULL;
    new_node->checksum = 0;
    new_node->unk_0x28 = 1;
    new_node->size = 0;
    new_node->alloc_type = -1;
    new_node->block_pos = -1;
    new_node->code_pos = out_pos;
    new_node->parent_node = parent;
    new_node->left_node = NULL;
    new_node->right_node = NULL;
    *out_node = new_node;
    return true;
}
