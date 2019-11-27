#include "vc.h"

func_tree_node_t kz_tree[0x100] = {0};
int kz_tree_status[8] = {0};

bool kz_cpuTreeTake(func_tree_node_t **out_node, int *out_pos, int size){
    bool found_node = false;
    for(int i=0;i<8;i++){
        if(kz_tree_status[i] == -1) continue;
        for(int j=0;j<32;j++){
            if(kz_tree_status[i] & (1 << j)) continue;
            *out_pos = i * 32 + j;
            found_node = true;
            break;
        }
        if(found_node) break;
    }
    *out_node = &kz_tree[*out_pos];
    *out_pos |= 0x10000;
    return found_node;
}

__attribute__((section(".init"))) int _start(){
    return 0;
}