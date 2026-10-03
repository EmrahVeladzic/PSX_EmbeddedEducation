ENTRY(_start)

STACK_SIZE    = 0x8000;
STACK_PADDING = 0x100;

MEMORY
{
    MAIN_RAM (rwx) : ORIGIN = 0x80010000, LENGTH = 0x1f0000
}

SECTIONS
{
    
    _ktextDest = 0x80000080;

    .text : ALIGN(4) {
        _textStart = .;
        *(.text .text.* .gnu.linkonce.t.*)
        *(.plt)
        *(.MIPS.stubs)
        . = ALIGN(4);
        _textEnd = .;
    } > MAIN_RAM

    .ktextSrc : ALIGN(4) {
        _ktextStart = .;
        KEEP(*(.ktext))
        . = ALIGN(4);
        _ktextEnd = .;
    } > MAIN_RAM

    .rodata : ALIGN(4) {
        _rodataStart = .;
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        *(.eh_frame)
        *(.gcc_except_table .gcc_except_table.*)
        . = ALIGN(4);
        _rodataEnd = .;
    } > MAIN_RAM

    .data : ALIGN(4) {
        _dataStart = .;
        *(.data .data.* .gnu.linkonce.d.*)
        . = ALIGN(4);
        _dataEnd = .;
    } > MAIN_RAM

    .sdata : ALIGN(4) {
        _sdataStart = .;
        _gp = ALIGN(16) + 0x7ff0;
        *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
        *(.lit8)
        *(.lit4)
        *(.sdata .sdata.* .gnu.linkonce.s.*)
        . = ALIGN(4);
        _sdataEnd = .;
    } > MAIN_RAM

    .sbss (NOLOAD) : ALIGN(4) {
        _bssStart  = .;
        _sbssStart = .;
        *(.dynsbss)
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
        *(.scommon)
        . = ALIGN(4);
        _sbssEnd = .;
    } > MAIN_RAM

    .bss (NOLOAD) : ALIGN(4) {
        _bssLargeStart = .;
        *(.dynbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(4);
        _bssLargeEnd = .;
        _bssEnd = .;

    } > MAIN_RAM

    .dummy (NOLOAD) : ALIGN(4) {
        _dummyStart = .;
        KEEP(*(.dummy))
        . = ALIGN(4);
        _dummyEnd = .;

    } > MAIN_RAM

    .stack (NOLOAD) : ALIGN(4) {
        . = ALIGN(8);
        _heapStart = .;
        _stackTop    = ORIGIN(MAIN_RAM) + LENGTH(MAIN_RAM) - STACK_PADDING;
        . = _stackTop - STACK_SIZE;
        _stackBottom = .;
        _heapEnd = (_stackBottom - 8) & ~7;
        . = . + STACK_SIZE + STACK_PADDING;
    } > MAIN_RAM

    _end = .;

    /DISCARD/ : {
        *(.reginfo)
        *(.MIPS.abiflags)
        *(.pdr)
        *(.gnu.attributes)
        *(.comment)
        *(.note .note.*)
    }


}