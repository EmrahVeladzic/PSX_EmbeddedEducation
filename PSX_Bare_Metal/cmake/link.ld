ENTRY(_start)

STACK_SIZE = 0x4000;

MEMORY{

    KERNEL_RAM(rwx):ORIGIN = 0x80000000 LENGTH = 0x10000
    MAIN_RAM(rwx):ORIGIN = 0x80010000 LENGTH = 0x1f0000

}

SECTIONS{
    
    .ktext 0x80000080 : {

        _ktextStart = .;
        KEEP(*(.ktext))
        _ktextEnd = .;

    } > KERNEL_RAM

    .text : {
 
        _textStart = 0x80010000;

        *(.text .text.* .gnu.linkonce.t.*)
        *(.plt)
        *(.MIPS.stubs)
        
        _textEnd = .;

    } > MAIN_RAM  
       

    .rodata : {

        _rodataStart = .;

        *(.rodata .rodata.* .gnu.linkonce.r.*)

        _rodataEnd = .;

    } > MAIN_RAM

    .data : {

        _dataStart = .;

        *(.data .data.* .gnu.linkonce.d.*)

        _dataEnd = .;

    } > MAIN_RAM

    .sdata : {

        _sdataStart = .;

        _gp = ALIGN(16) + 0x7ff0;
        *(.sdata .sdata.* .gnu.linkonce.s.*)

        _sdataEnd = .;

    } > MAIN_RAM

    .sbss (NOLOAD) : {

        . = ALIGN((.!=0)? 4 : 1);
        _sbssStart = .;

        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.scommon)

        _sbssEnd = .;

    } > MAIN_RAM

    .bss (NOLOAD) : {

        . = ALIGN((.!=0)? 4 : 1);
        _bssStart = .;

        *(.bss .bss.* .gnu.linkonce.b.*)
        *(.COMMON)

        . = ALIGN((.!=0)? 4 : 1);
        _bssEnd = .;

    } > MAIN_RAM

    .dummy (NOLOAD) : {

        _dummyStart = .;

        KEEP(*(.dummy))

        _dummyEnd = .;

    } > MAIN_RAM

    .stack (NOLOAD) : {

        _stackEnd = ORIGIN(MAIN_RAM) + LENGTH(MAIN_RAM);
        . = _stackEnd - STACK_SIZE;
        _stackStart = .;
        . = . + STACK_SIZE;

    } > MAIN_RAM

    _end = .;

}