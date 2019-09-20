.section  .text, "ax", @progbits
.set      nomips16
.set      nomicromips

.global   _start
.ent      _start
.type     _start, @function
_start:
la $t0, 0xa4600000
la $t1, ((RAM) & 0x1FFFFFFF)
sw $t1, 0x00($t0)
la $t1, ((ROM) | 0x10000000)
sw $t1, 0x04($t0)
la $t1, ((SIZE) - 1)
sw $t1, 0x0C($t0)
dma_wait:
lw $t1, 0x10($t0)
andi $t1, $t1, 0x01
bnez $t1, dma_wait
la $t1, 0x02
sw $t1, 0x10($t0)
j (RAM)
nop
.end _start

.fill 0x60 - (. - _start)
