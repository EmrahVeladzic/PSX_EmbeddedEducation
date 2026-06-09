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

    addiu $sp, $sp, -72

    sw $ra,  0($sp)
    sw $v0,  4($sp)
    sw $v1,  8($sp)
    sw $a0, 12($sp)
    sw $a1, 16($sp)
    sw $a2, 20($sp)
    sw $a3, 24($sp)
    sw $t0, 28($sp)
    sw $t1, 32($sp)
    sw $t2, 36($sp)
    sw $t3, 40($sp)
    sw $t4, 44($sp)
    sw $t5, 48($sp)
    sw $t6, 52($sp)
    sw $t7, 56($sp)
    sw $t8, 60($sp)
    sw $t9, 64($sp)

    lui  $t0, 0x1F80
    ori  $t1, $t0, 0x1070
    lw   $t1, 0($t1)
    ori  $t2, $t0, 0x1074
    lw   $t2, 0($t2)
    and  $t1, $t1, $t2


    andi $t3, $t1, 0x0001
    beqz $t3, not_vblank
    nop
    jal  vblank_interrupt
    nop

not_vblank: 


    andi $t3, $t1, 0x0002
    beqz $t3, not_gpu
    nop
    jal  gpu_interrupt
    nop

not_gpu:

    andi $t3, $t1, 0x0004
    beqz $t3, not_cdrom
    nop
    jal  cdrom_interrupt
    nop

not_cdrom:

    lw $t9, 64($sp)
    lw $t8, 60($sp)
    lw $t7, 56($sp)
    lw $t6, 52($sp)
    lw $t5, 48($sp)
    lw $t4, 44($sp)
    lw $t3, 40($sp)
    lw $t2, 36($sp)
    lw $t1, 32($sp)
    lw $t0, 28($sp)
    lw $a3, 24($sp)
    lw $a2, 20($sp)
    lw $a1, 16($sp)
    lw $a0, 12($sp)
    lw $v1,  8($sp)
    lw $v0,  4($sp)
    lw $ra,  0($sp)
    addiu $sp, $sp, 72

    mfc0 $k0, $14    
    nop
    jr   $k0
    rfe


