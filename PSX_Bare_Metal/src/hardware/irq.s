.set noreorder
.text

.globl dma_interrupt
.globl cdrom_interrupt
.globl gpu_interrupt
.globl vblank_interrupt

vblank_interrupt:

    addiu  $sp, $sp, -8
    sw     $ra, 0($sp)
    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    lw     $t1, 0($t0)
    andi   $t1, $t1, 0xFFFE
    sw     $t1, 0($t0)
    jal generic_irq_test
    nop
    lw     $ra, 0($sp)
    addiu  $sp, $sp, 8
    jr  $ra
    nop

gpu_interrupt:

    addiu  $sp, $sp, -8
    sw     $ra, 0($sp)
    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    lw     $t1, 0($t0)
    andi   $t1, $t1, 0xFFFD
    sw     $t1, 0($t0)
    jal generic_irq_test
    nop
    lw     $ra, 0($sp)
    addiu  $sp, $sp, 8
    jr  $ra
    nop

cdrom_interrupt:

    addiu  $sp, $sp, -8
    sw     $ra, 0($sp)
    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    lw     $t1, 0($t0)
    andi   $t1, $t1, 0xFFFB
    sw     $t1, 0($t0)
    jal cdrom_irq
    nop
    lw     $ra, 0($sp)
    addiu  $sp, $sp, 8
    jr  $ra
    nop

dma_interrupt:

    addiu  $sp, $sp, -8
    sw     $ra, 0($sp)
    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    lw     $t1, 0($t0)
    andi   $t1, $t1, 0xFFF7
    sw     $t1, 0($t0)
    jal dma_irq
    nop
    lw     $ra, 0($sp)
    addiu  $sp, $sp, 8
    jr  $ra
    nop


