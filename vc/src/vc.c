#include <stdlib.h>
#include "vc.h"

func_tree_node_t kz_tree[0x200] = {0};
int kz_tree_status[0x10] = {0};
recomp_ctxt_t *tree_ctx = NULL;
func_tree_node_t *kz_tree_root = NULL;

extern __attribute__((section(".bss"))) void *__bss_end;
extern __attribute__((section(".bss"))) void *__bss_start;
extern __attribute__((section(".sbss"))) void *__sbss_end;
extern __attribute__((section(".sbss"))) void *__sbss_start;

bool kz_cpuTreeTake(func_tree_node_t **out_node, int *out_pos, int size){
    bool found_node = false;
    for(int i=0;i<0x10;i++){
        if(kz_tree_status[i] == -1) continue;
        for(int j=0;j<32;j++){
            if(kz_tree_status[i] & (1 << j)) continue;
            *out_pos = i * 32 + j;
            found_node = true;
            kz_tree_status[i] |= (1 << j);
            break;
        }
        if(found_node) break;
    }
    *out_node = &kz_tree[*out_pos];
    *out_pos |= 0x10000;
    return found_node;
}

HOOK bool hb_hook(void **dest, int size){
    return hb_init(dest,size);
}

ENTRY int _start(){
    char *p = (char*)&__bss_start;
    char *end = (char*)&__bss_end;
    while (p < end){
        *p++ = 0;
    }
    p = (char*)&__sbss_start;
    end = (char*)&__sbss_end;
    while (p < end){
        *p++ = 0;
    }
    return 0;
}