.set noreorder
.text

.globl dma_interrupt
.globl cdrom_interrupt
.globl gpu_interrupt
.globl vblank_interrupt
.globl spu_interrupt

vblank_interrupt:
    addiu  $sp, $sp, -8
    sw     $ra, 0($sp)
    jal    generic_irq_test
    nop
    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    li     $t1, 0xFFFFFFFE
    sw     $t1, 0($t0)
    lw     $ra, 0($sp)
    addiu  $sp, $sp, 8
    jr     $ra
    nop

gpu_interrupt:
    addiu  $sp, $sp, -8
    sw     $ra, 0($sp)
    jal    generic_irq_test
    nop
    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    li     $t1, 0xFFFFFFFD
    sw     $t1, 0($t0)
    lw     $ra, 0($sp)
    addiu  $sp, $sp, 8
    jr     $ra
    nop

cdrom_interrupt:
    addiu  $sp, $sp, -8
    sw     $ra, 0($sp)
    jal    cdrom_irq
    nop
    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    li     $t1, 0xFFFFFFFB
    sw     $t1, 0($t0)
    lw     $ra, 0($sp)
    addiu  $sp, $sp, 8
    jr     $ra
    nop

dma_interrupt:
    addiu  $sp, $sp, -8
    sw     $ra, 0($sp)
    jal    dma_irq
    nop
    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    li     $t1, 0xFFFFFFF7
    sw     $t1, 0($t0)
    lw     $ra, 0($sp)
    addiu  $sp, $sp, 8
    jr     $ra
    nop

spu_interrupt:
    addiu  $sp, $sp, -8
    sw     $ra, 0($sp)
    jal    spu_irq
    nop
    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    li     $t1, 0xFFFFFDFF
    sw     $t1, 0($t0)
    lw     $ra, 0($sp)
    addiu  $sp, $sp, 8
    jr     $ra
    nop
