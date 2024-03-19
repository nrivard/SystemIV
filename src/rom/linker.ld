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
	USER	: org = ORIGIN(VECTORS) + LENGTH(VECTORS), l = ORIGIN(RAM) + LENGTH(RAM) - ORIGIN(USER)
}

SECTIONS
{
  .text : ALIGN(4)
  {
    _code = .;
    *(.text*)
    *(.rodata*)
    _zip_start = .;
    *(.zipdata)
    _zip_end = .;
    _code_end = .;
  } >FLASH

  .data : ALIGN(4)
  {
    *(.early_data*)
    _data_start = .;
    _data_load_start = _data_start - ADDR(.data) + LOADADDR(.data);
    *(.data*)
    _data_end = .;
    _data_load_end = _data_end - ADDR(.data) + LOADADDR(.data);
  } >KRES2 AT>FLASH

   .bss (NOLOAD) : ALIGN(4)
  {
    _bss_start = .;
    *(.bss*)
    _bss_end = .;
  } >KRES2
 
  _end = .;
}
