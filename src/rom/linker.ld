OUTPUT_FORMAT("elf32-m68k")

ENTRY(START)

INCLUDE ../lib/common.ld

/* RAM layout */
MEMORY
{
	VECTS	: org = 0x00000000, l = 0x000400,
	OS		: org = 0x00000400, l = 0x000200,
	USER	: org = ORIGIN(OS) + LENGTH(OS), l = ORIGIN(RAM) + LENGTH(RAM) - ORIGIN(USER)
}

PROVIDE(VECT_START = ORIGIN(VECTS));

SECTIONS
{
  .text : ALIGN(4)
  {
    _code_start = .;
    *(.vectors*)
    *(.text*)
    *(.rodata*)
    _code_end = .;
  } > ROM

  .data : ALIGN(4)
  {
    _data_start = .;
    *(.data*)
    _data_end = .;
  } > OS AT > ROM

   .bss (NOLOAD) : ALIGN(4)
  {
    _bss_start = .;
    *(.bss*)
    _bss_end = .;
  } > OS
 
  _end = .;
}

_data_load_start = LOADADDR(.data);
_data_load_end = _data_load_start + (_data_end - _data_start);