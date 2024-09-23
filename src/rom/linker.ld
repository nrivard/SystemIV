OUTPUT_FORMAT("elf32-m68k")

ENTRY(START)

/* physical memory layout */
MEMORY
{
  	RAM	: org = 0x00000000, l = 1M
  	ROM	: org = 0x00400000, l = 1M
	  XPN	: org = 0x00800000, l = 1M
  	MFP	: org = 0x00E00000, l = 1M
}

/* RAM layout */
MEMORY
{
	VECTS	: org = 0x00000000, l = 0x000400,
	OS		: org = 0x00000400, l = 0x000200,
	USER	: org = ORIGIN(OS) + LENGTH(OS), l = ORIGIN(RAM) + LENGTH(RAM) - ORIGIN(USER)
}

PROVIDE(VECT_START = ORIGIN(VECTS));
PROVIDE(_bootloader_start = 0x00002000); /* addr to load bootloader */

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