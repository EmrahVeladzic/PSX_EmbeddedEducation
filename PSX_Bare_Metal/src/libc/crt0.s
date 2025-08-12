.globl _start
.set WORD, 4

.text

_start:

    la      $sp, _stackEnd
    la		$t0, _bssStart
    la      $t1, _bssEnd 
    move 	$t2, $zero	

_bss_clear:

    sw		$t2, 0($t0)
    addiu   $t0, $t0, WORD
    blt		$t0, $t1, _bss_clear
    nop    

_proceed:
    
    jal     initHeap
    jal		main			

