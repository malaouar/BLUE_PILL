/* Linker script for BLUE PILL STM32F103C8T6  128K flash, 20K RAM. */

/* Define memory regions. */

MEMORY
{
    rom : ORIGIN = 0x08002000, LENGTH = 128K
    ram : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
    .text : { *(.text*) } > rom
    .rodata : { *(.rodata*) } > rom
    .bss : { *(.bss*) } > ram
}

