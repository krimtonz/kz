
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "vc.h"

/*
 *
 * treeinit initalizes the function binary tree
 * Added recording of it's location
 * 
*/

bool treeInit(n64_cpu_t *cpu,int code_end)

{
    _start();
    recomp_ctxt_t *prVar1;
    
    prVar1 = cpu->recomp_ctx;
    if (prVar1 == NULL) {
        return false;
    }
    prVar1->node_cnt = 0;
    prVar1->f_size = 0;
    prVar1->code_end = code_end;
    prVar1->n64_start = 0;
    prVar1->n64_end = -0x80000000;
    prVar1->code_tree_root = NULL;
    prVar1->other_tree_root = NULL;
    prVar1->new_f_cnt = 0;
    prVar1->field_0x74 = 0;
    prVar1->tree_to_clean = 0;
    prVar1->node_being_cleaned = NULL;
    prVar1->node_cleaned_found = 0;
    tree_ctx = prVar1;
    memset(kz_tree,0,sizeof(kz_tree));
    memset(kz_tree_status,0,sizeof(kz_tree_status));
    return true;
}

