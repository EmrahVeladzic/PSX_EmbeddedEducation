.set noreorder
.globl load_transform
.globl get_vector
.globl get_matrix
.globl load_vector
.globl load_matrix
.globl multiply_with_current
.globl set_screen_offset
.globl queue_mesh
.globl interpolate_vec3
.globl interpolate_svec3
.globl interpolate_quat

.text

.set RSQRT_SHIFT, 3
.set RSQRT_COUNT, ((2048) >> RSQRT_SHIFT) + 1
.set OT_SLOTS, 2048

.set GTE_RT_IR,     0x049E012
.set GTE_RT_IR_TR,  0x0498012
.set GTE_RTPT,  0x0280030
.set GTE_AVSZ3, 0x158002D
.set GTE_NCLIP, 0x1400006
.set GTE_INTPL, 0x0980011
.set GTE_GPF,   0x0198003D

load_transform:

    addiu $sp, $sp, -24
    sw  $s0, 0($sp)
    sw  $s1, 4($sp)
    sw  $s2, 8($sp)
    sw  $s3, 12($sp)
    sw  $s4, 16($sp)

    move $s0, $a0

    lw   $a3, 16($a0)
    lw   $a1, 12($a0)

    sll  $a2, $a3, 16
    sra  $a2, $a2, 16       #z
    sra  $a3, $a3, 16       #w

    mult $a2, $a3
    mflo $t8                #z*w

    sll  $a0, $a1, 16
    sra  $a0, $a0, 16       #x

    mult $a2, $a2
    mflo $t2                #z^2

    sra  $a1, $a1, 16       #y
    li   $t9, 4096          #1.0

    mult $a1, $a1
    mflo $t1                #y^2

    addu $v0, $t1, $t2      #y^2+z^2
    sra  $v0, $v0, 11

    mult $a0, $a1
    mflo $t3                #x*y

    lh $s1, 20($s0)         #scale_x
    subu $v0, $t9, $v0      #r11

    mult $v0, $s1           
    mflo $v0                #r11*scale_x

    lh $s2, 22($s0)         #scale_y
    lh $s3, 24($s0)         #scale_z

    mult $a1, $a3
    mflo $t7                #y*w

    subu $v1, $t3, $t8      #xy-zw
    sra $v0, $v0, 12        #r11_final

    mult $a2, $a0
    mflo $t5                #z*x

    sra  $v1, $v1, 11       #r12
    andi $v0, $v0, 0xFFFF            
  
    mult $v1, $s2           #r12*scale_y
    mflo $v1

    sra $v1, $v1, 12        #r12_final
    sll  $v1, $v1, 16

    mult $a0, $a3
    mflo $t6                #x*w

    or   $v0, $v0, $v1      #r11|r12
    ctc2 $v0, $0

    mult $a1, $a2
    mflo $t4                #y*z

    addu $v0, $t5, $t7      #zx+yw
    sra  $v0, $v0, 11       #r13

    mult $v0, $s3           #r13*scale_z
    mflo $v0

    lw  $s4, 0($s0)         #trans_x
    addu $v1, $t3, $t8      #xy+zw

    mult $a0, $a0
    mflo $t0                #x^2

    sra $v0, $v0, 12        #r13_final
    sra  $v1, $v1, 11       #r21

    mult $v1, $s1 
    mflo $v1                #r21*scale_x

    ctc2 $s4, $5
    andi $v0, $v0, 0xFFFF
    

    sra $v1, $v1, 12        #r21_final

    sll  $v1, $v1, 16
    or   $v0, $v0, $v1      #r13|r21

    ctc2 $v0, $1
    addu $v0, $t0, $t2      #x^2+z^2

    sra  $v0, $v0, 11
    subu $v0, $t9, $v0      #r22

    mult $v0, $s2
    mflo $v0                #r22*scale_y

    subu $v1, $t4, $t6      #yz-xw
    sra  $v1, $v1, 11       #r23

    sra  $v0, $v0, 12       #r22_final


    mult $v1, $s3           
    mflo $v1                #r23*scale_z

    lw   $s4, 4($s0)        #trans_y    
    andi $v0, $v0, 0xFFFF

    sra  $v1, $v1, 12       #r23_final
    sll  $v1, $v1, 16

    or   $v0, $v0, $v1      #r22|r23
    ctc2 $v0, $2

    subu $v0, $t5, $t7      #zx-yw
    sra  $v0, $v0, 11       #r31

    mult $v0, $s1
    mflo $v0                #r31*scale_x

    ctc2 $s4, $6

    addu $v1, $t4, $t6      #yz+xw
    sra  $v1, $v1, 11       #r32

    mult $v1, $s2
    mflo $v1                #r32*scale_y

    lw   $s4, 8($s0)        #trans_z

    sra  $v0, $v0, 12       #r31_final
    andi $v0, $v0, 0xFFFF

    ctc2 $s4, $7

    sra  $v1, $v1, 12       #r32_final
    sll  $v1, $v1, 16
    or   $v0, $v0, $v1      #r31|r32
    ctc2 $v0, $3

    addu $v0, $t0, $t1      #x^2+y^2
    sra  $v0, $v0, 11
    subu $v0, $t9, $v0      #r33

    mult $v0, $s3       
    mflo $v0                #r33*scale_z

    lw  $s0, 0($sp)
    lw  $s1, 4($sp)
    lw  $s2, 8($sp)
    lw  $s3, 12($sp)
    lw  $s4, 16($sp)
    sra $v0, $v0, 12        #r33_final

    andi $v0, $v0, 0xFFFF   

    ctc2 $v0, $4

 
    jr   $ra
    addiu $sp, $sp, 24
  

