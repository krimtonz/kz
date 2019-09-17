.n64
.relativeinclude on

.create "../patched.nzse.lite.z64", 0
.incbin "../base.nzse.z64"

.definelabel G_PAYLOAD_VROM, 0x02EE8000
.definelabel G_PAYLOAD_SIZE, filesize("../bin/kz-lite-NZSE/kz.bin") + 0x60
.definelabel G_PAYLOAD_ADDR, 0x80690000
.definelabel G_KZ_ADDR, G_PAYLOAD_ADDR + 0x60

.orga 0x10
.word 0x5354631C, 0x03A2DEF0

; Add dmatable entry for the payload code
.orga 0x205F0
.word G_PAYLOAD_VROM, (G_PAYLOAD_VROM + G_PAYLOAD_SIZE), G_PAYLOAD_VROM, 0

; ==================================================================================================
; Base game editing region
; ==================================================================================================

.headersize(0x800A5AC0 - 0xB3C000)
; ==================================================================================================
; Custom Code Payload
; ==================================================================================================

; ==================================================================================================
; Set the size of the main heap
; ==================================================================================================
.include "kz-lite-NZSE.asm"

.org HEAP_SIZE_ADDR
lui     t8, hi(G_PAYLOAD_ADDR)
addiu   t8, lo(G_PAYLOAD_ADDR)
.skip 4
sw      a1, 0x1528(v1)

.org DMA_HOOK_ADDR
    addiu   sp, sp, -0x340
    sw      ra, 0x002C(sp)
    lui     a0, hi(G_PAYLOAD_ADDR)
    addiu   a0, lo(G_PAYLOAD_ADDR)
    lui     a1, hi(G_PAYLOAD_VROM)
    addiu   a1, lo(G_PAYLOAD_VROM)
    lui     a2, hi(G_PAYLOAD_SIZE)
    jal     0x80080C90
    addiu   a2, lo(G_PAYLOAD_SIZE)
    jal     ainit
    lui     a0, 0x0004

; ==================================================================================================
; Game Class Frame Start Hook 
; ==================================================================================================
.org INPUT_HOOK_ADDR
jal INPUT_HOOK_REPLACE
nop

; 801737A0
; Replaces jalr ra, t9
;          nop
.org MAIN_HOOK_ADDR
jal     G_KZ_ADDR
or      a1, r0, t9

; disable dpad navigation in pause menu
.orga 0xC9C260
    nop


; ==================================================================================================
; New code region
; ==================================================================================================

.headersize(G_PAYLOAD_ADDR - G_PAYLOAD_VROM)
.org G_PAYLOAD_ADDR
ainit:
    lui     s0, 0x801C
    jr		ra
    addiu   s0, s0, 0xD910
.org G_KZ_ADDR
.incbin("../bin/kz-lite-NZSE/kz.bin")
.align 8
.close