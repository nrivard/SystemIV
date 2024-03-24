OUTPUT_FORMAT("elf32-m68k")

ENTRY(Start)

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
	VECTORS	: org = 0x00000000, l = 0x000400,
	OS		: org = 0x00000400, l = 0x000100,
	USER	: org = ORIGIN(OS) + LENGTH(OS), l = ORIGIN(RAM) + LENGTH(RAM) - ORIGIN(USER)
}

SECTIONS
{
  .text : ALIGN(4)
  {
    _code = .;
    *(.text*)
    *(.rodata*)
    _code_end = .;
  } > ROM

   .bss (NOLOAD) : ALIGN(4)
  {
    _bss_start = .;
    *(.bss*)
    _bss_end = .;
  } > OS
 
  _end = .;
}