get_vector:

    cfc2 $t0, $5
    cfc2 $t1, $6
    cfc2 $t2, $7

    sw  $t0, 0($a0)
    sw  $t1, 4($a0)
    sw  $t2, 8($a0)

    jr $ra
    move $v0, $a0

get_matrix:

    cfc2 $t0, $0
    cfc2 $t1, $1
    cfc2 $t2, $2
    cfc2 $t3, $3
    cfc2 $t4, $4

    sw  $t0, 0($a0)
    sw  $t1, 4($a0)
    sw  $t2, 8($a0)
    sw  $t3, 12($a0)
    sh  $t4, 16($a0)

    jr $ra
    move $v0, $a0

load_vector:

    lw  $t0, 0($a0)
    lw  $t1, 4($a0)
    lw  $t2, 8($a0)

    ctc2 $t0, $5
    ctc2 $t1, $6

    jr $ra
    ctc2 $t2, $7

load_matrix:

    lw  $t0, 0($a0)
    lw  $t1, 4($a0)
    lw  $t2, 8($a0)
    lw  $t3, 12($a0)
    lh  $t4, 16($a0)

    ctc2 $t0, $0
    ctc2 $t1, $1
    ctc2 $t2, $2
    ctc2 $t3, $3

    jr $ra
    ctc2 $t4, $4
    



multiply_with_current:

    lh  $t0, 12($a1)
    lh  $t1, 18($a1)

    mtc2 $t0, $9
    lh  $t2, 24($a1)

    mtc2 $t1, $10
    mtc2 $t2, $11

    lh  $t3, 14($a1)
    lh  $t4, 20($a1)

    cop2    GTE_RT_IR #rx1
    lh  $t5, 26($a1)
    lh  $t6, 16($a1)
    lh  $t7, 22($a1)
    lh  $t8, 28($a1)

    mfc2 $t0, $9
    mfc2 $t1, $10
    mfc2 $t2, $11

    sh  $t0, 12($a0)
    sh  $t1, 18($a0)
    sh  $t2, 24($a0)

    mtc2 $t3, $9
    mtc2 $t4, $10
    mtc2 $t5, $11

    lw  $t0, 0($a1)
    lw  $t1, 4($a1)

    cop2    GTE_RT_IR #rx2

    lw  $t2, 8($a1)
    mfc2 $t3, $9
    mfc2 $t4, $10
    mfc2 $t5, $11

    sh  $t3, 14($a0)


    mtc2 $t6, $9
    mtc2 $t7, $10
    mtc2 $t8, $11

    sh  $t4, 20($a0)
    sh  $t5, 26($a0)

    cop2    GTE_RT_IR #rx3
    mfc2 $t6, $9
    mfc2 $t7, $10
    mfc2 $t8, $11

    mtc2 $t0, $9
    mtc2 $t1, $10
    mtc2 $t2, $11


    sh  $t6, 16($a0)
    sh  $t7, 22($a0)
    sh  $t8, 28($a0) 

    cop2 GTE_RT_IR_TR #t
    mfc2 $t0, $25
    mfc2 $t1, $26
    mfc2 $t2, $27

    sw  $t0, 0($a0)
    sw  $t1, 4($a0)
    sw  $t2, 8($a0) 

    jr $ra
    move $v0, $a0   


set_screen_offset:

    ctc2 $a0, $24
    ctc2 $a1, $25
    ctc2  $a3, $29

    jr $ra
    ctc2 $a2, $26


