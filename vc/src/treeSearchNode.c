
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vc.h"

/*
 *
 * treeSearchNode searches the tree starting at start for a node containing the function 
 * n64_addr, added a manual change to search kz root if we're starting at a root to search
 * the kz tree instead
 * 
*/

HOOK bool kz_treeSearchNode(func_tree_node_t *node, int n64_addr, func_tree_node_t **found_node) {
    recomp_ctxt_t *tree_ctx = gSystem->cpu->recomp_ctx;
    if(node == tree_ctx->code_tree_root || node == tree_ctx->other_tree_root) {
        if(n64_addr > 0x8003DF00 && n64_addr < 0x80080000) {
            node = kz_tree;
        }
    }

    while(node != NULL) {
        if(n64_addr >= node->n64_start && n64_addr < node->n64_end) {
            *found_node = node;
            return true;
        }

        if(n64_addr < node->n64_start) {
            node = node->left_node;
        } else if(n64_addr > node->n64_start) {
            node = node->right_node;
        } else {
            node = NULL;
        }
    }

    return false;
}
