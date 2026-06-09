.set noreorder
.globl _start

.set WORD,       4

.set I_STAT_ADDR,  0x1F801070
.set I_MASK_ADDR,  0x1F801074
   

.set SR_RUN,     0x5000FF01

.text

_start:

    lui  $t0, %hi(I_MASK_ADDR)
    sw   $zero,  %lo(I_MASK_ADDR)($t0)

    mtc0    $zero, $12
    nop


    la      $gp, _gp
    la      $sp, _stackTop

    la		$t0, _bssStart
    la      $t1, _bssEnd 

_bss_clear:

    beq     $t0, $t1, _bss_done
    nop
    sw      $zero, 0($t0)
    nop
    addiu   $t0, $t0, WORD
    j       _bss_clear
    nop

_bss_done:


    lui  $t0, %hi(I_STAT_ADDR)
    sw   $zero, %lo(I_STAT_ADDR)($t0)

    la    $t0, _ktextStart
    la    $t1, _ktextEnd
    la    $t2, _ktextDest

    beq   $t0, $t1, _vec_copy_done   
    nop

_vec_copy:

    lw    $t3, 0($t0)
    nop
    sw    $t3, 0($t2)
    addiu $t0, $t0, WORD
    bne   $t0, $t1, _vec_copy
    addiu $t2, $t2, WORD

_vec_copy_done:

    jal flush_icache
    nop

    li    $t0, SR_RUN
    mtc0  $t0, $12
    nop

_proceed:

    jal   main
    nop

_hang:

    b     _hang
    nop
