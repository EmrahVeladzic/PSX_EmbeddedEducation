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


    