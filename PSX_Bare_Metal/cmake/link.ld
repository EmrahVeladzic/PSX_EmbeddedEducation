ENTRY(_start)

STACK_SIZE = 0x4000;

MEMORY{

    MAIN_RAM(rwx):ORIGIN = 0x80010000 LENGTH = 0x1f0000

}

SECTIONS{
    
    .text : {
 
        _textStart = .;

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