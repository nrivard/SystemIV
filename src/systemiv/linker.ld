OUTPUT_FORMAT("elf32-m68k")

ENTRY(START)

INCLUDE ../lib/common.ld

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
