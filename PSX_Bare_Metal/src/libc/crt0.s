.globl _start
.set WORD, 4

.text

_start:

    la      $gp, _gp
    la      $sp, _stackEnd
    la		$t0, _bssStart
    la      $t1, _bssEnd 
    la      $t2, _sbssStart
    la      $t3, _sbssEnd
    move 	$t4, $zero	

_bss_clear:

    sw		$t4, 0($t0)
    addiu   $t0, $t0, WORD
    bltu		$t0, $t1, _bss_clear
    nop    

_sbss_clear:

    sw		$t4, 0($t2)
    addiu   $t2, $t2, WORD
    bltu		$t2, $t3, _sbss_clear
    nop

_proceed:
    
    jal     initHeap
    jal		main			

