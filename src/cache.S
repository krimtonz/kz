.section  .text, "ax", @progbits
.set      nomips16
.set      nomicromips
.set      noat
.global   osInvalICache
.ent      osInvalICache
.type     osInvalICache, @function
osInvalICache:
    blez    $a1, .L2
    li      $t3, 0x4000
    sltu    $at, $a1, $t3
    beq     $at, 0, .L3
    or      $t0, $a0, 0
    addu    $t1, $a0, $a1
    sltu    $at, $t0, $t1
    beq     $at, 0, .L1
    andi    $t2, $t0, 0x1F
    addiu   $t1, $t1, -0x20
    subu    $t0, $t0, $t2
.L1:
    cache   0x10, 0x0000($t0)
    sltu    $at, $t0, $t1
    addiu   $t0, $t0, 0x20
    bne     $at, 0, .L1
.L2:
    jr      $ra
.L3:
    lui     $t0, 0x8000
    addu    $t1, $t0, $t3
    addiu   $t1, $t1, -0x20
.L4:
    cache   0x0, 0x0000($t0)
    sltu    $at, $t0, $t1
    addiu   $t0, $t0, 0x20
    bne     $at, 0, .L4
    jr      $ra

.end osInvalICache
.size osInvalICache, . - osInvalICache

.section  .text, "ax", @progbits
.set      nomips16
.set      nomicromips
.set      noat
.global   osInvalDCache
.ent      osInvalDCache
.type     osInvalDCache, @function
osInvalDCache:
    blez    $a1, .L7
    li      $t3, 0x2000
    sltu    $at, $a1, $t3
    beq     $at, 0, .L8
    or      $t0, $a0, 0
    addu    $t1, $a0, $a1
    sltu    $at, $t0, $t1
    beq     $at, 0, .L7
    andi    $t2, $t0, 0xf
    addiu   $t1, $t1, -0x10
    beq     $t2, 0, .L5
    subu    $t0, $t0, $t2
    cache   0x15, 0x0000($t0)
    sltu    $at, $t0, $t1
    beq     $at, 0, .L7
    addiu   $t0, $t0, 0x10
.L5:
    andi    $t2, $t1, 0xf
    beq     $t2, 0, .L5
    subu    $t1, $t1, $t2
    cache   0x15, 0x0010($t1)
    sltu    $at, $t1, $t0
    bne     $at, 0, .L7
.L6:
    cache   0x11, 0x0000($t0)
    sltu    $at, $t0, $t1
    addiu   $t0, $t0, 0x10
    bne     $at, 0, .L6
.L7:
    jr      $ra
.L8:
    lui     $t0, 0x8000
    addu    $t1, $t0, $t3
    addiu   $t1, $t1, -0x10
.L9:
    cache   0x1, 0x0000($t0)
    sltu    $at, $t0, $t1
    addiu   $t0, $t0, 0x10
    bne     $at, 0, .L9
    jr      $ra

.end osInvalDCache
.size osInvalDCache, . - osInvalDCache

.section  .text, "ax", @progbits
.set      nomips16
.set      nomicromips
.set      noat
.global   osWritebackDCache
.ent      osWritebackDCache
.type     osWritebackDCache, @function
osWritebackDCache:
    blez    $a1, .L11
    li      $t3, 0x2000
    sltu    $at, $a1, $t3
    beq     $at, 0, .L12
    or      $t0, $a0, 0
    addu    $t1, $a0, $a1
    sltu    $at, $t0, $t1
    beq     $at, 0, .L11
    andi    $t2, $t0, 0xf
    addiu   $t1, $t1, -0x10
    subu    $t0, $t0, $t2
.L10:
    cache   0x19, 0x0000($t0)
    sltu    $at, $t0, $t1
    bne     $at, 0, .L10
.L11:
    jr      $ra
.L12:
    lui     $t0, 0x8000
    addu    $t1, $t0, $t3
    addiu   $t1, $t1, -0x10
.L13:
    cache   0x1, 0x0000($t0)
    sltu    $at, $t0, $t1
    addiu   $t0, $t0, 0x10
    bne     $at, 0, .L13
    jr      $ra
.end osWritebackDCache
.size osWritebackDCache, . - osWritebackDCache