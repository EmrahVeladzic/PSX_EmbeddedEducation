.set noreorder
.text

.globl cdrom_interrupt
.globl init_cdrom
.globl vblank_interrupt

vblank_interrupt:

    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    lw     $t1, 0($t0)
    andi   $t1, $t1, 0xFFFE
    sw     $t1, 0($t0)

    j generic_irq_test
    nop
    jr  $ra
    nop

gpu_interrupt:

    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    lw     $t1, 0($t0)
    andi   $t1, $t1, 0xFFFD
    sw     $t1, 0($t0)

    j generic_irq_test
    nop
    jr  $ra
    nop

cdrom_interrupt:

    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1070
    lw     $t1, 0($t0)
    andi   $t1, $t1, 0xFFFB
    sw     $t1, 0($t0)

    j generic_irq_test
    nop
    jr  $ra
    nop

