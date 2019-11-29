#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vc.h"

bool treeBalance(recomp_ctxt_t *ctx)

{
    func_tree_node_t *pfVar1;
    func_tree_node_t *pfVar2;
    func_tree_node_t *pfVar3;
    int iVar4;
    uint32_t uVar5;
    func_tree_node_t *pfVar6;
    int iVar7;
    uint32_t uVar8;
    
    iVar7 = 0;
    do {
        if (iVar7 == 0) {
            pfVar6 = ctx->code_tree_root;
        }
        else if(iVar7 == 1){
            pfVar6 = ctx->other_tree_root;
        }else{
            pfVar6 = kz_tree_root;
        }
        if (pfVar6 != NULL) {
            iVar4 = 0;
            pfVar1 = pfVar6;
            while (pfVar1 = pfVar1->right_node, pfVar1 != NULL) {
                iVar4 = iVar4 + 1;
            }
            if (0xb < iVar4) {
                pfVar3 = pfVar6->right_node;
                uVar5 = iVar4 / 2;
                pfVar1 = pfVar6;
                if (uVar5 != 0) {
                    uVar8 = uVar5 >> 3;
                    if (uVar8 != 0) {
                        do {
                            pfVar1 = pfVar1->right_node->right_node->right_node->right_node->right_node->
                                             right_node->right_node->right_node;
                            uVar8 = uVar8 - 1;
                        } while (uVar8 != 0);
                        uVar5 = uVar5 & 7;
                        if (uVar5 == 0) goto LAB_8004e250;
                    }
                    do {
                        pfVar1 = pfVar1->right_node;
                        uVar5 = uVar5 - 1;
                    } while (uVar5 != 0);
                }
LAB_8004e250:
                pfVar1->parent_node->right_node = NULL;
                pfVar6->right_node = pfVar1;
                pfVar1->parent_node = pfVar6;
                do {
                    pfVar2 = pfVar1;
                    pfVar1 = pfVar2->left_node;
                } while (pfVar2->left_node != NULL);
                pfVar2->left_node = pfVar3;
                pfVar3->parent_node = pfVar2;
            }
            iVar4 = 0;
            pfVar1 = pfVar6;
            while (pfVar1 = pfVar1->left_node, pfVar1 != NULL) {
                iVar4 = iVar4 + 1;
            }
            if (0xb < iVar4) {
                pfVar3 = pfVar6->left_node;
                uVar5 = iVar4 / 2;
                pfVar1 = pfVar6;
                if (uVar5 != 0) {
                    uVar8 = uVar5 >> 3;
                    if (uVar8 != 0) {
                        do {
                            pfVar1 = pfVar1->left_node->left_node->left_node->left_node->left_node->left_node->
                                             left_node->left_node;
                            uVar8 = uVar8 - 1;
                        } while (uVar8 != 0);
                        uVar5 = uVar5 & 7;
                        if (uVar5 == 0) goto LAB_8004e2fc;
                    }
                    do {
                        pfVar1 = pfVar1->left_node;
                        uVar5 = uVar5 - 1;
                    } while (uVar5 != 0);
                }
LAB_8004e2fc:
                pfVar1->parent_node->left_node = NULL;
                pfVar6->left_node = pfVar1;
                pfVar1->parent_node = pfVar6;
                do {
                    pfVar6 = pfVar1;
                    pfVar1 = pfVar6->right_node;
                } while (pfVar6->right_node != NULL);
                pfVar6->right_node = pfVar3;
                pfVar3->parent_node = pfVar6;
            }
        }
        iVar7 = iVar7 + 1;
        if (2 < iVar7) {
            return true;
        }
    } while( true );
}

