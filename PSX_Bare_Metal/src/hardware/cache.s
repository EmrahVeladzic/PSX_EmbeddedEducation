
.set noreorder
.globl flush_icache
 
.set KSEG1_BASE,  0xa0000000
.set CACHE_CTRL,  0xfffe0130       
.set CC_TAG_IS1,  0x804           
.set CC_SCRATCH,  1 << 7        

.set SR_IsC,      1 << 16          

.text

.section .text, "ax", @progbits
flush_icache:

    la $t0, _flush_icache_inner
    lui $t1, %hi(KSEG1_BASE)
    or  $t0, $t0, $t1
    
    mfc0 $t5, $12

    jr $t0
    lui $t0, %hi(CACHE_CTRL)


.section .text, "ax", @progbits
_flush_icache_inner:

    lw $t4, %lo(CACHE_CTRL)($t0)
    mtc0 $zero, $12

    li  $t1, ~CC_SCRATCH
    and $t1, $t1, $t4
    ori $t1, CC_TAG_IS1
    sw  $t1, %lo(CACHE_CTRL)($t0)

    li $t3, SR_IsC
    mtc0 $t3, $12

    li  $t1, 0xF00

_clear_loop:

	sw    $zero, 0x00($t1)
	sw    $zero, 0x10($t1)
	sw    $zero, 0x20($t1)
	sw    $zero, 0x30($t1)
	sw    $zero, 0x40($t1)
	sw    $zero, 0x50($t1)
	sw    $zero, 0x60($t1)
	sw    $zero, 0x70($t1)
	sw    $zero, 0x80($t1)
	sw    $zero, 0x90($t1)
	sw    $zero, 0xa0($t1)
	sw    $zero, 0xb0($t1)
	sw    $zero, 0xc0($t1)
	sw    $zero, 0xd0($t1)
	sw    $zero, 0xe0($t1)
	sw    $zero, 0xf0($t1)

    bgtz  $t1, _clear_loop
    addiu $t1, -0x100

    mtc0  $zero, $12
    nop
    sw $t4, %lo(CACHE_CTRL)($t0)
    mtc0  $t5, $12    


    jr $ra
    nop

