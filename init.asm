ainit:
    addiu sp, sp, -0x340
    sw ra, 0x002C(sp)
    sw a0, 0x0340(sp)
    
    li      s0, 0x801BD910
    j       0x801748C8
    nop