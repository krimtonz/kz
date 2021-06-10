#include <stdlib.h>
#include <string.h>
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

void wiimote_pad_merge(int idx, int16_t *pad) {
    if(idx == 0) {
        // DPAD-LEFT
        if(wiimote_pad & 0x0001) {
            *pad |= 0x0001;
        }

        // DPAD-RIGHT
        if(wiimote_pad & 0x0002) {
            *pad |= 0x0002;
        }

        // DPAD-DOWN
        if(wiimote_pad & 0x0004) {
            *pad |= 0x0004;
        }

        // DPAD-UP
        if(wiimote_pad & 0x0008) {
            *pad |= 0x0008;
        }

        // B
        if(wiimote_pad & 0x0400) {
            *pad |= 0x0020;
        }

        // A
        if(wiimote_pad & 0x0800) {
            // Set c-stick maximum down.
            ((int8_t*)pad)[5] = -60;
        }
    }

    unk_status_set(idx);

    // replace the displaced instruction.
    __asm__ volatile("li 23, 0;" :::);
}

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

    memset(kz_tree, 0, NUM_TREE_NODES * sizeof(*kz_tree));

    return true;

}
