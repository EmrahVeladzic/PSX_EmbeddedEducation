.section .ktext, "ax", @progbits     
.globl interrupt_vector  
interrupt_vector:
    j       interrupt_handler
    nop

.text
interrupt_handler:
    
    b vblank_interrupt
    nop


    mfc0 $k0, $14    
    rfe

.globl enter_crit_section
.globl exit_crit_section
enter_crit_section:

    mfc0 $t0,$12
    nop
    li   $t1,0xFFFFFFFE
    and  $t0,$t0,$t1
    mtc0 $t0,$12
    nop    
    jr	$ra   

exit_crit_section:

    mfc0 $t0,$12
    nop
    ori  $t0,$t0,0x01
    mtc0 $t0,$12
    nop    
    jr  $ra
