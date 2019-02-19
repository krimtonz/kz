ainit:
    addiu sp, sp, -0x340
    sw s0, 0x0018(sp)
    sw ra, 0x002C(sp)
    sw s4, 0x0028(sp)
    sw s3, 0x0024(sp)
    sw s2, 0x0020(sp)
    sw s1, 0x001c(sp)
    sw a0, 0x0340(sp)
    
    li      s0, 0x801BD910
    j       0x801748C8
    nop