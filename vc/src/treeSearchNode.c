
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

HOOK bool treeSearchNode(func_tree_node_t *start,int n64_addr,func_tree_node_t **found_node)

{
    int iVar1;
    
    if (start == NULL) {
        return false;
    }
    if ((start == tree_ctx->other_tree_root || start == tree_ctx->code_tree_root) && n64_addr > 0x80800000){
        start = kz_tree;
    }
    do {
        iVar1 = start->n64_start;
        if ((iVar1 <= n64_addr) && (n64_addr < start->n64_end)) {
            *found_node = start;
            return true;
        }
        if (n64_addr < iVar1) {
            start = start->left_node;
        }
        else {
            if (iVar1 < n64_addr) {
                start = start->right_node;
            }
            else {
                start = NULL;
            }
        }
    } while (start != NULL);
    return false;
}

