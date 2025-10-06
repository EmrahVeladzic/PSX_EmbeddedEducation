.globl check_bit_at_addr
check_bit_at_addr:

    lw      $t0, 0($a0)
    li      $t1, 1
    sllv    $t1, $t1, $a1    
    and     $v0, $t0, $t1
    jr  $ra
    nop
