.n64
.relativeinclude on

.create "patched.nzsj.z64", 0
.incbin "base.nzsj.z64"

;.definelabel G_PAYLOAD_VROM, 0x02EE8000
.definelabel G_PAYLOAD_VROM, 0x02FB2000
.definelabel G_PAYLOAD_SIZE, filesize("bin/NZSJ/kz.bin") + 0x20
.definelabel G_PAYLOAD_ADDR, 0x80800000
.definelabel G_KZ_ADDR, G_PAYLOAD_ADDR + 0x20

.orga 0x10
.word 0x5354631C, 0x03A2DEF0

; Add dmatable entry for the payload code
.orga 0x22120
.word G_PAYLOAD_VROM, (G_PAYLOAD_VROM + G_PAYLOAD_SIZE), G_PAYLOAD_VROM, 0

; ==================================================================================================
; Base game editing region
; ==================================================================================================

;.headersize(0x800A5AC0 - 0xB3C000)
.headersize(0x800A75E0 - 0xB5F000)
; ==================================================================================================
; Custom Code Payload
; ==================================================================================================


;.org 0x801748A0
.org 0x80170168
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

; 801737A0
; Replaces jalr ra, t9
;          nop
;.org 0x801737A0
.org 0x8016F0C0
jal     G_KZ_ADDR
or      a1, r0, t9

; ==================================================================================================
; New code region
; ==================================================================================================

.headersize(G_PAYLOAD_ADDR - G_PAYLOAD_VROM)
.org G_PAYLOAD_ADDR
ainit:
    lui     s0, 0x801C
    jr		ra
    addiu s0, s0, 0x89e0
    ;addiu   s0, s0, 0xD910
.org G_KZ_ADDR
.incbin("bin/NZSJ/kz.bin")
.align 8
.close
