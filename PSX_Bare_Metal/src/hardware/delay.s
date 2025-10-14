.set noreorder
.globl delay_microseconds
delay_microseconds:
    sll     $t0, $a0, 8
    sll     $t1, $a0, 4
    addu    $t0, $t0, $t1
    subu    $t0, $t0, $a0
    addiu   $t0, $t0, 4
    sra     $t0, $t0, 3

    addiu   $t0, $t0, -9

delay_loop:
    bgtz    $t0, delay_loop
    addiu   $t0, $t0, -2

    jr      $ra
    nop 
    