queue_mesh:

    addiu $sp, $sp, -16
    sw   $s0, 0($sp)
    sw   $s1, 4($sp)
    sw   $s2, 8($sp)
    sw   $s3, 12($sp)

    lhu $t0, 12($a0) #c_vtx
    lw $t1, 4($a1) #packet_cursor
    beqz $t0, qm_done
    lw $t3, 0($a0) #vtx_bfr
    lhu $t0, 14($a0) #c_tris
    lw $t2, 8($a1) #packet_end
    beqz $t0, qm_done
    lw $t4, 8($a0) #ind_bfr
    beq $t1, $t2, qm_done 
    lw $t5, 0($a1) #o_table
    lw $t6, 12($a1) #color_cmd 
    lw $t7, 16($a1) #clut 
    lw $t8, 20($a1) #tpage 
    lbu $a0, 16($a0) #material 
    
    srl $t9, $t6, 26
    lui $s0, 0x00FF
    andi $t9, $t9, 0x1 #textured

    ori $s0, $s0, 0xFFFF #24-bit mask

    
qm_loop:

    lhu $s1, 0($t4) #index 0
    lhu $s2, 2($t4) #index 1
    lhu $s3, 4($t4) #index 2

    sll $s1, $s1, 3 
    sll $s2, $s2, 3
    sll $s3, $s3, 3

    addu $s1, $s1, $t3 #&vtx0
    addu $s2, $s2, $t3 #&vtx1
    addu $s3, $s3, $t3 #&vtx2

    lwc2 $0, 0($s1)          
    lwc2 $1, 4($s1)              
    lwc2 $2, 0($s2)             
    lwc2 $3, 4($s2)             
    lwc2 $4, 0($s3)             
    lwc2 $5, 4($s3)             

    lhu  $a2, 6($s1) #uv0
    lhu  $a3, 6($s2) #uv1       

    cop2 GTE_RTPT            

    lhu  $v1, 6($s3) #uv2

    or   $a2, $a2, $t7 #clut         
    or   $a3, $a3, $t8 #tpage    

    addiu $t4, $t4, 6 #ind_bfr += 3   

    andi $a0, $a0, 0x1 #culling_check 

    bnez $a0, qm_draw

    addiu $t0, $t0, -1 #triangle_done

    cop2 GTE_NCLIP
    mfc2 $v0, $24     

    nop
    bgez $v0, qm_next #culled
    nop

qm_draw:

 
    cop2 GTE_AVSZ3           
   
    mfc2 $s1, $7

    beqz $s1, qm_next
    nop


    sltiu $v0, $s1, OT_SLOTS
    bnez $v0, qm_valid

    nop
    li  $s1, OT_SLOTS-1

qm_valid:

    sll $s1, $s1, 2
    addu $s1, $s1, $t5

    addiu $s2, $t1, 32
    sltu  $s2, $t2, $s2
    bnez  $s2, qm_done 
    nop

    lw $s3, 0($s1)
    beqz $t9, qm_untextured    

    sw  $t6, 4($t1) #write_packet_cmd   

    swc2 $12, 8($t1)    
    sw  $a2 ,12($t1)
    swc2 $13, 16($t1)           
    sw   $a3, 20($t1)        
    swc2  $14, 24($t1)
    sw   $v1, 28($t1)
    and $v1, $s3, $s0 #masking

    lui $a2, 0x0700 #textured_tag
    or  $a2, $a2, $v1

    sw  $a2, 0($t1) 
    xor $s3, $s3, $v1
    and $a3, $t1, $s0
    or $s3, $s3, $a3

    sw $s3, 0($s1)

    b    qm_next
    addiu $t1, $t1, 32    


qm_untextured:

    swc2 $12, 8($t1)         
    swc2 $13, 12($t1)        
    swc2 $14, 16($t1)        
    and $v1, $s3, $s0 #masking

    lui $a2, 0x0400 #flat_tag
    or  $a2, $a2, $v1
    sw  $a2, 0($t1) 

    xor $s3, $s3, $v1
    and $a3, $t1, $s0
    or $s3, $s3, $a3

    sw $s3, 0($s1)
    addiu $t1, $t1, 20   


qm_next:

    bnez $t0, qm_loop
    nop

qm_done: 

    move $v0, $t1

    lw   $s0, 0($sp)
    lw   $s1, 4($sp)
    lw   $s2, 8($sp)
    lw   $s3, 12($sp)

    jr $ra
    addiu $sp, $sp, 16


interpolate_vec3:

    lwc2 $9, 0($a1)
    lwc2 $10, 4($a1)
    lwc2 $11, 8($a1)
    mtc2 $a3, $8

    lw  $t0, 0($a2)
    lw  $t1, 4($a2)
    lw  $t2, 8($a2)
    ctc2 $t0, $21
    ctc2 $t1, $22
    ctc2 $t2, $23

    nop 
    nop

    cop2 GTE_INTPL

    swc2 $9,  0($a0)
    swc2 $10, 4($a0)
    swc2 $11, 8($a0)

    jr $ra
    move $v0, $a0


