/*
* vc.h
*
* definitions for vc optinization fixes
*/

#ifndef _VC_H
#define _VC_H

#include <stdint.h>
#include <stdbool.h>

#ifndef VC_VERSION
#error no vc version specified
#endif

#define NARJ    0x00
#define NARE    0x01

#define ENTRY __attribute__((section(".init")))
#define HOOK  __attribute__((section(".text.hook")))

typedef struct cpu_cache_entry_s    cpu_cache_entry_t;
typedef struct func_tree_node_s     func_tree_node_t;
typedef struct n64_cpu_s            n64_cpu_t;
typedef struct recomp_ctxt_s        recomp_ctxt_t;

struct cpu_cache_entry_s {
    int                 n64_addr;
    void               *vc_addr;
    func_tree_node_t   *node;
};

struct func_tree_node_s {
    int                 field_0x0;
    void               *code;
    int                 field_0x8;
    int                *field_0xc;
    int                 n64_start;
    int                 n64_end;
    int                 code_ref_table;
    int                 ref_cnt;
    int                 field_0x20;
    int                 checksum;
    int                 field_0x28;
    int                 field_0x2c;
    int                 alloc_type;
    int                 block_pos;
    int                 code_pos;
    func_tree_node_t   *parent_node;
    func_tree_node_t   *left_node;
    func_tree_node_t   *right_node;
};

struct n64_cpu_s {
    int                 status;
    char                field_0x4[0x14];
    int                *field_0x18;
    char                field_0x1c[0x14];
    int                 cache_cnt;
    char                field_0x34[0x4];
    int                 pc;
    int                 field_0x3c;
    char                field_0x40[0xC];
    func_tree_node_t   *working_node;
    int                 field_0x50;
    int                 field_0x54;
    char                field_0x58[0x14];
    uint32_t            gpr[64];
    char                field_0x16c[0x938];
    int                 field_0xaa4;
    char                field_0xaa8[0xC];
    int                 field_0xab4;
    char                field_0xab8[0xA0];
    void               *cpuExecuteOpcode_function;
    void               *cpuExecuteJump_function;
    void               *cpuExecuteCall_function;
    void               *cpuExecuteIdle_function;
    void               *cpuExecuteLoadStore_function;
    void               *cpuExecuteLoadStoreF_function;
    int                 field_0xb70;
    int                 field_0xb74;
    char                field_0xb78[0x8];
    int                *mem_domain[256];
    char                mem_idx[65536];
    void               *sm_blk_code;
    void               *lg_blk_code;
    int                 sm_blk_status[256];
    int                 lg_blk_status[13];
    recomp_ctxt_t      *recomp_ctx;
    char                field_0x113c0[0xDC0];
    int                 field_0x12180;
    int                 field_0x12184;
    char                field_0x12188[0x4];
    int                 field_0x1218c;
    char                field_0x12190[0x20];
    int                 field_0x121b0;
    char                field_0x121b4[0xC];
};

struct recomp_ctxt_s {
    uint16_t            node_cnt;
    uint16_t            field_0x2;
    int                 f_size;
    int                 code_end;
    int                 n64_start;
    int                 n64_end;
    char                field_0x14[32];
    int                 field_0x34;
    char                field_0x38[44];
    func_tree_node_t   *kz_tree_root;
    func_tree_node_t   *code_tree_root;
    func_tree_node_t   *other_tree_root;
    int                 new_f_cnt;
    int                 field_0x74;
    int                 tree_to_clean;
    func_tree_node_t   *node_being_cleaned;
    int                 node_cleaned_found;
};

bool    kz_cpuTreeTake  (func_tree_node_t **out_node, int *out_pos, int size);
bool    treeBalance     (recomp_ctxt_t *ctx);
int     _start          ();

#if VC_VERSION == NARJ
#define xlHeapTake_addr     0x800887e0
#define cpuTreeTake_addr    0x8004c8f0
#define cpuHeapTake_addr    0x8004c5c0
#define treeInitNode_addr   0x8004d5c4
#define heap_size_hook_addr 0x80091f54
#define treeAdjustRoot_addr 0x8004e3b0
#define treeInsertNode_addr 0x8004e140
#define treeInsert_addr     0x8004e030
#define treeInit_addr       0x8004d56c
#define treeSearchNode_addr 0x8004e594
#define ramSetSize_addr     0x8005083c
#define ramSetSizeCall_addr 0x8001563c
#define mem2_heap_size_addr 0x80091FF4
#else
#define xlHeapTake_addr     0x80088790
#define cpuTreeTake_addr    0x8004c87c
#define cpuHeapTake_addr    0x8004c54c
#define treeInitNode_addr   0x8004d550
#define heap_size_hook_addr 0x80091ea4
#define treeAdjustRoot_addr 0x8004e33c
#define treeInsertNode_addr 0x8004e0cc
#define treeInsert_addr     0x8004dfbc
#define treeInit_addr       0x8004d4f8
#define treeSearchNode_addr 0x8004e520
#define ramSetSizeCall_addr 0x8001563c
#define ramSetSize_addr     0x800507c8
#define mem2_heap_size_addr 0x80091F44
#endif

#define hb_init_addr        0x90000800

typedef bool    (*xlHeapTake_t)     (void **dst, int size);
typedef bool    (*cpuTreeTake_t)    (func_tree_node_t **out_node, int *out_pos, int size);
typedef bool    (*treeAdjustRoot_t) (n64_cpu_t *cpu,int n64_start,int n64_end);
typedef bool    (*treeInsertNode_t) (func_tree_node_t **start,int n64_start,int n64_end,func_tree_node_t **new_node);
typedef bool    (*ramSetSize_t)     (void **dst, int size);

#define xlHeapTake      ((xlHeapTake_t)     xlHeapTake_addr)
#define cpuTreeTake     ((cpuTreeTake_t)    cpuTreeTake_addr)
#define treeAdjustRoot  ((treeAdjustRoot_t) treeAdjustRoot_addr)
#define treeInsertNode  ((treeInsertNode_t) treeInsertNode_addr)
#define hb_init         ((ramSetSize_t)     hb_init_addr)

extern func_tree_node_t     kz_tree[0x200];
extern int                  kz_tree_status[0x10];
extern recomp_ctxt_t       *tree_ctx;
extern func_tree_node_t    *kz_tree_root;

#endif