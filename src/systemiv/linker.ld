OUTPUT_FORMAT("elf32-m68k")

ENTRY(START)

INCLUDE ../lib/common.ld

/* physical memory layout */
MEMORY
{
  	RAM	: org = 0x00000000, l = 1M
  	ROM	: org = 0x00400000, l = 1M
	  XPN	: org = 0x00800000, l = 1M
  	MFP	: org = 0x00E00000, l = 1M
}

SECTIONS
{
  .text.init _os_start : ALIGN(4)
  {
    _init = .;
    KEEP(*(.init))
    _init_end = .;
  } > RAM

    .text ALIGN(4) :
  {
    _code = .;
    *(.text*)
    *(.rodata*)
    _code_end = .;
  } > RAM

  .data : ALIGN(4)
  {
    _data_start = .;
    *(.data*)
    _data_end = .;
  } > RAM

   .bss (NOLOAD) : ALIGN(4)
  {
    _bss_start = .;
    *(.bss*)
    _bss_end = .;
  } > RAM
 
  _end = .;
}

_data_load_start = LOADADDR(.data);
_data_load_end = _data_load_start + (_data_end - _data_start);