.set noreorder
.section .ktext, "ax", @progbits     
.globl interrupt_vector  
interrupt_vector:
    j       interrupt_handler
    nop

.text
interrupt_handler:

    addiu $sp, $sp, -68

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


    lui    $a0, 0x1F80
    ori    $a0, $a0, 0x1070
    li     $a1, 0
    jal    check_bit_at_addr
    nop

    beqz   $v0, not_vblank
    nop

    jal    vblank_interrupt
    nop

not_vblank: 

    lui    $a0, 0x1F80
    ori    $a0, $a0, 0x1070
    li     $a1, 1
    jal    check_bit_at_addr
    nop

    beqz   $v0, not_gpu
    nop

    jal    gpu_interrupt
    nop

not_gpu:

    lui    $a0, 0x1F80
    ori    $a0, $a0, 0x1070
    li     $a1, 2
    jal    check_bit_at_addr
    nop

    beqz   $v0, not_cdrom
    nop

    jal    cdrom_interrupt
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
    addiu $sp, $sp, 68

    mfc0 $k0, $14    
    nop
    jr   $k0
    rfe


loop:
j loop
nop

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

