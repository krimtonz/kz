#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vc.h"

HOOK bool kz_treeInsert(n64_cpu_t *cpu, int n64_start, int n64_end) {
    recomp_ctxt_t *tree = cpu->recomp_ctx;
    func_tree_node_t *new_node;
    int node_added;

    if(tree == NULL) {
        return false;
    }

    if(n64_start < tree->code_end && n64_end > tree->code_end) {
        treeAdjustRoot(cpu, n64_start, n64_end);
    }

    tree->node_cnt++;
    tree->f_size += sizeof(func_tree_node_t);


    if(n64_start != 0x80000180) {
        if(n64_start < tree->n64_start) {
            tree->n64_start = n64_start;
        }

        if(n64_end > tree->n64_end) {
            tree->n64_end = n64_end;
        }
    }

    if(n64_start > 0x8003DF00 && n64_end < 0x80080000) {
        node_added = treeInsertNode(&kz_tree_root, n64_start, n64_end, &new_node);
    } else if(n64_start < tree->code_end) {
        node_added = treeInsertNode(&tree->code_tree_root, n64_start, n64_end, &new_node);
    } else if(n64_start > tree->code_end) {
        node_added = treeInsertNode(&tree->other_tree_root, n64_start, n64_end, &new_node);
    } else {
        return false;
    }

    if(node_added) {
        return kz_treeBalance(tree);
    }

    return false;
}
