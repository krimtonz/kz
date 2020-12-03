#include <stdlib.h>
#include "vc.h"
#include "vc_dev.h"

#define NUM_TREE_NODES 0x80

func_tree_node_t *kz_tree = NULL;
int kz_tree_status[NUM_TREE_NODES / 32] = {0};
func_tree_node_t *kz_tree_root = NULL;

extern __attribute__((section(".bss"))) void *__bss_end;
extern __attribute__((section(".bss"))) void *__bss_start;
extern __attribute__((section(".sbss"))) void *__sbss_end;
extern __attribute__((section(".sbss"))) void *__sbss_start;

bool kz_cpuTreeTake(func_tree_node_t **out_node, int *out_pos, size_t size){
    bool found_node = false;

    for(int i = 0; i < 2; i++){
        if(kz_tree_status[i] == -1){
            continue;
        } 

        for(int j = 0; j < 32; j++){
            if(kz_tree_status[i] & (1 << j)) {
                continue;
            } 
            *out_pos = i * 32 + j;
            found_node = true;
            kz_tree_status[i] |= (1 << j);
            break;
        }

        if(found_node) {
            break;
        }
    }

    // if the kz tree is exhausted, try taking from the game's tree.
    if(!found_node) {
        return cpuTreeTake(out_node, out_pos, size);
    } else {
        *out_node = &kz_tree[*out_pos];
        *out_pos |= 0x10000;
    }
    
    return found_node;
}

ENTRY bool _start(void **dest, int size) {
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
    
    if(!hb_init(dest,size)){
        return false;
    }

    if(!init_vc_dev()) {
        return false;
    }

    if(!xlHeapTake((void**)&kz_tree, NUM_TREE_NODES * sizeof(*kz_tree))) {
        return false;
    }

    return true;

}
