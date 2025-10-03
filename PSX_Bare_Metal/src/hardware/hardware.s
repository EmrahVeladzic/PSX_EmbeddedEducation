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

    b generic_irq_test
    nop


cdrom_interrupt:

    jal    generic_irq_test
    nop
    jr     $ra
    nop

init_cdrom:

    li     $t1, 1
    
    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1800
    sb     $t1, 0($t0)

    lui    $t0, 0x1F80
    ori    $t0, $t0, 0x1802   
    sb     $t1, 0($t0)        
    jr     $ra
    nop
    

