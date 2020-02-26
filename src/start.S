.equ stack_size, 0x2000

.section    .text.startup, "ax", @progbits
.set        nomips16
.set        nomicromips
.global     _start
.ent        _start
.type       _start, @function

_start:
    la  $t0, __init_data
    sw  $sp, 0x0000($t0)
    sw  $ra, 0x0004($t0)
    la  $sp, _stack + stack_size - 0x10
    jal main
    la  $t0, __init_data
    lw  $sp, 0x0000($t0)
    lw  $ra, 0x0004($t0)
    jr  $ra

.end        _start
.size       _start, . - _start

.section    .text, "ax", @progbits
.set        nomips16
.set        nomicromips
.global     _kz_exit
.ent        _kz_exit
.type       _kz_exit, @function

_kz_exit:
    addiu   $sp, $sp, -0x0008
    sw      $ra, 0x0004($sp)
    la      $t0, __init_data
    lw      $t1, 0x0000($t0)
    sw      $sp, 0x0000($t0)
    move    $sp, $t1
    lw      $t9, 0x0008($t0)
    jalr    $t9
    la      $t0, __init_data
    lw      $t1, 0x0000($t0)
    sw      $sp, 0x0000($t0)
    move    $sp, $t1
    lw      $ra, 0x0004($sp)
    addiu   $sp, $sp, 0x0008
    jr      $ra

.end        _kz_exit
.size       _kz_exit, . - _kz_exit

.section    .data, "aw", @progbits
.align 2
.type       __init_data, @object
.local      __init_data
__init_data:
.space 0x8
.type       exit_func, @object
.global     exit_func
exit_func:
.space 0x4
.size       exit_func, . - exit_func
.size       __init_data, . - __init_data

.section    .stack, "aw", @nobits
.align 3
.type       _stack, @object
.local      _stack
_stack:
.space      stack_size
.size       _stack, . - _stack