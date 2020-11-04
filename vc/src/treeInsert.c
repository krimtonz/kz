#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vc.h"

HOOK bool kz_treeInsert(n64_cpu_t *cpu,uint32_t n64_start,int n64_end)

{
    bool bVar1;
    bool ret;
    recomp_ctxt_t *ctx;
    func_tree_node_t *new_node;
    
    ctx = cpu->recomp_ctx;
    if (ctx == NULL) {
        ret = false;
    }
    else {
        if (((int)n64_start < ctx->code_end) && (ctx->code_end < n64_end)) {
            treeAdjustRoot(cpu,n64_start,n64_end);
        }
        if(n64_start <= 0x8003DF00 || n64_start >= 0x80080000){
            ctx->node_cnt = ctx->node_cnt + 1;
            ctx->f_size = ctx->f_size + 0x48;
        }
        if (n64_start != 0x80000180) {
            if ((int)n64_start < ctx->n64_start) {
                ctx->n64_start = n64_start;
            }
            if (ctx->n64_end < n64_end) {
                ctx->n64_end = n64_end;
            }
        }
        if((int)n64_start > 0x8003DF00 && (int)n64_start < 0x80080000) {
            bVar1 = treeInsertNode(&kz_tree_root,n64_start,n64_end,&new_node);
        }
        else if((int)n64_start == ctx->code_end){
            // I have no idea why this is here
            return false;
        }else if((int)n64_start < ctx->code_end){
            bVar1 = treeInsertNode(&ctx->code_tree_root,n64_start,n64_end,&new_node);
        }
        else {
            bVar1 = treeInsertNode(&ctx->other_tree_root,n64_start,n64_end,&new_node);
        }
        if (bVar1 == false) {
            ret = false;
        }
        else {
            ret = kz_treeBalance(ctx);
        }
    }
    return ret;
}
