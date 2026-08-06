.set noreorder
.set noat
.section .ktext, "ax", @progbits
.globl interrupt_vector
interrupt_vector:
la   $k0, interrupt_handler
jr   $k0
nop

.text
interrupt_handler:
addiu $sp, $sp, -88
sw    $at,  0($sp)
sw    $ra,  4($sp)
sw    $v0,  8($sp)
sw    $v1, 12($sp)
sw    $a0, 16($sp)
sw    $a1, 20($sp)
sw    $a2, 24($sp)
sw    $a3, 28($sp)
sw    $t0, 32($sp)
sw    $t1, 36($sp)
sw    $t2, 40($sp)
sw    $t3, 44($sp)
sw    $t4, 48($sp)
sw    $t5, 52($sp)
sw    $t6, 56($sp)
sw    $t7, 60($sp)
sw    $t8, 64($sp)
sw    $t9, 68($sp)
sw    $s0, 80($sp)
mfhi  $t0
sw    $t0, 72($sp)
mflo  $t0
sw    $t0, 76($sp)

lui   $t0, 0x1F80
ori   $t1, $t0, 0x1070
lw    $t1, 0($t1)
ori   $t2, $t0, 0x1074
lw    $t2, 0($t2)
nop
and   $s0, $t1, $t2

andi  $t3, $s0, 0x0001
beqz  $t3, not_vblank
nop
jal   vblank_interrupt
nop
not_vblank:
andi  $t3, $s0, 0x0002
beqz  $t3, not_gpu
nop
jal   gpu_interrupt
nop
not_gpu:
andi  $t3, $s0, 0x0004
beqz  $t3, not_cdrom
nop
jal   cdrom_interrupt
nop
not_cdrom:
andi  $t3, $s0, 0x0008
beqz  $t3, not_dma
nop
jal   dma_interrupt
nop
not_dma:
andi  $t3, $s0, 0x0200
beqz  $t3, not_spu
nop
jal   spu_interrupt
nop
not_spu:

lw    $s0, 80($sp)
lw    $t0, 76($sp)
nop
mtlo  $t0
lw    $t0, 72($sp)
nop
mthi  $t0
lw    $t9, 68($sp)
lw    $t8, 64($sp)
lw    $t7, 60($sp)
lw    $t6, 56($sp)
lw    $t5, 52($sp)
lw    $t4, 48($sp)
lw    $t3, 44($sp)
lw    $t2, 40($sp)
lw    $t1, 36($sp)
lw    $t0, 32($sp)
lw    $a3, 28($sp)
lw    $a2, 24($sp)
lw    $a1, 20($sp)
lw    $a0, 16($sp)
lw    $v1, 12($sp)
lw    $v0,  8($sp)
lw    $ra,  4($sp)
lw    $at,  0($sp)
addiu $sp, $sp, 88

mfc0  $k0, $14
nop
jr    $k0
rfe
