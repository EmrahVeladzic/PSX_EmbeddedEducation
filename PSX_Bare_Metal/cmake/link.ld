ENTRY(_start)


MEMORY{

    MAIN_RAM(rwx):ORIGIN = 0x80010000 LENGTH = 0x1f0000

}

SECTIONS {
    .text : {
        . = ALIGN(4);
        _textStart = .;
        *(.text .text.*)
        *(.gnu.linkonce.t.*)
        *(.mips16.fn.*)
        *(.mips16.call.*)
        *(.MIPS.stubs)
        _textEnd = .;
    } > MAIN_RAM

    .data : {
        . = ALIGN(4);
        _dataStart = .;
        *(.data .data.*)
        *(.gnu.linkonce.d.*)
        _dataEnd = .;
    } > MAIN_RAM
    
    .rodata : {
        . = ALIGN(4);
        _rodataStart = .;
        *(.rodata .rodata.*)
        *(.gnu.linkonce.r.*)
        _rodataEnd = .;
    } > MAIN_RAM

    .sdata : {
        . = ALIGN(4);
        _sdataStart = .;
        *(.sdata .sdata.*)
        *(.gnu.linkonce.s.*)
        _sdataEnd = .;
    } > MAIN_RAM

    .bss (NOLOAD) : {
        . = ALIGN(4);
        _bssStart = .;
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        _bssEnd = .;
    } > MAIN_RAM
    
    .sbss (NOLOAD) : {
        . = ALIGN(4);
        _sbssStart = .;
        *(.sbss .sbss.*)
        *(.gnu.linkonce.sb.*)
        _sbssEnd = .;
    } > MAIN_RAM

    . = ALIGN(16);
    _gp = ALIGN(16) + 0x7ff0;

    .preinit_array : {
        _preinitArrayStart = .;
        KEEP(*(.preinit_array*))
        _preinitArrayEnd = .;
    } > MAIN_RAM

    .init_array : {
        _initArrayStart = .;
        KEEP(*(.init_array*))
        _initArrayEnd = .;
    } > MAIN_RAM

    .fini_array : {
        _finiArrayStart = .;
        KEEP(*(.fini_array*))
        _finiArrayEnd = .;
    } > MAIN_RAM

    _end = .;
    
}