interpolate_svec3:

    lh   $t0, 0($a1)
    lh   $t1, 2($a1)
    lh   $t2, 4($a1)
    mtc2 $t0, $9
    mtc2 $t1, $10
    mtc2 $t2, $11
    mtc2 $a3, $8

    lh  $t0, 0($a2)
    lh  $t1, 2($a2)
    lh  $t2, 4($a2)
    ctc2 $t0, $21
    ctc2 $t1, $22
    ctc2 $t2, $23

    nop
    nop

    cop2 GTE_INTPL

    mfc2 $t0, $9
    mfc2 $t1, $10
    mfc2 $t2, $11

    sh $t0, 0($a0)
    sh $t1, 2($a0)
    sh $t2, 4($a0)

    jr $ra
    move $v0, $a0



interpolate_quat:
 
    lh $t0, 0($a1) # ax
    lh $t1, 2($a1) # ay
    lh $t2, 4($a1) # az
    lh $t3, 6($a1) # aw
    lh $t4, 0($a2) # bx
    lh $t5, 2($a2) # by
    lh $t6, 4($a2) # bz
    lh $t7, 6($a2) # bw


    mult $t0, $t4
    mflo $t8    #ax * bx
    nop
    nop

    mult $t1, $t5
    mflo $t9    #ay*by
    nop
    addu $t8, $t8, $t9

    mult $t2, $t6
    mflo $t9    #az*bz
    nop
    addu $t8, $t8, $t9

    mult $t3, $t7
    mflo $t9    #aw*bw
    nop

    addu $t8, $t8, $t9 #dot = (axbx) + (ayby) + (azbz) + (awbw)


    
    bgez $t8, q_no_flip #dot<0 => bx = -bx, by = -by, bz = -bz, bw = -bw
    nop

    subu $t4, $zero, $t4
    subu $t5, $zero, $t5
    subu $t6, $zero, $t6
    subu $t7, $zero, $t7


q_no_flip:

    mtc2 $a3, $8 #factor

    mtc2 $t0, $9    #ax
    mtc2 $t1, $10   #ay
    mtc2 $t2, $11   #az


    ctc2 $t4, $21   #bx
    ctc2 $t5, $22   #by
    ctc2 $t6, $23   #bz

    nop
    nop

    cop2 GTE_INTPL

    subu $t9, $t7, $t3 #bw - aw
    mult $t9, $a3 
    mflo $t9 
    sra  $t9, 12 # (bw-aw)*factor
    addu $t3, $t3, $t9 # q = aw + (bw-aw) * factor

    mfc2 $t0, $9  
    mfc2 $t1, $10 
    mfc2 $t2, $11 

    mult $t0, $t0
    mflo $t8                    
    nop
    nop
    mult $t1, $t1
    mflo $t9                    
    nop
    addu $t8, $t8, $t9
    mult $t2, $t2
    mflo $t9                   
    nop
    addu $t8, $t8, $t9
    mult $t3, $t3
    mflo $t9            
    nop

    addu $t8, $t8, $t9
    sra  $t8, $t8, 12   #n2 = |q|^2 = x^2 + y^2 + z^2 + w^2


    addiu $t8, $t8, -2048
    sra   $t8, $t8, RSQRT_SHIFT  #reciprocal square root index

    bgez $t8, q_upper_clamp

    li   $t9, RSQRT_COUNT
    move $t8, $zero #low_clamp


q_upper_clamp:

    sltu  $t9, $t8, $t9          
    bnez  $t9, q_clamped          
    nop                          
    li    $t8, RSQRT_COUNT - 1 #high_clamp 


q_clamped:

    sll $t8, $t8, 1 #offset
    la $t9, rsqrt_tab #inverse square root table
    addu $t9, $t9, $t8 #table[offset]

    lhu $t9, 0($t9) #1 / sqrt(n2)
    nop

    mtc2 $t9, $8
    mult $t3, $t9 #norm_w
    nop

    cop2 GTE_GPF #norm_xyz

    mflo $t3
    sra  $t3, $t3, 12 #normalized

    mfc2 $t0, $9
    mfc2 $t1, $10
    mfc2 $t2, $11


    sh  $t3, 6($a0)
    sh  $t0, 0($a0)
    sh  $t1, 2($a0)
    sh  $t2, 4($a0) 


 
    jr   $ra
    move $v0, $a0
 

