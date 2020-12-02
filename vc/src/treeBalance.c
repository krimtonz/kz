#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vc.h"

HOOK bool kz_treeBalance(recomp_ctxt_t *ctx) {
    int i;

    for(i = 0; i < 3; i++) {
        func_tree_node_t *start;
        func_tree_node_t *node;
        func_tree_node_t *node2;
        int depth;

        switch(i) {
            case 0:
                start = ctx->code_tree_root;
                break;
            case 1:
                start = ctx->other_tree_root;
                break;
            case 2:
                start = kz_tree_root;
                break;
        }

        if (start == NULL) {
            continue;
        }

        for(depth = 0, node = start; node->right_node != NULL; node = node->right_node) {
            depth++;
        }

        if(depth >= 12) {
            node = start;
            node2 = start->right_node;
            for(depth = depth / 2; depth != 0; depth--) {
                node = node->right_node;
            }

            node->parent_node->right_node = NULL;
            start->right_node = node;
            node->parent_node = start;

            while(node->left_node != NULL) {
                node = node->left_node;
            }
            
            node->left_node = node2;
            node2->parent_node = node;
        }

        for(depth = 0, node = start; node->left_node != NULL; node = node->left_node) {
            depth++;
        }

        if(depth >= 12) {
            node = start;
            node2 = start->left_node;
            for(depth = depth / 2; depth != 0; depth--) {
                node = node->left_node;
            }

            node->parent_node->left_node = NULL;
            start->left_node = node;
            node->parent_node = start;

            while(node->right_node != NULL) {
                node = node->right_node;
            }

            node->right_node = node2;
            node2->parent_node = node;
        }
    }

    return true;
}

