.n64
.relativeinclude on

.create "../patched.nzsj10.lite.z64", 0
.incbin "../base.nzsj10.z64"

.definelabel G_PAYLOAD_VROM, 0x02FB1040
.definelabel G_PAYLOAD_SIZE, filesize("../bin/kz-lite-NZSJ10/kz.bin") + 0x60
.definelabel G_PAYLOAD_ADDR, 0x80690000
.definelabel G_KZ_ADDR, G_PAYLOAD_ADDR + 0x60

.orga 0x10
.word 0x5354631C, 0x03A2DEF0

; Add dmatable entry for the payload code
.orga 0x22120
.word G_PAYLOAD_VROM, (G_PAYLOAD_VROM + G_PAYLOAD_SIZE), G_PAYLOAD_VROM, 0

; ==================================================================================================
; Base game editing region
; ==================================================================================================

.headersize(0x800A76A0 - 0x00B5F000)
; ==================================================================================================
; Custom Code Payload
; ==================================================================================================

.org 0x80170078
    addiu   sp, sp, -0x340
    sw      ra, 0x002C(sp)
    lui     a0, hi(G_PAYLOAD_ADDR)
    addiu   a0, lo(G_PAYLOAD_ADDR)
    lui     a1, hi(G_PAYLOAD_VROM)
    addiu   a1, lo(G_PAYLOAD_VROM)
    lui     a2, hi(G_PAYLOAD_SIZE)
    jal     0x80080C84
    addiu   a2, lo(G_PAYLOAD_SIZE)
    jal     ainit
    lui     a0, 0x0004

; ==================================================================================================
; Game Class Frame Start Hook 
; ==================================================================================================

; 801737A0
; Replaces jalr ra, t9
;          nop
.org 0x8016EFD0
jal     G_KZ_ADDR
or      a1, r0, t9

; Disables D-Pad Movement in pause menu
;.orga 0xCBD530
;    nop

; ==================================================================================================
; New code region
; ==================================================================================================

.headersize(G_PAYLOAD_ADDR - G_PAYLOAD_VROM)
.org G_PAYLOAD_ADDR
ainit:
    lui     s0, 0x801C
    jr		ra
    addiu s0, s0, 0x87a0
.org G_KZ_ADDR
.incbin("../bin/NZSJ10/kz.bin")
.align 8
.close
