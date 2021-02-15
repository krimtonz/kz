/*
* vc.h
*
* definitions for vc optinization fixes
*/

#ifndef _VC_H
#define _VC_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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
typedef struct thread_context_s     thread_context_t;

struct thread_context_s {
    /* 0x0000 */ uint32_t gpr[32];
    /* 0x0080 */ uint32_t cr;
    /* 0x0084 */ uint32_t lr;
    /* 0x0088 */ uint32_t ctr;
    /* 0x008C */ uint32_t xer;
    /* 0x0090 */ double fpr[32];
    /* 0x0190 */ uint64_t fscr;
    /* 0x0198 */ uint32_t srr0;
    /* 0x019C */ uint32_t srr1;
    /* 0x01A0 */ char unk_0x1A0[8];
    /* 0x01A8 */ uint32_t gqr[7];
    /* 0x01C8 */ uint64_t ps[32];
    /* 0x02C8 */ char unk_0x2C8[0x14];
    /* 0x02DC */ thread_context_t **queue;
    /* 0x02E0 */ thread_context_t *next;
    /* 0x02E4 */ thread_context_t *prev;
    /* 0x02E8 */ char unk_0x2E8[0x1C];
    /* 0x0304 */ void *stack_start;
    /* 0x0308 */ void *stack_end;
    /* 0x030C */ char unk_0x30C[12];
}; // size = 0x318

typedef bool (*lb_t)(void *dev_obj, uint32_t addr, uint8_t *dest);
typedef bool (*lh_t)(void *dev_obj, uint32_t addr, uint16_t *dest);
typedef bool (*lw_t)(void *dev_obj, uint32_t addr, uint32_t *dest);
typedef bool (*ld_t)(void *dev_obj, uint32_t addr, uint64_t *dest);
typedef bool (*sb_t)(void *dev_obj, uint32_t addr, uint8_t *src);
typedef bool (*sh_t)(void *dev_obj, uint32_t addr, uint16_t *src);
typedef bool (*sw_t)(void *dev_obj, uint32_t addr, uint32_t *src);
typedef bool (*sd_t)(void *dev_obj, uint32_t addr, uint64_t *src);

typedef struct{
    char        unk_0x00_[0x04];    /* 0x00 */
    void       *dev_obj;            /* 0x04 */
    uint32_t    kseg0_offset;       /* 0x08 */
    lb_t        lb;                 /* 0x0C */
    lh_t        lh;                 /* 0x10 */
    lw_t        lw;                 /* 0x14 */
    ld_t        ld;                 /* 0x18 */
    sb_t        sb;                 /* 0x1C */
    sh_t        sh;                 /* 0x20 */
    sw_t        sw;                 /* 0x24 */
    sd_t        sd;                 /* 0x28 */
    uint32_t    phys_addr;          /* 0x2C */
    uint32_t    size;               /* 0x30 */
} cpu_dev_t;

struct cpu_cache_entry_s {
    int                 n64_addr;
    void               *vc_addr;
    func_tree_node_t   *node;
};

typedef struct {
    int     n64_addr;
    void   *branch_addr;
} ext_call_t;

struct func_tree_node_s {
    int                 unk_0x00;
    void               *code;
    int                 unk_0x08;
    int                *unk_0x0C;
    int                 n64_start;
    int                 n64_end;
    ext_call_t         *code_ref_table;
    int                 ref_cnt;
    int                 state;
    int                 checksum;
    int                 unk_0x28;
    int                 size;
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
    cpu_dev_t          *mem_domain[256];
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

typedef struct {
    char    unk_0x00[4];
    char   *dram_ptr;
    size_t  dram_size;
} n64_ram_t;

typedef struct{
    char        unk_0x00[0x28];
    n64_cpu_t  *cpu;
    void       *pif;
    n64_ram_t  *ram;
} gClassSystem_t;

typedef struct {
    const char *name;
    size_t size;
    int unk_08;
    bool (*event_handler)(void *heap, int event, void *arg);
} class_type_t;

bool    kz_treeSearchNode(func_tree_node_t *node, int n64_addr, func_tree_node_t **found_node);
bool    kz_cpuTreeTake  (func_tree_node_t **out_node, int *out_pos, size_t size);
bool    kz_treeBalance  (recomp_ctxt_t *ctx);

#define hb_init     ((bool (*)(void**, int)) 0x90000800)

#define vc_extern extern __attribute__((section(".data")))
vc_extern bool  xlHeapTake          (void **dst, int size);
vc_extern bool  cpuTreeTake         (func_tree_node_t **out_node, int *out_pos, size_t size);
vc_extern bool  treeAdjustRoot      (n64_cpu_t *cpu, int n64_start, int n64_end);
vc_extern bool  treeInsertNode      (func_tree_node_t **start, int n64_start, int n64_end, func_tree_node_t **new_node);
vc_extern bool  ramSetSize          (void **dst, int size);
vc_extern bool  xlObjectMake        (void **obj, void *parent, class_type_t *class);
vc_extern bool  cpuMapObject        (n64_cpu_t *cpu, void *dev, uint32_t addr_start, uint32_t addr_end, uint32_t arg4);
vc_extern bool  cpuSetDevicePut     (n64_cpu_t *cpu, cpu_dev_t *dev, void *sb, void *sh, void *sw, void *sd);
vc_extern bool  cpuSetDeviceGet     (n64_cpu_t *cpu, cpu_dev_t *dev, void *lb, void *lh, void *lw, void *ld);
vc_extern bool  treeCallerCheck     (n64_cpu_t *cpu, func_tree_node_t *node, uint32_t arg2, uint32_t n64_start, uint32_t n64_end);
vc_extern bool  xlHeapGetFreeArena1 (int *free);
vc_extern bool  xlHeapGetFreeArena2 (int *free);
vc_extern gClassSystem_t *gSystem;

extern func_tree_node_t    *kz_tree;
extern int                  kz_tree_status[];
extern func_tree_node_t    *kz_tree_root;

#endif
